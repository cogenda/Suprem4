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
/*   setup.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:50:12 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef ALLIANT
#include <cncall.h>
#endif
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "diffuse.h"
#include "material.h"
#include "impurity.h"
#include "matrix.h"

#ifdef ALLIANT
extern int setup_2d();
#endif



/************************************************************************
 *									*
 *	do_setup( ) - This routine is a simple cover function to 	*
 * do a setup for the matrices in question.				*
 *									*
 *  Original:	MEL	5/88						*
 *									*
 ************************************************************************/
do_setup( rhs2 )
double *rhs2;		/*the two norm of the right hand side*/
{
    register int i, bi, bs;
    int is = imptosol[I];
    int vs = imptosol[V];
    double *inter;	/*normalized interstitial concentration*/
    double *vacan;	/*normalized vacancy concentration*/

    inter = salloc(double, nn);
    vacan = salloc(double, nn);

    /*zero everything we can think of*/
    zero_setup(&cs);

    /*compute the active concentration, electron concentration*/
    get_active( FALSE, cs.temp, cs.new, net, dnet, cs.newa, equil, dequ );

    /*compute the scaled defect concentrations*/
    /*compute the number of free interstitials and vacancies*/
    /*$dir no_recurrence*/
    for(i = 0; i < nn; i++) {
	inter[i] = cs.new[is][i] / equil[is][i];
	vacan[i] = cs.new[vs][i] / equil[vs][i];
    }

    /*compute the matrix blocks and boundary conditions*/
    for(bi = 0; bi < cs.nsol; bi++) {
	bs = cs.sol[bi];

	/*do all the diffusing terms*/
	setup_2d(bs, cs.temp, cs.nsol, cs.sol, inter, vacan, cs.new, cs.newa,
		       equil, cs.nco, bval, bcnum );
    }

    /*compute the time terms*/
    for(bi = 0; bi < cs.nsol; bi++) {
	bs = cs.sol[bi];
	rhs2[bs] = 0.0;

	/*now compute all the time terms necessary*/
	if ( (IS_STEADY(soltoimp[bs])) || (cs.type == SS) ) {
	    for(i = 0; i < nn; i++) {
		rhs[bs][i] = newb[bs][i];
		if ( rhs[bs][i] < 1.0e18 )
		    rhs2[bs] += rhs[bs][i] * rhs[bs][i];
		else
		    rhs2[bs] += 1.0e18;
		if ( a[bs][bs][i] == 0.0 ) a[bs][bs][i] = 1.0;
	    }
	}
	else {
	    /*if we have a special time dependent routine...*/
	    impur[soltoimp[bs]].time_val( bs, rhs2, &cs );
	}
    }

    free(inter);
    free(vacan);

}



/************************************************************************
 *									*
 *	zero_setup( ) - This routine zeroes the appropriate arrays 	*
 *  before the setup computation.					*
 *									*
 *  Original:	MEL	5/88						*
 *									*
 ************************************************************************/
zero_setup(cs)
struct call_str *cs;
{
    register int bi, bs, i, j, js;

    for(bi = 0; bi < cs->nsol; bi++) {
	bs = cs->sol[bi];

	/*$dir no_recurrence*/
#ifdef ardent
	dzero( equil[bs], nn );
	dzero( dequ[bs], nn );
	dzero( net, nn );
	dzero( dnet[bs], nn );
	dzero( rhs[bs], nn );
	dzero( newb[bs], nn );
	dzero( cs->newa[bs], nn );
#else
	for(i = 0; i < nn; i++)
	    equil[bs][i] = dequ[bs][i] = net[i] = dnet[bs][i] =
	    rhs[bs][i] = newb[bs][i] = cs->newa[bs][i] = 0.0;
#endif
	for(j = 0; j < cs->nsol; j++) {
	    js = cs->sol[j];
	    if ( blktype[bs][js] != B_NONE )
		dzero(a[bs][js], bia[bs][js][nn]+baoff[bs][js]);

	}
    }
}




/************************************************************************
 *									*
 *	bval_compute( ) - This routine computes the boundary value 	*
 *  structure and returns the number of boundary conditions to compute.	*
 *									*
 *  Original:	MEL	5/88						*
 *									*
 ************************************************************************/
bval_compute( temp, chem, del )
float temp;
double **chem;
double del;
{
    register int i;
    register int nx1, nx2;

    for(i = 0; i < bcnum; i++) {
	bval[i].delta = del;
	bval[i].temp = temp;
	bval[i].rhs = newb;

	/*get the node and arrays into easy access*/
	nx1 = bval[i].nx[0];
	nx2 = bval[i].nx[1];

	/*load up the velocity and area change variables if needed*/
	if ( imptosol[DELA] != -1 ) {
	    bval[i].dela[0] = chem[ imptosol[DELA] ][nx1];
	    bval[i].dela[1] = chem[ imptosol[DELA] ][nx2];
	    if ( (nd[nx2]->mater == Si) || (nd[nx2]->mater == Poly)
					|| (nd[nx2]->mater == GaAs) ) {
		bval[i].vel[0] = chem[imptosol[XVEL]][nx2];
		bval[i].vel[1] = chem[imptosol[YVEL]][nx2];
	    }
	    else {
		bval[i].vel[0] = chem[imptosol[XVEL]][nx1];
		bval[i].vel[1] = chem[imptosol[YVEL]][nx1];
	    }
	}
	else  {
	    bval[i].vel[0] = bval[i].vel[1] = 0.0;
	    bval[i].dela[0] = bval[i].dela[1] = 0.0;
	}
	bval[i].cord[0] = pt[ nd[nx1]->pt ]->cord[0];
	bval[i].cord[1] = pt[ nd[nx1]->pt ]->cord[1];

    }
}








