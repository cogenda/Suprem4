/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *************************************************************************/
/*   Electron.c                Version 5.2     */
/*   Last Modification : 7/3/91 15:44:10  */


#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "material.h"
#include "device.h"
#include "matrix.h"
#include "diffuse.h"


/************************************************************************
 *									*
 *	Nmobil(temp, noni, inter, vacan) - This routine calculates	*
 *  the interstitial diffusivity given the temperature, n/ni, and	*
 *  local defect concentrations.					*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
double Nmobil()
{
    return(0.0);
}



/************************************************************************
 *									*
 *	Ncoupling( ) - This routine computes the recombination of 	*
 *  interstitials with the other species.				*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
Ncoupling( temp, area, new, equil, dequ, rhs )
float temp;
double *area;		/*the nodal areas*/
double **new;		/*all the concentrations*/
double **equil;		/*the equilibrium concentration*/
double **dequ;		/*the derivitive of equilibrium concentration*/
double **rhs;		/*the current right hand side*/
{
}



/************************************************************************
 *									*
 *	Nboundary - This routine handles the computation of electrons	*
 *  boundary conditions at node i and solutions j and k.  The time step	*
 *  is delta and the temperature is temp.  Vel is the velocity of the	*
 *  boundary.								*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
Nboundary( bval )
struct bound_str *bval;
{
}


	
/************************************************************************
 *									*
 *	Phi_n( ) - This routine computes the quasi-fermi level and	*
 *  derivatives given the potential, material, and electron 		*	
 *  concentration							*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
double Phi_n( elec, psi, ec, mat, dqdp, dqde )
double elec, psi, ec;
int mat;
double *dqdp, *dqde;
{
    double qfn;

    if ( IS_SEMI( mat ) ) {
	qfn = - log( elec / Ncon(mat) ) * devVt + psi - ec;
	*dqdp = 1.0;
	*dqde = - devVt / elec;
    }
    else {
	qfn = 0.0;
	*dqdp = 0.0; 
	*dqde = 0.0;
    }

    return( qfn );
}




/************************************************************************
 *									*
 *	Electron( par, param ) - This routine gets the Electron 	*
 *  parameters from the cards and sets them up. 			*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
electron( par, param )
char *par;
int param;
{ 
    int mat, mat2 = -1;

#   define fetch( V, S, F ) if (is_specified(param,S)) V=F*get_float(param, S)
#   define listed( S ) ( get_bool(param,S) && is_specified(param,S) )

    /*get the material number specified*/
    if ( get_bool( param, "silicon" ) )	mat = Si;
    if ( get_bool( param, "oxide" ) )	mat = SiO2;
    if ( get_bool( param, "oxynitride" ) )	mat = OxNi;
    if ( get_bool( param, "poly" ) )	mat = Poly;
    if ( get_bool( param, "nitride" ) )	mat = SiNi;
    if ( get_bool( param, "gaas" ) )	mat = GaAs;
    if ( get_bool( param, "gas" ) )	mat = GAS;

    /*get the interface material if given*/
    /*
    if ( listed( "/silicon" ) )		mat2 = Si;
    if ( listed( "/oxide" ) )		mat2 = SiO2;
    if ( listed( "/oxynitride" ) )	mat2 = OxNi;
    if ( listed( "/nitride" ) )		mat2 = SiNi;
    if ( listed( "/poly" ) )		mat2 = Poly;
    if ( listed( "/gaas" ) )		mat2 = GaAs;
    if ( listed( "/gas" ) )		mat2 = GAS;
    */

    fetch( Econ(mat), "Ec", 1.0 );
    fetch( Ncon(mat), "Nc", 1.0 );

    /*if no second material listed, no interfce parameters can be*/
    if ( mat2 != -1 ) {
    }
}


