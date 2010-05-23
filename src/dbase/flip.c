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
/*   flip.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:46 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <math.h>

#include "global.h"
#include "constant.h"
#include "geom.h"


/*-----------------FLIP-------------------------------------------------
 * Triangle flipper - makes triangulation Delauney.
 * Returns number of triangles flipped or -1 for error.
 *----------------------------------------------------------------------*/
int flip()
{
    int it,j,count,flips=0,all_done,*todo,*done;
    struct tms before, after;
    char buf[100];
    int yes;

    /*no flipping required in one dimension*/
    if ( mode != TWOD ) return(0);

    times(&before);

    /*...Need to remember which points have been done, which to do. */
    if (!(
	(todo = scalloc(int, 1+ned)) &&
	(done = scalloc(int, 1+ned))
       )) return(-1);

    for (it=0; it < ned; it++) todo[it] = TRUE;

  /*...Smoothing loop. */
    count = 0;
    do {
	all_done = TRUE;
	for (it=0; it < ned; it++) {
	    done[it] = !todo[it];
	    todo[it] = FALSE;
	    }

	for(it = 0; it < ned; it++) {
	    /*if we've checked it*/
	    if (done[it]) continue;

	    /*if it belongs only to one element*/
	    if (num_tri_edg(it) != 2) continue;

	    /*make sure the two elements are in the same region*/
	    if ( reg_tri(tri_edg(it,0)) != reg_tri(tri_edg(it,1)) ) continue;

	    if ( gimme_ehed(it) < 0 ) {
		if ((yes = ck_flip(it)) != 1) continue;

		tri_flip( it, tri_edg(it,0), tri_edg(it,1) );
		flips++;
		all_done = FALSE;

		/*mark all neighbor edges as needing checking*/
		for(j = 0; j < num_edge_nd( nd_edg(it,0) ); j++)
		    todo[ edge_nd( nd_edg(it,0), j) ] = TRUE;
		for(j = 0; j < num_edge_nd( nd_edg(it,1) ); j++)
		    todo[ edge_nd( nd_edg(it,1), j) ] = TRUE;
	    }
	}
    } while (!all_done && ++count < 30);

    free(todo);	free(done);

    times(&after);
    sprintf(buf, "obtuse triangle flip (%d)", flips);
    print_time( buf, &before, &after);
    return(flips);
}




/*-----------------DO_FLIP----------------------------------------------
 * Mechanics of flip.
 *----------------------------------------------------------------------*/
tri_flip( it, tp, tq )
int it, tp, tq;
{
    int i, n1, n2, jp, jq, tr;
    int sp[4], sn[6], se[6], fixme;

    /*...Find which points are unique. */
    jp = twhich (tp, tq);
    jq = twhich (tq, tp);

    /*get the four nodes the form the quad*/
    sp[0] = vert_tri(tp, jp);
    sp[1] = nd_edg(it,0);
    sp[2] = vert_tri(tq, jq);
    sp[3] = nd_edg(it,1);

    /*change the edge in question*/
    sub_edge_nd(nd_edg(it,0), it );
    sub_edge_nd(nd_edg(it,1), it );
    n1 = vert_tri(tp, jp);
    edg[it]->nd[0] = n1;
    add_edge_nd(n1, it);
    n2 = vert_tri(tq, jq);
    edg[it]->nd[1] = n2;
    add_edge_nd(n2, it);

    /*...Copy points out - order is guaranteed to remain c-clockwise. */
    sn[0] = tri[tq]->nb[nwhich(tq,sp[3])];
    sn[1] = tq;
    sn[2] = tri[tp]->nb[nwhich(tp,sp[3])];
    sn[3] = tri[tp]->nb[nwhich(tp,sp[1])];
    sn[4] = tp;
    sn[5] = tri[tq]->nb[nwhich(tq,sp[1])];

    se[0] = tri[tq]->edg[nwhich(tq,sp[3])];
    se[1] = it;
    se[2] = tri[tp]->edg[nwhich(tp,sp[3])];
    se[3] = tri[tp]->edg[nwhich(tp,sp[1])];
    se[4] = it;
    se[5] = tri[tq]->edg[nwhich(tq,sp[1])];

    /*...and back in. */
    for (i=0; i<3; i++) {
	tri[tp]->nd[i] = sp[i];
	tri[tq]->nd[i] = sp[(i+2)%4];
	tri[tp]->nb[i] = sn[i];
	tri[tq]->nb[i] = sn[i+3];
	tri[tp]->edg[i] = se[i];
	tri[tq]->edg[i] = se[i+3];
    }

    /*update the edge to element pointers*/
    sub_ele_edg(se[3], tp);
    sub_ele_edg(se[0], tq);
    add_ele_edg(se[0], tp);
    add_ele_edg(se[3], tq);


    /*update the neighbors of the triangles*/
    for (tr=tp; tr==tp || tr==tq; tr += tq-tp) { /* for each of tp, tq */
	if ((fixme = tri[tr]->nb[0]) < 0) continue;
	for (i = 0; i < 3; i++)
	    if (tri[fixme]->nb[i] == tp+tq-tr) tri[fixme]->nb[i] = tr;
    }

    /*now update the nd to tri list*/
    add_tri_nd(n1, tq);
    add_tri_nd(n2, tp);
    sub_tri_nd(vert_tri(tp, twhich(tp,tq)), tq);
    sub_tri_nd(vert_tri(tq, twhich(tq,tp)), tp);

    /*repair the geometry*/
    sclk_tri(tp);
    do_geom(tp);
    sclk_tri(tq);
    do_geom(tq);


}


/*-----------------CK_FLIP----------------------------------------------
* Check the right situation for flip.
* Returns 1 for flipped or -1 for error.
*----------------------------------------------------------------------*/
int ck_flip(we)
int we;
{
    int t1, t2, ix1, ix2;
    int n1, n2, n3, n4, p1, p2, p3, p4;
    double xsym, ysym, sgn1, sgn2, sgn;
    double slope, cut_p;

    t1  = tri_edg(we,0);
    t2  = tri_edg(we,1);
    ix1 = ewhich(t1,we);
    ix2 = ewhich(t2,we);

    n1 = vert_tri(t1,ix1);
    n2 = vert_tri(t2,ix2);
    n3 = nd_edg(we,0);
    n4 = nd_edg(we,1);

    p1 = pt_nd(n1);
    p2 = pt_nd(n2);
    p3 = pt_nd(n3);
    p4 = pt_nd(n4);

    if (xcord(p1) == xcord(p2)) {
       xsym = xcord(p1);
       sgn1 = xcord(p3) - xsym;
       sgn2 = xcord(p4) - xsym;
       sgn  = sgn1 * sgn2;
    }
    else if (ycord(p1) == ycord(p2)) {
       ysym = ycord(p1);
       sgn1 = ycord(p3) - ysym;
       sgn2 = ycord(p4) - ysym;
       sgn  = sgn1 * sgn2;
    }
    else {
       slope = (ycord(p1) - ycord(p2))/(xcord(p1) - xcord(p2));
       cut_p = xcord(p1) * ycord(p2) - xcord(p2) * ycord(p1);
       cut_p /= (xcord(p1) - xcord(p2));

       sgn1 = ycord(p3) - (slope * xcord(p3) + cut_p);
       sgn2 = ycord(p4) - (slope * xcord(p4) + cut_p);
       sgn  = sgn1 * sgn2;
    }

    if (sgn < 0) return(1);
    else return(0);
}