/************************************************************************
 *									*
 *	time_val( ) - This routine computes the time derivative of the	*
 *  specified impurity.							*
 *									*
 *  Original:	MEL	8/88 (rewrite of two older routines)		*
 *									*
 ************************************************************************/
time_val( sol, rhsnm, cs )
int sol;
double *rhsnm;
struct call_str *cs;
{
    register int i;
    double cpl;
    double gam, igam;
    double t1, t2, t3;

    if ( cs->type == TR ) {
	cpl = 2.0 / cs->new_del;

	/*trapazoidal rule setup*/
	for(i = 0; i < nn; i++) {
	    a[sol][sol][i] += cpl * cs->nco[i];

	    /*right hand side sum of flux terms*/
	    rhs[sol][i] = newb[sol][i] + oldb[sol][i];

	    /*the time terms*/
	    rhs[sol][i] -= cpl*(cs->nco[i]*cs->new[sol][i] - cs->oco[i]*cs->old[sol][i]);

	    rhsnm[sol] += rhs[sol][i] * rhs[sol][i];
	    if ( a[sol][sol][i] == 0.0 ) a[sol][sol][i] = 1.0;
	}
    }
    else if ( cs->type == BDF ) {
	/* wieghting terms */
	gam = cs->old_del / ( cs->new_del + cs->old_del );
	igam = 1.0 - gam;
	t1 = (2.0 - gam) / cs->new_del;
	t2 = (1.0 / gam) / cs->new_del;
	t3 = (igam * igam / gam) / cs->new_del;

	for( i = 0; i < nn; i++) {
	    a[sol][sol][i] += cs->nco[i] * t1;

	    /*right hand side terms*/
	    rhs[sol][i] = newb[sol][i];
	    rhs[sol][i] -=   cs->nco[i] * t1 * cs->new[sol][i]
			   - cs->mco[i] * t2 * cs->mid[sol][i]
			   + cs->oco[i] * t3 * cs->old[sol][i];

	    rhsnm[sol] += rhs[sol][i] * rhs[sol][i];
	    if ( a[sol][sol][i] == 0.0 ) a[sol][sol][i] = 1.0;
	}
    }
}





/************************************************************************
 *									*
 *	setup_2d( ) - This routine sets up the linearized diffusion 	*
 *  equation for a single impurity.					*
 *									*
 *  Original:	MEL	12/84						*
 *  Modified:   MEL	6/85	handle the field terms			*
 *									*
 ************************************************************************/
setup_2d( sol, temp, nsol, solve, inter, vacan, chem, act, equil, newarea, bval, bcnum )
int sol;		/*the solution number to setup*/
float temp;		/*the temperature*/
int nsol;
int *solve;
double *inter;		/*the interstitial concentrations*/
double *vacan;		/*the vacancy concentrations*/
double **chem;		/*the current concentrations*/
double **act;		/*the current concentrations*/
double **equil;		/*the current equilibrium concentrations*/
double *newarea;	/*the area of each node*/
struct bound_str *bval;
int bcnum;
{
    register int imp;
    register int i;
    int nx1, nx2;

    imp = soltoimp[ sol ];

    /*Setup the diffision terms if we have a mobile species*/
    if ( impur[imp].block_set != NULL )
	impur[imp].block_set(sol, temp, nsol, solve, newarea, chem, act, inter, vacan );

    /*if no boundary conditions, we are done*/
    if ( impur[ imp ].boundary == NULL ) return;
    if ( IS_PSEUDO( imp ) ) return;

    /*now handle the boundary conditions*/
    for(i = 0; i < bcnum; i++) {

	/*load up the local variables*/
	nx1 = bval[i].nx[0];
	nx2 = bval[i].nx[1];
	bval[i].conc[0] = chem[sol][nx1];
	bval[i].conc[1] = chem[sol][nx2];
	bval[i].eq[0]   = equil[sol][nx1];
	bval[i].eq[1]   = equil[sol][nx2];

	impur[ imp ].boundary( &(bval[i]) );
    }
}



/************************************************************************
 *									*
 *	init_rhs - setup the initial right hand side for time stepping	*
 *									*
 *  Original:	MEL	8/88						*
 *									*
 ************************************************************************/
init_rhs( temp, old, olda, oarea )
float temp;
double **old;
double **olda;
double *oarea;
{
    int i, j, si;
    double rhsnm[MAXIMP];	/*the two norm of the right hand side*/

    /*compute the appropriate boundary condition elements*/
    bval_compute( temp, old, 0.0 );

    /*load the setup structure*/
    cs.type = TR;
    cs.old_del = 0.0;
    cs.new_del = 1.0e-12;
    cs.temp = temp;
    cs.old = old;
    cs.new = old;
    cs.olda = olda;
    cs.newa = olda;
    cs.nco = oarea;
    cs.oco = oarea;
    cs.type = TR;
    cs.sol = tm_sol;
    cs.nsol = tm_nsol;
    cs.elim = tm_elim;
    cs.il = tm_il;
    cs.l = tm_l;
    cs.loff = tm_loff;

    /*now call the real solve routine*/
    do_setup( rhsnm );

    for(i = 0; i < tm_nsol; i++) {
	si = tm_sol[i];
	for(j = 0; j < nn; j++) oldb[si][j] = newb[si][j];
    }

}
