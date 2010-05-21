/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *     Copyright c 1991 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *									 *
 *************************************************************************/
/*   active.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:49:53 */

#include <stdio.h>
#ifndef CONVEX
#include <math.h>
#else
#include <fastmath.h>
#endif
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "defect.h"
#include "diffuse.h"
#include "material.h"
#include "plot.h"
#include "device.h"

/************************************************************************
 *									*
 *	active() - This routine computes the active concentration and	*
 * the electron concentration at the passed node.			*
 *									*
 * Original:	7/87							*
 *									*
 ************************************************************************/
get_active( simple, temp, chem, net, dnet, active, equil, dequ )
int simple;
float temp;
double **chem;		/*the solution values to use in calculation*/
double *net;		/*the net active concentration*/
double **dnet;		/*partial of the active concentration*/
double **active;	/*the active substitutional concentrations*/
double **equil;		/*the equilibrium concentration*/
double **dequ;		/*the equilibrium concentration*/
{
    register int i, j, r, imp, mat;
    double Vt = kb * temp;
    int vsol = imptosol[V];
    int isol = imptosol[I];
    int psol = imptosol[Psi];

    /*zero the net doping and calculate the n/ni*/
    if ( IS_PSEUDO( Psi ) ) {
	for(i = 0; i < nn; i++) {
	    net[i] = 0.0;
	    noni[i] = exp( chem[psol][i] / Vt );
	}
    }
    else {
	for(i = 0; i < nn; i++) {
	    if ( IS_SEMI( mat_nd(i) ) ) {
		noni[i] = exp( chem[psol][i] / Vt );
		net[i] = - Ni(nd[i]->mater) * (noni[i] - 1 / noni[i]);
		dnet[psol][i] = - Ni(nd[i]->mater) * (noni[i] + 1 / noni[i]) / Vt ;
	    }
	    else {
		net[i] = 0.0;
		dnet[psol][i] = 0.0;
		noni[i] = 1.0;
	    }
	}
    }

    /*compute equilibrium values*/
    get_Cstar( I, nn, temp, noni, equil[isol], dequ[isol] );
    get_Cstar( V, nn, temp, noni, equil[vsol], dequ[vsol] );

    /*compute the non clustered concentration of the dopants*/
    for( j = 0; j < n_imp; j++ ) {
	imp = soltoimp[j];
	if ( impur[imp].active != NULL )  {
	    impur[imp].active( simple, nn, temp, chem, active, equil, noni );

	    if ( !simple ) {
		if ( imp == As ) {
		    /*$dir no_recurrence*/
		    for(i = 0; i < nn; i++) {
			net[i] += active[j][i];
			dnet[j][i] = dact[j][j][i];
			dnet[vsol][i] += dact[j][vsol][i];
			dnet[psol][i] += dact[j][psol][i];
		    }
		}
		else {
		    /*$dir no_recurrence*/
		    if (IS_ACCEPTOR(imp)) {
			for(i = 0; i < nn; i++) {
			    net[i] -= active[j][i];
			    dnet[j][i] = -dact[j][j][i];
			}
		    } else {
			for(i = 0; i < nn; i++) {
			    net[i] += active[j][i];
			    dnet[j][i] =  dact[j][j][i];
			}
		    }
		}
	    }
	    else {
		/*$dir no_recurrence*/
		if (IS_ACCEPTOR(imp)) {
		    for(i = 0; i < nn; i++) net[i] -= active[j][i];
		} else {
		    for(i = 0; i < nn; i++) net[i] += active[j][i];
		}
	    }
	}
    }
    gaas_act(temp, net);
}

gaas_act(temp, net)
float temp;
double *net;		/*the net active concentration*/
{
    int r, mat;
    register int i,j;
    double a0, a1, b0, b1, n;
    float string_to_real();

    fmacro("T", temp, "%12e");
    for(r = 0; (r < nreg); r++) {

	mat = mat_reg(r);
	if ( (act_a[mat][0] != NULL) && (act_b[mat][0] != NULL) ) {
	    a0 = string_to_real( act_a[mat][0], -1.0);
	    if (a0 < 0) return;
	    b0 = string_to_real( act_b[mat][0], -1.0);
	    if (b0 < 0) return;
	}
	else {
	    a0 = b0 = 0.0;
	}
	if ( (act_a[mat][1] != NULL) && (act_b[mat][1] != NULL) ) {
	    a1 = string_to_real( act_a[mat][1], -1.0);
	    if (a1 < 0) return;
	    b1 = string_to_real( act_b[mat][1], -1.0);
	    if (b1 < 0) return;
	}
	else {
	    a1 = b1 = 0.0;
	}
	if ( (a0 == 0.0) && (a1 == 0.0) ) continue;
	for(j = 0; j < num_nd_reg(r); j++) {
	    i = nd_reg(r, j);
	    n = net[i];
	    if ( (n > 0) && a0 )      net[i] = a0 * n / ( 1.0 + n / b0 );
	    else if ( (n < 0) && a1 ) net[i] = a1 * n / ( 1.0 + n / b1 );
	}
    }
    umacro("T");
}



#ifdef OLDCODE
/************************************************************************
 *									*
 *	compute_sub - This routine computes the substitutional portion	*
 * of each dopant given the pairing rates with defects.			*
 *									*
 * Original:	7/87							*
 *									*
 ************************************************************************/
