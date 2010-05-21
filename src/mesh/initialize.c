/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *************************************************************************/
/*   initialize.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:30:48  */

#include <stdio.h>
#include <math.h>
#include <global.h>
#include <constant.h>
#include "dbaccess.h"
#include <impurity.h>	/* For B, P, Sb, As */
#include <material.h>	/* For B, P, Sb, As */




/************************************************************************
 *									*
 *	initialize( par, param ) - This routine allows an initial read	*
 *  of a mesh or can generate a rectangular one.  It also reads the	*
 *  background concentration and the wafer orientation.			*
 *									*
 *  Original :	MEL	Apr, 1986					*
 *									*
 ************************************************************************/
initialize( par, param )
char *par;
int param;
{
    int ier;
    int lflip;
    int by_nd, impa;
    float scale;
    double conc, ivratio;
    int imp = -1;
    char *infile;

    /*get the type of mesh to be read, and the filename*/
    infile = get_string( param, "infile" );
    conc = get_float( param, "conc" );
    if (is_specified(param,"boron")     &&get_bool(param, "boron"))     imp=B;
    if (is_specified(param,"antimony")  &&get_bool(param, "antimony"))  imp=Sb;
    if (is_specified(param,"arsenic")   &&get_bool(param, "arsenic"))   imp=As;
    if (is_specified(param,"phosphorus")&&get_bool(param,"phosphorus")) imp=P;
    if (is_specified(param,"gold")      &&get_bool(param,"gold"))       imp=Au;
    if (is_specified(param,"gallium")   &&get_bool(param,"gallium"))    imp=Ga;
    if (is_specified(param,"beryllium") &&get_bool(param,"beryllium"))	imp=iBe;
    if (is_specified(param,"magnesium") &&get_bool(param,"magnesium"))	imp=iMg;
    if (is_specified(param,"selenium")  &&get_bool(param,"selenium"))	imp=iSe;
    if (is_specified(param,"isilicon")  &&get_bool(param,"isilicon"))	imp=iSi;
    if (is_specified(param,"tin")       &&get_bool(param,"tin"))	imp=iSn;
    if (is_specified(param,"germanium") &&get_bool(param,"germanium"))	imp=iGe;
    if (is_specified(param,"zinc")      &&get_bool(param,"zinc"))	imp=iZn;
    if (is_specified(param,"carbon")    &&get_bool(param,"carbon"))	imp=iC;
    if (is_specified(param,"generic")   &&get_bool(param,"generic"))	imp=iG;
    ivratio = is_specified( param, "interval.r")
	      ? get_float( param, "interval.r")
	      : 0;
    /* Get substrate orientation - parser guarantees one specified */
    switch( get_int( param, "orientation")) {
	case 100: sub_ornt = 0; break;
	case 110: sub_ornt = 1; break;
	case 111: sub_ornt = 2; break;
	default: fprintf( stderr, "Parser error\n");
    }
    /* See if the mesh modification parameters are given */
    lflip = get_bool(param, "flip.y");
    scale = get_float( param, "scale");

    /*do we have an structure file input? */
    if ( infile ) {
	if ((ier = ig2_read(infile, lflip, scale)) < 0) return(ier);
	by_nd = nn == 0;
    }
    else {
	/* At this point we must already have the lines, regions, bounds .*/
	if ((ier = squares( ivratio)) < 0) return(ier);
	by_nd = TRUE;
    }
    create_db( by_nd );

    /* create the pointers for the new impurity */
    /*if we got to n_imp, then add a new one*/
    if ( imp != -1 ) {
	switch( imp ) {
	case As  : impa = Asa;
		   break;
	case Sb  : impa = Sba;
		   break;
	case B   : impa = Ba;
		   break;
	case P   : impa = Pa;
		   break;
	case iBe : impa = iBea;
		   break;
	case iMg : impa = iMga;
		   break;
	case iSe : impa = iSea;
		   break;
	case iSi : impa = iSia;
		   break;
	case iSn : impa = iSna;
		   break;
	case iGe : impa = iGea;
		   break;
	case iZn : impa = iZna;
		   break;
	case iC  : impa = iCa;
		   break;
	case iG  : impa = iGa;
		   break;
	default  : impa = 0;
		   break;
	}
	add_impurity( imp, conc, -1);
	if (impa) {
	    add_impurity(impa, conc, -1);
	    SET_FLAGS(impa, ACTIVE);
	}
    }
    return(0);
}
