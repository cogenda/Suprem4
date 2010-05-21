/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   Phosphorus.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:49:47 */


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
#define Dix0(M)	impur[P].constant[M][1][IS_IMPLANTED(P)]
		/*exp constant with neutral V*/
#define DixE(M)	impur[P].constant[M][2][IS_IMPLANTED(P)]
		/*pre exp constant with neg V*/
#define Dim0(M)	impur[P].constant[M][3][IS_IMPLANTED(P)]
		/*exp constant with neg V*/
#define DimE(M)	impur[P].constant[M][4][IS_IMPLANTED(P)]
		/*pre exp constant with neg V*/
#define Did0(M)	impur[P].constant[M][5][IS_IMPLANTED(P)]
		/*exp constant with neg V*/
#define DidE(M)	impur[P].constant[M][6][IS_IMPLANTED(P)]
		/*pre exp constant with neutral V*/
#define Dvx0(M)	impur[P].constant[M][7][IS_IMPLANTED(P)]
		/*exp constant with neutral V*/
#define DvxE(M)	impur[P].constant[M][8][IS_IMPLANTED(P)]
		/*pre exp constant with neg V*/
#define Dvm0(M)	impur[P].constant[M][9][IS_IMPLANTED(P)]
		/*exp constant with neg V*/
#define DvmE(M)	impur[P].constant[M][10][IS_IMPLANTED(P)]
		/*pre exp constant with neg V*/
#define Dvd0(M)	impur[P].constant[M][11][IS_IMPLANTED(P)]
		/*exp constant with neg V*/
#define DvdE(M)	impur[P].constant[M][12][IS_IMPLANTED(P)]

/*definitions of the segregation coefficients as a function of material*/
#define Seg0(M1, M2) 	impur[P].seg[SEG0][M1][M2]
#define SegE(M1, M2) 	impur[P].seg[SEGE][M1][M2]
#define Trn0(M1, M2) 	impur[P].seg[TRN0][M1][M2]
#define TrnE(M1, M2) 	impur[P].seg[TRNE][M1][M2]

/*solid solubility data*/
static double Psolid[2][100];
static int Pssmax;
static double Psstemp = 0.0;


/************************************************************************
 *									*
 *	This file contains definitions of the Phosphorus routines.  It 	*
 *  includes the routine to calculate diffusion coefficients, the one	*
 *  to calculate any coupling terms between species, and the one to	*
 *  to calculate the boundary condition terms for the Phosphorus.	*
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
 *	Pdiff_coeff(temp, noni, inter, vacan) - This routine returns	*
 *  the phosphorus diffusivity as a function of temperature, n/ni, 	*
 *  interstitial and vacancy concentrations.				*
 *									*
 *  Original:	MEL	1/85						*
 *  Revised:	MEL	3/86	(multiple materials)			*
 *									*
 ************************************************************************/
double Pdiff_coeff(temp, new, equil, noni, idf, vdf, iprt, vprt)
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
    double Did[MAXMAT];
    double Dvx[MAXMAT];
    double Dvm[MAXMAT];
    double Dvd[MAXMAT];
    double Vt = kb * temp;

    /*first calculate the fixed stuff*/
    for(i = 0; i < MAXMAT; i++) {
	Dix[i] = Dix0(i) * exp ( - DixE(i) / (kb * temp) );
	Dim[i] = Dim0(i) * exp ( - DimE(i) / (kb * temp) );
	Did[i] = Did0(i) * exp ( - DidE(i) / (kb * temp) );
	Dvx[i] = Dvx0(i) * exp ( - DvxE(i) / (kb * temp) );
	Dvm[i] = Dvm0(i) * exp ( - DvmE(i) / (kb * temp) );
	Dvd[i] = Dvd0(i) * exp ( - DvdE(i) / (kb * temp) );
    }

    for(i = 0; i < nn; i++) {
	mat = nd[i]->mater;

	idf[i] = Dix[mat] + noni[i] * (noni[i] * Did[mat] + Dim[mat]);
	vdf[i] = Dvx[mat] + noni[i] * (noni[i] * Dvd[mat] + Dvm[mat]);

	vprt[i] = noni[i] * (2.0 * Dvd[mat] * noni[i] + Dvm[mat]) / Vt;
	iprt[i] = noni[i] * (2.0 * Did[mat] * noni[i] + Dim[mat]) / Vt;
    }

}





/************************************************************************
 *									*
 *	Pboundary - This routine handles the computation of the 	*
 *  phosphorus boundary conditions at the location and values passed.	*
 *  time step is delta and the temperature is temp.  Vel is the 	*
 *  velocity of the boundary.						*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
Pboundary( bval )
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

    sol = imptosol[P];

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





/************************************************************************
 *									*
 *	Pactive( conc, ans, temp ) - This routine calculates the 	*
 *  electrically active portion of the Arsenic concentration.		*
 *									*
 *  Original:	MEL	7/87						*
 *									*
 ************************************************************************/
