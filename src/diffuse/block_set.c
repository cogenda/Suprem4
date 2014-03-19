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
/*   block_set.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:49:55 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "defect.h"
#include "material.h"
#include "diffuse.h"
#include "matrix.h"

#ifndef ardent
#define pragma define
#endif



/* all diffusion like blocks are found here */


/************************************************************************
 *									*
 *	block_set - The parent routine for all the diffusion blocks	*
 *									*
 * Original:	MEL	10/86 ish					*
 * 		MEL	1/88						*
 *									*
 ************************************************************************/
PSbblock_set( ss, temp, nsol, solve, cpl, chem, sub, inter, vacan )
int ss;			/*block number being solved*/
float temp;
int nsol;
int *solve;
double *cpl;			/*coupling areas*/
double **chem;			/*pointer to solutions at this timestep*/
double **sub;			/*pointer to solutions at this timestep*/
double *inter;			/*scaled interstitial conc*/
double *vacan;			/*scaled vacancy conc*/
{
    register int i;
    int imp = soltoimp[ss];
    int vs = imptosol[V];
    int is = imptosol[I];
    int ps = imptosol[Psi];
    double *val;	/*the value diffusing*/
    double *lval;	/*the log of the value diffusing*/
    double *sqval;	/*the square root of the value diffusing*/
    double *dval[MAXIMP]; /*derivitives of the values*/
    double sign = (IS_ACCEPTOR(imp)?(-1.0):(1.0)) / (kb * temp);
    double *idf;	/*the diffusivity with an interstitial mechanism*/
    double *vdf;	/*the diffusivity with an vacancy mechanism*/
    double *iprt;	/*the partial with an interstitial mechanism*/
    double *vprt;	/*the partial with an vacancy mechanism*/

    /*allocate space*/
    idf  = salloc( double, nn+1 );
    vdf  = salloc( double, nn+1 );
    iprt = salloc( double, nn+1 );
    vprt = salloc( double, nn+1 );
    val  = salloc( double, nn+1 );
    lval  = salloc( double, nn+1 );
    sqval  = salloc( double, nn+1 );
    dval[ss]  = salloc( double, nn+1 );
    dval[is]  = salloc( double, nn+1 );
    dval[vs]  = salloc( double, nn+1 );
    dval[ps]  = salloc( double, nn+1 );

    /*call the diffusivity routine to get the data*/
    impur[imp].diff_coeff(temp, chem, equil, noni, idf, vdf, iprt, vprt);

    /*compute the pair terms for each model type*/
    /*interstitials first*/
    /*$dir no_recurrence*/
    for(i = 0; i < nn; i++) {
	val[i] = chem[ss][i] * inter[i];
	lval[i] = log(val[i]);
	sqval[i] = sqrt(val[i]);
	dval[ss][i] = inter[i];
	dval[is][i] = chem[ss][i] / equil[is][i];
	dval[vs][i] = 0.0;
	dval[ps][i] = - dval[is][i]*inter[i]*dequ[is][i];
    }
    chrg_block_set(ss,ss,idf,iprt,val,lval,sqval,dval,chem[ps],sign);
    if ( methdata.defmodel == FULLCPL )
	chrg_block_set(is,ss,idf,iprt,val,lval,sqval,dval,chem[ps],sign);

    /*now vacancies*/
    /*$dir no_recurrence*/
    for(i = 0; i < nn; i++) {
	val[i] = chem[ss][i] * vacan[i];
	lval[i] = log(val[i]);
	sqval[i] = sqrt(val[i]);
	dval[ss][i] = vacan[i];
	dval[is][i] = 0.0;
	dval[vs][i] = chem[ss][i] / equil[vs][i];
	dval[ps][i] = - dval[vs][i]*vacan[i]*dequ[vs][i];
    }
    chrg_block_set(ss,ss,vdf,vprt,val,lval,sqval,dval,chem[ps],sign);
    if ( methdata.defmodel == FULLCPL )
	chrg_block_set(vs,ss,vdf,vprt,val,lval,sqval,dval,chem[ps],sign);

    free( val  );
    free( lval  );
    free( sqval  );
    free( dval[ss] );
    free( dval[is] );
    free( dval[vs] );
    free( dval[ps] );
    free( idf  );
    free( vdf  );
    free( iprt );
    free( vprt );
}


/************************************************************************
 *									*
 *	block_set - The parent routine for all the diffusion blocks	*
 *									*
 * Original:	MEL	10/86 ish					*
 * 		MEL	1/88						*
 *									*
 ************************************************************************/
