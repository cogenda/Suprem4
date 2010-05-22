/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   defect.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:49:58 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "defect.h"
#include "material.h"
#include "matrix.h"
#include "diffuse.h"

static int vadd = 0, iadd = 0, tadd = 0;

/************************************************************************
 *									*
 *	defect() - This routine sets up the defect card and its params.	*
 *									*
 *  Original:	MEL	11/85						*
 *									*
 ************************************************************************/
init_pseudo( temp )
double temp;
{
    register int i, s;

    /*compute the temperature dependent interstitial and vacancy parameters*/
    comp_intparam( temp );
    comp_vacparam( temp );

    /*test for vacancies*/
    if (imptosol[V] == -1) {
	add_impurity( V, 0.0, -1 );
	s = imptosol[V];
	for(i = 0; i < nn; i++) nd[i]->sol[s] = Cstar(V, nd[i]->mater);
	vadd = TRUE;
    }

    /*test to see if interstitials are already included*/
    if (imptosol[I] == -1) {
	add_impurity( I, 0.0, -1 );
	s = imptosol[I];
	for(i = 0; i < nn; i++) nd[i]->sol[s] = Cstar(I, nd[i]->mater);
	iadd = TRUE;
    }

    /*set up traps if need be*/
    if ( (imptosol[T] == -1) && trap_on ) {
	add_impurity( T, 0.0, -1 );
	s = imptosol[T];
	for(i = 0; i < nn; i++) nd[i]->sol[s] = Tfrac(nd[i]->mater);
	tadd = TRUE;
    }

    /*set up the potential*/
    if ( imptosol[ Psi ] == -1 )  {
	add_impurity( Psi, 0.0, -1 );
	s = imptosol[Psi];
	for(i = 0; i < nn; i++) nd[i]->sol[s] = 0.0;
    }
}


/************************************************************************
 *									*
 *	get_defaults - This routine assigns the needed values their 	*
 * equilibrium value as an initial condition.				*
 *									*
 * Original:	MEL	5/88						*
 *									*
 ************************************************************************/
get_defaults(imp)
int imp;
{

    if ( imp == I ) {
	iadd = FALSE;
    }
    if ( imp == V ) {
	vadd = FALSE;
    }

}





/************************************************************************
 *									*
 *	get_Cstar( c ) - This routine calculates the equilibrium conc	*
 *  of interstitials as a function of n/ni.				*
 *									*
 *  Original:	MEL	11/85						*
 *									*
 ************************************************************************/
get_Cstar( imp, nv, temp, noni, equil, dequ )
int imp;
int nv;
float temp;
double *noni;
double *equil;
double *dequ;
{
    register int i, mat;
    register int f = which(imp);
    double num, den, c;
    double Vt = kb * temp;

    /*$dir no_recurrence*/
    for( i = 0; i < nv; i++ ) {
	mat = nd[i]->mater;
	c = noni[i];
	den  = Dfrac[f][mat][CN]
		+ Dfrac[f][mat][CM] + Dfrac[f][mat][CDM] + Dfrac[f][mat][CTM]
		+ Dfrac[f][mat][CP] + Dfrac[f][mat][CDP] + Dfrac[f][mat][CTP];

	/*compute the concentration of these guys*/
	num  = Dfrac[f][mat][CN]
		+ ( Dfrac[f][mat][CM] + ( Dfrac[f][mat][CDM]
			+ Dfrac[f][mat][CTM] * c ) * c ) * c
		+ ( Dfrac[f][mat][CP] + ( Dfrac[f][mat][CDP]
			+ Dfrac[f][mat][CTP] / c ) / c ) / c ;
	equil[i] = Cstar(f,mat) * num / den;

	/*compute the derivative w/r to c*/
	num = (Dfrac[f][mat][CM]
		+ (2*Dfrac[f][mat][CDM] + 3*Dfrac[f][mat][CTM]* c) * c)
		- (Dfrac[f][mat][CP]
		+ (2*Dfrac[f][mat][CDP] + 3*Dfrac[f][mat][CTP]/ c) / c) / (c*c);

	dequ[i] = Cstar(f,mat) * c * num / (den * Vt);
    }
}

