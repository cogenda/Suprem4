/*	fill_grid.c		Version 1.2	*/
/*	Last Modification:  1/11/91 10:42:43		*/

/*
 *	fill_grid.c
 *	   fills rectangular data grid based on triangular data
 *
 *	   This is a kludge for now and should be made more elegant in
 *	    	the future.
 *
 *	Goodwin Chin
 *	April 10, 1989
 *
 */

#include <stdio.h>
#include <math.h>

#ifdef CONVEX
#include <strings.h>
#else
#include <string.h>
#endif

#include "constant.h"
#include "global.h"
#include "geom.h"
#include "material.h"
#include "implant.h"
#include "impurity.h"
#include "plot.h"
#include "bound.h"

fill_grid(xsize, ysize, xdata, ydata, data, val, mat_num,
    win_xmin_index, win_xmax_index, win_ymin_index, win_ymax_index,
    min_value)
int xsize;
int ysize;
float *xdata;
float *ydata;
float *data;
float *val;
int mat_num;
int win_xmin_index;
int win_xmax_index;
int win_ymin_index;
int win_ymax_index;
float min_value;
{
    int i;
    register int j;
    register int k;
    int node_num;
    int point_num;
    int x_lo;
    int x_hi;
    int y_lo;
    int y_hi;
    int min_xindex;
    int min_yindex;
    int max_xindex;
    int max_yindex;
    int window();
    int index;
    int pt_num;
    extern pt_in_tri();
    float xcoordinate;
    float ycoordinate, c[3];
    float interpolate();
    float value;
    float OXIDE_VALUE; 
    float NITRIDE_VALUE;

/* fix problem where we are at interface boundary */

    OXIDE_VALUE = min_value - 1.0;
    NITRIDE_VALUE = min_value - 2.0;
    for ( i = 0; i < ne; i++ )  {
	if (!leaf(tri[i])) continue;
	/* now let's fill in the points associated with the triangle
	 *  vertices	
	 */
	if (((nd[(tri[i])->nd[0]])->mater != mat_num) &&
		(mat_num != 7)) continue;
	min_xindex = xsize;
 	min_yindex = ysize;
	max_xindex = 0;
	max_yindex = 0;

	for ( j = 0; j < 3; j++ )  {
	    node_num = tri[i]->nd[j];
	    point_num = nd[node_num]->pt;
	    
	    window( pt[point_num]->cord[0],
		    pt[point_num]->cord[1],
		    xdata, ydata, xsize, ysize,
		    &x_lo, &x_hi, &y_lo, &y_hi); 

	     if (x_lo < min_xindex) 
		    min_xindex = x_lo;
	     if (x_hi > max_xindex)
		    max_xindex = x_hi;

	     if (y_lo < min_yindex)
		    min_yindex = y_lo;
	     if (y_hi > max_yindex)
		    max_yindex = y_hi;

	} /* end for j */


	/* not that the vertices are done, lets see if there are
	 *   any points we need to fill that are inside the triangle.
	 *   to make life easy, lets check all THE possible points
	 *   between the x_index and y_index extremes
	 */


	for ( j = min_xindex; j <= max_xindex; j++ )  {
	    if ((j < win_xmin_index) || (j > win_xmax_index)) continue;
	    for ( k = min_yindex; k <= max_yindex; k++ )  {
	        if ((k < win_ymin_index) || (k > win_ymax_index)) continue;
		index = (k - win_ymin_index) * (win_xmax_index -
		    win_xmin_index + 1) + (j - win_xmin_index);
		c[0] = xcoordinate = xdata[j] * 1.0e-4;
		c[1] = ycoordinate = ydata[k] * 1.0e-4;
	
		if (pt_in_tri(c,i))  {
		    if ( ((nd[(tri[i])->nd[0]])->mater == SiO2) &&
				(mat_num != SiO2))
			value = OXIDE_VALUE;
		    else if ( ((nd[(tri[i])->nd[0]])->mater == SiNi) &&
				(mat_num != SiNi))
			value = NITRIDE_VALUE;
		    else   
		    	value = interpolate(xcoordinate, ycoordinate, i, val);
		    pt_num = vertex_tri(xcoordinate, ycoordinate, i);

		    /*  clip value before inserting it into solution array */
	  	    if ((value != OXIDE_VALUE) && (value != NITRIDE_VALUE)) {
			if (value < gmin) value = gmin;
			else if (value > gmax) value = gmax;
		    }

	/* for multiple nodes, take value with maximum absolute value, unless
		one of the nodes is silicon.  then use the silicon value */
		    if (pt_num != -1)  {
			if (si_interface(pt_num))   {
			    if (nd[tri[i]->nd[0]]->mater == Si)
				data[index] = value;
			}
			else  {
		    	    if ( fabs((double)value) > 
				fabs((double)data[index]) )
				    data[index] = value;
			}
 		    }
		    else
			data[index] = value;
		} /* end if (pt_in_tri) */
	    } /* end for k */
	} /* end for j */

    } /* end for i */

    return(0);
} /* end subroutine */




