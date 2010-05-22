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
/*   triang.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:58 */

#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "constant.h"
#include "dbaccess.h"

double dmin(X,Y) double X,Y; {return(X<=Y? X : Y);}
double dmax(X,Y) double X,Y; {return(X>=Y? X : Y);}
double dabs(X) double X; {return((X >= 0)? X : -X);}

/*-----------------TRIANG-----------------------------------------------
 * Triangulate a region. The core routine.
 *----------------------------------------------------------------------*/
triang(ir, rbn, bflag, spflag)
int ir;
int rbn;
int bflag;
int spflag;
{
    int ie,f,save_nreg,divide(),split,nr;
    struct LLedge *bp,*nbp,*chop(),*quadsplit();
    double l_edge();
    struct sreg_str *r;

    mgeom = 0.6;
    debug1 = FALSE;
    debug2 = FALSE;
    ck_clock(ir, TRUE);

  /*
   * Save values of nreg Storage beyond original
   * limits is used for temporary stack.
   */
    save_nreg = nsreg;

  /*
   * Copy region to working area (don't want to hack user regions)
   */
    dupl(ir);

    while( (sreg[nsreg-1]->len > 5) && rect_div(sreg[nsreg-1], &bp, &nbp) ) {
	ie = sp_reg (bp, nbp, bp);
	split = TRUE;
	/*work on the shorter region, which on the stack top*/
	dvrecedg( sreg[nsreg-1], sreg[nsreg-2], ie, bp );

	/*the top region needs triangles generated*/
	rect_tri( nsreg-1, rbn );
	free_skel(nsreg-1);
    }

    /*this routine makes nice grid along the boundary*/
    if ( (sreg[nsreg-1]->len > 5) && bflag ) {
	nr = nsreg-1;
	for(f=1, bp=sreg[nr]->bnd; (bp != sreg[nr]->bnd)||f; f=0, bp=bp->next)
	    if ( ask(edg[bp->edge], ESURF ) ) div_edg(bp->edge);
    }

  /*...Triangulation loop */
    split=0;
    while (nsreg > save_nreg) {

	nr = nsreg - 1;
	if (debug2) reg_pl(nr);

      /*
       * If the next region is a triangle, save it.
       * The triangle gets region number ir.
       */
	if (sreg[nr]->len == 3) {
	    (void)cr_tri(rbn, nr);

	    free_skel( nr );
	    split = FALSE;
	    }

      /*...Special-case quadrilaterals */
	else if (sreg[nr]->len == 4) {
	    bp = quadsplit(sreg[nr]);
	    if (!bp)
		panic("Error in quadsplit");
	    ie = sp_reg (bp->prev, bp->next, NULL);
	    split = TRUE;
	}

      /*...Cut off an acute triangle? */
	else if (bp = chop (sreg[nr], FALSE)) {
	    ie = sp_reg (bp->prev, bp->next, NULL);
	    split = TRUE;
	    }

      /*...Cut it in half? */
	else if (divide(sreg[nr],&bp,&nbp)) {
	    ie = sp_reg (bp, nbp, NULL);
	    if (spflag) div_edg( ie );
	    split = TRUE;
	    }

      /*
       * Well nothing else worked, so we cut off the least awful
       * triangle we can.
       */
	 else {
	    bp = chop(sreg[nr],TRUE);
	    if (!bp)
		panic("error in chop");
	    ie = sp_reg (bp->prev, bp->next, NULL);
	    split = TRUE;
	    }

      if (debug2 && split) {
	  reg_pl(nr);
	  reg_pl(nr);
      }
    }

    return;
}



/*-----------------CK_CLOCK---------------------------------------------
 * Initialize and check counter-clockwise structures for region # ir.
 *----------------------------------------------------------------------*/
