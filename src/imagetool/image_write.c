
/*	image_write.c		Version 5.1	*/
/*	Last Modification:  7/3/91 15:46:17		*/

/*	image_write.c		Version 1.3		*/
/*	Last Modification:	12/12/89 11:27:29		*/

/*----------------------------------------------------------------------
**  Copyright 1989 by
**  The Board of Trustees of the Leland Stanford Junior University
**  All rights reserved.
**
**  This routine may not be used without the prior written consent of
**  the Board of Trustees of the Leland Stanford University.
**----------------------------------------------------------------------
**/

/*  image_write.c - save selected data in format compatible for imagetool
 *
 *	Goodwin Chin
 *	May 30, 1989
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constant.h"
#include "geom.h"
#include "global.h"


#define STATIC_ALLOCATION_TIME
#include "bound.h"

/**********************************************************************/

image_write( filename, par, param )
char *filename;
char *par;
int param;
{



    float min_z = MAXFLOAT;
    float max_z = -MAXFLOAT;
    float gxmin;
    float gxmax;
    float gymin;
    float gymax;
    float min_value;
    float *xdata;
    float *ydata;
    static float *mydata;


    int i;
    int win_xmin_index = 10000;
    int win_xmax_index = -10000;
    int win_ymin_index = 10000;
    int win_ymax_index = -10000;
    int xsize;
    int ysize;
    int material = 7;  /* all materials */
    int macfile = 1;

/*  variables from structure card */
    int pixelx;
    int pixely;
    int nxfac;
    int nyfac;
    int mode;

    extern float *z;
    extern make_grid();
    extern fill_grid();
    extern makeframe();
    extern dev_lmts();


    /* Defaults for these are set up in the key file */
    pixelx = get_int( param, "pixelx" );
    pixely = get_int( param, "pixely" );
    nxfac = get_int( param, "nxfac" );
    nyfac = get_int( param, "nyfac" );
    mode = get_int( param, "mode" );

    /* do some error checking on raster image size */
    if (get_bool( param, "mac"))  {
       if ((pixelx > MACMAXX+1) || (pixely > MACMAXY+1))  {
	  fprintf(stderr, "Maximum size allowed is %d by %d\n",
		  MACMAXX, MACMAXY);
	  return(ERROR);
       }
    }
    else  {
       if ((pixelx > SUNMAXX+1) || (pixely > SUNMAXY+1))  {
	  fprintf(stderr, "Maximum size allowed is %d by %d\n",
		  SUNMAXX, SUNMAXY);
	  return(ERROR);
       }
    }

    /* assign values if no defaults given:
     *   x,y : use entire device
     *   z   : use min/max for all data
     */
    dev_lmts( &min_x, &max_x, &min_y, &max_y);
    min_x *= 1.0e4;
    max_x *= 1.0e4;
    min_y *= 1.0e4;
    max_y *= 1.0e4;
    for ( i = 0; i < nn; i++ )  {
	if ( z[i] < min_z )
	    min_z = z[i];
	if ( z[i] > max_z )
	    max_z = z[i];
    }

    if ( is_specified( param, "x.min"))
	gxmin = get_float( param, "x.min");
    else
	gxmin = min_x;
    if ( is_specified( param, "x.max"))
	gxmax = get_float( param, "x.max");
    else
	gxmax = max_x;
    if ( is_specified( param, "y.min"))
	gymin = get_float( param, "y.min");
    else
	gymin = min_y;
    if ( is_specified( param, "y.max"))
	gymax = get_float( param, "y.max");
    else
	gymax = max_y;
    if ( is_specified( param, "z.min"))
	gmin = get_float( param, "z.min");
    else
	gmin = min_z;
    if ( is_specified( param, "z.max"))
	gmax = get_float( param, "z.max");
    else
	gmax = max_z;

    if( gmin==gmax) {
       if( gmin < 0)
	  gmax = 0;
       else if (gmin == 0)
       {
	  gmin = -1;
	  gmax = 1;
       }
       else gmin = 0;
    }

    min_value = (float)(floor((double)min_z) - 1.0);

    xsize = pixelx/nxfac;
    ysize = pixely/nyfac;

    if (make_grid(xsize, ysize, &xdata, &ydata, gxmin, gxmax,
	    gymin, gymax, &win_xmin_index,
	    &win_xmax_index, &win_ymin_index, &win_ymax_index) != 0)  {
	fprintf(stderr, "error creating index aray\n");
	return(ERROR);
    }

    if ((mydata = (float *)malloc((win_xmax_index - win_xmin_index + 1) *
		(win_ymax_index - win_ymin_index + 1) * sizeof(float)))
		== NULL) {
	fprintf(stderr, "memory allocation failed\n");
	return(ERROR);
    }

    for (i = 0; i < (win_xmax_index - win_xmin_index + 1) *
	    (win_ymax_index - win_ymin_index + 1); i++)
	mydata[i] = min_value;


    if (fill_grid(xsize + 3, ysize + 3, xdata, ydata, mydata, z, material,
	    win_xmin_index, win_xmax_index, win_ymin_index, win_ymax_index,
	    min_value) != 0)  {
	fprintf(stderr, "error filling data array\n");
	return(ERROR);
    }

    if (makeframe(mydata, pixelx, pixely, nxfac, nyfac,
	    filename, min_value, mode, macfile) != 0)  {
	fprintf(stderr, "error in creating binary image file\n");
	return(ERROR);
    }

    free(xdata);
    free(ydata);
    free(mydata);
    return(0);
}