compute_sub( nv, temp, chem, active, dsub, net, dnet )
int nv;
float temp;
double **chem;
double **active;
double **dsub;
double *net;
double **dnet;
{
    register int i, j;
    register int imp;
    register int mat;
    int isol = imptosol[I];
    int vsol = imptosol[V];
    int psol = imptosol[Psi];
    double sg, sign[MAXCHG];
    double ifr, vfr, pifr, pvfr;
    double den, den2;

    /*for all impurities*/
    for(i = 0; i < n_imp; i++) {
	imp = soltoimp[i];
	if (impur[imp].active == NULL ) continue;
	sg = (IS_ACCEPTOR(imp))?(-1.0):(1.0);
	sign[N] = sg;
	sign[M] = sg - 1.0;
	sign[D] = sg - 2.0;
	sign[R] = sg + 1.0;

	/*$dir no_recurrence*/
	for(j = 0; j < nv; j++) {
	    mat = nd[j]->mater;

	    /*compute the terms needed for the substitutional portion*/
	    ifr = Ichg[N][j] * Kcpl(I,mat,imp,N) + Ichg[M][j] * Kcpl(I,mat,imp,M) +
		  Ichg[D][j] * Kcpl(I,mat,imp,D) + Ichg[R][j] * Kcpl(I,mat,imp,R);
	    pifr = pIchg[N][j] * Kcpl(I,mat,imp,N) + pIchg[M][j] * Kcpl(I,mat,imp,M) +
		   pIchg[D][j] * Kcpl(I,mat,imp,D) + pIchg[R][j] * Kcpl(I,mat,imp,R);

	    vfr = Vchg[N][j] * Kcpl(V,mat,imp,N) + Vchg[M][j] * Kcpl(V,mat,imp,M) +
		  Vchg[D][j] * Kcpl(V,mat,imp,D) + Vchg[R][j] * Kcpl(V,mat,imp,R);
	    pvfr = pVchg[N][j] * Kcpl(V,mat,imp,N) + pVchg[M][j] * Kcpl(V,mat,imp,M) +
		   pVchg[D][j] * Kcpl(V,mat,imp,D) + pVchg[R][j] * Kcpl(V,mat,imp,R);

	    den = 1.0 + ifr * chem[isol][j] + vfr * chem[vsol][j];

	    /*compute appropriate derivitives of the substitutional portion*/
	    dact[i][i][j]    = dsub[i][j] / den;
	    den2 = den * den;
	    dact[i][isol][j] = - active[i][j] * ifr / den2;
	    dact[i][vsol][j] = - active[i][j] * vfr / den2;
	    dact[i][psol][j] = - active[i][j] * 
				 (pifr*chem[isol][j] + pvfr*chem[vsol][j]) / den2;

	    /*compute the substitutional portion*/
	    active[i][j] /= den;

	    /*compute how much of this is charged...*/
	    ifr = sign[N] * Ichg[N][j] * Kcpl(I,mat,imp,N) + 
		  sign[M] * Ichg[M][j] * Kcpl(I,mat,imp,M) +
		  sign[D] * Ichg[D][j] * Kcpl(I,mat,imp,D) + 
		  sign[R] * Ichg[R][j] * Kcpl(I,mat,imp,R);
	    pifr = sign[N] * pIchg[N][j] * Kcpl(I,mat,imp,N) + 
		   sign[M] * pIchg[M][j] * Kcpl(I,mat,imp,M) +
		   sign[D] * pIchg[D][j] * Kcpl(I,mat,imp,D) + 
		   sign[R] * pIchg[R][j] * Kcpl(I,mat,imp,R);

	    vfr = sign[N] * Vchg[N][j] * Kcpl(V,mat,imp,N) + 
		  sign[M] * Vchg[M][j] * Kcpl(V,mat,imp,M) +
		  sign[D] * Vchg[D][j] * Kcpl(V,mat,imp,D) + 
		  sign[R] * Vchg[R][j] * Kcpl(V,mat,imp,R);
	    pvfr = sign[N] * pVchg[N][j] * Kcpl(V,mat,imp,N) + 
		   sign[M] * pVchg[M][j] * Kcpl(V,mat,imp,M) +
		   sign[D] * pVchg[D][j] * Kcpl(V,mat,imp,D) + 
		   sign[R] * pVchg[R][j] * Kcpl(V,mat,imp,R);

	    net[j] += active[i][j] * (sg + ifr * chem[isol][j] + vfr * chem[vsol][j]);

	    /*compute appropriate derivitives of the charged portion*/
	    den = sg + ifr * chem[isol][j] + vfr * chem[vsol][j];
	    dnet[i][j]    += dact[i][i][j] * den;
	    dnet[isol][j] += dact[i][isol][j] * den;
	    dnet[vsol][j] += dact[i][vsol][j] * den;
	    dnet[psol][j] += dact[i][psol][j] * den;

	    dnet[isol][j] += active[i][j] * ifr;
	    dnet[vsol][j] += active[i][j] * vfr;
	    dnet[psol][j] += active[i][j] * (pifr * chem[isol][j] + pvfr * chem[vsol][j]);

	}
    }
}
#endif
	
