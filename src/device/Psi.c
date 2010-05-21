/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *************************************************************************/
/*   Psi.c                Version 5.2     */
/*   Last Modification : 7/3/91 15:44:14  */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "material.h"
#include "matrix.h"
#include "diffuse.h"
#include "device.h"

#   define fetch(N,V,S,A)\
    if (is_specified (param, N)) V = S*get_float (param, N) + A

/*definitions for impurity constants as a function of material*/
#define eps(M)	impur[Psi].constant[M][1][0] /*material relative permitivitty*/

/*definitions of the segregation coefficients as a function of material*/
#define Qss(M1, M2) 	impur[Psi].seg[SEG0][M1][M2]


/************************************************************************
 *									*
 *	Psiboundary - This routine handles the computation of potential	*
 *  boundary conditions.						*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
Psiboundary(bval)
struct bound_str *bval;
{
    int mat0 = bval->mat[0];
    int mat1 = bval->mat[1];
    int row0 = bval->loc[0][0];
    int row1 = bval->loc[1][1];
    int cp0 = bval->loc[0][1];
    int cp1 = bval->loc[1][0];
    double f;
    int Ps = imptosol[Psi];

    right_side( bval->loc[0][0], imptosol[Psi], bval->rhs, bval->cpl * Qss(mat0,mat1) );

    /*make sure the values are the same across materials*/
    f = 1.0e20 * bval->cpl;
    left_side(row0, Ps, f);
    a[Ps][Ps][cp1] -= f;
    left_side(row1, Ps, f);
    a[Ps][Ps][cp0] -= f;

    f *= (bval->conc[0] - bval->conc[1]);
    right_side(row0, Ps, bval->rhs, -f);
    right_side(row1, Ps, bval->rhs, f);

}





/************************************************************************
 *									*
 *	psi( par, param ) - This routine gets the potential parameters	*
 *  set up and initialized.						*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
psi_card( par, param )
char *par;
int param;
{ 
    int mat;
    int mat2 = -1;

    /*get the material number specified*/
    if ( get_bool( param, "silicon" ) )	mat = Si;
    if ( get_bool( param, "oxide" ) )	mat = SiO2;
    if ( get_bool( param, "oxynitride" ) )	mat = OxNi;
    if ( get_bool( param, "poly" ) )	mat = Poly;
    if ( get_bool( param, "nitride" ) )	mat = SiNi;
    if ( get_bool( param, "gas" ) )	mat = GAS;
    if ( get_bool( param, "aluminum" ) )	mat = Al;
    if ( get_bool( param, "gaas" ) )	mat = GaAs;

    /*fetch the values for each constant in this material*/
    fetch("eps", eps(mat), 8.85418e-14, 0.0);

    /*now fetch any segregation data that comes our way*/
    if (get_bool( param, "/silicon" ) && ( is_specified( param, "/silicon" )) )
	mat2 = Si;
    if (get_bool( param, "/oxide" )   && ( is_specified( param, "/oxide" )) )
	mat2 = SiO2;
    if (get_bool( param, "/oxynitride" )   && ( is_specified( param, "/oxynitride" )) )
	mat2 = OxNi;
    if (get_bool( param, "/poly" )    && ( is_specified( param, "/poly" )) )
	mat2 = Poly;
    if (get_bool( param, "/nitride" ) && ( is_specified( param, "/nitride" )) )
	mat2 = SiNi;
    if (get_bool( param, "/gas" )     && ( is_specified( param, "/gas" )) )
	mat2 = GAS;
    if (get_bool( param, "/gaas" )    && ( is_specified( param, "/gaas" )) )
	mat2 = GaAs;
    
    if (mat2 != -1) {
	fetch("Qss", Qss(mat,mat2)=Qss(mat2,mat), 1.0, 0.0);
    }
}



