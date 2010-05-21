h24065
s 00003/00161/00105
d D 2.1 93/02/03 15:29:29 suprem45 2 1
c Initial IV.GS release
e
s 00266/00000/00000
d D 1.1 93/02/03 15:28:43 suprem45 1 0
c date and time created 93/02/03 15:28:43 by suprem45
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


D 2
#ifdef GOODWIN
#define CONTOUR_DONE	20
#define PEN_UP		10


static int clean;  /* reinitialize data structure */

E 2
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
D 2
    int line;
    int label, print;
    int symb;
    char str[2];
    float hsize, wsize;
    float *strptr;
    float *strend;
    float **string;  /* memory allocation will be done in mark_triangle */
    int string_len; /* length of string */
    int len;
#ifdef DB
    FILE *fp;
    int points_plotted = 0;
    int i;
#endif DB

    if ( InvalidMeshCheck()) return( -1);
    if( znn != nn ) {
	fprintf(stderr, "Z variable is out of date w/r to mesh");
	return( -1);
    }

    if ( mode == ONED ) {
	fprintf(stderr, "Can't do contouring in one dimension\n");
	return( - 1);
    }

    string = (float **)malloc( sizeof(float) );

    value = get_float( param, "value" );
    label = is_specified(param, "label" ) && get_bool( param, "label" );
    print = is_specified(param, "print" ) && get_bool( param, "print" );


    if ( clean == 0) clean = 1;

    line  = get_int( param, "line.type" );
    hsize = 0.02 * (ymax - ymin);
    wsize = 0.02 * (xmax - xmin);
    if ( is_specified( param, "symb" ) ) {
	symb = get_int(param, "symb");
	str[0] = symb;
	str[1] = '\0';
    }
    else symb = -1;

    gnline( line );

    string_len = make_contour( string, Si, value, clean );

    /* contour points now stored in string.  now plot things */

#ifdef DB
    fp = fopen( "debugfile", "a" );
    strptr = *string;
    for ( i = 0; i < string_len; i++ )
	fprintf( fp, "\tstrptr[%d] is %f\n", i, strptr[i] );
    fclose( fp );
#endif DB

    strptr = *string;
    strend = strptr + string_len;


    while ( strptr < strend ) {
	if ( *strptr == PEN_UP )  {
	    if ( print ) {
		strptr++;
		printf("\n%16s\t%16s\n", "x", "y");
		printf("%16g\t%16g\n", *strptr, *(strptr + 1) );
	    }
	    else {
		strptr++;
		gmove( *strptr, *(strptr + 1) );

		/*plot label here just for kicks */
		if ( label ) {
		    gmove( *(strptr + 2), *(strptr + 3) );
	   

		    /* guess we need to do some checking here to see if axis have been
		     *	flipped before we plot.  i'm not sure how we can plot
		     *	data points OK but not labels, but that's the breaks
		     */
		    if ( yflip ) {
			/* reset all things gplot */
			gscale( 1.0, -1.0 );
			gplot2( G_CLIPL, G_ONLOG, xmin, -ymax );
			gplot2( G_CLIPH, G_ONLOG, xmax, -ymin );

			len = my_numb2( *(strptr + 2), *(strptr + 3), value, hsize,
			    wsize, 0.0, 0.5, 1.0, "g" );	

			/* resture the gplot variables */
			gscale( 1.0, -1.0 );
			gplot2( G_CLIPL, G_ONLOG, xmin, ymin );
			gplot2( G_CLIPH, G_ONLOG, xmax, ymax );
		    }
		    else
			len = my_numb2( *(strptr + 2), *(strptr + 3), value, hsize,
				    wsize, 0.0, 0.5, 1.0, "g" );	

		    gmove( *strptr, *(strptr + 1) );
		}
	
		if ( symb != -1 )  /* draw a symbol */
		symbl2( *strptr, *(strptr + 1), str, 1, hsize, wsize,
		    0.0, 0.5, 0.5 );
	    }
	}
	else  {
	    if ( print ) {
		printf("%16g\t%16g\n", *strptr, *(strptr + 1) );
	    }
	    else {
		if ( symb == -1 )   /* draw a line */
		    gdraw( *strptr, *(strptr + 1) );
		else  {
		    symbl2( *strptr, *(strptr + 1), str, 1, hsize, wsize,
			    0.0, 0.5, 0.5 );
		    gdraw( *strptr, *(strptr + 1) );
		}
	    }
	}
	strptr += 2;
    }
#ifdef DB
    fp = fopen( "debugfile", "a" );
    fprintf( fp, "\n\n\t%d points were plotted\n", points_plotted );
    fclose( fp );
#endif DB
    free( string );
	    
    ggtoa();
}
#endif




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
E 2
    float arr[MAXVRT][3], p[2][3];
    int t,i,nx;
I 2
    int line_type;
E 2
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
I 2
    line_type = get_int(param, "line.type");
E 2
    if ( is_specified( param, "symb" ) ) {
	symb = get_int(param, "symb");
    }
    else symb = -1;

D 2
    xgNewSet();
E 2
I 2
    xgNewSetLT(line_type);
E 2
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
E 1
