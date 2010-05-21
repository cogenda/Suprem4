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
/*   sp_edge.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:53 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "material.h"
#include "skel.h"
extern char *alloc_pt(), *alloc_nd(), *alloc_tri();

#define DMIN(A,B) (A<B)?(A):(B)
#define DMIN3(A,B,C) DMIN( A, DMIN( B, C) )

#define MIN_GRID_SPACE 2.5e-8


/*-----------------SP_EDGE----------------------------------------------
 * This does the database grunt work of adding one new point to an edge.
 *----------------------------------------------------------------------*/
int sp_edge(ie,c,ip,mv)
int ie;		/* Edge to split */
float c[MAXDIM];	/*place to split*/
int *ip;		/* New point*/
int mv;
{
    double l1, l2;

    if ( ask( edg[ie], REGS ) ) mv = FALSE;
    
    l1 = len_edg(ie);
    l2 = dist(cord_arr(pt_edg(ie,0)), c);

    /*if either is a triple point, don't move them*/
    if ( (num_nd(pt_edg(ie,0)) > 2) || (num_nd(pt_edg(ie,1)) > 2 ) ) {
	if (mv && (l2 / l1 > 0.95)) {
	    move_edge( ie, 1, c );
	    *ip = pt_edg(ie,1);
	    return(-1);
	}
	else if ( mv && (l2 / l1 < 0.05) ) {
	    move_edge( ie, 0, c );
	    *ip = pt_edg(ie,0);
	    return(-2);
	}
	else if ( l2 < MIN_GRID_SPACE ) {
	    move_edge( ie, 0, c );
	    *ip = pt_edg(ie,0);
	    return(-2);
	}
	else if ( (l1-l2) < MIN_GRID_SPACE ) {
	    move_edge( ie, 1, c );
	    *ip = pt_edg(ie,1);
	    return(-1);
	}
	else
	    return(sp_gredg(ie,c,ip));
    }
    else {
	if (mv && (l2 / l1 > 0.75)) {
	    move_edge( ie, 1, c );
	    *ip = pt_edg(ie,1);
	    return(-1);
	}
	else if ( mv && (l2 / l1 < 0.25) ) {
	    move_edge( ie, 0, c );
	    *ip = pt_edg(ie,0);
	    return(-2);
	}
	else if ( l2 < MIN_GRID_SPACE ) {
	    move_edge( ie, 0, c );
	    *ip = pt_edg(ie,0);
	    return(-2);
	}
	else if ( (l1-l2) < MIN_GRID_SPACE ) {
	    move_edge( ie, 1, c );
	    *ip = pt_edg(ie,1);
	    return(-1);
	}
	else {
	    /*create the new point and edge*/
	    return(sp_gredg(ie,c,ip));
	}
    }
}



sp_gredg(ie, c, p)
int ie;		/* Edge to split */
float c[MAXDIM];	/*place to split*/
int *p;		/* New point*/
{
    int ip, in;
    int eoth, en1;

    eoth = insert_pt_edg( ie, c, p );
    ip = *p;

    /*get the current edge ends into local storage*/
    in = node_mat(nd_pt(ip,0), mat_nd(nd_edg(ie,0)));

    /*update this edge and its neighbors*/
    en1 = upd_edg( ie, in );

    if ( eoth != -1 ) {
	in = node_mat(nd_pt(ip,0), mat_nd(nd_edg(eoth,0)));

	/*update this edge and its neighbors*/
	(void)upd_edg( eoth, in );
    }
    return(en1);
}


