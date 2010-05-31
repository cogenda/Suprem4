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
/*   tridata.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:41:00 */

#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include "skel.h"

/*-----------------SP_REG-----------------------------------------------
 * Split region at top of stack (nsreg), leaving result
 * in regions nreg, nreg+1. Shorter half is at nreg+1, unless
 * bp_edge is non-null.  Then the region which contains that
 * edge is left on top.
 *----------------------------------------------------------------------*/
int sp_reg (lep1, lep2, bp_look)
	struct LLedge *lep1, *lep2;	/* Edges to link */
	struct LLedge *bp_look;
{
    int ie,ir,iR,f,enew,ltmp,llen,slen,lnreg,shreg,i;
    struct LLedge *bp,*bnd,*plep1,*plep2,*maxa,*less;
    struct sreg_str *parent;
    double intang();
    int fnd;

  /*...Make lep2-lep1 the shorter path. */
    if ( bp_look == NULL ) {
	for (llen=0,bp=lep2 ; bp != lep1; bp=bp->next) llen++;
	for (slen=0,bp=lep1 ; bp != lep2; bp=bp->next) slen++;
	if  (llen < slen) {
	    bp=lep1; lep1=lep2; lep2=bp;
	    ltmp=llen; llen=slen; slen=ltmp;
	}
    }
    else {
	fnd = FALSE;
	for (llen=0,bp=lep2 ; bp != lep1; bp=bp->next) llen++;
	for (slen=0,bp=lep1 ; bp != lep2; bp=bp->next) slen++;

	for (bp=lep2 ; bp != lep1; bp=bp->next) fnd = fnd || bp == bp_look;

	if  (fnd) {
	    bp=lep1; lep1=lep2; lep2=bp;
	    ltmp=llen; llen=slen; slen=ltmp;
	}
    }

  /*...Create new edge with ends of split. */
    enew = mk_edg(nB(lep1), nB(lep2));
    set( edg[enew], REGS );
    if (debug1) edge_pl(enew); 

  /*...Remove parent, create son and daughter regions */
    parent = sreg[nsreg-1];
    sreg[nsreg--]= NULL;
    lnreg = cr_sreg(parent->mat);
    shreg = cr_sreg(parent->mat);

  /*...Add new edge as root of new regions. */
    ad_edge(lnreg,enew,NULL,TRUE,BEFORE);
    ad_edge(shreg,enew,NULL,FALSE,BEFORE);

  /*...Splice old boundary into two new boundaries. */
    plep1 = lep1->prev;
    plep2 = lep2->prev;
    sreg[lnreg]->len = 1+llen;	/* 1+ because we didn't count the divider. */
    sreg[shreg]->len = 1+slen;

    sreg[lnreg]->bnd->next = lep2;
    sreg[lnreg]->bnd->prev = plep1;
    lep2->prev           = sreg[lnreg]->bnd;
    plep1->next          = sreg[lnreg]->bnd;

    sreg[shreg]->bnd->next = lep1;
    sreg[shreg]->bnd->prev = plep2;
    lep1->prev            = sreg[shreg]->bnd;
    plep2->next           = sreg[shreg]->bnd;

  /* 
   * Update region pointers of edges. 
   * Since the long region is where the old one was, skip it.
   */
    bnd = sreg[shreg]->bnd;
    for (bp = bnd->next; bp != bnd; bp=bp->next) {
	ie = bp->edge;
	for(i = 0; i < num_skel_edg(ie); i++) {
	    if (skel_edg(ie,i)==lnreg) skel_edg(ie,i) = shreg;
	}
    }

  /* 
   * Update angle lists.
   * A. Split list of angles, preserving order. Omit angle at
   *    beginning, end of divider.
   */
    maxa = parent->maxa;
    for (f=1, bp = maxa; (bp != maxa) || f; bp = less,f=0) {
	less = bp->lt;
	if (bp != lep1 && bp != lep2) {
	    ie = bp->edge;
	    for(i = 0; i < num_skel_edg(ie); i++) 
		if (skel_edg(ie,i) >= nsreg-2) ir = skel_edg(ie,i);
	    add_ang(sreg[ir],bp);
	}
    }

  /*...B. Compute new angles and add to lists in appropriate positions */
    for (ir = lnreg, iR = shreg ; ir <= shreg; ir++, iR--) {
	bnd = sreg[ir]->bnd;
	bnd->ang   = intang (nB(bnd->prev), nB(bnd), nB(bnd->next));
	sreg[iR]->bnd->next->ang -= bnd->ang;

	add_ang(sreg[ir], bnd);
	add_ang(sreg[iR], sreg[iR]->bnd->next);
	}

  /*...Finally, get rid of parent storage. */
    free(parent);
    return(enew);
}



/*-----------------DUPL-------------------------------------------------
 * DUPL : duplicate a region, leaving the new copy at the top of the stack.
 * The user region is duplicated before triangulating it becuase the 
 * triangulation recursively breaks whatever region it's working on into
 * smaller pieces, and we want the user regions to remain after we're done.
 *----------------------------------------------------------------------*/
int dupl(ir)
int ir;
{

    int rnew,f;
    struct LLedge *bnd, *bp;
    int ep;

  /*   
   *...A. Create new region.
   */
    rnew = cr_sreg (sreg[ir]->mat);

  /*...B. Duplicate edges and linked list. */
    bnd = sreg[ir]->bnd;
    for (f=1, bp = bnd; (bp != bnd) || f; bp = bp->next, f=0) {
	ep = bp->edge;

	/* Add edge to rnew. */
	ad_edge(rnew, ep, sreg[rnew]->bnd, MAYBE, BEFORE);
    }

    ck_clock(rnew, TRUE);

}

