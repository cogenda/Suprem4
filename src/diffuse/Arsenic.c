/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   Arsenic.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:49:36 */


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
#define Dix0(M)	impur[As].constant[M][1][IS_IMPLANTED(As)]
		/*exp constant with neutral V*/
#define DixE(M)	impur[As].constant[M][2][IS_IMPLANTED(As)]
		/*pre exp constant with pos V*/
#define Dim0(M)	impur[As].constant[M][3][IS_IMPLANTED(As)]
		/*exp constant with pos V*/
#define DimE(M)	impur[As].constant[M][4][IS_IMPLANTED(As)]
		/*fractional interstitialcy*/
#define Fi(M)	impur[As].constant[M][5][IS_IMPLANTED(As)]
		/*pre exponential part of clustering*/
#define Ctn0(M)	impur[As].constant[M][6][IS_IMPLANTED(As)]
		/*activiation energy of clustering*/
#define CtnE(M)	impur[As].constant[M][7][IS_IMPLANTED(As)]
		/*clustering power*/
#define CtnF(M)	impur[As].constant[M][8][IS_IMPLANTED(As)]

/*definitions of the segregation coefficients as a function of material*/
#define Seg0(M1, M2) 	impur[As].seg[SEG0][M1][M2]
#define SegE(M1, M2) 	impur[As].seg[SEGE][M1][M2]
#define Trn0(M1, M2) 	impur[As].seg[TRN0][M1][M2]
#define TrnE(M1, M2) 	impur[As].seg[TRNE][M1][M2]


/* solid solubility data */
static double Assolid[2][100];
static int Asssmax;
static double Assstemp = 0.0;

/************************************************************************
 *									*
 *	This file contains definitions of the Arsenic routines.  It 	*
 *  includes the routine to calculate diffusion coefficients, the one	*
 *  to calculate any coupling terms between species, and the one to	*
 *  to calculate the boundary condition terms for the Arsenic.		*
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
 *	Asdiff_coeff(temp, noni, inter, vacan) - This routine returns	*
 *  the arsenic diffusivity as a function of temperature, n/ni, 	*
 *  interstitial and vacancy concentrations.				*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
double Asdiff_coeff(temp, new, equil, noni, idf, vdf, iprt, vprt)
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
    double Dim[MAXMAT];
    double diff, part, Vt = kb * temp;

    /*first calculate the fixed stuff*/
    for(i = 0; i < MAXMAT; i++) {
	Dix[i] = Dix0(i) * exp ( - DixE(i) / (kb * temp) );
	Dim[i] = Dim0(i) * exp ( - DimE(i) / (kb * temp) );
    }

    for(i = 0; i < nn; i++) {
	mat = nd[i]->mater;

	/*calculate the total diffusivity*/
	diff = Dix[mat] + Dim[mat] * noni[i];
	part = Dim[mat] * noni[i] / Vt;

	idf[i] = Fi( mat ) * diff;
	vdf[i] = (1.0 - Fi( mat )) * diff;

	iprt[i] = Fi( mat ) * part;
	vprt[i] = (1.0 - Fi( mat )) * part;
    }

}


/************************************************************************
 *									*
 *	Asactive( conc, ans, temp ) - This routine calculates the 	*
 *  electrically active portion of the Arsenic concentration.		*
 *  This routine assumes that the material in question is silicon.	*
 *									*
 *  Original:	MEL	3/85						*
 *									*
 ************************************************************************/
