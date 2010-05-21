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
/*   triheur.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:41:01 */

#include <math.h>
#include <stdio.h>
#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include "skel.h"

/*-----------------CHOP-------------------------------------------------
 * try to cut a triangle off a region.
 * this gets called twice, the first time searching for a good-looking
 * chop, the second time taking whatever we must.
 * Returns answer in the form of a pointer to the middle edge of the tri.
 *----------------------------------------------------------------------*/
struct LLedge * chop (r, must)
    struct sreg_str *r;		/* Region to split. */
    int must;			/* Is some split necessary? */
{
    struct LLedge *bestp,*bp,*mina, *bq;
    int f,i,j,k, l, triok;
    double h,h1,h2,lej,m,g,best,mgeo, angM,sk_cross(),good_tri(),dmin();
    float p[MAXDIM], dp[MAXDIM], q[MAXDIM], dq[MAXDIM], alp[MAXDIM];

    bestp = 0;
    best = 0;
    if (must) {	      /* If some split is necessary... */
	angM = PI-EPS;	  /* Consider any triangle angle up to PI */
	mgeo = EPS ;      /* Don't be fussy about geometry. */
    }
    else {
	angM = PI/2;	/* Consider acute triangles only. */
	mgeo = mgeom;
    }

    /*...Start with smallest angles in region and work upwards. */
    mina = r->maxa->gt;
    for (f=1,bp = mina; (bp != mina) || f; bp = bp->gt, f=0) {
	if (bp->ang > angM) break;

	/*...Is the triangle worth anything? */
	i = nB(bp->prev); j = nB(bp); k = nB(bp->next);

	/*get the vector of the split*/
	p[0] = xcord(pt_nd(i));		p[1] = ycord(pt_nd(i));
	dp[0] = xcord(pt_nd(k)) - p[0];	dp[1] = ycord(pt_nd(k)) - p[1];
	q[0] = xcord(pt_nd(j));		q[1] = ycord(pt_nd(j));

	/*test all other vertices*/
	for(triok=TRUE, bq=bp->next; triok && (nF(bq) != i); bq = bq->next) {
	    l = nF(bq);

	    /*get the vector from the middle to this node*/
	    dq[0] = xcord(pt_nd(l)) - q[0];	dq[1] = ycord(pt_nd(l)) - q[1];

	    /*if parallel, no problem...*/
	    if (lil(p,dp,q,dq,alp)) continue;

	    /*if crossing isn't within the arc at middle*/
	    if ( (alp[0] < -EPS) || (alp[0] > 1.0+EPS) ) continue;

	    /*if node is inside triangle, reject*/
	    if ( alp[1] > (1.0-EPS) ) triok = FALSE;
	}

	if (triok) {
	    if ((g=good_tri(i,j,k)) > mgeo) {

		/*...Is it too big? */
		lej = ndist(i,k);
		h1=dmin(get_space(pt_nd(i)),lej)/dmax(get_space(pt_nd(i)),lej);
		h2=dmin(get_space(pt_nd(k)),lej)/dmax(get_space(pt_nd(k)),lej);

		if ((h=dmin(h1,h2)) > mgeo) {

		    /*...Got one! If it passed the strict test, it's good.
		     *...Otherwise keep looking for something better. */
		    if (!must) {
			return(bp);
		    }
		    else {
			m = dmin(h,g);
			if (m > best) {
			    best = m;
			    bestp = bp;
			    }
			}
		    }
		}
	    }
	}
    return(bestp);
}

/*-----------------DIVIDE-----------------------------------------------
 * Choose good ways to cut a region in half.
 * Returns pointers to the edge following each end of the cut.
 *----------------------------------------------------------------------*/
int divide(r, lep1, lep2)
struct sreg_str *r;
struct LLedge **lep1,**lep2;
{
    struct LLedge *bp1,*bp2,*maxa;
    double piby2, piby4, ang1, ang2, intang(), sk_cross();
    int f1,f2;

    piby2 = PI/2;
    piby4 = PI/4;


