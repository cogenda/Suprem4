h27609
s 00060/00027/00157
d D 2.1 93/02/03 15:10:36 suprem45 2 1
c Initial IV.GS release
e
s 00184/00000/00000
d D 1.1 93/02/03 15:08:33 suprem45 1 0
c date and time created 93/02/03 15:08:33 by suprem45
e
u
U
f e 0
t
T
I 1
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
implant( par, param )
char *par;
int param;
{ 
    register int i, j;
D 2
    int imp, ion, sol; 
E 2
I 2
    int imp, impa, ion, sol; 
E 2
    int isol, vsol;
    int damage;
    double dose, energy;
    int before[4], after[4];
    double ang;
    
    if( InvalidMeshCheck()) return -1;

    /*get the impurity number of the place to put the implant*/
D 2
    imp_select( param, &imp, &ion );
E 2
I 2
    if (imp_select( param, &imp, &ion ) == -1) return(-1);
E 2
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

    times(before);
    do_implant( ion, ang, dose, energy, damage, sol, isol, vsol );
    times(after);
    print_time("Implantation calculation", before, after);

    /*untranslate the mesh onto the new angle coordinates*/
    for(i = 0; i < np; i++) {
	for(j = 0; j < mode; j++) pt[i]->cord[j] = pt[i]->cordo[j];
    }

    /*initialize the active concentration*/
    switch(imp) {
D 2
    case As :  add_impurity( Asa, 1.0, -1 );
	       break;
    case Sb :  add_impurity( Sba, 1.0, -1 );
	       break;
    case B  :  add_impurity( Ba, 1.0, -1 );
	       break;
    case P  :  add_impurity( Pa, 1.0, -1 );
	       break;
E 2
I 2
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
E 2
    }

I 2
    /* mark the impurity as implanted */
    SET_FLAGS(imp, IMPLANTED_IMP);
    if (impa) {
	add_impurity( impa, 1.0, -1 );
	SET_FLAGS(impa, ACTIVE|IMPLANTED_IMP);
    }

E 2
    /*clean up the malloc space*/
    return(0);
}





/************************************************************************
 *									*
 *	imp_select( param ) - select sorts the solution position for the*
 *  implant card given.							*
 *									*
 *  Original:	MEL	12/85						*
I 2
 *  Mod 1:	SEH	02/92  Add GaAs elements			*
E 2
 *									*
 ************************************************************************/
imp_select( param, imp, ion )
int param;
int *imp;
int *ion;
{
D 2
    int ci, cs, as, p, sb, b, bf2;
E 2
I 2
    int	 i, gaas, si;
E 2

D 2
    /*get the boolean impurity indicators*/
    ci = get_bool( param, "silicon" );
    as = get_bool( param, "arsenic" );
    p  = get_bool( param, "phosphorus" );
    sb = get_bool( param, "antimony" );
    b  = get_bool( param, "boron" );
    bf2 = get_bool( param, "bf2" );
    cs = get_bool( param, "cesium" );

    if ( ci ) {
E 2
I 2
    if ( get_bool( param, "silicon" ) ) {
E 2
	*ion = *imp = I;
    }
D 2
    else if ( as ) {
E 2
I 2
    else if ( get_bool( param, "arsenic" ) ) {
E 2
	*ion = *imp = As;
    }
D 2
    else if ( p ) {
E 2
I 2
    else if ( get_bool( param, "phosphorus" ) ) {
E 2
	*ion = *imp = P;
    }
D 2
    else if ( sb ) {
E 2
I 2
    else if ( get_bool( param, "antimony" ) ) {
E 2
	*ion = *imp = Sb;
    }
D 2
    else if ( b ) {
E 2
I 2
    else if ( get_bool( param, "boron" ) ) {
E 2
	*ion = *imp = B;
    }
D 2
    else if ( bf2 ) {
E 2
I 2
    else if ( get_bool( param, "bf2" ) ) {
E 2
	*imp = B;
	*ion = BF2;
    }
D 2
    else if ( cs ) {
E 2
I 2
    else if ( get_bool( param, "cesium" ) ) {
E 2
	*ion = *imp = Cs;
    }
I 2
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
E 2

    /*then add a new one*/
    add_impurity( *imp, 1.0e+5, -1 );
I 2

    return(0);
E 2
}
E 1