BAsblock_set( ss, temp, nsol, solve, cpl, chem, sub, inter, vacan )
int ss;			/*block number being solved*/
float temp;
int nsol;
int *solve;
double *cpl;			/*coupling areas*/
double **chem;			/*pointer to solutions at this timestep*/
double **sub;			/*pointer to solutions at this timestep*/
double *inter;			/*scaled interstitial conc*/
double *vacan;			/*scaled vacancy conc*/
{
    register int i;
    int imp = soltoimp[ss];
    int vs = imptosol[V];
    int is = imptosol[I];
    int ps = imptosol[Psi];
    double *val;	/*the value diffusing*/
    double *lval;	/*the log of the value diffusing*/
    double *sqval;	/*the square root of the value diffusing*/
    double *dval[MAXIMP]; /*derivitives of the values*/
    double sign = (IS_ACCEPTOR(imp)?(-1.0):(1.0)) / (kb * temp);
    double *idf;	/*the diffusivity with an interstitial mechanism*/
    double *vdf;	/*the diffusivity with an vacancy mechanism*/
    double *iprt;	/*the partial with an interstitial mechanism*/
    double *vprt;	/*the partial with an vacancy mechanism*/
    double t1;

    /*allocate space*/
    idf  = salloc( double, nn+1 );
    vdf  = salloc( double, nn+1 );
    iprt = salloc( double, nn+1 );
    vprt = salloc( double, nn+1 );
    val  = salloc( double, nn+1 );
    lval  = salloc( double, nn+1 );
    sqval  = salloc( double, nn+1 );
    dval[ss]  = salloc( double, nn+1 );
    dval[is]  = salloc( double, nn+1 );
    dval[vs]  = salloc( double, nn+1 );
    dval[ps]  = salloc( double, nn+1 );

    /*call the diffusivity routine to get the data*/
    impur[imp].diff_coeff(temp, chem, equil, noni, idf, vdf, iprt, vprt);

    /*interstitials first*/
    /*$dir no_recurrence*/
    for(i = 0; i < nn; i++) {
	val[i] = sub[ss][i] * inter[i];
	lval[i] = log(val[i]);
	sqval[i] = sqrt(val[i]);
	dval[ss][i] = inter[i] * dact[ss][ss][i];
	dval[is][i] = sub[ss][i] / equil[is][i];
	dval[vs][i] = inter[i] * dact[ss][vs][i];
	dval[ps][i] = inter[i] * (dact[ss][ps][i] - dval[is][i] * dequ[is][i]);
    }
    chrg_block_set(ss,ss,idf,iprt,val,lval,sqval,dval,chem[ps],sign);
    if ( methdata.defmodel == FULLCPL )
	chrg_block_set(is,ss,idf,iprt,val,lval,sqval,dval,chem[ps],sign);

    /*now vacancies*/
    /*$dir no_recurrence*/
    for(i = 0; i < nn; i++) {
	val[i] = sub[ss][i] * vacan[i];
	lval[i] = log(val[i]);
	sqval[i] = sqrt(val[i]);
	dval[ss][i] = vacan[i] * dact[ss][ss][i];
	dval[is][i] = 0.0;
	t1 = sub[ss][i]/equil[vs][i];
	dval[vs][i] = vacan[i]*dact[ss][vs][i]+t1;
	dval[ps][i] = vacan[i] * ( dact[ss][ps][i] - t1 * dequ[vs][i] );
    }
    chrg_block_set(ss,ss,vdf,vprt,val,lval,sqval,dval,chem[ps],sign);
    if ( methdata.defmodel == FULLCPL )
	chrg_block_set(vs,ss,vdf,vprt,val,lval,sqval,dval,chem[ps],sign);

    free( val  );
    free( lval  );
    free( sqval );
    free( dval[ss] );
    free( dval[is] );
    free( dval[vs] );
    free( dval[ps] );
    free( idf  );
    free( vdf  );
    free( iprt );
    free( vprt );
}




/************************************************************************
 *									*
 *	block_set( old, foo, olddiff, newdiff, b, A ) -			*
 * This routine sets up the diffusion equation block for an impurity.	*
 * The impurity diffusion area routine is passed in as 		*
 * diff_oeff.  old and new are arrays of values at the last time step	*
 * and the new timestep.						*
 *									*
 * Original:	MEL	12/84						*
 *									*
 ************************************************************************/
