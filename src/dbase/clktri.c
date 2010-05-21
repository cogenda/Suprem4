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
/*   clktri.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:37 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"



/************************************************************************
 *									*
 *	clock_tri() - This routine makes sure the triangles are clock	*
 *  wise.								*
 *									*
 *      Original     	MEL	90	Based on meshgen		*
 *									*
 ************************************************************************/
clock_tri()
{
    register int ie;

    /*preform this check for everybody...*/
    for(ie = 0; ie < ne; ie++) {
	if ( ! ask(tri[ie], CLKWS) ) sclk_tri(ie);
    }
}


sclk_tri(ie)
int ie;
{
    register int i, j, k, tt;
    float xi, xj, xk;
    float yi, yj, yk;
    float s, den;

    set(tri[ie], CLKWS);
    switch( mode ) {
    case ONED :
	/*get the node numbers*/
	i = tri[ie]->nd[0];
	j = tri[ie]->nd[1];

	/*get the coordinate terms*/
	xi = pt[ nd[i]->pt ]->cord[0];
	xj = pt[ nd[j]->pt ]->cord[0];

	if ((xi - xj) < 0.0) {
	    tri[ie]->nd[0] = j;
	    tri[ie]->nd[1] = i;
	    tt = tri[ie]->nb[1];
	    tri[ie]->nb[1] = tri[ie]->nb[0];
	    tri[ie]->nb[0] = tt;
	}
	break;
    
    case TWOD :
	/*get the point numbers*/
	i = tri[ie]->nd[0];
	j = tri[ie]->nd[1];
	k = tri[ie]->nd[2];

	/*get the coordinate terms*/
	xi = pt[ nd[i]->pt ]->cord[0];	yi = pt[ nd[i]->pt ]->cord[1];
	xj = pt[ nd[j]->pt ]->cord[0];	yj = pt[ nd[j]->pt ]->cord[1];
	xk = pt[ nd[k]->pt ]->cord[0];	yk = pt[ nd[k]->pt ]->cord[1];

	/*calculate the denominator for the coupling*/
	den = (xk - xi) * (yj - yk) - (yk - yi) * (xj - xk);

	/*triangle area is one half of this value*/
	s = -0.5 * den;

	/*if area is less than 0, reorder the points and repeat*/
	if (s < 0.0) {
	    tri[ie]->nd[1] = k;
	    tri[ie]->nd[2] = j;
	    /*maintain neighbor and edge ordering*/
	    tt = tri[ie]->nb[1];
	    tri[ie]->nb[1] = tri[ie]->nb[2];
	    tri[ie]->nb[2] = tt;
	    tt = tri[ie]->edg[1];
	    tri[ie]->edg[1] = tri[ie]->edg[2];
	    tri[ie]->edg[2] = tt;
	}
    }
}
