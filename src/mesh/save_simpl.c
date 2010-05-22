/*	save_simpl.c		Version 5.1		*/
/*	Last Modification:	7/3/91 08:30:55		*/

/*
 *	save_simpl.c
 *	   save structure information in a form readable by SIMPL-DIX
 *
 *	   This is a kludge for now and should be made more elegant in
 *	    	the future.
 *
 *	Goodwin Chin
 *	December 7, 1988
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef CONVEX
#include <strings.h>
#else
#include <string.h>
#endif
#include "constant.h"
#include "simpl-dix.h"
#include "simpl.h"
#include "global.h"
#include "geom.h"
#include "material.h"
#include "implant.h"
#include "plot.h"
#include "bound.h"

#define BUFFERSIZE 80

float xloc[GRID_X_SIZE];
float yloc[GRID_Y_SIZE];
float left = 0.0;
float right = 0.0;
float top = 0.0;
float bottom = 0.0;
int xlocsize;
int ylocsize;

char *Mnames[MAXMAT] = {"AIR", "OXID", "NTRD", "SI", "POLY",
	"OXIN", "METL", "RST"};

simpl_write( SIMPLfilename, SIMPLheaderfilename )
char *SIMPLfilename;
char *SIMPLheaderfilename;

{
    FILE *SIMPLfile;
    FILE *SIMPLheader;
    int write_SIMPLPolygon();
    int write_SIMPLGrid();
    int i;
    int local_x_grid_size = GRID_X_SIZE/1;
    int local_y_grid_size = GRID_Y_SIZE/1;
    float spacing;
    char buffer[BUFFERSIZE];

    /* write information */
    if ( (SIMPLfile = fopen(SIMPLfilename, "w")) == 0 )
	panic( "error in opening SIMPL savefile" );


    /* write header information */
    if ((SIMPLheader = fopen(SIMPLheaderfilename, "r")) == 0)
	panic("error in opening SIMPLheaderfilename");

/* look for window information.  place in first line of file. if the
 *   line is blank then no information present
 */


    fgets(buffer, BUFFERSIZE, SIMPLheader);
    while (!feof(SIMPLheader))  {
	fputs(buffer, SIMPLfile);
	fgets(buffer, BUFFERSIZE, SIMPLheader);
    }
    fclose(SIMPLheader);


/*  need to do something about extraneous stuff on the left side.
 *	Look for only small deviations (<1%).  else assume that
 *	offset is real and not an artifact.  To make life easy
 *	we'll just truncate at 0.01 micron increments.
 */

    dev_lmts(&left, &right, &top, &bottom);
    left *= 1.0e4;
    right *= 1.0e4;
    top *= 1.0e4;
    bottom *= 1.0e4;


/*  we need to move window so that it always starts from 0, even
 *	if the actual silicon starts from a negative value
 */
    /* add 2.0um of air so that doping shows up OK */
    /* forced format to 0.001 micron resulution since
     *   that is all the resolution of the layout
     *   is only 0.01 micron. This takes care of some
     *   possible round-off error
     */
    fprintf( SIMPLfile, "%.2f\t%.2f\t0.0\t%.2f\n", left,
 	    right, bottom + 2.0 );



    /* heuristic on algorithm for generating xloc and yloc.  if we have
     *   a deep substrate (say > 6.0um) we space non-uniformly based on
     *   tx and ty. else we space uniformly
     */
    spacing = (right - left)/(float)(local_x_grid_size-1);
    for (i = 0; i < local_x_grid_size - 1; i++)
	xloc[i] = left + i * spacing;
    xloc[local_x_grid_size - 1] = right;
    xlocsize = local_x_grid_size;

    spacing = (bottom - top)/(float)(local_y_grid_size-1);
    for (i = 0; i < local_y_grid_size - 1; i++)
	yloc[i] = top + i * spacing;
    yloc[local_y_grid_size - 1] = bottom;
    ylocsize = local_y_grid_size;

    write_SIMPLPolygon( SIMPLfile );

    write_SIMPLGrid(SIMPLfile, xloc, xlocsize, yloc, ylocsize);

    fclose( SIMPLfile );

    return( TRUE );
}