neut_block_set( sol, temp, nu1, nu2, area, chem, nu3, nu4, nu5 )
int sol;		/*block number being solved*/
float temp;		/*temperature*/
int nu1;		/*not used*/
int *nu2;		/*not used*/
double *area;		/*coupling coefficients*/
double **chem;		/*pointer to solutions at this timestep*/
double **nu3;		/*not used*/
double *nu4;		/*not used*/
double *nu5;		/*not used*/
{
    register int i, u, l;		/*indices, what else?*/
    register int col;		/*column of the matrix being worked on*/
    register int row;		/*row of the matrix being worked on*/
    int imp = soltoimp[sol];	/*impurity we are working on*/
    double *b = newb[sol];	/*the right hand side at the new time*/
    double *ann = a[sol][sol];	/*the left hand block*/
    double *anp = a[sol][imptosol[Psi]];	/*the left hand block*/
    double cpl;			/*diffusivity times the coupling coefficient*/
    double grimp; 		/*gradients of impurity, potential, defects*/
    double *idf;	/*the diffusivity with an interstitial mechanism*/
    double *vdf;	/*the diffusivity with an vacancy mechanism*/
    double *iprt;	/*the partial with an interstitial mechanism*/
    double *vprt;	/*the partial with an vacancy mechanism*/
    double ehed;
    double pccol, pcrow;

    /*allocate space*/
    idf  = salloc( double, nn+1 );
    vdf  = salloc( double, nn+1 );
    iprt = salloc( double, nn+1 );
    vprt = salloc( double, nn+1 );

    /*call the diffusivity routine to get the data*/
    impur[imp].diff_coeff( temp, chem, equil, noni, idf, vdf, iprt, vprt );

    /*for all the edges*/
    for(i = 0; i < ned; i++) {
	col = edg[i]->nd[0];
	row = edg[i]->nd[1];
	u = edg[i]->u;
	l = edg[i]->l;
	ehed = edg[i]->cpl;

	/*compute the diffusivities on the edge*/
	cpl = ehed * 0.5 * (idf[row] + idf[col] + vdf[row] + vdf[col]);

	/*compute the gradients required*/
	grimp = chem[sol][col] - chem[sol][row];

	/*right hand side terms*/
	b[row] += cpl * grimp;
	b[col] -= cpl * grimp;

	/*derivitive of grad w/r to conc of imp*/
	ann[l]   -= cpl;
	ann[u]   -= cpl;
	ann[col] += cpl;
	ann[row] += cpl;
    }
    free( idf  );
    free( vdf  );
    free( iprt );
    free( vprt );
}




/************************************************************************
 *									*
 *	block_set( old, foo, olddiff, newdiff, b, A ) -			*
 * This routine sets up the diffusion equation block for an impurity.	*
 * The impurity diffusion routine is passed in as 		*
 * diff_oeff.  old and foo are arrays of values at the last time step	*
 * and the new timestep.						*
 *									*
 * Original:	MEL	4/87						*
 *									*
 ************************************************************************/
