/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   predep.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:50:08  */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "material.h"
#include "diffuse.h"


/************************************************************************
 *									*
 *	predep( ) - This routine checks the gas specified and its type	*
 *  and sets up any initial conditions that may be needed.		*
 *									*
 *  Original:	MEL	6/85						*
 *									*
 ************************************************************************/
predep( )
{ 
    float conc , Oss();
    int i, j, ie;
    int imp, impa;

    /* clear all gas concentrations */
    for( i = 0; i < nn; i++ )
	if ( nd[i]->mater == GAS ) 
	    for( j = 0; j < n_imp; j++ ) 
		if ( IS_DIFFUSE( soltoimp[j] ) ) nd[i]->sol[j] = 0.1 * ABE[soltoimp[j]];

    /*worry about getting the gas right*/
    /*here are checks on all impurity types as they are added*/
    if ( (gas_type != GAS_B) && (gas_type != GAS_As) &&
	 (gas_type != GAS_Sb) && (gas_type != GAS_P) &&
	 (gas_type != GAS_DRYO2) && (gas_type != GAS_WETO2) &&
	 (gas_type != GAS_Be) && (gas_type != GAS_Mg) &&
	 (gas_type != GAS_Se) && (gas_type != GAS_Si) &&
	 (gas_type != GAS_Sn) && (gas_type != GAS_Ge) &&
	 (gas_type != GAS_Zn) && (gas_type != GAS_C) &&
	 (gas_type != GAS_G)
    ) return;

    /*for oxidation, always "predep" XVEL, YVEL*/
    if (gas_type == H2O || gas_type == O2) {
	add_impurity( XVEL, 0.0, -1); 
	add_impurity( YVEL, 0.0, -1);
	add_impurity( DELA, 0.0, -1);
    }

    /*but that's all, unless we're actually diffusing oxidant*/
    if ((gas_type == GAS_DRYO2 || gas_type == GAS_WETO2) &&
	(oxide_model == O_ANALYTIC || oxide_model == O_ERFG ||
	 oxide_model == O_ERF1 || oxide_model == O_ERF2))
	return;

    /*deduce impurity number from gas number: they are the same now*/
    imp = gas_type;

    /*
     *Some time in the misty future this line will read something like
     *conc = solid_solubility (impurity, material, partial_pressure)
     *Better still we will use a material dependent number in the loop.
     *Right now we only have a ss routine for oxidant.
     */
    if (imp == H2O || imp == O2)
	conc = Oss (imp, SiO2);
    else
	conc = gas_conc;

    /*make sure selected impurity is in (and complementary ones out)*/
    if (imp == H2O) lose_impurity( O2);
    if (imp == O2)  lose_impurity( H2O);

    add_impurity( imp, 1.0, -1);

    /*initialize the active concentration*/
    switch(imp) {
    case As  : impa = Asa;  break;
    case Sb  : impa = Sba;  break;
    case B   : impa = Ba;   break;
    case P   : impa = Pa;   break;
    case iBe : impa = iBea; break;
    case iMg : impa = iMga; break;
    case iSe : impa = iSea; break;
    case iSi : impa = iSia; break;
    case iSn : impa = iSna; break;
    case iGe : impa = iGea; break;
    case iZn : impa = iZna; break;
    case iC  : impa = iCa;  break;
    case iG  : impa = iGa;  break;
    default  : impa = 0;    break;
    }
    if (impa) {
	add_impurity( impa, 1.0, -1 );
	SET_FLAGS(impa, ACTIVE);
    }

    /*set the gas node concs*/
    for(ie = 0; ie < np; ie++) {
	if ( is_surf(ie) ) 
	    set_sol_nd( node_mat(nd_pt(ie,0), GAS), imptosol[imp], conc );
    }
    return;
}
