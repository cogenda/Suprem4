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
/*   plot_1d.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:39:01 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "plot.h"
#include "material.h"

/************************************************************************
 *									*
 *	plot_1d( par, param ) - this routine is the father of the 	*
 *  plot_1d software.  It calls appropriate routines to set up 		*
 *  and do plots.							*
 *									*
 *  Original:	MEL	1/85	(modeled on pisces2)			*
 *									*
 ************************************************************************/
plot_1d( par, param )
char *par;
int param;
{
    int boundary;
    int line_type;
    int clear;
    float x;
    float y;
    float val;
    int i;
    float txmin, txmax;
    float tymin, tymax;
    struct d_str *data;
    int count;
    int ptype;
    int symb = -1;
    char *xlab;
    int mat1, mat2, byarc;
    char pbuf[120];
    int deb;
    int axis;

    if ( InvalidMeshCheck()) return( -1);
    if( znn != nn ) {
	fprintf(stderr, "Z variable is out of date w/r to mesh");
	return( -1);
    }

    data = salloc( struct d_str, 2*ne );
    deb = pl_debug;
    pl_debug = FALSE;

    /*
     * --- Read parameters --------------------
     */
    boundary  = get_bool(param, "boundary");
    clear  = get_bool(param, "clear");
    axis  = get_bool(param, "axis");
    line_type = get_int(param, "line.type");
    if ( is_specified( param, "symb" ) ) symb = get_int(param, "symb");
    x         = get_float(param, "x.value") * 1e-4;
    y         = get_float(param, "y.value") * 1e-4;
    if ( is_specified( param, "x" ) ) {
	ptype = XSEC;
	val = x;
    }
    else if ( is_specified( param, "y" ) || (mode == ONED) ) {
	ptype = YSEC;
	val = (mode == ONED)?0.0:y;
    }
    else {
	ptype = BND;
	mat1 = ChosenMater( par, param, 0);
	mat2 = ChosenMater( par, param, 1);
	if( mat2 < 0) mat2 = ChosenBC( par, param, 1);
	if( mat2 == -1+BC_OFFSET)
	    mat2 = BC_OFFSET + get_int( param, "/code");
    }
    byarc = get_int( param, "arclength");

    /*
     * --- get the actual data --------------------
     */
    count = do_1d( ptype, val, data, mat1, mat2, byarc);


    /*
     * --- Decide on plot bounds, starting with the device limits ----------
     */

    dev_lmts(&txmin, &txmax, &tymin, &tymax);

    /*figure out which is actually going to be needed*/
    if ( ptype == XSEC ) {
	txmin = tymin;
	txmax = tymax;
	xlab = "y in microns";
    }
    else
	xlab = "x in microns";

    /*Check those bounds really do cover the data values*/
    if( data[0].x       < txmin) txmin = data[0].x;
    if( data[count-1].x < txmin) txmin = data[count-1].x;
    if( data[0].x       > txmax) txmax = data[0].x;
    if( data[count-1].x > txmax) txmax = data[count-1].x;

    /*get the device max in the z direction*/
    for( tymax = tymin = z[0], i = 1; i < nn; i++) {
	if (z[i] > tymax) tymax = z[i];
	if (z[i] < tymin) tymin = z[i];
    }

    /*check the overide variables from the user*/
    if ( is_specified(param, "x.max") )
	txmax = get_float( param, "x.max" ) * 1e-4;
    if ( is_specified(param, "x.min") )
	txmin = get_float( param, "x.min" ) * 1e-4;
    if ( is_specified(param, "y.max") )
	tymax = get_float( param, "y.max" );
    if ( is_specified(param, "y.min") )
	tymin = get_float( param, "y.min" );

    /*watch out for brain damage*/
    CheckBound( &txmin, &txmax);
    CheckBound( &tymin, &tymax);

    /*
     * --- Initialize graphics --------------------
     */
   if (clear) xgClear();


    yflip = FALSE;
    /*set the labels*/
    xgAxisLabels( xlab, label, title );
    xgSetScale(1.0e4, 1.0);

    /*x and y axis log or linear*/
    xgLogAxis(FALSE, sel_log);
    if ( axis )
	xgSetBounds(txmin*1.0e4, txmax*1.0e4, tymin, tymax);

    if (boundary) {
	if ( ptype == XSEC)
	    mat_edges( TRUE, x, tymin, tymax );
	else if (ptype == YSEC)
	    mat_edges( FALSE, y, tymin, tymax );
    }

    /*
     * --- Do some drawing at last --------------------
     */
    xgNewSet();
    xgSetName(label);
    if ( symb != -1 ) xgSetMark(symb);

    for(i = 0; i < count; i++)
	xgPoint(data[i].x, data[i].y);

    xgUpdate(0);
    pl_debug=deb;

    free(data);
    return(0);

}

/*----------------------------------------------------------------------
 * Fix equal or swapped bounds.
 *----------------------------------------------------------------------*/
CheckBound( lo, hi )
    float *lo, *hi;
{
    float swap;

    if (*lo == *hi) {
	if( *lo == 0) {
	    *lo = -1.0;
	    *hi = 1.0;
	}
	else{
	    *lo *= 0.9;
	    *hi *= 1.1;
	}
    }

    if( *lo > *hi) {
	swap = *lo; *lo = *hi; *hi = swap;
    }
}