IVblock_set( ss, temp, nsol, solve, area, chem, sub, inter, vacan )
int ss;			/*block number being solved*/
float temp;
int nsol;
int *solve;
double *area;			/*coupling area*/
double **chem;			/*pointer to solutions at this timestep*/
double **sub;			/*pointer to solutions at this timestep*/
double *inter;			/*scaled interstitial conc*/
double *vacan;			/*scaled vacancy conc*/
{
    defect_block( ss, temp, chem, inter, vacan, area );
}


/************************************************************************
 *									*
 *	 defect_block- This routine works on the defect diffusion	*
 * matrices.								*
 *									*
 * Original:	MEL	1/88						*
 *									*
 ************************************************************************/
defect_block( sol, temp, chem, inter, vacan, area )
int sol;			/*block number being solved*/
float temp;
double **chem;
double *inter, *vacan;
double *area;
{
    register int i, l, u;		/*indices, what else?*/
    register int col;		/*column of the matrix being worked on*/
    register int row;		/*row of the matrix being worked on*/
    int imp = soltoimp[sol];	/*impurity we are working on*/
    int ps = imptosol[Psi];
    double *add = a[sol][sol];	/*the jacobian*/
    double *b = newb[sol];	/*the correct right hand side term*/
    double *aps = a[sol][ps];	/*the jacobian with respect to potential*/
    double jacob, pcrow, pccol;
    double grad, grimp, cpl;
    double diff;
    double eqed;
    double *val;		/*the solution value appropriate*/
    double *idf, *vdf, *iprt, *vprt;

    if ( IS_PSEUDO(imp) ) {
	/*simple fermi level approximation*/
	for(row = 0; row < nn; row++) {
	    b[row] = equil[sol][row] - chem[sol][row];
	    add[row] = 1.0;
	    aps[row] = - dequ[sol][row];
	}
	return;
    }

    /*perform coupling between the various species of defects*/
    if ( imp == I ) {
	Icoupling( temp, area, chem, equil, dequ, newb );
	val = inter;
    }
    else if ( imp == V ) {
	Vcoupling( temp, area, chem, equil, dequ, newb );
	val = vacan;
    }

    /*traps do not diffuse, so quit here..*/
    if( imp == T ) return;

    idf  = salloc( double, nn+1 );
    vdf  = salloc( double, nn+1 );
    iprt = salloc( double, nn+1 );
    vprt = salloc( double, nn+1 );

    /*call the diffusivity routine to get the data*/
    impur[imp].diff_coeff( temp, chem, equil, noni, idf, vdf, iprt, vprt );

    /*for all the columns*/
    for(i = 0; i < ned; i++) {
	col = edg[i]->nd[0];
	row = edg[i]->nd[1];
	u = edg[i]->u;
	l = edg[i]->l;
	cpl = edg[i]->cpl;

	/*do the concentrations on this edge*/
	eqed = 0.5 * (equil[sol][row] + equil[sol][col]);

	/*compute the diffusivities on the edge*/
	diff = 0.5 * (idf[row] + idf[col] + vdf[row] + vdf[col]);

	/*compute the gradients required*/
	grimp = val[col] - val[row];

	/*the full flux is...*/
	grad = diff * cpl * eqed * grimp;

	/*right hand side terms*/
	b[row] += grad;
	b[col] -= grad;

	/*row derivative w/r to val*/
	jacob = diff * cpl * eqed;

	/*derivitive of grad w/r to conc of defect*/
	pccol    = jacob / equil[sol][col];
	pcrow    = jacob / equil[sol][row];
	add[l]   -= pccol;
	add[u]   -= pcrow;
	add[col] += pccol;
	add[row] += pcrow;

	/*derivative of grad w/r to equil*/
	pccol    = - jacob * val[col] * dequ[sol][col] / equil[sol][col];
	pcrow    = - jacob * val[row] * dequ[sol][row] / equil[sol][row];
	aps[l]   -= pccol;
	aps[u]   -= pcrow;
	aps[col] += pccol;
	aps[row] += pcrow;

	pcrow = -0.5 * grimp * diff * cpl * dequ[sol][row];
	pccol =  0.5 * grimp * diff * cpl * dequ[sol][col];
	aps[l]   -= pccol;
	aps[u]   -= pcrow;
	aps[col] += pccol;
	aps[row] += pcrow;
    }
    free( idf );
    free( vdf );
    free( iprt );
    free( vprt );
    return;
}



