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
/*   dbase.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:35 */

#include <stdlib.h>

#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include "skel.h"
#include "material.h"
#include "regrid.h"

/*-----------------CR_REG-----------------------------------------------
 *Create a new skeleton region, return the index
 *----------------------------------------------------------------------*/
int cr_sreg (mat)
int mat;	/* Material number of region. */
{
    int ir = nsreg;

    if (nsreg >= MAXREG) panic("Region table overflow");

    sreg[ir] = salloc(struct sreg_str, 1);

    sreg[ir]->len = 0;
    sreg[ir]->mat = mat;
    sreg[ir]->maxa = sreg[ir]->mina = sreg[ir]->bnd = 0;
    nsreg ++;

    return(ir);
}

/*-----------------CR_TRI-----------------------------------------------
 * Create a new triangle.
 *----------------------------------------------------------------------*/
cr_tri (rn, ir)
int rn;		/* Region it belongs to. */
int ir;		/*skeleton region we are using*/
{
    struct LLedge *bnd = sreg[ir]->bnd;
    int nv[3];
    int nb[3];
    int nt;

    if ( sreg[ir]->len != 3 )
	panic("internal error:non-triangle passed to cr_tri");

    nv[0] = nB(bnd->prev);
    nv[1] = nB(bnd);
    nv[2] = nB(bnd->next);

    /*check for the edges*/
    nb[0] = nb[1] = nb[2] = BC_OFFSET;
    if ( is_surf(pt_nd(nv[1])) && is_surf(pt_nd(nv[2])) ) nb[0] = EXPOSED;
    if ( is_surf(pt_nd(nv[0])) && is_surf(pt_nd(nv[2])) ) nb[1] = EXPOSED;
    if ( is_surf(pt_nd(nv[1])) && is_surf(pt_nd(nv[0])) ) nb[2] = EXPOSED;

    nt = mk_ele( 3, nv, 3, nb, rn, FALSE );

    add_ele_edg(bnd->prev->edge, nt);
    add_ele_edg(bnd->edge, nt);
    add_ele_edg(bnd->next->edge, nt);

    set_edg_ele(nt, 0, bnd->edge);
    set_edg_ele(nt, 1, bnd->next->edge);
    set_edg_ele(nt, 2, bnd->prev->edge);

    return( nt );
}

/*-----------------AD_EDGE----------------------------------------------
 * Add edge to region around edge lep.
 *----------------------------------------------------------------------*/
ad_edge (ir, ie, lep, iscc, pos)
   int ir;		/* Region to add to. */
   int ie;		/* Edge to add. */
   struct LLedge *lep;	/* Edge to enter around */
   int iscc; 		/* Does  this edge conform with region order? */
   int pos;		/* Whether before or after lep. */
{
   struct LLedge *new,*first,*last;

  /*...Check against bad call. */
   if (!sreg[ir] || !edg[ie]) panic("Bad args to ad_edge");
   if (sreg[ir]->bnd && !lep) panic("No position given to ad_edge");

  /*...Create new link */
   new = salloc( struct LLedge, 1 );

   new->edge = ie;
   new->iscc = iscc;
   new->ang  = -999;
   new->gt = new->lt = 0;

   /*...Include in linked list. */
    if (sreg[ir]->bnd == 0) {     	/* Initialize a region. */
       sreg[ir]->bnd = new->next = new->prev = new;
       }
    else {				/* Insert into existing region */
	first = (pos==BEFORE)? lep : lep->next;
	last =  first->prev;

	last->next = new;	new->prev  = last;
	new->next  = first;	first->prev = new;
       }
    sreg[ir]->len++;

    /*add region to this edges list*/
    add_skel_edg(ie, ir);

    return;
}


/*-----------------EINDEX-----------------------------------------------
 * Find pointer from a region to an edge - simple linear search.
 * Returns 0    (bad region or couldn't find it)
 *         pointer (usually)
 *----------------------------------------------------------------------*/
struct LLedge * eindex (ir, ie)
    int ir;	/* Region to search */
    int ie;	/* Edge to find. */
{
    struct LLedge *bp;

    bp = sreg[ir]->bnd;
    do {
	if (bp->edge==ie) return(bp);
	bp = bp->next;
    }
    while (bp != sreg[ir]->bnd);

    return(0);	/* No luck if we got here. */
}


