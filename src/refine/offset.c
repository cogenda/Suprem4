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
/*   offset.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:46 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "impurity.h"
#include "regrid.h"
#include "refine.h"



/************************************************************************
 *									*
 *	offset() - This routine computes an offset to line p in line op	*
 *  hopefully avoiding the worst sorts of loops and ugliness.		*
 *									*
 *  Original:	MEL	4/87   						*
 *		LCC     4/90						*
 ************************************************************************/
gen_offset( nump, p, pop, thick, space )
int nump;		/*the number of points in p*/
struct line *p;		/*the line*/
struct line **pop;	/*the offset*/
double thick;		/*size of the offset*/
double space;		/*size of the spacing*/
{
    register int i, j;	/*every routine needs indices*/
    int nop = 0;	/*current number of surface points*/
    double nx, ny, d;		/*the normal to the curve*/
    double x1,y1, x2,y2;	/*the offset to the line segment*/
    double x, y;
    double rot[2][2];
    double err = space * 0.01;
    int maxop;
    struct line *op;

    maxop = 100;
    *pop = salloc( struct line, maxop );
    op = *pop;

    if ( mode == ONED ) {
        /* generate offset point */
	op[0].x   = -thick + p[0].x;
        op[0].p   = -1;
	op[0].map = 0;
	nop       = 1;
	return( nop );
    }

    /* if TWOD! */

    /*compute the initial points on the curve*/
    ny = p[0].x - p[1].x;
    nx = p[1].y - p[0].y;

    d = sqrt( nx*nx + ny*ny );

    op[0].x = thick * nx / d + p[0].x - thick;
    op[0].y = thick * ny / d + p[0].y;
    op[0].map = 0;
    op[0].p = -1;

    op[1].x = thick * nx / d + p[0].x;
    op[1].y = thick * ny / d + p[0].y;
    op[1].map = 0;
    op[1].p = -1;

    op[2].x = thick * nx / d + p[1].x;
    op[2].y = thick * ny / d + p[1].y;
    op[2].map = 1;
    op[2].p = -1;
    nop = 3;

    /*process these guys on a pair by pair basis*/
    for( i = 1; i < nump-1; i++) {

	/*compute the normal to this line segment*/
	ny = p[i].x - p[i+1].x;
	nx = p[i+1].y - p[i].y;
	d = sqrt( nx*nx + ny*ny );

	/*compute the new points*/
	x1 = thick * nx / d + p[i].x;
	y1 = thick * ny / d + p[i].y;
	x2 = thick * nx / d + p[i+1].x;
	y2 = thick * ny / d + p[i+1].y;

	/*compute the distance between the two points*/
	d = sqrt( (op[nop-1].x-x1) * (op[nop-1].x-x1) +
		  (op[nop-1].y-y1) * (op[nop-1].y-y1) );

	/*if these two points are real close, just add the new one*/
	if ( d < err ) {
	    op[nop].x = x2;
	    op[nop].y = y2;
	    op[nop].map = i+1;
	    op[nop].p = -1;
	    nop++;
	    if ( nop == maxop ) {
		maxop += 100;
		*pop = sralloc( struct line, maxop, op );
		op = *pop;
	    }
	}
	else {
	    /*fill in around the aparent corner*/
	    rot[0][0] = cos( space / thick );
	    rot[0][1] = - sin( space / thick );
	    rot[1][0] = - rot[0][1];
	    rot[1][1] = rot[0][0];

	    /*fill in between the previous point and the final*/
	    do {

		/*get the vector from the corner to the last point*/
		x = op[nop-1].x - p[i].x;
		y = op[nop-1].y - p[i].y;

		/*make sure the point is thickness away*/
		op[nop].x = rot[0][0] * x + rot[0][1] * y + p[i].x;
		op[nop].y = rot[1][0] * x + rot[1][1] * y + p[i].y;
		op[nop].map = i;
		op[nop].p = -1;
		nop++;
		if ( nop == maxop ) {
		    maxop += 100;
		    *pop = sralloc( struct line, maxop, op );
		    op = *pop;
		}

		d = sqrt( ( op[nop-1].x-x1 ) * ( op[nop-1].x-x1 ) +
			  ( op[nop-1].y-y1 ) * ( op[nop-1].y-y1 ) );

	    } while( d > 1.5 * space );
	    nop--;
	    add_toline( &nop, op, x1, y1, i, err );
	    if ( nop == maxop ) {
		maxop += 100;
		*pop = sralloc( struct line, maxop, op );
		op = *pop;
	    }
	    add_toline( &nop, op, x2, y2, i+1, err );
	    if ( nop == maxop ) {
		maxop += 100;
		*pop = sralloc( struct line, maxop, op );
		op = *pop;
	    }
	}
    }
    /*make sure the last outer is mapper to last inner*/
    op[nop-1].map = nump - 1;

    /*compute the normal to this line segment*/
    ny = p[nump-2].x - p[nump-1].x;
    nx = p[nump-1].y - p[nump-2].y;
    d = sqrt( nx*nx + ny*ny );

    op[nop].x = thick * nx / d + p[nump-1].x + thick;
    op[nop].y = thick * ny / d + p[nump-1].y;
    op[nop].map = 0;
    op[nop].p = -1;
    nop++;

    /*de_loop the outer line...*/
    while( de_loop(&nop, op, err) );

    /*strip the first and last pseudo points*/
    del_fromline( 0, &nop, op );
    nop--;

    /*get the p maps set up*/
    p[0].map = 0;
    for(i = 1; i < nop; i++) {
	p[op[i].map].map = i;
	for(j = op[i-1].map+1; j < op[i].map; j++) p[j].map = i;
    }

    return(nop);
}



add_toline( nop, op, x, y, m, err )
int *nop;
struct line *op;
double x, y;
int m;
double err;
{
    double dis, dx, dy;

    if ( *nop > 1 ) {
	dx = op[*nop-1].x - x;
	dy = op[*nop-1].y - y;
	dis = sqrt( dx * dx + dy * dy );
    }
    else dis = MAXFLOAT;

    if ( dis > err ) {
	op[*nop].x = x;
	op[*nop].y = y;
	op[*nop].map = m;
	op[*nop].p = -1;
	(*nop)++;
    }
    else {
	op[*nop-1].x = x;
	op[*nop-1].y = y;
	op[*nop-1].map = m;
	op[*nop-1].p = -1;
    }
}


del_fromline( index, nop, op )
int index;
int *nop;
struct line *op;
{
    int i;

    for(i = index; i < *nop-1; i++) {
	op[i].x = op[i+1].x;
	op[i].y = op[i+1].y;
	op[i].p = op[i+1].p;
	op[i].map = op[i+1].map;
    }
    *nop -= 1;
}
