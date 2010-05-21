/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   erfit.c                Version 5.1     */
/*   Last Modification : 7/3/91  10:52:30 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "impurity.h"

/*-----------------ERFC_GROWTH------------------------------------------
 * Implement Hee-Gook Lee's analytic approximation for bird's beaks.
 * Original: CSR June 85.
 *----------------------------------------------------------------------*/

extern double erf();    /* From the math library but not in math.h */
extern double erfc();
#define root2 1.414213562373094
#define rootpi 1.7724539
#define nlog10 2.3025851
#define rnlog10 1.5174271

double old_t1d;			/* Patch for when t1d gets back_pedalled */
double subnormal[2] = {0, -1};	/* Normal to substrate */
float ThinOxideCorr();

erfc_growth (temp, ornt, oxhow, dt)
    float temp;		/* Processing temperature */
    int ornt;		/* Orientation of substrate */
    int oxhow;		/* Dry or wet oxidation */
    double dt;

{
int in;				/* node walker */

float 
    arate, brate,		/* deal-grove coefficients */
    dx1 = 0, dx2 = 0,		/* new growth, displacement into substrate */
    xn,				/* coordinates of a node */
    earg,			/* argument of error function */
    xdep;			/* x-dependence of erfc term */


    /* In 1d just need field thickness and DG coefficients. */

    /* Get the Deal-Grove coefficients, accounting for HCL etc */
    deal_grove (temp, ornt, oxhow, &arate, &brate);

    /* Calculate 1-D growth in the field region */
    /* Uses static variable t1d = total field thickness so far (ugh) */

    dx1 = ( brate / (2 * t1d + arate) );
    dx1 += ThinOxideCorr( t1d, subnormal);

    dx2 = dx1 / alpha[SiO2][Si];

    old_t1d = t1d;
    t1d += dx1 * dt;

    /* Check we actually grew something */
    if (t1d == old_t1d) return;

    /* Compute displacements */

    /* Look at each node */
    for (in = 0; in < nn; in++) {

	/* Ignore bulk silicon nodes*/
	if( nd[ in]->mater == Si && node_mat( in, SiO2) < 0) continue;
	
	/* Compute lateral dependence of vertical displacement */
	xn = pt[ nd[ in]->pt ]->cord[0];
	earg = root2 / spread * (xn - xorg) / (t1d - tinit);
	xdep = 0.5 * (1 + erf (earg));

	/* 
	 * Si3N4, etc nodes move with the SiO2 but Si nodes move 
	 * in the opposite direction.
	 */
         nd[ in ]->sol[ imptosol[YVEL] ] = xdep * ( (nd[in]->mater == Si) ? 
						     dx2 : 
						     (dx2 -dx1));

        }

    return;
}

/*-----------------FIXUP_T1D--------------------------------------------
 * A kludge routine to accound for dt being munged after we
 * have already calculated the dx
 *----------------------------------------------------------------------*/
fixup_t1d (cutback)
    float cutback;
{
    t1d = old_t1d + (t1d - old_t1d) * cutback;
}

/*-----------------ERF1_GROWTH------------------------------------------
 * Another horrible little analytic approximation, from
 * IEEE TED-34 May 1987
 *----------------------------------------------------------------------*/