chrg_block_set( ss, sol, df, dprt, val, lval, sqval, dval, pot, spot)
int ss;			/*the storage location*/
int sol;		/*impurity solution number*/
double *df;		/*the diffusivity*/
double *dprt;		/*the diffusivity derivitive*/
double *val;		/*the value diffusing*/
double *lval;		/*the log of the value diffusing*/
double *sqval;		/*the sqrt of the value diffusing*/
double *dval[MAXIMP];	/*the derivitive of w/r to sol variables*/
double *pot;		/*the potential values*/
double spot;		/*the sign for the potential terms*/
{
    register int i, u, l;	/*indices, what else?*/
    register int col;		/*column of the matrix being worked on*/
    register int row;		/*row of the matrix being worked on*/
    int ps = imptosol[ Psi ];	/*location of the potential terms*/
    int is = imptosol[ I ];	/*location of the interstitial terms*/
    int vs = imptosol[ V ];	/*location of the vacancy terms*/
    double cpl, ehed;		/*diffusivity times the coupling coefficient*/
    double ced;		/*the concentrations on this edge*/
    double grimp, grpot;	/*gradients of impurity, potential, defects*/
    double diff;
    double grad;
    double pcrow, pccol, jacrow, jaccol;
    double *ass, *asp, *asi, *asv;
    double *dvs, *dvp, *dvi, *dvv;

    ass = a[ss][sol];
    asp = a[ss][ps];
    asi = a[ss][is];
    asv = a[ss][vs];
    dvs = dval[sol];
    dvp = dval[ps];
    dvi = dval[is];
    dvv = dval[vs];

    /*for all edges*/
    /*for all the edges*/
    for(i = 0; i < ned; i++) {
	col = edg[i]->nd[0];
	row = edg[i]->nd[1];
	u = edg[i]->u;
	l = edg[i]->l;
	ehed = edg[i]->cpl;

	/*do the interstitial portion of the flux first*/
	ced = sqval[row] * sqval[col];

	/*first compute some useful gradient information*/
	grimp = val[col] - val[row];
	grpot = spot * (pot[col] - pot[row]);

	/*compute the diffusivities on the edge*/
	diff = 0.5 * (df[row] + df[col]);

	/*compute the interstitial flux portion*/
	cpl     = ehed * diff;
	grad    = grimp + ced * grpot;
	newb[ss][row] += cpl * grad;
	newb[ss][col] -= cpl * grad;

        /*compute the derivitive w/r to value*/
        if(sqval[row] != 0.0)
          jacrow  = cpl * ( 1.0 - grpot * 0.5 * sqval[col] / sqval[row]);
        else
          jacrow  = cpl;

        if(sqval[col] != 0.0)
          jaccol  = cpl * ( 1.0 + grpot * 0.5 * sqval[row] / sqval[col]);
        else
          jaccol  = cpl;


	/*compute the derivative w/r to the chemical concentration*/
	pcrow   = jacrow * dvs[row];
	pccol   = jaccol * dvs[col];
	ass[l]   -= pccol;
	ass[u]   -= pcrow;
	ass[col] += pccol;
	ass[row] += pcrow;

	/*compute the derivative w/r to the chemical concentration*/
	pcrow   = jacrow * dvp[row];
	pccol   = jaccol * dvp[col];
	asp[l]   -= pccol;
	asp[u]   -= pcrow;
	asp[col] += pccol;
	asp[row] += pcrow;

	/*compute the derivative w/r to the chemical concentration*/
	pcrow   = jacrow * dvv[row];
	pccol   = jaccol * dvv[col];
	asv[l]   -= pccol;
	asv[u]   -= pcrow;
	asv[col] += pccol;
	asv[row] += pcrow;

	/*compute the derivative w/r to the interstitial concentration*/
	pcrow   = jacrow * dvi[row];
	pccol   = jaccol * dvi[col];
	asi[l]   -= pccol;
	asi[u]   -= pcrow;
	asi[col] += pccol;
	asi[row] += pcrow;

	/*derivitive of diffusivity w/r to psi*/
	pcrow   = ehed * (diff * ced * spot - 0.5 * dprt[row] * grad);
	pccol   = ehed * (diff * ced * spot + 0.5 * dprt[col] * grad);
	asp[l]   -= pccol;
	asp[u]   -= pcrow;
	asp[col] += pccol;
	asp[row] += pcrow;

    }
}


#ifndef DEVICE
/************************************************************************
 *									*
 *	block_set( old, new, olddiff, newdiff, b, A ) -			*
 * This routine sets up the diffusion equation block for an impurity.	*
 * The impurity diffusion area routine is passed in as 		*
 * diff_oeff.  old and new are arrays of values at the last time step	*
 * and the new timestep.						*
 *									*
 * Original:	MEL	5/87						*
 *									*
 ************************************************************************/
poisson_block( sol, temp, nsol, solve, area, chem, nu1, nu2, nu3 )
int sol;			/*block number being solved*/
float temp;
int nsol;
int *solve;
double *area;			/*coupling area*/
double **chem;			/*pointer to solutions at this timestep*/
double **nu1;			/*not used*/
double *nu2;			/*not used*/
double *nu3;			/*not used*/
{
    register int i, j;
    register int col;
    register int row;
    double *app = a[sol][sol];
    double *b = newb[sol];
    int ao = aoff;
    double *val = chem[sol];
    int foo[MAXIMP], nfoo;
    double tmp, c, ni, t1;
    double eps, p, n, cpl, grad;
    double Vt = kb * temp;

    /*charge neutral solution for diffusion*/
    if ( IS_PSEUDO( Psi ) ) {
	for(nfoo = j = 0; j < nsol; j++)
	    if ( blktype[sol][solve[j]] == B_DIAG ) foo[nfoo++] = solve[j];

	/*simple analytic function*/
	for(i = 0; i < nn; i++) {
	    if ( (nd[i]->mater == Si) || (nd[i]->mater == Poly) ) {
		app[i] = 1.0 / Vt;

		ni = 2.0 * Ni(nd[i]->mater);
		c = net[i] / ni;

		t1 = sqrt( c * c + 1.0 );
		tmp = 1.0 / (ni * t1);

		for(j = 0; j < nfoo; j++) a[sol][foo[j]][i] -= tmp * dnet[foo[j]][i];

		b[i] = log( c + t1 ) - val[i] / Vt;
	    }
	    else {
		app[i] = 1.0 / Vt;
		b[i] = 0.0;
	    }
	}
    }
}
#endif
