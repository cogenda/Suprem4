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
/*   diffuse.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:50:02 */

#include <stdio.h>
#include <sys/times.h>
#include <math.h>

#include "global.h"
#include "constant.h"
#include "geom.h"
#include "diffuse.h"
#include "defect.h"
#include "matrix.h"
#include "impurity.h"
#include "material.h"
#include "expr.h"


/************************************************************************
 *									*
 *	diffuse( par, param ) - diffuse solves the partial differential *
 *  equations associated with the diffusion process.     		*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
int diffuse( char *par, int param )
{
    float time, temp, cs;
    struct tms before, after;
    int cont;
    int dump;
    int r;
    char *movie;

    if( InvalidMeshCheck()) return( -1);

    /*change the solution flags from the default configuration*/
    CLEAR_FLAGS(Psi, ALL_FLAGS);
    SET_FLAGS(Psi, (PSEUDO | MOBILE | DIFFUSING | STEADY | LOCKSTEP) );

    time = get_float( param, "time" ) * 60;		/*convert to seconds*/
    last_temp = temp = get_float( param, "temp" ) + 273; 	/*convert to Kelvin*/
    cont = is_specified( param, "cont" ) && get_bool( param, "cont" );
    if (get_bool (param, "argon")) gas_type = GAS_ARGON;
    if (get_bool (param, "nitrogen")) gas_type = GAS_ARGON;
    if (get_bool (param, "ammonia")) gas_type = GAS_ARGON;
    if (get_bool (param, "dryo2")) gas_type = GAS_DRYO2;
    if (get_bool (param, "weto2")) gas_type = GAS_WETO2;
    if (get_bool (param, "antimony")) gas_type = GAS_Sb;
    if (get_bool (param, "arsenic")) gas_type = GAS_As;
    if (get_bool (param, "boron")) gas_type = GAS_B;
    if (get_bool (param, "phosphorus")) gas_type = GAS_P;
    if (get_bool (param, "beryllium")) gas_type = GAS_Be;
    if (get_bool (param, "magnesium")) gas_type = GAS_Mg;
    if (get_bool (param, "selenium")) gas_type = GAS_Se;
    if (get_bool (param, "isilicon")) gas_type = GAS_Si;
    if (get_bool (param, "tin")) gas_type = GAS_Sn;
    if (get_bool (param, "germanium")) gas_type = GAS_Ge;
    if (get_bool (param, "zinc")) gas_type = GAS_Zn;
    if (get_bool (param, "carbon")) gas_type = GAS_C;
    if (get_bool (param, "generic")) gas_type = GAS_G;

    if (is_specified( param, "solid.sol"))
	gas_conc = -1.0;
    else
	gas_conc = get_float( param, "gas.conc" );
    pressure = get_float( param, "pressure");

    dump = get_int (param, "dump");
    movie = get_string( param, "movie" );

    /*if we are oxidizing the wafer, drop down a initial layer to work on*/
    if ( oxidizing ) {

	if (!cont) {
	    /* Reset the 1d thickness. */
	    t1d = tinit;

	    /* Add a thin oxide layer to any exposed silicon surfaces */
	    NativeOxide(tinit);
	    geom("after native oxide deposit");
	}
    }
    else {
	/*check for polysilicon*/
	for(r = 0; (r < nreg) && (mat_reg(r) != Poly); r++);

	/*no poly silicon*/
	if ( r == nreg ) {
	    lose_impurity( XVEL);	lose_impurity( YVEL);   lose_impurity( DELA);
	}
	lose_impurity( O2);	lose_impurity( H2O);
	lose_impurity( Sxx);	lose_impurity( Sxy);	lose_impurity( Syy);
    }

    if ( is_specified( param, "gold.surf" ) ) {
	cs = get_float( param, "gold.surf" );
	add_impurity( Au, cs, GAS);
    }

    /*check for any predep cases that are necessary*/
    predep();

    if (!cont) {
	total = 0.0;

	/*compute temperature dependent material stuff*/
	comp_mat( temp );

	/*initialize defects to equilibrium values as a function of temp*/
	init_pseudo( temp );
    }
    else {
	methdata.init_time = 0.0;
    }

    /*initialize temperature dependent material stuff*/
    comp_mat( temp );

    /*initialize defects to equilibrium values as a function of temp*/
    init_pseudo( temp );

    /*initialize the diffusion code*/
    init_diffuse();

    if ( methdata.defmodel == FULLCPL ) {
	printf("***************************************************************\n");
	printf("*                                                             *\n");
	printf("*  Parameter extraction has not been completed for the full   *\n");
	printf("* coupling model.  This model should be considered a research *\n");
	printf("* tool only.                                                  *\n");
	printf("*                                                             *\n");
	printf("***************************************************************\n");
    }

    times(&before);
    solve_time( time, temp, methdata.init_time, methdata.timemeth,
		dump, movie, cont);
    times(&after);
    print_time("total diffusion time", &before, &after);
    damage_read=FALSE;

    return(0);
}



