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
/*   surface.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:56 */

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
 *	find_surf( p ) - find and sort the surface points left to right *
 *  and store in the sf array.						*
 *									*
 *  Original:	MEL	4/87   						*
 *		LCC     4/90   	                    			*
 ************************************************************************/
find_surf( pp )
struct line **pp;		/*the point numbers*/
{
    register int i, j;	/*every routine needs indices*/
    register int ns = 0;	/*current number of surface points*/
    int left, right, p1, p2, n1, n2, e1;
    int ln, leftnode;
    int el, ind, e;
    float nrm[MAXDIM], nx, ny;
    int done, found;
    struct line *p;
    int maxop;

    maxop = 100;
    *pp = salloc( struct line, maxop );
    p = *pp;

    switch ( mode ) {

    case TWOD :
	/*find the left most edge on the exposed surface*/
	for(i = 0; i < ned; i++) {
	    if (!ask(edg[i], ESURF)) continue;

	    /*figure the left most edge*/
	    n1 = nd_edg(i,0);
	    n2 = nd_edg(i,1);
	    p1 = pt_edg(i,0);
	    p2 = pt_edg(i,1);

	    /*order these so the normal points out*/
	    (void)local_normal( n1, -1, nrm );
	    ny = XC(p1) - XC(p2);
	    nx = YC(p2) - YC(p1);

	    if ( (nx * nrm[0]) + (ny * nrm[1]) > 0.0 ) {
		left = p1;
		right = p2;
		ln = n1;
	    }
	    else {
		left = p2;
		right = p1;
		ln = n2;
	    }

	    if ( ns == 0 ) {
		p[0].p = left;  p[0].x = XC(left);  p[0].y = YC(left);
		p[1].p = right; p[1].x = XC(right); p[1].y = YC(right);
		leftnode = ln;
		e1 = i;
		ns = 2;
	    }
	    else if ( XC(left) < p[0].x ) {
		p[0].p = left;  p[0].x = XC(left);  p[0].y = YC(left);
		p[1].p = right; p[1].x = XC(right); p[1].y = YC(right);
		e1 = i;
		leftnode = ln;
	    }
	    else if ( (XC(left) == p[0].x) ) {
		/*depends on the way we are facing*/
		if ( (YC(left) > YC(right)) && (YC(left) > p[0].y) ) {
		    p[0].p = left;  p[0].x = XC(left);  p[0].y = YC(left);
		    p[1].p = right; p[1].x = XC(right); p[1].y = YC(right);
		    e1 = i;
		    leftnode = ln;
		}
		else if ( (YC(left) < YC(right)) && (YC(left) < p[0].y) ) {
		    p[0].p = left;  p[0].x = XC(left);  p[0].y = YC(left);
		    p[1].p = right; p[1].x = XC(right); p[1].y = YC(right);
		    e1 = i;
		    leftnode = ln;
		}
	    }
	}

	/*build the surface list...*/
	el = e1;
	ind = (leftnode == nd_edg(el, 0))?1:0;
	done = FALSE;
	while ( !done ) {
	    p1 = pt_edg(el,ind);

	    found = FALSE;
	    for(j = 0; j < num_nd(p1) && !found; j++) {
		n1 = nd_pt(p1,j);
		for(i = 0; (i < num_edge_nd(n1)) && !found; i++) {
		    e = edge_nd(n1,i);
		    if ( (e != el) && ask(edg[e], ESURF) ) {
			found = TRUE;
			if ( nd_edg(e,0) == n1 ) ind = 1;
			else ind = 0;
			p[ns].p = pt_edg(e, ind);
			p[ns].x = XC(p[ns].p);
			p[ns].y = YC(p[ns].p);
			ns++;
			if ( ns == maxop ) {
			    maxop += 100;
			    *pp = sralloc( struct line, maxop, p );
			    p = *pp;
			}
		    }
		}
	    }
	    el = e;
	    done = !found;
	}
    break;

 case ONED :
    /* 1-D need surface point only */
    for (i=0; i<ne; i++) {

	/* find out nd[0] or nd[1] to be surface point? */
	/* really have to check both nodes? yes!        */
        if (tri[i]->nb[0] == BC_OFFSET+2) {
            p[0].p = nd[tri[i]->nd[1]]->pt;
	    p[0].x = XC(p[0].p);
        }
        else if (tri[i]->nb[1] == BC_OFFSET+2) {
            p[0].p = nd[tri[i]->nd[0]]->pt;
	    p[0].x = XC(p[0].p);

        /* Be alert! p[0].p associate to point not node */
        }
    }
    ns=1;
    break;
 }

    return(ns);
}

