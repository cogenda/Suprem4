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
/*   implant.c                Version 5.1     */
/*   Last Modification : 7/3/91 12:08:47 */

#include <stdio.h>
#include <math.h>
#include <sys/times.h>

#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "material.h"
#include "implant.h"
#include "matrix.h"
#include "defect.h"
#include "expr.h"

/************************************************************************
 *									*
 *	implant( par, param ) - Implant calculates a simple gaussian 	*
 *  or a pearson model of an implant.					*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
int implant( char * par, int param )
{
    register int i, j;
    int imp, impa, ion, sol;
    int isol, vsol;
    int damage;
    double dose, energy;
    struct tms before, after;
    double ang;

    if( InvalidMeshCheck()) return -1;

    /*get the impurity number of the place to put the implant*/
    if (imp_select( param, &imp, &ion ) == -1) return(-1);
    sol = imptosol[imp];

    /*get the main descriptive values for the implant*/
    dose     = get_float( param, "dose" );
    energy    = get_float( param, "energy" );

    /*get the model type for this implant*/
    if ( get_bool( param, "pearson" ) )
        imp_model = PEARS;
    else if ( get_bool( param, "gaussian" ) )
	imp_model = GAUSS;

    /*check for override of the model values*/
    if ( is_specified( param, "range" ) && is_specified( param, "std.dev" ) ) {
	override = TRUE;
	Rp     = get_float( param, "range" );
	delRp  = get_float( param, "std.dev" );
	Rgam   = get_float( param, "gamma" );
	Rkurt  = get_float( param, "kurtosis" );
    }
    else override = FALSE;

    /*do a damage calculation??*/
    if ( is_specified( param, "damage" ) && get_bool( param, "damage" ) )
	damage = TRUE;
    else
	damage = FALSE;

    /*check out the angle of the implant*/
    if ( is_specified( param, "angle" ) )
	ang = get_float( param, "angle" ) * PI / 180.0;
    else
	ang = 0.0;

    /*get defects created*/
    if ( damage ) {

	if ( last_temp != 0.0 )
	    init_pseudo( last_temp );
	else
	    init_pseudo( 1173.0 );
	get_defaults( NULL );

	isol = imptosol[I];
	vsol = imptosol[V];
	damage_read = TRUE;
    }

    times(&before);
    do_implant( ion, ang, dose, energy, damage, sol, isol, vsol );
    times(&after);
    print_time("Implantation calculation", &before, &after);

    /*untranslate the mesh onto the new angle coordinates*/
    for(i = 0; i < np; i++) {
	for(j = 0; j < mode; j++) pt[i]->cord[j] = pt[i]->cordo[j];
    }

    /*initialize the active concentration*/
    switch(imp) {
    case As  :  impa = Asa;  break;
    case Sb  :  impa = Sba;  break;
    case B   :  impa = Ba;   break;
    case P   :  impa = Pa;   break;
    case iBe :  impa = iBea; break;
    case iMg :  impa = iMga; break;
    case iSe :  impa = iSea; break;
    case iSi :  impa = iSia; break;
    case iSn :  impa = iSna; break;
    case iGe :  impa = iGea; break;
    case iZn :  impa = iZna; break;
    case iG  :  impa = iGa;  break;
    default  :  impa = 0;    break;
    }

    /* mark the impurity as implanted */
    SET_FLAGS(imp, IMPLANTED_IMP);
    if (impa) {
	add_impurity( impa, 1.0, -1 );
	SET_FLAGS(impa, ACTIVE|IMPLANTED_IMP);
    }

    /*clean up the malloc space*/
    return(0);
}





/************************************************************************
 *									*
 *	imp_select( param ) - select sorts the solution position for the*
 *  implant card given.							*
 *									*
 *  Original:	MEL	12/85						*
 *  Mod 1:	SEH	02/92  Add GaAs elements			*
 *									*
 ************************************************************************/
int imp_select( int param, int *imp, int *ion )
{
    int	 i, gaas, si;

    if ( get_bool( param, "silicon" ) ) {
	*ion = *imp = I;
    }
    else if ( get_bool( param, "arsenic" ) ) {
	*ion = *imp = As;
    }
    else if ( get_bool( param, "phosphorus" ) ) {
	*ion = *imp = P;
    }
    else if ( get_bool( param, "antimony" ) ) {
	*ion = *imp = Sb;
    }
    else if ( get_bool( param, "boron" ) ) {
	*ion = *imp = B;
    }
    else if ( get_bool( param, "bf2" ) ) {
	*imp = B;
	*ion = BF2;
    }
    else if ( get_bool( param, "cesium" ) ) {
	*ion = *imp = Cs;
    }
    else if ( get_bool( param, "beryllium" ) ) {
	*ion = *imp = iBe;
    }
    else if ( get_bool( param, "magnesium" ) ) {
	*ion = *imp = iMg;
    }
    else if ( get_bool( param, "selenium" ) ) {
	*ion = *imp = iSe;
    }
    else if ( get_bool( param, "isilicon" ) ) {
	*ion = *imp = iSi;
    }
    else if ( get_bool( param, "tin" ) ) {
	*ion = *imp = iSn;
    }
    else if ( get_bool( param, "germanium" ) ) {
	*ion = *imp = iGe;
    }
    else if ( get_bool( param, "zinc" ) ) {
	*ion = *imp = iZn;
    }
    else if ( get_bool( param, "carbon" ) ) {
	*ion = *imp = iC;
    }
    else if ( get_bool( param, "generic" ) ) {
	*ion = *imp = iG;
    }

    /*then add a new one*/
    add_impurity( *imp, 1.0e+5, -1 );

    return(0);
}
