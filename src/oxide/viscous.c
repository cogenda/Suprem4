/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   viscous.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:52:44 */

#include "global.h"
#include "constant.h"
#include "geom.h"
#undef EXTERN
#define EXTERN			/* FEgeom isn't included in main.c */
#include "FEgeom.h"
#undef EXTERN
#define EXTERN extern
#include "material.h"
#include "FEmath.h"
#include <assert.h>
#include <stdio.h>

/* By arrangement with FEconvert.c */
#define TRIOX	0
#define ELAST6  1
#define ELAST7  2
#define DUMMY   3

/* another hack by Zak */
#define NEL 7
extern int noop();
extern int write_problem(), read_problem();
extern int triox_stiff(), triox_bc(), triox_nodal_stress();
extern int FE_oxbulk(), sup4_ecoeff();
extern int tri6_stiff(), tri6_Snodal_stress();
extern int tri7_stiff(), tri7_nodal_stress();
extern int oxload();


/************************************************************************
 *									*
 *	visco_growth()	- Controller for the viscous flow model		*
 *									*
 ************************************************************************/
float proc_temp;

visco_growth( temp, dt)
    float temp;
    double dt;
{
    proc_temp = temp;

    /*Generate appropriate data structures for the FE method*/
    if(FEconvert( TRIOX, 3) < 0) return;

    /* Boundary conditions */
    FEoxbc();

    /* Call the FE solver on the oxide problem */
    FEmathset( 1);
    FEsolve( verbose>= V_CHAT );

    /* Store the displacements and stresses in S4's data structure */
    FE2s4ox();

    if( SilStress) {
	/* Postprocessing for fun and profit: do silicon stress calc */
	/* New BC's */
	FEdf = 2;
	FEsilbc();

	/* New math */
	FEmathset( 0);

	/* and away we go. */
	FEsolve( verbose >= V_CHAT);

	/* Store silicon stresses in S4 data structure. */
	FE2s4sil();
    }

    /* Later */
    FEfree();
    return;
}


/*-----------------Stress_Analysis--------------------------------------
 * Thermal elastic stresses.
 *----------------------------------------------------------------------*/
stress_analysis( par, param)
     char *par;
     int param;
{
    float temp1=1000, temp2=1000;
    int element;

    if( InvalidMeshCheck()) return -1;

    if (is_specified( param, "temp1")) temp1 = 273+get_float( param, "temp1");
    if (is_specified( param, "temp2")) temp2 = 273+get_float( param, "temp2");

    ThermSig( temp1, temp2);
    AddIntSig();
    
    if (!is_specified( param, "nel"))     element = ELAST6;
    else if (get_int( param, "nel") == 6) element = ELAST6;
    else if (get_int( param, "nel") == 7) element = ELAST7;
    else {
	fprintf( stderr, "Only know 6 & 7 noded elements\n");
	return(-1);
    }

    /* fixed by Zak to ensure that data structures are initialized
     *  for the case of Stress calculations without any previous
     *  stress-dependent oxidation.
     */    
    FEdesc[0].nel = NEL;
    FEdesc[0].gaussp = 3;
    FEdesc[0].stiff = triox_stiff;
    FEdesc[0].coeff = FE_oxbulk;
    FEdesc[0].bound = triox_bc;
    FEdesc[0].nodal_stress = triox_nodal_stress;

    FEdesc[1].nel = 6;
    FEdesc[1].gaussp = 3;
    FEdesc[1].stiff = tri6_stiff;
    FEdesc[1].coeff = sup4_ecoeff;
    FEdesc[1].bound = noop;
    FEdesc[1].nodal_stress = tri6_Snodal_stress;

    FEdesc[2].nel = 7;
    FEdesc[2].gaussp = 3;
    FEdesc[2].stiff = tri7_stiff;
    FEdesc[2].coeff = sup4_ecoeff;
    FEdesc[2].bound = noop;
    FEdesc[2].nodal_stress = tri7_nodal_stress;

    FEdesc[3].nel = NEL;
    FEdesc[3].gaussp = 0;
    FEdesc[3].stiff = NULL;
    FEdesc[3].coeff = NULL;
    FEdesc[3].bound = NULL;
    FEdesc[3].nodal_stress = NULL;

    FEdesc[4].nel = 7;
    FEdesc[4].gaussp = 3;
    FEdesc[4].stiff = tri7_stiff;
    FEdesc[4].coeff = sup4_ecoeff;
    FEdesc[4].bound = oxload;
    FEdesc[4].nodal_stress = tri7_nodal_stress;

    /* Generate appropriate data structures for the FE method */
    if(FEconvert( element, 2)<0) return(-1);

    /*
     * Set up boundary conditions.
     */
    FEbc( temp1, par, param);
	

    /* Call the FE solver */
    FEmathset( 0);
    FEsolve( verbose >= V_CHAT );

    /*store the displacements and stresses somewhere permanent*/
    FE2s4all();
    
    FEfree();
    return(0);
}

FEmathset( flow)
    int flow;			/* Doing flow or stress calc? */
{
    float Oss(), Ovel();
    static double dummy[]={0.0,1.0};
    
    /* Use B/A to scale velocities */
    if( flow)
	FEabe[0] = FEabe[1] = Ovel( proc_temp, gas_type, Oss( gas_type, SiO2), dummy, 1.0);
    else
	FEabe[0] = FEabe[1] = 1.0;
    
    /* Use gas boundary condition to scale concentrations */
    if( flow)
	FEabe[2] = Oss( gas_type, SiO2);
    
    /* Boundary rows make flow problem asymmetric. */
    if( !flow)	FEsymm = 1;
    else	FEsymm = 0;

    /* Force 1 loop solution for linear problems */
    if( stress_dep) FEnonloop = 250;
    else	    FEnonloop = 1;

    /* Direct solution, no iterative nonsense. */
    FElinmeth = F_FULL;

    /* Do some sort of minimum degree order and reset things when done */
    FEorder = O_MF | O_RESETORDER;

    /* No accuracy checks on linear solution */
    FEcheck = 0;
}