Asactive( simple, nn, temp, conc, act, equil, noni )
int simple;
int nn;
float temp;
double **conc;
double **act;
double **equil;
double *noni;
{
    static double Ctn[MAXMAT];
    static double screen[MAXMAT];
    double tmp, der2, der, npow, np2;
    double vac;
    register int i, j, mat;
    register int ss=imptosol[As];
    register int ps=imptosol[Psi];
    register int vs=imptosol[V];
    double Vt = kb * temp;
    double f, p, a, bAsss, Asss;
    double b = 0.90;
    double tmpact, tmpdacts, tmpdactv, tmpdactp;


    /*compute the solid solubility at this temperature*/
    Assstemp = temp;
    for(i = 0; (i < Asssmax) && (Assolid[0][i] < temp); i++);

    /*if we are at the low end*/
    if ( i == 0 ) {
	tmp = (temp - Assolid[0][0]) / (Assolid[0][1] - Assolid[0][0]);
	Asss = tmp * (Assolid[1][1] - Assolid[1][0]) + Assolid[1][0];
    }
    else if ( i == Asssmax ) {
	j = Asssmax - 1;
	tmp = (temp - Assolid[0][j]) / (Assolid[0][j-1] - Assolid[0][j]);
	Asss = tmp * (Assolid[1][j-1] - Assolid[1][j]) + Assolid[1][j];
    }
    else {
	tmp = (temp - Assolid[0][i-1]) / (Assolid[0][i] - Assolid[0][i-1]);
	Asss = tmp * (Assolid[1][i] - Assolid[1][i-1]) + Assolid[1][i-1];
    }

    f = 1.0;
    p = Asss * (1.0 - b); 
    a = Asss - p * log( p );
    bAsss = b * Asss;


    for(i = 0; i < MAXMAT; i++) Ctn[i] = screen[i] = 1.0;
    Ctn[Si] = Ctn0(Si) * exp( CtnE(Si) / Vt ) * Ni(Si);
    Ctn[Poly] = Ctn0(Poly) * exp( CtnE(Poly) / Vt ) * Ni(Poly);

    /*if simple, don't compute the appropriate derviatives*/
    if ( simple ) {
	/*repeat for all the nodes*/
	for(i = 0; i < nn; i++) {
	    mat = nd[i]->mater;

	    if ( (INSULATE(nd[i]->mater)) || (Ctn[mat] == 1.0) ) {
		tmpact = conc[ss][i];
	    }
	    else {

		if ( methdata.defmodel != FULLCPL ) {
		    /*compute the terms needed*/
		    npow = Ctn[mat] * noni[i];
		    tmp = (1.0 + npow * npow);
		    tmpact = conc[ss][i] / tmp;
		}
		else {
		    /*compute the terms needed*/
		    npow = Ctn[mat] * noni[i];
		    tmp = (1.0 + npow * npow * conc[vs][i] / equil[vs][i] );
		    tmpact = conc[ss][i] / tmp;
		}
	    }

/*
	    act[ss][i] = tmpact;
*/

	    if ( INSULATE( nd[i]->mater ) ) {
		act[ss][i] = tmpact;
	    }
	    else {
		if ( tmpact > Asss ) {
		    act[ss][i] = a + p * log( tmpact - bAsss );
		    act[ss][i] *= f;
		}
		else {
		    act[ss][i] = f * tmpact;
	 	}
	    }
	}
    }
    
    /*ok - compute the derivatives*/
    else {
	/*repeat for all the nodes*/
	for(i = 0; i < nn; i++) {
	    mat = nd[i]->mater;

	    if ( (INSULATE(nd[i]->mater)) || (Ctn[mat] == 1.0) ) {
		tmpact = conc[ss][i];
		tmpdacts = 1.0;
		tmpdactv = 0.0;
		tmpdactp = 0.0;
	    }
	    else {

		if ( TRUE ) { /*methdata.defmodel != FULLCPL ) {*/
		    /*compute the terms needed*/
		    npow = Ctn[mat] * noni[i];
		    np2 = npow * npow;
		    tmp = (1.0 + np2);

		    /*compute some useful stuff*/
		    der = 1.0 / tmp;
		    der2 = -1.0 / (tmp * tmp);

		    tmpact = conc[ss][i] * der;
		    tmpdacts = der;
		    tmpdactv = 0.0;
		    tmpdactp = conc[ss][i] * der2 * 2.0 * np2 / Vt;
		}
		else {
		    /*compute the terms needed*/
		    npow = Ctn[mat] * noni[i];
		    np2 = npow * npow;
		    vac = conc[vs][i] / equil[vs][i];
		    tmp = (1.0 + vac * np2);

		    /*compute some useful stuff*/
		    der = 1.0 / tmp;
		    der2 = -1.0 / (tmp * tmp);

		    tmpact = conc[ss][i] * der;
		    tmpdacts = der;
		    tmpdactv = conc[ss][i] * der2 * np2 / equil[vs][i];
		    tmpdactp = conc[ss][i] * der2 * vac * np2 * 
				      ( 2.0 / Vt - dequ[vs][i] / equil[vs][i] );
		}
	    }
/*
	    act[ss][i] = tmpact;
	    dact[ss][ss][i] = tmpdacts;
	    dact[ss][vs][i] = tmpdactv;
	    dact[ss][ps][i] = tmpdactp;
*/

	    if ( INSULATE( nd[i]->mater ) ) {
	        act[ss][i] = tmpact;
	        dact[ss][ss][i] = tmpdacts;
	        dact[ss][vs][i] = tmpdactv;
	        dact[ss][ps][i] = tmpdactp;
	    }
	    else {
		if ( tmpact > Asss ) {
		    act[ss][i] = a + p * log( tmpact - bAsss );
		    dact[ss][ss][i] = f * p / (tmpact - bAsss) * tmpdacts;
		    dact[ss][vs][i] = f * p / (tmpact - bAsss) * tmpdactv;
		    dact[ss][ps][i] = f * p / (tmpact - bAsss) * tmpdactp;
	    	}
		else {
	    	    act[ss][i] = f * tmpact;
	    	    dact[ss][ss][i] = f * tmpdacts;
	    	    dact[ss][vs][i] = f * tmpdactv;
	    	    dact[ss][ps][i] = f * tmpdactp;
		}
	    }
	}
    }
} 