upd_edg( ie, in )
int ie;
int in;
{
    int i, enew;
    int n1;
    struct list_str tneigh;

    tneigh.num = 0;
    tneigh.all = 0;
    tneigh.list = NULL;

    /*get the node ends into local*/
    n1 = nd_edg(ie,1);

    /*create the new edge*/
    enew = mk_edg(in, n1);
    edg[enew]->flags = edg[ie]->flags;

    /*update the original edge*/
    sub_edge_nd(edg[ie]->nd[1], ie);
    edg[ie]->nd[1] = in;
    edg[ie]->len = l_edge(ie);
    add_edge_nd(in, ie);

    /*build a neighbor list*/
    for(i = 0; i < num_ele_edg(ie); i++) add_list( &tneigh, ele_edg(ie, i) );

    for(i = 0; i < tneigh.num; i++) new_edg_tri(ie, enew, tneigh.list[i]);
    dis_list(&tneigh);

    /*build a neighbor list*/
    for(i = 0; i < num_skel_edg(ie); i++) add_list( &tneigh, skel_edg(ie, i) );

    for(i = 0; i < tneigh.num; i++) new_edg_skel(ie, enew, tneigh.list[i]);
    dis_list(&tneigh);

    return(enew);
}


move_edge(ie, ind, c)
int ie, ind;
float *c;
{
    int i;

    /*move the point to the given new cordinates*/
    for(i = 0; i < mode; i++) set_cord(pt_edg(ie,ind),i,c[i]);
    edg[ie]->len = l_edge(ie);
}


/*create the new point on the edge, and split the edge*/
insert_pt_edg( e, c, p )
int e;		/* Edge to split */
float c[MAXDIM];	/*place to split*/
int *p;		/* New point*/
{
    int ip, eoth, etst;
    double ha;
    int in, n1, p1, p2, tn;
    int i,j;
    double rat, irat;

    *p = ip = mk_pt( 2, c );
    ha = DMIN( pdist(ip, pt_edg(e,0)), pdist(ip, pt_edg(e,1)) );
    set_space(ip, ha);
    if ( ask(edg[e], ESURF) ) {
	set_surf(ip);
	in = mk_nd(ip, GAS);
    }
    if ( ask(edg[e], EBACK) ) {
	set_back(ip);
	in = mk_nd(ip, GAS);
    }
    in = mk_nd(ip, mat_nd(nd_edg(e,0)));	

    /*interpolate the solutions*/
    rat = pdist(ip, pt_edg(e,0)) / pdist(pt_edg(e,0), pt_edg(e,1));
    irat = 1.0 - rat;
    for(i = 0; i < num_sol(in); i++)
	set_sol_nd(in,i, rat*sol_nd(nd_edg(e,1),i)+irat*sol_nd(nd_edg(e,0),i));

    /*is this a boundary edge?*/
    p1 = pt_edg(e,0);
    p2 = pt_edg(e,1);
    n1 = nd_edg(e,0);
    eoth = -1;
    for(i = 0; (i < num_nd(p1)) && (eoth == -1); i++) {
	if ( (tn = nd_pt(p1,i)) != n1 ) {
	    /*check all the edges at this node*/
	    for(j = 0; (j < num_edge_nd(tn)) && (eoth == -1); j++) {
		etst = edge_nd(tn,j);
		if ( pt_edg(etst, (pt_edg(etst,0)==p1)?1:0) == p2 ) {
		    /*found it!*/
		    eoth = etst;
		}
	    }
	}
    }
    if ( eoth != -1 ) {
	in = mk_nd(ip, mat_nd(nd_edg(eoth,0)));	
	
	/*interpolate the solutions*/
	rat = pdist(ip, pt_edg(eoth,0)) / pdist(pt_edg(eoth,0), pt_edg(eoth,1));
	irat = 1.0 - rat;
	for(i = 0; i < num_sol(in); i++)
	    set_sol_nd(in,i, 
		 rat*sol_nd(nd_edg(eoth,1),i)+irat*sol_nd(nd_edg(eoth,0),i));
    }

    return(eoth);
}


