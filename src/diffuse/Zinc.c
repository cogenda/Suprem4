/*************************************************************************
 *									 *
 *     Copyright c 1991 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   Zinc.c                Version 1.0     */
/*
 * $Log:
 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "matrix.h"
#include "material.h"
#include "diffuse.h"

/*definitions for impurity constants as a function of material*/
		 /*pre exp constant with neutral V*/
#define Dix0(M)  impur[iZn].constant[M][1][IS_IMPLANTED(iZn)]
		 /*exp constant with neutral V*/
#define DixE(M)  impur[iZn].constant[M][2][IS_IMPLANTED(iZn)]
		 /*pre exp constant with neg V*/
#define Dip0(M)  impur[iZn].constant[M][3][IS_IMPLANTED(iZn)]
		 /*exp constant with neg V*/
#define DipE(M)  impur[iZn].constant[M][4][IS_IMPLANTED(iZn)]
		 /*pre exp constant with 2x neg V*/
#define Dipp0(M) impur[iZn].constant[M][5][IS_IMPLANTED(iZn)]
		 /*exp constant with 2x neg V*/
#define DippE(M) impur[iZn].constant[M][6][IS_IMPLANTED(iZn)]
		 /*fractional interstitialcy*/
#define Fi(M)    impur[iZn].constant[M][7][IS_IMPLANTED(iZn)]

/*definitions of the segregation coefficients as a function of material*/
#define Seg0(M1, M2) 	impur[iZn].seg[SEG0][M1][M2]
#define SegE(M1, M2) 	impur[iZn].seg[SEGE][M1][M2]
#define Trn0(M1, M2) 	impur[iZn].seg[TRN0][M1][M2]
#define TrnE(M1, M2) 	impur[iZn].seg[TRNE][M1][M2]


/* solid solubility data */
static double Znsolid[2][100];
static int Znssmax;
static double Znsstemp = 0.0;

/************************************************************************
 *									*
 *	This file contains definitions of the Zinc routines.  It 	*
 *  includes the routine to calculate diffusion coefficients, the one	*
 *  to calculate any coupling terms between species, and the one to	*
 *  to calculate the boundary condition terms for the Zinc.		*
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
 *	Zndiff_coeff(temp, noni, inter, vacan) - This routine returns	*
 *  the zinc diffusivity as a function of temperature, n/ni, 		*
 *  interstitial and vacancy concentrations.				*
 *									*
 *  Original:	SEH	8/91   (Based on Arsenic.c by MEL)              *
 *									*
 ************************************************************************/
double Zndiff_coeff(temp, new, equil, noni, idf, vdf, iprt, vprt)
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
    double Dix[MAXMAT];
    double Dip[MAXMAT], Dipp[MAXMAT];
    double diff, part, Vt = kb * temp;

    /*first calculate the fixed stuff*/
    for(i = 0; i < MAXMAT; i++) {
	Dix[i]  = Dix0(i)  * exp ( - DixE(i)  / Vt );
	Dip[i]  = Dip0(i)  * exp ( - DipE(i)  / Vt );
	Dipp[i] = Dipp0(i) * exp ( - DippE(i) / Vt );
    }

    for(i = 0; i < nn; i++) {
	mat = nd[i]->mater;

	/*calculate the total diffusivity*/
	diff = Dix[mat]
		+ (Dip[mat] + Dipp[mat] / noni[i]) / noni[i];
	part = - (Dip[mat] + 2.0 * Dipp[mat] / noni[i]) / (noni[i] * Vt);

	idf[i] = Fi( mat ) * diff;
	vdf[i] = (1.0 - Fi( mat )) * diff;

	iprt[i] = Fi( mat ) * part;
	vprt[i] = (1.0 - Fi( mat )) * part;
    }

}


/************************************************************************
 *									*
 *	Znactive( conc, ans, temp ) - This routine calculates the 	*
 *  electrically active portion of the Zinc concentration.		*
 *  This routine assumes that the material in question is zinc.		*
 *									*
 *  Original:	SEH	8/91   (Based on Arsenic.c by MEL)              *
 *									*
 ************************************************************************/
Znactive( simple, nn, temp, conc, act, equil, noni )
int simple;
int nn;
float temp;
double **conc;
double **act;
double **equil;
double *noni;
{
    register int i;
    register int j;
    register int ss=imptosol[iZn];
    double f;
    double p, a, Znss, bZnss;
    double b = 0.90;
    double tmp;


    /*compute the solid solubility at this temperature*/
    Znsstemp = temp;
    for(i = 0; (i < Znssmax) && (Znsolid[0][i] < temp); i++);

    /*if we are at the low end*/
    if ( i == 0 ) {
	tmp = (temp - Znsolid[0][0]) / (Znsolid[0][1] - Znsolid[0][0]);
	Znss = tmp * (Znsolid[1][1] - Znsolid[1][0]) + Znsolid[1][0];
    }
    else if ( i == Znssmax ) {
	j = Znssmax - 1;
	tmp = (temp - Znsolid[0][j]) / (Znsolid[0][j-1] - Znsolid[0][j]);
	Znss = tmp * (Znsolid[1][j-1] - Znsolid[1][j]) + Znsolid[1][j];
    }
    else {
	tmp = (temp - Znsolid[0][i-1]) / (Znsolid[0][i] - Znsolid[0][i-1]);
	Znss = tmp * (Znsolid[1][i] - Znsolid[1][i-1]) + Znsolid[1][i-1];
    }

    f = 1.0;
    p = Znss * (1.0 - b); 
    a = Znss - p * log( p );
    bZnss = b * Znss;

    /*if simple, don't compute the appropriate derviatives*/
    if ( simple ) {
	/*repeat for all the nodes*/
	for(i = 0; i < nn; i++) {

	    if ( INSULATE( nd[i]->mater ) ) {
		act[ss][i] = conc[ss][i];
	    }
	    else {
		if ( conc[ss][i] > Znss ) {
		    act[ss][i] = a + p * log( conc[ss][i] - bZnss );
		    act[ss][i] *= f;
		}
		else {
		    act[ss][i] = f * conc[ss][i];
	 	}
	    }
	}
    }
    
    /*ok - compute the derivatives*/
    else {
	/*repeat for all the nodes*/
	for(i = 0; i < nn; i++) {

	    if ( INSULATE( nd[i]->mater ) ) {
	        act[ss][i] = conc[ss][i];
	        dact[ss][ss][i] = 1.0;
	    }
	    else {
		if ( conc[ss][i] > Znss ) {
		    act[ss][i] = a + p * log( conc[ss][i] - bZnss );
		    dact[ss][ss][i] = f * p / (conc[ss][i] - bZnss);
		    act[ss][i] *= f;
	    	}
		else {
	    	    act[ss][i] = f * conc[ss][i];
	    	    dact[ss][ss][i] = f;
		}
	    }
	}
    }
} 




