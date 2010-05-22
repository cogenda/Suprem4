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
/*   print_1d.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:39:05 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "plot.h"
#include "material.h"

/************************************************************************
 *									*
 *	print_1d(par, param ) - this routine is the father of the 	*
 *  print_1d software.  It calls appropriate routines to set up 	*
 *  and do prints.							*
 *									*
 *  Original:	MEL	1/85	(modeled on pisces2)			*
 *									*
 ************************************************************************/
print_1d(par, param )
char *par;
int param;
{
    float x, xmin, xmax, ymin, ymax;
    float y;
    int layers = FALSE;
    int ptype;
    int i;
    char *format;
    struct d_str *data;
    int count;
    int lnum, lmat;
    double dose;
    double tmp;
    int mat1, mat2, byarc;

    if ( InvalidMeshCheck()) return( -1);
    if( znn != nn ) {
	fprintf(stderr, "Z variable is out of date w/r to mesh");
	return( -1);
    }

    data = salloc( struct d_str, 2*ne );

    /*
     * --- Collect parameters --------------------
     */
    x         = get_float(param, "x.value") * 1e-4;
    y         = get_float(param, "y.value") * 1e-4;
    if ( is_specified(param, "x.value") )
	ptype = XSEC;
    else if ( is_specified( param, "y" ) || (mode == ONED) ) {
	ptype = YSEC;
	y = (mode == ONED)?0.0:y;
    }
    else {
	ptype = BND;
	mat1 = ChosenMater( par, param, 0);
	mat2 = ChosenMater( par, param, 1);
	if (mat2 < 0) mat2 = ChosenBC( par, param, 1);
    }
    if( is_specified( param, "format")) {
	char *s = get_string( param, "format");
	format = salloc( char, strlen(s)+2);
	sprintf( format, "%%%s", s);
    }
    else format = "%-16e";
    byarc = get_bool( param, "arclength");

    if ( is_specified( param, "layers" )  && get_bool( param, "layers" ) ) {
	layers = TRUE;
    }

    /*
     * --- Collect the data --------------------
     */
    if ( ptype == XSEC )
	count = do_1d( ptype, x, data , mat1, mat2, byarc);
    else
	count = do_1d( ptype, y, data , mat1, mat2, byarc);

    /*
     * --- Decide on bounds: start with the device limits
     */
    dev_lmts(&xmin, &xmax, &ymin, &ymax);

    if ( ptype == XSEC ) {
	xmin = ymin;
	xmax = ymax;
    }

    /* check that the actual bounds aren't larger */
    if( data[0].x       < xmin) xmin = data[0].x;
    if( data[count-1].x < xmin) xmin = data[count-1].x;
    if( data[0].x       > xmax) xmax = data[0].x;
    if( data[count-1].x > xmax) xmax = data[count-1].x;

    /*check the overide variables from the user*/
    if ( is_specified(param, "x.max") )
	xmax = get_float( param, "x.max" ) * 1e-4;
    if ( is_specified(param, "x.min") )
	xmin = get_float( param, "x.min" ) * 1e-4;
    CheckBound( &xmin, &xmax);

    /*
     * --- A layer-type print? --------------------
     */
    if ( layers ) {

	/*initialize the layer counters*/
	lnum = 1;
	lmat = data[0].mat;
	tmp = data[0].x;

	/*get the half dose in the first cell*/
	dose = 0.5 * (data[1].x - data[0].x) * data[0].y;

	/*print out the headers*/
	printf("layer  material        thickness  Integrated\n");
	printf("num    type            microns    %s\n", label);
	printf(" %-2d    ", 0);
	printf("%-14s  ",  1+MatNames[ lmat ] );
	printf("%-9.3f  ", tmp * 1e4 );
	printf("%-16e\n", 0.0 );

	for(i = 1; i < count; i++) {

	    /*sum the dose for the previous layer*/
	    dose += 0.5 * (data[i].x - data[i-1].x) * data[i].y;

	    /*if a layer change - material or z sign change*/
	    if ( (data[i].mat != lmat) || (data[i-1].y * data[i].y < 0.0) ) {
		printf(" %-2d    ", lnum);
		printf("%-14s  ",  1+MatNames[ lmat ] );

		/*figure out the new layer change location*/
		if (data[i].mat != lmat) {
		    printf("%-9.3f  ", data[i-1].x * 1e4 );
		}
		else {
		    /*interpolate to get the zero crossing*/
		    tmp = data[i-1].y / (data[i].y - data[i-1].y);
		    tmp = tmp * (data[i-1].x - data[i].x) + data[i-1].x;
		    printf("%-9.3f  ", tmp * 1e4 );
		}

		printf("%-16e\n", dose );
		lnum++;
		lmat = data[i].mat;
		dose = 0.0;
	    }
	    if (i != count-1)
		dose += 0.5 * (data[i+1].x - data[i].x) * data[i].y;
	}
	printf(" %-2d    ", lnum);
	printf("%-14s  ",  1+MatNames[ lmat ] );
	printf("%-9.3f  ", data[count-1].x * 1e4 );
	printf("%-16e\n", dose );
    }

    /*
     * --- A cross-section type print? --------------------
     */
    else {
	if (ptype == XSEC)
	    fprintf(stdout, "y coordinate in um\t%16s\tMaterial\n", label);
	else
	    fprintf(stdout, "x coordinate in um\t%16s\tMaterial\n", label);

	for(i = 0; i < count; i++) {
	    if ( (data[i].x >= xmin) && (data[i].x <= xmax) ) {
		printf("  %16.8f\t", data[i].x * 1e4);
		printf( format, data[i].y);
		printf("\t%-16s\n", 1+MatNames[ data[i].mat]);
	    }
	}
    }
    free(data);
    return(0);
}