int ck_clock(ir, fl)
int ir;
int fl;
{
    struct LLedge *ep,*tmp,*bnd,**ptrs;
    struct edg_str *enext,*eprev;
    int f,acomp(),mod(),n1,n0,no,io;
    double tang,intang();
    static char err[80];
    int cnt = 0;

    /* A. generate iscc = whether the edge parallels the region. */


    bnd = sreg[ir]->bnd;
    for(f=1,ep=bnd; (ep!=bnd) || f; ep=ep->next, f=0) {
	n1 = nd_edg(ep->edge,1);
	n0 = nd_edg(ep->edge,0);
	enext = edg[ep->next->edge];
	eprev = edg[ep->prev->edge];
	if (((n1==enext->nd[1]) || (n1==enext->nd[0])) &&
	    ((n0==eprev->nd[1]) || (n0==eprev->nd[0])))
	   ep->iscc = TRUE;

	if (((n1==eprev->nd[1]) || (n1==eprev->nd[0])) &&
	    ((n0==enext->nd[0]) || (n0==enext->nd[1])))
	   ep->iscc = FALSE;

	if ((ep->iscc != FALSE) && (ep->iscc != TRUE)) {
	    if ( mode == TWOD ) {
		sprintf(err, "Region discontinuous around edge %d",ep->edge);
		panic(err);
	    }
	    else {
		cnt++;
	    }
	}
    }

    if ( mode == ONED ) {
	if ( (cnt != 0) && (cnt != 2) )
	    panic("Region discontinuous");
	else
	    return;
    }

    if ( !fl ) return;


    /*
     * B. Calculate internal angles. Sum of external angles
     *    should be 2*pi.
     */
    tang = 0;
    for(f=1,ep=bnd ; (ep!=bnd) || f; ep=ep->next,f=0) {
	ep->ang = intang (nB(ep->prev), nB(ep), nB(ep->next));
	tang += PI - ep->ang;
	}

    if (debug2) printf("Region %d has angle %24.15lf\n",ir,tang);

    /* Fine if 2*PI */
    if (dabs(tang-2*PI) < EPS)
	; /* No complaints */


    /* Hopeless if zero. */
    else if (dabs(tang) < EPS) panic("Region crosses itself.");

    /* Fix if -2*PI */
    else if (dabs(tang+2*PI) < EPS) {
	for(f=1,ep=bnd; (ep!=bnd) || f; ep=ep->next,f=0) {
	    tmp = ep->next;		/* Swap pointers to make it c-c. */
	    ep->next = ep->prev;
	    ep->prev = tmp;
	    ep->iscc = !ep->iscc;	/* If it was cc before it isn't now */
	    ep->ang = 2*PI - ep->ang;
	    }
	}

    /* Huh? */
    else panic("Region has malformed boundary");

    /* C. Sort angles by size. */
    ptrs = salloc( struct LLedge *, ned );

    for (no=0,f=1,ep=bnd; (ep != bnd)|| f; ep=ep->next,f=0)
	ptrs[no++]=ep;

    qsort(ptrs, no, sizeof(struct LLedge *), acomp);

#define MOD(A,B) ((A)>=0)?((A)%(B)):((B)-1 - (-1-(A))%(B))
    sreg[ir]->maxa=ptrs[no-1];
    sreg[ir]->mina=ptrs[0];
    for (io=0; io<no ; io++) {
	ptrs[io]->gt = ptrs[MOD(io+1,no)];
	ptrs[io]->lt = ptrs[MOD(io-1,no)];
	}

    free(ptrs);
    return;

} /* End of ck_clock. */

/*
 * ACOMP - comparison routine for quicksort.
 */
int acomp (lep1, lep2)
    struct LLedge **lep1,**lep2;
{
    double v;
    v = (*lep1)->ang - (*lep2)->ang;
    if      (v<0)  return(-1);
    else if (v==0) return(0);
    else           return(1);
}



free_skel( nr )
{
    int f, i, r, e;
    struct LLedge *bp, *bnd, *nbp;
    int *new_sreg;

    new_sreg = salloc(int, nsreg);

    /*...And dispose of the region. */
    bnd = sreg[nr]->bnd;
    for (f=1, bp=bnd; (bp != bnd) || f; bp = nbp, f=0) {
	 nbp = bp->next;
	 sub_skel_edg(bp->edge, nr);
	 free(bp);
    }
    free(sreg[nr]);
    sreg[nr] = NULL;

    for(i = f = 0; f < nsreg; f++)
	if (sreg[f] != NULL) {
	    sreg[i] = sreg[f];
	    new_sreg[f] = i++;
	}
	else
	    new_sreg[f] = -1;
    nsreg = i;

    /*update region pointers*/
    for(r = 0; r < nreg; r++)
	if ( reg[r]->sreg != -1 ) reg[r]->sreg = new_sreg[reg[r]->sreg];

    /*update edge information*/
    for(e = 0; e < ned; e++) {
	for(i = 0; i < num_skel_edg(e); i++) {
	    r = skel_edg(e,i);
	    if ( new_sreg[r] != -1 ) edg[e]->skel.list[i] = new_sreg[r];
	}
    }

    sreg[nsreg] = NULL;
    free(new_sreg);
}