/************************************************************************
 *									*
 *	Znboundary - This routine handles the computation of the	*
 *  zinc boundary conditions at node i and solutions j and k.		*
 *  The time step is delta and the temperature is temp.  Vel is the	*
 *  velocity of the boundary.						*
 *									*
 *  Original:	SEH	8/91   (Based on Arsenic.c by MEL)              *
 *									*
 ************************************************************************/
Znboundary( bval )
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

    sol = imptosol[iZn];

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

    /*compute the moving boundary flux - if required*/
    if ( (bval->dela[0] != 0.0) && (mat1 == SiO2) ) {
	a[sol][sol][row0]  -= bval->dela[0];
	a[sol][sol][cp1]   += bval->dela[0];
	f = bval->dela[0] * bval->conc[0];
	right_side( row0, sol, bval->rhs, f );
	right_side( row1, sol, bval->rhs, -f );
    }
    if ( (bval->dela[1] != 0.0) && (mat0 == SiO2) ) {
	a[sol][sol][row1]  -= bval->dela[1];
	a[sol][sol][cp0]   += bval->dela[1];
	f = bval->dela[1] * bval->conc[1];
	right_side( row1, sol, bval->rhs, f );
	right_side( row0, sol, bval->rhs, -f );
    }
    
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




/************************************************************************
 *									*
 *	Zinc( par, param ) - This routine gets the 			*
 *  zinc diffusion parameters from the cards and sets them up.		*
 *  They are then used by the rest of the program.			*
 *									*
 *  Original:	SEH	8/91   (Based on Arsenic.c by MEL)              *
 *									*
 ************************************************************************/
zinc( par, param )
char *par;
int param;
{ 
    int mat;
    int mat2 = -1;
    register int i,j;
    int tmpfl, imp_flag, gro_flag;
    double t, c;

    /* get type */
    if ( Listed( "acceptor" ) ) {
	SET_FLAGS(iZn, ACTIVE_ACCEPTOR);
	SET_FLAGS(iZna, ACTIVE_ACCEPTOR);
    }
    if ( Listed( "donor" ) ) {
	CLEAR_FLAGS(iZn, ACTIVE_ACCEPTOR);
	CLEAR_FLAGS(iZna, ACTIVE_ACCEPTOR);
    }

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

    tmpfl = GET_FLAGS(iZn);			/* save state */
    for (; i <= j ; i++) {
	if ( i == 0 )	CLEAR_FLAGS(iZn, IMPLANTED_IMP);
	else		SET_FLAGS(iZn, IMPLANTED_IMP);
	Fetch( Dix0(mat) , "Dix.0" );
	Fetch( DixE(mat) , "Dix.E" );
	Fetch( Dip0(mat) , "Dip.0" );
	Fetch( DipE(mat) , "Dip.E" );
	Fetch( Dipp0(mat) , "Dipp.0" );
	Fetch( DippE(mat) , "Dipp.E" );
	Fetch( Fi(mat)   , "Fi" );
    }

    /* reset implanted source state */
    CLEAR_FLAGS(iZn, ALL_FLAGS);
    SET_FLAGS(iZn, tmpfl);

    /*deal with the solid solubility numbers*/
    if ( get_bool( param, "ss.clear") && is_specified( param, "ss.clear" )) {
	Znssmax = 0;
	Znsstemp = 0.0;
	Znsolid[0][0] = 0.0;
    }
    if ( is_specified( param, "ss.conc" ) && is_specified( param, "ss.temp" ) ) {
	t = get_float( param, "ss.temp" ) + 273.0;
	c = get_float( param, "ss.conc" );

	/*find the spot to store these at*/
	for(i = 0; (i < Znssmax) && (t > Znsolid[0][i]); i++);

	/*is this guy equal?*/
	if ( t == Znsolid[0][i] ) 
	    Znsolid[1][i] = c;
	else {
	    /*move everyone after up a spot*/
	    for(j = Znssmax-1; j >= i; j--) {
		Znsolid[0][j+1] = Znsolid[0][j];
		Znsolid[1][j+1] = Znsolid[1][j];
	    }
	    Znssmax++;

	    /*insert the new one*/
	    Znsolid[0][i] = t;
	    Znsolid[1][i] = c;
	}
	Znsstemp = 0.0;
    }

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

