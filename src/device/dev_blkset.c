/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of	 *
 *                      Florida. All rights reserved.		 	 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of University of Florida*
 *									 *
 *************************************************************************/
/*   dev_blkset.c                Version 5.2     */
/*   Last Modification : 7/3/91  15:44:18 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "device.h"
#include "material.h"
#include "diffuse.h"
#include "matrix.h"

#define qchrg  1.62e-19

/************************************************************************
 *									*
 *	block_set( old, new, olddiff, newdiff, b, A ) -			*
 *									*
 * Original:	MEL	2/89						*
 *									*
 ************************************************************************/
poisson_block( sol, temp, nsol, solve, coeff, chem, nu1, nu2, nu3 )
int sol;		/*block number being solved*/
float temp;
int nsol;		/*number of solutions being worked on*/
int *solve;		/*the solution values*/
double *coeff;		/*coupling coefficients*/
double **chem;		/*pointer to solutions at this timestep*/
double **nu1;		/*not used*/
double *nu2;		/*not used*/
double *nu3;		/*not used*/
{
    register int i, j;
    register int col;
    register int row;
    double *app = a[sol][sol];
    double *b = newb[sol];
    int foo[MAXIMP], nfoo;
    int l, u;
    double *val = chem[sol];
    double tmp, c, ni, t1;
    double scal[MAXMAT], cpl, grad;
    double Vt, Vti;

    Vt  = 8.62e-5 * temp;
    Vti = 1.0 / Vt;
    for(nfoo = j = 0; j < nsol; j++)
	if ( blktype[sol][solve[j]] != B_NONE ) foo[nfoo++] = solve[j];

    /*charge neutral solution for diffusion*/
    if ( IS_PSEUDO( Psi ) ) {
        /*simple analytic function*/
        for(i = 0; i < nn; i++) {
            if ( IS_SEMI(nd[i]->mater) ) {
                app[i] = Vti;

                ni = 2.0 * Ni(nd[i]->mater);
                c = net[i] / ni;

                t1 = sqrt( c * c + 1.0 );
                tmp = 1.0 / (ni * t1);

                for(j = 0; j < nfoo; j++) a[sol][foo[j]][i] -= tmp * dnet[foo[j]
][i];
                b[i] = log( c + t1 ) - val[i] * Vti;
            }
            else {
                app[i] = Vti;
                b[i] = 0.0;
            }
        }
    }
    /*full blown poisson solution for device analysis*/
    else {
	for(i = 0; i < MAXMAT; i++) scal[i] = Eps(i) / qchrg;

	/*for all the edges*/
	for(i = 0; i < ned; i++) {
	    col = edg[i]->nd[0];
	    row = edg[i]->nd[1];
	    cpl = scal[mat_nd(col)] * edg[i]->cpl;
	    u = edg[i]->u;
	    l = edg[i]->l;

	    /*compute the fluxes*/
	    grad = val[col] - val[row];

	    /*right hand side terms*/
	    b[row] += cpl * grad;
	    b[col] -= cpl * grad;

	    app[l]   -= cpl;
	    app[u]   -= cpl;
	    app[col] += cpl;
	    app[row] += cpl;
	}

	for(col = 0; col < nn; col++) {
	    /*compute the charge terms*/
	    b[col] += coeff[col] * net[col];
	    for(j = 0; j < nfoo; j++)
		a[sol][foo[j]][col] -= coeff[col] * dnet[foo[j]][col];
	}
    }
}


/************************************************************************
 *									*
 *	block_set( old, new, olddiff, newdiff, b, A ) -			*
 *									*
 * Original:	MEL	2/89						*
 *									*
 ************************************************************************/
hole_block( sol, nu1, nu2, nu3, coeff, chem, nu4, nu5, nu6 )
int sol;			/*block number being solved*/
float nu1;			/*not used*/
int nu2;			/*not used*/
int *nu3;			/*not used*/
double *coeff;			/*coupling coefficients*/
double **chem;			/*pointer to solutions at this timestep*/
double **nu4, *nu5, *nu6;	/*not used*/
{
    register int mat, i;
    double *ahh = a[sol][sol];
    double *ahp = a[sol][imptosol[Psi]];
    double *b = newb[sol];
    double *val = chem[sol];
    double *pot = chem[imptosol[Psi]];
    double arg, d1, d2;

    /*charge neutral solution for diffusion*/
    if ( IS_PSEUDO( H ) ) {

	/*simple analytic Boltzmann statistics with fixed QF*/
	for(i = 0; i < nn; i++) {
	    mat = nd[i]->mater;
	    if (IS_SEMI(mat)) {
		arg = Phi_p( val[i], pot[i], Ev[i], mat, &(d1), &(d2) );
		b[i] = qfp - arg;
		ahp[i] = d1;
		ahh[i] = d2;
	    }
	    else {
		val[i] = 1.0e-32;
		ahh[i] = 1.0;
		b[i] = 0.0;
	    }
	}
    }
    else {
	printf("hole solutions are not available yet\n");
    }
}


/************************************************************************
 *									*
 *	block_set( old, new, olddiff, newdiff, b, A ) -			*
 *									*
 * Original:	MEL	2/89						*
 *									*
 ************************************************************************/
