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
/*   contour.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:38:46 */

#include <stdio.h>
#include <math.h>
#ifdef CONVEX
#include <strings.h>
#else
#include <string.h>
#endif
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "plot.h"
#include "material.h"


/************************************************************************
 *									*
 *	contour( par, param ) - This routine draws contour lines for 	*
 *  in the selected variable.						*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
contour( par, param )
char *par;
int param;
{
    float value;
    float arr[MAXVRT][3], p[2][3];
    int t,i,nx;
    int line_type;
    int deb;
    int symb;


    if ( InvalidMeshCheck()) return( -1);
    if( znn != nn ) {
	fprintf(stderr, "Z variable is out of date w/r to mesh");
	return( -1);
    }

    if ( mode == ONED ) {
	fprintf(stderr, "Can't do contouring in one dimension\n");
	return( - 1);
    }

    deb = pl_debug;
    pl_debug = FALSE;


    value = get_float( param, "value" );
    line_type = get_int(param, "line.type");
    if ( is_specified( param, "symb" ) ) {
	symb = get_int(param, "symb");
    }
    else symb = -1;

    xgNewSet();
    if (sel_log)
	xgSetValue(exp(log(10.0)*value));
    else
	xgSetValue(value);
    if ( symb != -1 ) xgSetMark(symb);

    /*repeat for all the triangles*/
    for( t = 0; t < ne; t++ ) {
	if (!leaf (tri[t])) continue;

	/*repeat for each triangle vertex*/
	for( i = 0; i < 3; i++ ) {
	    nx = tri[t]->nd[i];

	    /*get the x and y coordinates*/
	    arr[i][0] = pt[ nd[nx]->pt ]->cord[0];
	    arr[i][1] = pt[ nd[nx]->pt ]->cord[1];

	    /*get the z coordinate*/
	    arr[i][2] = z[ nx ];
	}

	/*now that we have all that done, calculate the line*/
	if ( intersect(arr, value, p) == 2 ) {
	    xgNewGroup();
	    xgPoint( p[0][0], p[0][1] );
	    xgPoint( p[1][0], p[1][1] );
	}
    }
    xgUpdate( FALSE );
    pl_debug = deb;
    return(0);
}