  /*...Try every combination of large angles. */
    maxa = r->maxa;
    for (f1=1, bp1=maxa;   ((bp1 != maxa) || f1) && bp1->ang > piby2;
	 					bp1=bp1->lt, f1=0) {

	for (f2=1, bp2=maxa; ((bp2 != maxa) || f2) && bp2->ang > piby2;
						bp2=bp2->lt, f2=0) {

	    if (bp2==bp1 || bp1==bp2->next || bp2==bp1->next) continue;

	  /*
	   * Check dividing line is inside the region, and
	   * angles are not too small.
	   */
	    ang1 = intang (nB(bp1->prev), nB(bp1), nB(bp2));
	    ang2 = intang (nB(bp2->prev), nB(bp2), nB(bp1));
	    if (ang1 < piby4 || ang2 < piby4) continue;
	    if (bp1->ang - ang1 < piby4 || bp2->ang - ang2 < piby4) continue;

	  /*...Check for [near] crossings */
	    if (sk_cross(r, bp1, bp2, FALSE) < 0.5) continue;

	  /*...Hey, must have found one. */
	    *lep1 = bp1;
	    *lep2 = bp2;
	     return(1);
	     }
	}
  /*...No luck if here. */
  return(0);
}

/*-----------------CROSS------------------------------------------------
 * Evaluates "closeness" of region nodes to given segment.
 * Return value is the closest node divided by a measure of the local
 * spacing. The bigger this value, the better.
 * -1 means crossing.
 *----------------------------------------------------------------------*/
double sk_cross(r, lp1, lp2, rect)
struct sreg_str *r;
struct LLedge *lp1,*lp2;
int rect; 		/*flag for rectangular checking*/
{
    struct LLedge *bp, *end;		/* Walks around the region. */
    struct edg_str split;		/* Line joining nB(lp1) and nB(lp2) */
    float alph[MAXDIM],pval[MAXDIM];	/* Present & prev projection of node on split */
    double sign,lej,near,h1,h2,hmax,ratio,first;
    double inter;
    int nnew;


    split.nd[0] = nB(lp1);
    split.nd[1] = nB(lp2);
    lej = ndist (nB(lp1), nB(lp2));
    dvpram (get_space(pt_nd(nB(lp1))), get_space(pt_nd(nB(lp2))),
		     lej, &nnew, &ratio, &first);
    near = MAXFLOAT;

    pval[0] = 0.5;	pval[1] = 1;
    end = lp1;
    for (sign=1, bp=lp1->next; bp != end ; bp=bp->next) {

	if (bp==lp2) {		/* Change sign crossing the equator. */
	    pval[0] = 0.5; pval[1] = sign = -1;
	    continue;
	}
	if ( rect && ( sign == 1.0 ) ) continue;

	d_perp (&split, pt_nd(nB(bp)), alph);

      /*...We have a crossing if previous y * this y < 0 */
	if (pval[1] * alph[1] < 0) {	/* Then check it intersects inside */
	    inter= (pval[1]*alph[0] - alph[1]*pval[0])/(pval[1] - alph[1]);
	    if (inter > -EPS && inter < 1+EPS) return(-1);
	}
	pval[1] = alph[1];	pval[0] = alph[0];

      /*...Now do a check to see if the node is too close to the split. */
	if (sign*alph[1] < 0) continue;		   /* Skip wraparound points. */
	if (alph[0] < -EPS || alph[0] > 1+EPS) continue; /* Not between ends. */
	if (rect && (alph[0] < EPS || alph[0] > 1-EPS) ) continue;
	h1 = get_space(pt_nd(nB(bp))) / lej;	      /* Local spacing at bp. */
	h2 = first + (ratio-1)*alph[0];		      /* l.s. along split.    */
	hmax = dmax(h1,h2);
	near = dmin (near, dabs(alph[1])/hmax);
    }

    return(near);
}

/*-----------------QUAD-------------------------------------------------
 * choose Vornoi partition of a quadrilateral.
 *----------------------------------------------------------------------*/
struct LLedge * quadsplit(rg)
struct sreg_str *rg;	/* Region to split */
{
    int f,i,j,k,l;
    float f1, f2;
    float alph[MAXDIM];
    float len;
    struct edg_str e;


    /*maximize the distance from off points to split*/
    i = nB(rg->bnd->prev);
    j = nB(rg->bnd);
    k = nB(rg->bnd->next);
    l = nF(rg->bnd->next);

    /*try the i,k split*/
    e.nd[0] = i;
    e.nd[1] = k;
    len = ndist(i,k);
    d_perp(&e, pt_nd(j), alph);
    f1 = fabs(len * alph[1]);
    d_perp(&e, pt_nd(l), alph);
    f1 *= fabs(len * alph[1]);

    /*try the j,l split*/
    e.nd[0] = j;
    e.nd[1] = l;
    len = ndist(j,l);
    d_perp(&e, pt_nd(i), alph);
    f2 = fabs(len * alph[1]);
    d_perp(&e, pt_nd(k), alph);
    f2 *= fabs(len * alph[1]);

    /*use the largest*/
    if ( f1 > f2 )
	return(rg->bnd);
    else
	return(rg->bnd->next);
}