Pactive( simple, nn, temp, conc, act, equil, noni )
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
    register int ss=imptosol[P];
    double f;
    double p, a, Pss, bPss;
    double b = 0.90;
    double tmp;

    /*compute the solid solubility at this temperature*/
    Psstemp = temp;
    for(i = 0; (i < Pssmax) && (Psolid[0][i] < temp); i++);

    /*if we are at the low end*/
    if ( i == 0 ) {
	tmp = (temp - Psolid[0][0]) / (Psolid[0][1] - Psolid[0][0]);
	Pss = tmp * (Psolid[1][1] - Psolid[1][0]) + Psolid[1][0];
    }
    else if ( i == Pssmax ) {
	j = Pssmax - 1;
	tmp = (temp - Psolid[0][j]) / (Psolid[0][j-1] - Psolid[0][j]);
	Pss = tmp * (Psolid[1][j-1] - Psolid[1][j]) + Psolid[1][j];
    }
    else {
	tmp = (temp - Psolid[0][i-1]) / (Psolid[0][i] - Psolid[0][i-1]);
	Pss = tmp * (Psolid[1][i] - Psolid[1][i-1]) + Psolid[1][i-1];
    }

    f = 1.0;
    p = Pss * (1.0 - b); 
    a = Pss - p * log( p );
    bPss = b * Pss;

    if ( !simple ) {
	for(i = 0; i < nn; i++) {

	    if ( INSULATE( nd[i]->mater ) ) {
		act[ss][i] = conc[ss][i];
		dact[ss][ss][i] = 1.0;
	    }
	    else {
		if ( conc[ss][i] > Pss ) {
		    act[ss][i] = a + p * log( conc[ss][i] - bPss );
		    dact[ss][ss][i] = f * p / (conc[ss][i] - bPss);
		    act[ss][i] *= f;
		}
		else {
		    act[ss][i] = f * conc[ss][i];
		    dact[ss][ss][i] = f;
		}
	    }
	}
    }
    else {
	for(i = 0; i < nn; i++) {

	    if ( INSULATE( nd[i]->mater ) ) {
		act[ss][i] = conc[ss][i];
	    }
	    else {
		if ( conc[ss][i] > Pss ) {
		    act[ss][i] = a + p * log( conc[ss][i] - bPss );
		    act[ss][i] *= f;
		}
		else {
		    act[ss][i] = f * conc[ss][i];
		}
	    }
	}
    }
}






/************************************************************************
 *									*
 *	Phosphorus( par, param ) - This routine gets the 		*
 *  Phosphorus diffusion parameters from the cards and sets them up.	*
 *  They are then used by the rest of the program.			*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
phosphorus( par, param )
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
	SET_FLAGS(P, ACTIVE_ACCEPTOR);
	SET_FLAGS(Pa, ACTIVE_ACCEPTOR);
    }
    if ( Listed( "donor" ) ) {
	CLEAR_FLAGS(P, ACTIVE_ACCEPTOR);
	CLEAR_FLAGS(Pa, ACTIVE_ACCEPTOR);
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

    tmpfl = GET_FLAGS(P);			/* save state */
    for (; i <= j ; i++) {
	if ( i == 0 )	CLEAR_FLAGS(P, IMPLANTED_IMP);
	else		SET_FLAGS(P, IMPLANTED_IMP);
	Fetch( Dix0(mat) , "Dix.0" );
	Fetch( DixE(mat) , "Dix.E" );
	Fetch( Dim0(mat) , "Dim.0" );
	Fetch( DimE(mat) , "Dim.E" );
	Fetch( Did0(mat) , "Dimm.0" );
	Fetch( DidE(mat) , "Dimm.E" );
	Fetch( Dvx0(mat) , "Dvx.0" );
	Fetch( DvxE(mat) , "Dvx.E" );
	Fetch( Dvm0(mat) , "Dvm.0" );
	Fetch( DvmE(mat) , "Dvm.E" );
	Fetch( Dvd0(mat) , "Dvmm.0" );
	Fetch( DvdE(mat) , "Dvmm.E" );
    }

    /* reset implanted source state */
    CLEAR_FLAGS(P, ALL_FLAGS);
    SET_FLAGS(P, tmpfl);

    /*deal with the solid solubility numbers*/
    if ( get_bool( param, "ss.clear") && is_specified( param, "ss.clear" )) {
	Pssmax = 0;
	Psstemp = 0.0;
	Psolid[0][0] = 0.0;
    }
    if ( is_specified( param, "ss.conc" ) && is_specified( param, "ss.temp" ) ) {
	t = get_float( param, "ss.temp" ) + 273.0;
	c = get_float( param, "ss.conc" );

	/*find the spot to store these at*/
	for(i = 0; (i < Pssmax) && (t > Psolid[0][i]); i++);

	/*is this guy equal?*/
	if ( t == Psolid[0][i] ) 
	    Psolid[1][i] = c;
	else {
	    /*move everyone after up a spot*/
	    for(j = Pssmax-1; j >= i; j--) {
		Psolid[0][j+1] = Psolid[0][j];
		Psolid[1][j+1] = Psolid[1][j];
	    }
	    Pssmax++;

	    /*insert the new one*/
	    Psolid[0][i] = t;
	    Psolid[1][i] = c;
	}
	Psstemp = 0.0;
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
