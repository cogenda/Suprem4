/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   Gold.c                Version 5.1     */
/*   Last Modification : 7/3/91  10:49:41 */


#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "matrix.h"
#include "material.h"

/*definitions for impurity constants as a function of material*/
		/*pre exp constant with neutral V*/
#define K0(M)	impur[Au].constant[M][1][IS_IMPLANTED(Au)]
		/*exp constant with neutral V*/
#define KE(M)	impur[Au].constant[M][2][IS_IMPLANTED(Au)]

/*definitions of the segregation coefficients as a function of material*/
#define Seg0(M1, M2) 	impur[Au].seg[SEG0][M1][M2]
#define SegE(M1, M2) 	impur[Au].seg[SEGE][M1][M2]
#define Trn0(M1, M2) 	impur[Au].seg[TRN0][M1][M2]
#define TrnE(M1, M2) 	impur[Au].seg[TRNE][M1][M2]


/************************************************************************
 *									*
 *	This file contains definitions of the gold routines.  It 	*
 *  includes the routine to calculate diffusion coefficients, the one	*
 *  to calculate any coupling terms between species, and the one to	*
 *  to calculate the boundary condition terms for the gold.		*
 *  Provided the user does not want to change the parameters or access	*
 *  other data, they should be able to modify these extensively without *
 *  any harm to the convergence or solution techniques.			*
 *  									*
 *  Several data base access routines are defined in the file species.c	*
 *  to make it easier for the user to implement routines and get at	*
 *  other data.								*
 *									*
 ************************************************************************/


/************************************************************************
 *									*
 *	Audiff_coeff(temp, noni, inter, vacan) - This routine returns	*
 *  the gold diffusivity as a function of temperature, n/ni, 		*
 *  interstitial and vacancy concentrations.				*
 *									*
 *  Original:	MEL	1/85						*
 *  Revised:	MEL	3/86	(multiple materials)			*
 *									*
 ************************************************************************/
double Audiff_coeff(temp, new, equil, noni, idf, vdf, iprt, vprt)
float temp;		/*temperature*/
double **new;		/*the current concentration set*/
double **equil;		/*the equilibrium concentration set*/
double *noni;		/*the scaled electron concentration*/
double *idf;		/*the interstitial diffusivity*/
double *vdf;		/*the vacancy diffusivity*/
double *iprt;		/*the partial of the interstitial diffusivity*/
double *vprt;		/*the partial of the vacancy diffusivity*/
{
    register int i;
    int mat;
    int sol = imptosol[Au];
    double Kval[MAXMAT];
    double c, diff, part;

    /*first calculate the fixed stuff*/
    for(i = 0; i < MAXMAT; i++) {
	Kval[i] = K0(i) * exp ( - KE(i) / (kb * temp) );
    }

    for(i = 0; i < nn; i++) {
	mat = nd[i]->mater;

	/*calculate the total diffusivity*/
	c = new[sol][i] * new[sol][i];
	diff = Kval[mat] / c;
	part = - Kval[mat] / (c * new[sol][i]);

	idf[i] = diff;
	vdf[i] = 0.0;

	iprt[i] = part;
	vprt[i] = 0.0;
    }

}





/************************************************************************
 *									*
 *	Auboundary - This routine handles the computation of the 	*
 *  gold boundary conditions at node i and solutions j and k.  The 	*
 *  time step is delta and the temperature is temp.  Vel is the 	*
 *  velocity of the boundary.						*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
Auboundary( bval )
struct bound_str *bval;
{
    double f;
    double h,m;
    int sol;		/*the solution location of antimony*/
    int row0 = bval->loc[0][0];
    int row1 = bval->loc[1][1];
    int cp0  = bval->loc[0][1];
    int cp1  = bval->loc[1][0];
    int mat0 = bval->mat[0];
    int mat1 = bval->mat[1];
    float temp = bval->temp;

    sol = imptosol[Au];

    /*calculate the transport terms*/
    m = Seg0(mat0,mat1) * exp ( - SegE(mat0, mat1) / (kb * temp) );
    h = sqrt( m ) * Trn0(mat0,mat1) * exp ( - TrnE(mat0, mat1) / (kb * temp) );

    /*material 1 side derivative of segregation terms*/
    f = h * bval->cpl;
    left_side(row0, sol, f);
    a[sol][sol][cp1] -= f;

    /*material 2 side derivative of segregation terms*/
    f = h * bval->cpl / m;
    left_side(row1, sol, f);
    a[sol][sol][cp0] -= f;

    /*right hand side of segregation terms*/
    f = bval->cpl * h * ( bval->conc[0] - bval->conc[1] / m );
    right_side(row0, sol, bval->rhs, -f);
    right_side(row1, sol, bval->rhs, f);

    /*if either side is gas, disallow any update*/
    if ( mat0 == GAS ) {
	clear_row(row0, sol);
	left_side (row0, sol, 1.0);
    }
    if ( mat1 == GAS ) {
	clear_row(row1, sol);
	left_side (row1, sol, 1.0);
    }
}