erf1_growth (temp, ornt, oxhow, dt, which)
    float temp;		/* Processing temperature */
    int ornt;		/* Orientation of substrate */
    int oxhow;		/* Dry or wet oxidation */
    double dt;
    int which;          /* Which miserable model */
{
int in,			/* node walker */
    syvel = imptosol[ YVEL];

float 
    arate, brate,/* deal-grove coefficients */
    dx1 = 0, dx2 = 0,	/* new growth, displacement into substrate */
    xn, yn,		/* coordinates of a node */
    xdep, lift_ratio,
    Lbb, Ht, string_to_real();

    /* Check that this has all been set up properly */
    if( tinitN < 0 || tinit < 0) {
        fprintf( stderr, "Bad values: nit.thick %e initial %e\n",
                         tinitN, tinit);
	return;
    }

    /* In 1d just need field thickness and DG coefficients. */

    /* Get the Deal-Grove coefficients, accounting for HCL etc */
    deal_grove (temp, ornt, oxhow, &arate, &brate);

    /* Calculate 1-D growth in the field region */
    /* Uses static variable t1d = total field thickness so far (ugh) */

    dx1 = ( brate / (2 * t1d + arate) );
    dx1 += ThinOxideCorr( t1d, subnormal);

    dx2 = dx1 / alpha[SiO2][Si];

    old_t1d = t1d;
    t1d += dx1 * dt;

    /* Check we actually grew something */
    if (t1d == old_t1d) return;

    /* Compute displacements */
    
    /* Get the characteristic lengths */
    fmacro("Eox", t1d*1e4, "%12e");
    fmacro("eox", tinit*1e4, "%12e");
    fmacro("temp", temp, "%12e");
    fmacro("Tox",  temp, "%12e");
    fmacro("en",  tinitN*1e4, "%12e");
    Lbb = 1e-4 * string_to_real( erf_Lbb, -1.0);
    Ht  =        string_to_real( erf_H,   -1.0);
    umacro("Eox"); umacro("eox"); umacro("temp"); umacro("Tox"); umacro("en");
    if (Lbb < 0) {
        fprintf( stderr, "Problem with Lbb expression, skipping...\n");
        return;
    }
    if (Lbb == 0) Lbb = 1e-9;
    if (Ht <= 0) {
        fprintf( stderr, "Problem with Ht expression, skipping...\n");
        return;
    }
    lift_ratio = Ht  / (1 - 0.44);
    if (lift_ratio > 1 || lift_ratio < 0.1) {
	fprintf( stderr, "I don't believe this lift ratio (Ht) %e\n", lift_ratio);
	return;
    }

    /* Make up our minds about which model if it hasn't been made up for us */
    if (which == O_ERFG) {
	if( 2*tinitN - 5*tinit <= 0.07e-4) which = O_ERF1;
        else                               which = O_ERF2;
    }

    /* Look at each node */
    for (in = 0; in < nn; in++) {

	/* Ignore bulk silicon nodes*/
	if( nd[ in]->mater == Si && node_mat( in, SiO2) < 0) continue;
	
	/* Compute lateral dependence of vertical displacement */
	/* - sign is for compatibility with erf model */
	/* Where are we with respect to the mask edge? */
	xn = -(pt[ nd[ in]->pt ]->cord[0] - xorg);
        yn = pt[ nd[ in]->pt ]->cord[1];
	
	if (which == O_ERF1) {
	    
	    /* Substrate curve */
	    if ( yn > 0) {
		float c1, b1;
                c1 = 0.5* rootpi * (1 - 2*lift_ratio);
		b1 = (-c1 + sqrt ( nlog10 + c1*c1 )) / Lbb;
                xdep = 0.5 * erfc (b1 * xn + c1);
	    }
	    /* Gas curve */
	    else {
		float b2, c2;
                c2 = 0.5* rootpi * (1 - 2*lift_ratio);
	        b2 = (-c2 + sqrt ( nlog10 + c2*c2)) / Lbb;
		xdep = 0.5 * erfc (b2 * xn + c2);
	    }
	}
	/* O_ERF2 */
	else {
	    /* Substrate curve */
	    if( yn > -1e-9) {
		if (xn > erf1_delta) {
		    float b1p = rnlog10 / Lbb;
		    xdep = lift_ratio * erfc( b1p * (xn - erf1_delta));
		}
		else {
		    float d1p = erf1_delta - erf1_q * (lift_ratio/(lift_ratio-1));
		    xdep = (d1p - xn) / (d1p - xn + erf1_q);
		}
	    }
	    /* Gas curve */
	    else {
		if (xn > 0) {
		    float b2p = rnlog10 / Lbb;
		    xdep = lift_ratio * erfc ( b2p * xn );
		}
		else {
		    float d2p = - erf1_q * (lift_ratio/(lift_ratio - 1));
		    xdep = (d2p - xn)/(d2p - xn + erf1_q);
		}
	    }
	}
	
	/* Silicon nodes definitely move down */
	if (nd[ in ]->mater == Si)
	    nd[ in ]->sol[ syvel ] = xdep * dx2;
	else
	    nd[ in ]->sol[ syvel ] = xdep * (dx2 - dx1);
	
    }

    return;
}