/************************************************************************
 *									*
 *	Asboundary - This routine handles the computation of the arsenic*
 *  boundary conditions at node i and solutions j and k.  The time step	*
 *  is delta and the temperature is temp.  Vel is the velocity of the	*
 *  boundary.								*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
Asboundary( bval )
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

    sol = imptosol[As];

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
 *	Arsenic( par, param ) - This routine gets the 			*
 *  arsenic diffusion parameters from the cards and sets them up.	*
 *  They are then used by the rest of the program.			*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
arsenic( par, param )
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
	SET_FLAGS(As, ACTIVE_ACCEPTOR);
	SET_FLAGS(Asa, ACTIVE_ACCEPTOR);
    }
    if ( Listed( "donor" ) ) {
	CLEAR_FLAGS(As, ACTIVE_ACCEPTOR);
	CLEAR_FLAGS(Asa, ACTIVE_ACCEPTOR);
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

    tmpfl = GET_FLAGS(As);			/* save state */
    for (; i <= j ; i++) {
	if ( i == 0 )	CLEAR_FLAGS(As, IMPLANTED_IMP);
	else		SET_FLAGS(As, IMPLANTED_IMP);
	Fetch( Dix0(mat) , "Dix.0" );
	Fetch( DixE(mat) , "Dix.E" );
	Fetch( Dim0(mat) , "Dim.0" );
	Fetch( DimE(mat) , "Dim.E" );
	Fetch( Fi(mat)   , "Fi" );
	Fetch( Ctn0(mat) , "Ctn.0" );
	Fetch( CtnE(mat) , "Ctn.E" );
	Fetch( CtnF(mat) , "Ctn.F" );
    }

    /* reset implanted source state */
    CLEAR_FLAGS(As, ALL_FLAGS);
    SET_FLAGS(As, tmpfl);

    /*deal with the solid solubility numbers*/
    if ( get_bool( param, "ss.clear") && is_specified( param, "ss.clear" )) {
	Asssmax = 0;
	Assstemp = 0.0;
	Assolid[0][0] = 0.0;
    }
    if ( is_specified( param, "ss.conc" ) && is_specified( param, "ss.temp" ) ) {
	t = get_float( param, "ss.temp" ) + 273.0;
	c = get_float( param, "ss.conc" );

	/*find the spot to store these at*/
	for(i = 0; (i < Asssmax) && (t > Assolid[0][i]); i++);

	/*is this guy equal?*/
	if ( t == Assolid[0][i] ) 
	    Assolid[1][i] = c;
	else {
	    /*move everyone after up a spot*/
	    for(j = Asssmax-1; j >= i; j--) {
		Assolid[0][j+1] = Assolid[0][j];
		Assolid[1][j+1] = Assolid[1][j];
	    }
	    Asssmax++;

	    /*insert the new one*/
	    Assolid[0][i] = t;
	    Assolid[1][i] = c;
	}
	Assstemp = 0.0;
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