electron_block( sol, nu1, nu2, nu3, coeff, chem, nu4, nu5, nu6 )
int sol;			/*block number being solved*/
float nu1;			/*not used*/
int nu2;			/*not used*/
int *nu3;			/*not used*/
double *coeff;			/*coupling coefficients*/
double **chem;			/*pointer to solutions at this timestep*/
double **nu4, *nu5, *nu6;	/*not used*/
{
    register int mat, i, j, k;
    double *aee = a[sol][sol];
    double *aep = a[sol][imptosol[Psi]];
    double *b = newb[sol];
    double *val = chem[sol];
    double *pot = chem[imptosol[Psi]];
    double arg, d1, d2;
    double *m, *v, *djdm, *djdv, *dvdn, *dvdp;

    /*charge neutral solution for diffusion*/
    if ( IS_PSEUDO( N ) ) {

	/*simple analytic Boltzmann statistics with fixed QF*/
	for(i = 0; i < nn; i++) {
	    mat = nd[i]->mater;
	    if ( IS_SEMI( mat ) ) {
		arg = Phi_n( val[i], pot[i], Ec[i], mat, &d1, &d2 );
		b[i] = qfn - arg;
		aep[i] = d1;
		aee[i] = d2;
	    }
	    else {
		val[i] = 1.0e-32;
		aee[i] = 1.0;
		b[i] = 0.0;
	    }
	}
    }
    else {
	m = scalloc( double, nn+1 );
	v = scalloc( double, nn+1 );
	dvdn = scalloc( double, nn+1 );
	dvdp = scalloc( double, nn+1 );
	djdm = scalloc( double, ia[nn]+aoff+1 );
	djdv = scalloc( double, ia[nn]+aoff+1 );

	/*compute the preliminaries*/
	for(i = 0; i < nn; i++) {
	    mat = nd[i]->mater;
	    if ( IS_SEMI( mat ) ) {
		v[i] = Phi_n( val[i], pot[i], Ec[i], mat, &(dvdp[i]), &(dvdn[i]) );
	    }
	    else {
		dvdp[i] = dvdn[i] = v[i] = 0.0;
	    }
	    m[i] = 500.0;
	}

	GSD_block_set(nn, m, NULL, val, v, b, coeff, djdm, aee, djdv);

	/*fold the derivatives in...*/
	for(i = 0; i < nn; i++) {
	    aee[i] += djdv[i] * dvdn[i];
	    aep[i] += djdv[i] * dvdp[i];
	    for(j = ia[i]; j < ia[i+1]; j++) {
		k = ia[j];
		aee[j] += djdv[j] * dvdn[k];
		aep[j] += djdv[j] * dvdp[k];
	    }
	}

	free( m );
	free( v );
	free( dvdn );
	free( dvdp );
	free( djdm );
	free( djdv );
    }
}




/************************************************************************
 *									*
 *	GSD_block_set - This routine performs a Gummel-Scharfetter	*
 *  discretizations on the flux m exp( u ) grad v and returns the	*
 *  derivative expressions.						*
 *									*
 * Original:	MEL	5/89						*
 *									*
 ************************************************************************/
GSD_block_set( nn, m, u, eu, v, cur, coeff, djdm, djdu, djdv )
int nn;			/*the number of nodes*/
double *m;		/*the mobility transport term*/
double *u, *eu;		/*the u and exp(u) variables*/
double *v;		/*the variable to take the gradient*/
double *cur;		/*the fluxes*/
double *coeff;		/*coupling coefficients*/
double *djdm;		/*the derivative of the fluxes w/r to m*/
double *djdu;		/*the derivative of the fluxes w/r to eu*/
double *djdv;		/*the derivative of the fluxes w/r to v*/
{
    register int row, j, col;
    double am, aeu, del, tmp;
    double pcrow, pccol;
    double sqr, sqc;

    for(col = 0; col < nn; col++) {
	for(j = ia[col]; j < ia[col+1]; j++) {
	    if ( coeff[j] == 0.0 ) continue;
	    row = ia[j];

	    am = (m[row] + m[col]) * 0.5;
	    sqr = sqrt( eu[row] );
	    sqc = sqrt( eu[col] );
	    aeu = sqr * sqc;
	    del = v[col] - v[row];

	    cur[row] += coeff[j] * am * aeu * del;
	    cur[col] -= coeff[j] * am * aeu * del;

	    /*the derivative w/r to the mobility*/
	    tmp = coeff[j] * aeu * del * 0.5;
	    djdm[col]    += tmp;
	    djdm[j+aoff] += tmp;
	    djdm[row]    -= tmp;
	    djdm[j]      -= tmp;

	    /*the derivative w/r to the aeu term*/
	    pcrow = coeff[j] * am * del * 0.5 * sqc / sqr;
	    pccol = coeff[j] * am * del * 0.5 * sqr / sqc;
	    djdu[col]    += pccol;
	    djdu[j+aoff] += pcrow;
	    djdu[row]    -= pcrow;
	    djdu[j]      -= pccol;

	    /*the derivitive w/r to the del*/
	    tmp = coeff[j] * am * aeu;
	    djdv[col]    += tmp;
	    djdv[j+aoff] -= tmp;
	    djdv[row]    += tmp;
	    djdv[j]      -= tmp;
	}
    }
}

