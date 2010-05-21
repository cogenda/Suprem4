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
/*   kill_node.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:57 */

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "constant.h"
#include "global.h"
#include "dbaccess.h"
#include "material.h"
#include "skel.h"

/*-----------------KILL_NODE--------------------------------------------
 * Remove a node from the grid and fix up the triangulation.
 * Algorithm:
 *     if edge node, internalize the node to be removed
 *     call kill_internal_node to do the dirty work
 * Node is passed in as a triangle/vertex pair to avoid O(N) search every
 * time we do this.
 *----------------------------------------------------------------------*/
kill_node(in)
int in; 		/* Number of node to operate on */
{
    register int i, j;
    int maxn, nb[100], tn, e, sr;
    float vec[MAXDIM], ang[50], tf;
    int reg,reg1,reg2;

    /*get all of the neighbors pulled together*/
    maxn = num_neigh_nd(in);
    for(i = 0; i < maxn; i++) nb[i] = neigh_nd(in,i);

    /*eliminate any neighbors that share the point*/
    for(i = 0; i < maxn; i++) {
	if ( pt_nd(nb[i]) == pt_nd(in) ) {
	    maxn--;
	    for(j = i; j < maxn; j++) nb[j] = nb[j+1];
	    i--;
	}
    }

    /*error checks on the number of neighbors*/
    if ( mode == ONED ) {
	if (maxn != 2) panic("can't do node removal in one dimension");
    }
    else if ( mode == TWOD ) {
	if ( maxn < 3 ) {
	    while ( num_tri_nd(in) > 0 ) rem_ele( tri_nd(in, 0), FALSE );
	    return;
	}

	/*compute the angular dependence*/
	for(i = 0; i < maxn; i++) {
	    vec[0] = cordinate(pt_nd(nb[i]),0) - cordinate(pt_nd(in),0);
	    vec[1] = cordinate(pt_nd(nb[i]),1) - cordinate(pt_nd(in),1);
	    ang[i] = atan2( vec[1], vec[0] );
	}

	/*bubble sort them into order*/
	for(i = 0; i < maxn; i++) {
	    for(j = i+1; j < maxn; j++) {
		if (ang[j] < ang[i]) {
		    tf = ang[i];
		    ang[i] = ang[j];
		    ang[j] = tf;
		    tn = nb[i];
		    nb[i] = nb[j];
		    nb[j] = tn;
		}
	    }
	}
    }

    /*build a skeleton from the list of nodes*/
    sr = cr_sreg(mat_nd(in));
    for(i = 0; i < maxn-1; i++) {
	e = mk_edg( nb[i], nb[i+1] );
	ad_edge(sr, e, sreg[sr]->bnd, MAYBE, BEFORE);
    }

    /*don't loop back around in one dimension*/
    if ( mode != ONED ) {
	e = mk_edg( nb[maxn-1], nb[0] );
	ad_edge(sr, e, sreg[sr]->bnd, MAYBE, BEFORE);
    }
    hinit(sr, MAXFLOAT);

    /*retriangulate the region*/
    reg = reg_tri(tri_nd(in,0));
    if (pl_interface(in,&reg1,&reg2) > 1)
       if (reg1 > reg2) reg = reg1;
       else reg = reg2;
    grid( sr, reg, FALSE, FALSE );

    /*kill triangles until none are left*/
    while ( num_tri_nd(in) > 0 ) rem_ele( tri_nd(in, 0), FALSE );
    free_skel(sr);
}

	

/*-----------------TROTATE----------------------------------------------
 * Tricky little routine to walk around a boundary edge looking
 * for the next node.
 *----------------------------------------------------------------------*/
trotate( ie, iv, clock, Pje, Poje, Pj, Pjj)
     int ie;			/* The triangle */
     int iv;			/* The index of the boundary node */
     int clock;			/* 1 for clockwise, 0 for cc */
     int *Pje;			/* The outside neighbor (loop-breaker) */
     int *Poje;			/* The inside neighbor */
     int *Pj;			/* index of rotation node in oje */
     int *Pjj;			/* index of leading node or trailing triangle */
{
    int j, jj, je, oje, delta, ir=tri[ie]->regnum, in=tri[ie]->nd[iv];
    
    /*
     * Rotate around given node (in) until we
     *  a: come back to where we started    -or-
     *  b: hit an interface.
     *  Loop invariants: tri[oje]->nd[j] is the pivot (in)
     *  		 tri[oje]->nb[jj] precedes oje.
     *                   je succeeds oje
     *  The neighbors must be correct for this to work.
     */

    delta = clock? 1 : 2;
    for (oje=ie, j = iv, jj = (j+delta)%3; ; ) {
	 
	 je = tri[ oje]->nb[ 3-j-jj];
	 if ((je < 0) || (je == ie) || (tri[ je]->regnum != ir)) break;

	 jj = twhich (je, oje);
	 j  = nwhich (je, in);
	 oje = je;
     }

    *Pjj = jj;
    *Pj = j;
    *Poje = oje;
    *Pje = je;
}
          