float interpolate( x, y, tri_num, val )
float x;
float y;
int tri_num;
float *val;

/*  use properties of normals to a plane to get answer.
 *	in this case first determine the equation of the normal to the
 *	plane by taking the cross product of 2 vectors in the plane.
 *	then create a new vector in the plane by connecting the new
 *	point (x,y) with an existing triangle vertex.  This vector
 *	MUST be perpendicular to the normal.  Thus we can solve for
 *	it's y-value uniquely.  
 */

{
    float normal_x = 0.0;
    float normal_y = 0.0;
    float normal_z = 0.0;
    float answer = 0.0;
    float cords[3][3];
    register int i;
    int node_num;

    for ( i = 0; i < 3; i++ )  {
	node_num = tri[tri_num]->nd[i];
	cords[i][0] = pt[ nd[node_num]->pt ]->cord[0];
	cords[i][1] = pt[ nd[node_num]->pt ]->cord[1];
	cords[i][2] = val[node_num];
    }

    normal_x = (cords[0][1] - cords[1][1]) * (cords[2][2] - cords[1][2])
	-    ( (cords[0][2] - cords[1][2]) * (cords[2][1] - cords[1][1]) );

    normal_y = (cords[0][2] - cords[1][2]) * (cords[2][0] - cords[1][0])
	-    ( (cords[0][0] - cords[1][0]) * (cords[2][2] - cords[1][2]) );

    normal_z = (cords[0][0] - cords[1][0]) * (cords[2][1] - cords[1][1])
	-    ( (cords[0][1] - cords[1][1]) * (cords[2][0] - cords[1][0]) );

    answer = cords[0][2] + ( (cords[0][0] - x) * normal_x
			+ (cords[0][1] - y) * normal_y  )/normal_z;
	
    return(answer);
}

/* vertex_tri determines whether point (x,y) is a vertex of triangle tri_num */
vertex_tri(x, y, tri_num)
float x;
float y;
int tri_num;
{
    register int index;
    
    if  ( (pt[nd[tri[tri_num]->nd[0]]->pt]->cord[0] == x) &&
    	  (pt[nd[tri[tri_num]->nd[0]]->pt]->cord[1] == y) )
	index = nd[tri[tri_num]->nd[0]]->pt;
    else if  ( (pt[nd[tri[tri_num]->nd[1]]->pt]->cord[0] == x) &&
    	  (pt[nd[tri[tri_num]->nd[1]]->pt]->cord[1] == y) )
	index = nd[tri[tri_num]->nd[1]]->pt;
    else if  ( (pt[nd[tri[tri_num]->nd[2]]->pt]->cord[0] == x) &&
    	  (pt[nd[tri[tri_num]->nd[2]]->pt]->cord[1] == y) )
	index = nd[tri[tri_num]->nd[2]]->pt;
    else
	index = -1;
    return(index);
}


/* si_interface determines whether point pt_num is at a silicon interface */
si_interface(pt_num)
int pt_num;
{
    register int i;
    int nnodes;
    nnodes = pt[pt_num]->nn;
    for (i = 0; i < nnodes; i++)  
	if (nd[pt[pt_num]->nd[i]]->mater == Si) return(1);
    return(0);
}