Aucoupling()
{
}



/************************************************************************
 *									*
 *	gold( par, param ) - This routine gets the 			*
 *  gold diffusion parameters from the cards and sets them up.	*
 *  They are then used by the rest of the program.			*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
gold( par, param )
char *par;
int param;
{ 
    int i, j;
    int tmpfl, imp_flag, gro_flag;
    int mat;
    int mat2 = -1;

    /*get the material number specified*/
    if ( get_bool( param, "silicon" ) )	mat = Si;
    if ( get_bool( param, "oxide" ) )	mat = SiO2;
    if ( get_bool( param, "oxynitride" ) )	mat = OxNi;
    if ( get_bool( param, "poly" ) )	mat = Poly;
    if ( get_bool( param, "nitride" ) )	mat = SiNi;
    if ( get_bool( param, "gas" ) )	mat = GAS;
    if ( get_bool( param, "gaas" ) )	mat = GaAs;

    /*fetch the values for each constant in this material*/
    i = 0; j = 1;
    imp_flag = get_bool( param, "implanted" );
    gro_flag = get_bool( param, "grown.in" );
    if ( gro_flag & !imp_flag ) j = 0;	/* parameters for grown.in only */
    if ( imp_flag & !gro_flag ) i = 1;	/* parameters for implanted only */

    tmpfl = GET_FLAGS(Au);			/* save state */
    for (; i <= j ; i++) {
	if ( i == 0 )	CLEAR_FLAGS(Au, IMPLANTED_IMP);
	else		SET_FLAGS(Au, IMPLANTED_IMP);
	Fetch( K0(mat) , "K.0" );
	Fetch( KE(mat) , "K.E" );
    }

    /* reset implanted source state */
    CLEAR_FLAGS(Au, ALL_FLAGS);
    SET_FLAGS(Au, tmpfl);

    /*now fetch any segregation data that comes our way*/
    if ( Listed( "/silicon" ) )	mat2 = Si;
    if ( Listed( "/oxide" ) )	mat2 = SiO2;
    if ( Listed( "/oxynitride" ) ) mat2 = OxNi;
    if ( Listed( "/poly" ) )	mat2 = Poly;
    if ( Listed( "/nitride" ) )	mat2 = SiNi;
    if ( Listed( "/gas" ) )	mat2 = GAS;
    if ( Listed( "/gaas" ) )	mat2 = GaAs;
    
    if (mat2 != -1) {
	if (is_specified(param, "Seg.0")) {
	    Seg0(mat2, mat) = get_float( param, "Seg.0" );
	    Seg0(mat, mat2) = 1.0 / Seg0(mat2, mat);
	}
	if (is_specified(param, "Seg.E")) {
	    SegE(mat2, mat) = get_float( param, "Seg.E" );
	    SegE(mat, mat2) = - SegE(mat2, mat);
	}
	if (is_specified(param, "Trn.0")) {
	    Trn0(mat, mat2) = get_float( param, "Trn.0" );
	    Trn0(mat2, mat) = Trn0(mat, mat2);
	}
	if (is_specified(param, "Trn.E")) {
	    TrnE(mat, mat2) = get_float( param, "Trn.E" );
	    TrnE(mat2, mat) = TrnE(mat, mat2);
	}
    }
}