new_edg_skel(ie,enew,ir)
int ie;		/* Original edge*/
int enew;	/* new edge*/
int ir;		/* Skeleton */
{
    struct LLedge *lep;

    lep = eindex(ir, ie);
    if (lep == NULL) panic("internal error: edge not in region (sp_edge)");

    /*...Add new edge after old, so that old retains its angle. */
    /*...This makes it necessary to fix clockwise edges.        */
    ad_edge(ir, enew, lep, lep->iscc, AFTER); 

    if (!lep->iscc) {
	lep->edge = enew;
	lep->next->edge = ie;
    }

    lep->next->ang = PI;
    add_ang (sreg[ir], lep->next);

}




int new_edg_tri(e,enew,it)
int e;		/* Edge to split */
int enew;
int it;
{
    int i, t1, t2;
    int nopp, nshare;
    int eshare, nopploc, etmp;
    int n[MAXVRT];

    if ( mode == ONED ) {

	/*triangles are same as edges in oned*/
	n[0] = nd_edg(e,0);
	n[1] = nd_edg(e,1);
	t1 = mk_ele_nd( 2, n, reg_tri(it) );
	sclk_tri(t1);
	do_geom(t1);
	add_ele_edg(e,t1);

	n[0] = nd_edg(enew,0);
	n[1] = nd_edg(enew,1);
	t2 = mk_ele_nd( 2, n, reg_tri(it) );
	sclk_tri(t2);
	do_geom(t2);
	add_ele_edg(enew,t2);

    }
    else if (mode == TWOD) {

	/*find the node opposite the new edges*/
	for(nopp = -1, i = 0; i < num_vert(it) && nopp == -1; i++) {
	    if ( (vert_tri(it,i) != nd_edg(e,0)) &&
		 (vert_tri(it,i) != nd_edg(e,1)) &&
		 (vert_tri(it,i) != nd_edg(enew,0)) &&
		 (vert_tri(it,i) != nd_edg(enew,1)) ) {
		     nopp = vert_tri(it,i);
		     nopploc = i;
		}
	}

	/*figure which is the shared node*/
	if ( (nd_edg(e,0) == nd_edg(enew,0)) ||
	     (nd_edg(e,0) == nd_edg(enew,1)) ) 
	    nshare = nd_edg(e,0);
	else 
	    nshare = nd_edg(e,1);

	/*make a new edge for the nshare, nopp pair*/
	eshare = mk_edg( nopp, nshare );

	/*now build some triangles*/
	n[0] = nopp;
	n[1] = nshare;
	n[2] = (nd_edg(e,1)==nshare)?(nd_edg(e,0)):(nd_edg(e,1));
	t1 = mk_ele_nd( 3, n, reg_tri(it) );

	/*figure the three edges*/
	set_edg_ele(t1, 2, eshare);
	set_edg_ele(t1, 0, e);
	if ( vert_tri(it, (nopploc+1)%3) == n[2] )
	    etmp = edg_ele(it, (nopploc+2)%3);
	else
	    etmp = edg_ele(it, (nopploc+1)%3);
	set_edg_ele(t1, 1, etmp);
	sclk_tri(t1);
	do_geom(t1);
	add_ele_edg(eshare,t1);
	add_ele_edg(e,t1);
	add_ele_edg(etmp,t1);

	n[0] = nopp;
	n[1] = nshare;
	n[2] = (nd_edg(enew,1)==nshare)?(nd_edg(enew,0)):(nd_edg(enew,1));
	t2 = mk_ele_nd( 3, n, reg_tri(it) );
	set_edg_ele(t2, 2, eshare);
	set_edg_ele(t2, 0, enew);
	if ( vert_tri(it, (nopploc+1)%3) == n[2] )
	    etmp = edg_ele(it, (nopploc+2)%3);
	else
	    etmp = edg_ele(it, (nopploc+1)%3);
	set_edg_ele(t2, 1, etmp);
	sclk_tri(t2);
	do_geom(t2);
	add_ele_edg(eshare,t2);
	add_ele_edg(enew,t2);
	add_ele_edg(etmp,t2);
    }
    for(i = 0; i < num_edge(it); i++) sub_ele_edg(edg_ele(it,i),it);
    fing_tri(it);
    clkws_dirty = neigh_dirty = TRUE;
}