write_SIMPLPolygon( SIMPLfile )
FILE *SIMPLfile;
{
    /* write string information describing geometries of layers present.
     *   things are not as tough as they seem since boundary information
     *   is already stored in ria and rja.  the format necessary is:
     *     line 1 : left,right,top,bottom values
     *     line 2 : number of strings
     *     the next lines contain the layer of the string,
     *	     the number of data points in the string, and a list
     *	     of these points.
     *
     */

    int ir = 0;
    int i = 0;
    int index = 0;
    int topleft = 0;
    int topright = 0;
    int bottomleft = 0;
    int bottomright = 0;
    int count = 0;
    int g;
    struct LLedge *bp, *bnd;
    struct d_str *poly_data;
    struct tmp_str  {
	float x;
	float y;
	int mat;
	int discard;
    }  *poly_array;

    /* go for the slow algorithm.  Save all the points of the polygon
     *   into a separate structure.  Convert to a clockwise orientation
     *   and search for topleft, topright
     *   bottomleft, and bottomright, respectively.
     */

    if ((poly_data =
	(struct d_str *)malloc(np * sizeof(struct d_str)))
		== NULL)  {
	    fprintf(stderr, "can't allocate memory for poly_data\n");
	    exit(-1);
    }

    fprintf( SIMPLfile, "%d\n", nreg );
    for ( ir = 0; ir < nreg; ir++ )  {
	fprintf( SIMPLfile, "%s\n", Mnames[mat_reg(ir)] );

	for(g = 1, index = 1, bp = bnd = sreg[reg[ir]->sreg]->bnd;
	    g || bp != bnd; g=0, index++, bp = bp->next) {

	    poly_data[index].x = xcord(pt_nd(nB(bp))) * 1.0e4;
	    poly_data[index].y = ycord(pt_nd(nB(bp))) * 1.0e4;
	    poly_data[index].mat = mat_reg(ir);

	}  /* end for index */

	/* although this is a waste in efficiency, it will make the
	 *  code more readable and easier to fix if necessary.
	 *  create another array called poly_array with the
	 *  points stored in CLOCKWISE order
	 */
	if ((poly_array =
		(struct tmp_str *)malloc(index * sizeof(struct tmp_str)))
		== NULL)  {
	    fprintf(stderr, "can't allocate memory for poly_array\n");
	    exit(-1);
	}
	for (i = 0; i < index; i++)  {
		poly_array[i].x = poly_data[index - i].x;
		poly_array[i].y = poly_data[index - i].y;
		poly_array[i].mat = poly_data[index - i].mat;
		poly_array[i].discard = 1;
	}


	/* walk through array finding points to discard. for the
	 *  bottom layer (assumed to be SI) we can't have points
	 *  on the bottom except for the left and right corners.
	 *  For the interface to SAMPLE to work properly we also
	 *  are not allowed to have points on the left and right
	 *  edges except for the points at the top and bottom.
	 */
	for (i = 0; i < index; i++)  {
	    topleft = (poly_array[(i+1)%index].x - poly_array[i].x > 0.001)
			&& (poly_array[(i - 1 + index)%index].y -
			poly_array[i].y > 0.001);
	    topright = (poly_array[i].x - poly_array[(i - 1 + index)%index].x
			> 0.001) &&
			(poly_array[(i + 1)%index].y -
			poly_array[i].y > 0.001);
	    bottomleft = (poly_array[(i - 1 + index)%index].x -
			poly_array[i].x > 0.001) &&
			(poly_array[i].y
			- poly_array[(i + 1)%index].y > 0.001);
	    bottomright = (poly_array[i].x -
			poly_array[(i + 1)%index].x > 0.001) &&
			(poly_array[i].y
			- poly_array[(i - 1 + index)%index].y > 0.001);

	    /* the magic check for points to delete */
	    if (! ( ( ((bottom - poly_array[i].y) < 0.001)
		      && (!bottomleft) && (!bottomright) )
		  ||(  ((poly_array[i].x - left) < 0.001)
		      && ((bottom - poly_array[i].y) > 0.001)
		      && (!topleft) && (!bottomleft))
		  ||(  ((right - poly_array[i].x) < 0.001)
		      && ((bottom - poly_array[i].y > 0.001))
		      && (!topright) && (!bottomright)) ) )  {
		count++;
		poly_array[i].discard = 0;
	   }
	} /* end for i */

	fprintf(SIMPLfile, "%d\n", count);

	/* print information */
	for (i = 0; i < index; i++)  {
	    if (poly_array[i].discard == 0)
	        fprintf(SIMPLfile, "%.2f  %.2f\n", poly_array[i].x,
		    bottom - poly_array[i].y);
		/* note y = bottom - poly_array[i].y  */
	} /* end for i */

	count = 0;
	free(poly_array);
    } /* end for ir */
    free(poly_data);
}

write_SIMPLGrid(SIMPLfile, xloc, xlocsize, yloc, ylocsize)
FILE *SIMPLfile;
float *xloc;
int xlocsize;
float *yloc;
int ylocsize;
{

    extern int fill_grid();
    int ix;
    int iy;
    static float *data;
    float min_value = MAXFLOAT;


    /* the format of this section of the file is as follows.
     *	line 1 - number of horizontal grid points(nx)
     *  lines 2 to nx+1 - the positions of the horizontal grid points
     *  line nx+2 - the number of vertical grid points(ny)
     *  lines nx+3 to nx+ny+2 - the positions of the vertical grid points
     *  lines nx+ny+3 to (nx*ny) + nx+ny+2 - the net doping
     */

    fprintf( SIMPLfile, "%d\n", xlocsize );

    for ( ix = 0; ix < xlocsize; ix++ )
	fprintf( SIMPLfile, "%.2f\n", xloc[ix] );


    fprintf( SIMPLfile, "%d\n", ylocsize );

    for ( iy = ylocsize - 1; iy >= 0; --iy )
	fprintf( SIMPLfile, "%.2f\n", bottom - yloc[iy] );

    data = salloc(float, xlocsize * ylocsize);
    for (ix = 0; ix < nn; ix++)
	if ( (z[ix] < min_value) && (nd[ix]->mater == Si) )
	    min_value = z[ix];
	else  if ( (z[ix] > gmax) && (nd[ix]->mater == Si) )
	    gmax = z[ix];

    gmin = min_value;

    (void)fill_grid(xlocsize, ylocsize, xloc, yloc, data, z, Si,
	0, xlocsize - 1, 0, ylocsize - 1, min_value);

    for ( iy = ylocsize - 1; iy >= 0; --iy )
	for ( ix = 0; ix < xlocsize; ix++ )
	    fprintf(SIMPLfile, "%e\n", data[iy * xlocsize + ix]);


}