/*-----------------ADD_ANG----------------------------------------------
 * Add new value to sorted list of region angles.
 * If the new angle is smaller than all previous, no search is done.
 *----------------------------------------------------------------------*/
int add_ang (r, lep)
    struct sreg_str *r;	/* Region to update. */
    struct LLedge *lep; /* New edge to add. */
{
    struct LLedge *mina,*ep,*less,*greater;
    int f;

    if (r->maxa == 0) {	/* New list ? */
	r->maxa = lep;
	lep->gt = lep->lt = lep;
	}

    else {			/* Old list. */
      /*...Walk around the region in order of increasing angle. */
	mina=r->maxa->gt;
	for (f = 1 , ep = mina; (ep != mina) || f; ep = ep->gt , f = 0)
	    if (ep->ang >= lep->ang) break;

      /*...Link in new value. */
	greater = ep;
	less = ep->lt;

	lep->gt = greater;
	lep->lt = less;
	less->gt = lep;
	greater->lt = lep;

      /*...If we got a new maximum, update maxa. */
	if (lep->ang > r->maxa->ang)
	    r->maxa = lep;

	}
    return;
}





/************************************************************************
 *									*
 *	triangle find - this routine gets the materials and t numbers	*
 * for a given triple point.						*
 *									*
 ************************************************************************/
trip_tri( p, t1, m1, t2, m2 )
int p;
int *t1, *m1, *t2, *m2;
{
    int i, j, m[2], n[2], mat, t, ft, nb;

    /*find the two non-gas materials*/
    for(j = i = 0; i < pt[p]->nn; i++) {
	mat = nd[ pt[p]->nd[i] ]->mater;
	if ( mat != GAS ) { m[j] = mat; n[j] = pt[p]->nd[i]; j++; }
    }

    /*find the triangle that is on the boundary*/
    for(ft = -1, i = 0; (i < num_tri_nd(n[0])) && (ft == -1); i++) {
	t = tri_nd(n[0], i);
	for(j = 0; j < nedg; j++) {
	    nb = tri[t]->nb[j];
	    if  ((nb > 0) && (mat_reg( reg_tri(nb) ) == m[1])) {
		ft = t;
	    }
	}
    }
    *t1 = ft;
    *m1 = m[0];

    /*find the triangle that is on the boundary*/
    for(ft = -1, i = 0; (i < num_tri_nd(n[1])) && (ft == -1); i++) {
	t = tri_nd(n[1], i);
	for(j = 0; j < nedg; j++) {
	    nb = tri[t]->nb[j];
	    if  ((nb > 0) && (mat_reg( reg_tri(nb)) == m[0])) {
		ft = t;
	    }
	}
    }
    *t2 = ft;
    *m2 = m[1];
}




/************************************************************************
 *									*
 *	triple_fix() - This routine splits up a triple point into	*
 *  one double and one triple that are separated by 10 angstroms.	*
 *									*
 *  Original:  MEL	5/88						*
 *									*
 ************************************************************************/
triple_fix(p, p2)
int p;		/*the triple point*/
int p2;		/*the other point*/
{
}
#ifdef foo
    register int i, j, k, lj, n, t, lt;
    double r;
    char *err;
    int newp, newn;

    /*find the triangle and edge for these guys*/
    for(t = -1, i = 0; (i < pt[p]->nn) && (t == -1); i++) {
	n = pt[p]->nd[i];
	for(j = 0; (j < num_tri_nd(n)) && (t == -1); j++) {
	    lt = tri_nd(n, j);
	    for(k = 0; k < nvrt; k++)
		if ( nd[ tri[lt]->nd[k] ]->pt == p2 ) t = lt;
	}
    }


    /*find the triangle edge we need to split*/
    for(i = 0; i < nvrt; i++){
	if ((nd[tri[t]->nd[i]]->pt != p) &&
	    (nd[tri[t]->nd[i]]->pt != p2)) lj = i;
    }

    if ( nd[tri[t]->nd[(lj+1)%3]]->pt == p )
	r = 0.05;
    else
	r = 0.95;

    split_edge(t, lj, r);

    /*move the new point to the end*/
    newp = np-1;

    /*add a gas node to this point...*/
    pt[newp]->flags = pt[newp]->flags | SURFACE;
}







#endif
