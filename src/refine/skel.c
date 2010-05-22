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
/*   skel.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:51 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <global.h>
#include <constant.h>
#include <geom.h>
#include <regrid.h>
#include <material.h>
#include <impurity.h>
#include "refine.h"
#include "skel.h"

#define endpt(A)	((dir)?(nF(A)):(nB(A)))
#define begpt(A)	((dir)?(nB(A)):(nF(A)))
#define nexedg(A)	((dir)?(A->next):(A->prev))

/*
 *	skel_reg - This routine creates a skeleton region from the
 *  original filled in region.
 */
skel_reg( r )
int r;
{
    register int i, ind, j;
    register int e1, el, e, sr;
    int t, f, n, done, fnd;
    double l_edge();

    /*create the skel*/
    sr = cr_sreg(mat_reg(r));

    if ( mode == ONED ) {
	bnd_reg(r,0,t,f);

	/*get the node on this face*/
	n = nd_face_ele(t,f,0);
	e1 = edge_nd(n,0);
	if ( nd_edg(e1,0) == n ) ind = 1;
	else ind = 0;
    }
    else {
	/*do a linear search to get a start edge*/
	for(fnd = FALSE, i = 0; (i < num_edg_reg(r)) && !fnd; i++) {
	    e1 = edg_reg(r, i);
	    if ( ask(edg[e1], ESURF) || ask(edg[e1], EBACK) )
		fnd = TRUE;
	    else if ( num_tri_edg(e1) == 1 )
		fnd = TRUE;
	    else {
		for(j = 0; j < num_tri_edg(e1); j++)
		    if ( reg_tri(tri_edg(e1,j)) != r ) fnd = TRUE;
	    }
	}
	ind = 1;
    }

    ad_edge( sr, e1, sreg[sr]->bnd, MAYBE, BEFORE );

    /*find the next connected boundary edge...*/
    el = e1;
    done = FALSE;

    while ( !done ) {
	n = nd_edg(el,ind);

	fnd = FALSE;
	for(i = 0; (i < num_edge_nd(n)) && !fnd; i++) {
	    e = edge_nd(n,i);
	    if ( (e != el) && (e != e1) && is_border(e) ) {
		/*make sure it is a border on this region!*/
		for(j = 0; j < num_ele_edg(e) &&
		       reg_tri(tri_edg(e,j)) != r; j++);

		if ( j < num_ele_edg(e) ) {
		    fnd = TRUE;
		    ad_edge(sr, e, sreg[sr]->bnd, MAYBE, BEFORE);
		}
	    }
	}
	if ( nd_edg(e,0) == n ) ind = 1;
	else ind = 0;
	el = e;
	done = !fnd;
    }
    hinit(sr, MAXFLOAT);
    ck_clock(sr, TRUE);
    return(sr);
}


#define IN 1
#define ON 2
#define OUT 0

/*
 *	sub_skel - Subtract skeleton number 2 from number 1.  This
 *  routine can possible make new resulting regions.  The first passed
 *  region is returned modified, a new region number can also be returned.
 */
sub_skel(s1, s2, s)
int s1, s2;
int *s;
{
    int nr, *inside;
    int allin, allout;

    /*get the inside array*/
    inside = salloc( int, nn );

    /*check for the points in / out */
    check_in( s1, s2, inside, &allin, &allout );

    /*if all the points are in or out, return*/
    if ( allin ) return( -1 );
    if ( allout ) return( 0 );

    /*repeat the mission for the other region*/
    check_in( s2, s1, inside, &allin, &allout );

    if ( mode == ONED ) {
	nr = sub_1dskel(s1, s2, s, inside );
    }
    else {
	nr = sub_2dskel(s1, s2, s, inside );
    }

    free(inside);
    return(nr);
}


sub_1dskel( s1, s2, s, inside )
int s1, s2;
int *s;
int *inside;
{
    struct LLedge *bp, *bq, *bnd, *edg_crs();
    int num = -1, e, enew;
    int g, ip;
    float c[MAXDIM];
    int out_node;
    int dir;

    /*compared to the other one, this is easy!*/
    /*walk the original region and find edges of the form in/on to out*/
    bnd = sreg[s1]->bnd;
    for(g = 1, bp = bnd; g || bp != bnd; g=0, bp = bp->next) {

	if ( ( (inside[nB(bp)] != OUT) && (inside[nF(bp)] == OUT) ) ||
	     ( (inside[nF(bp)] != OUT) && (inside[nB(bp)] == OUT) ) ) {

	    if ( (inside[nB(bp)] != OUT) && (inside[nF(bp)] == OUT) )
		dir = 1;
	    else
		dir = 0;

	    /*create a new skeleton...*/
	    num++;
	    s[num] = cr_sreg(sreg[s1]->mat);

	    /*add the fragment of the initial edge*/
	    e = bp->edge;
	    bq = edg_crs( e, s2, c );
	    out_node = endpt(bp);

	    /*shorten the edge*/
	    enew = sp_edge( e, c, &ip, TRUE );
	    mk_surf(ip);

	    /*convolutions to figure out the edge*/
	    if ( enew < 0 ) {
		if ( nd_edg(e,enew+2) != out_node )
		    ad_edge(s[num], e, sreg[s[num]]->bnd, MAYBE, BEFORE);
	    }
	    else if ( (out_node == nd_edg(e,0)) || (out_node == nd_edg(e,1)) )
		ad_edge(s[num], e, sreg[s[num]]->bnd, MAYBE, BEFORE);
	    else
		ad_edge(s[num], enew, sreg[s[num]]->bnd, MAYBE, BEFORE);

	    /*walk around the existing region until we cross again*/
	    for(bq = nexedg(bp);
		(inside[begpt(bq)] == OUT) && (inside[endpt(bq)] != IN);
		bq = nexedg(bq)) {
		e = bq->edge;
		ad_edge(s[num], e, sreg[s[num]]->bnd, MAYBE, BEFORE);
	    }
	}
    }
    return(num+1);
}


#define min2(A,B) ((A)<(B))?(A):(B)

sub_2dskel( s1, s2, s, inside )
int s1, s2;
int *s;
int *inside;
{
    int i, e, e1, enew, eadd;
    int first_node, last_node, out_node;
    int num = -1;
    int g, ip, in1;
    int dir;
    float c[3], len;
    struct LLedge *bnd, *bp, *bq, *bts, *bte, *edg_crs();
    double ha,hb,hsp,l_edge();

    /*walk the original region and find edges of the form in/on to out*/
    bnd = sreg[s1]->bnd;
    for(g = 1, bp = bnd; g || bp != bnd; g=0, bp = bp->next) {
	if ( ask(edg[bp->edge], MARKED) ) continue;
	set(edg[bp->edge], MARKED);

	/*if this point is in or on the etch region, make it a surface point*/
	if ( inside[nB(bp)] != OUT ) mk_surf(pt_nd(nB(bp)));

	if ( (inside[nB(bp)] != OUT) && (inside[nF(bp)] == OUT) ) {

	    /*create a new skeleton...*/
	    num++;
	    s[num] = cr_sreg(sreg[s1]->mat);

	    /*add the fragment of the initial edge*/
	    e = bp->edge;
	    out_node = nF(bp);
	    ha = l_edge(e);
	    hsp = min2( get_space(pt_edg(e,0)), get_space(pt_edg(e,0)));
	    bte = edg_crs( e, s2, c );

	    /*shorten the edge*/
	    enew = sp_edge( e, c, &ip, TRUE );
	    set_space( ip, min2(get_space(ip), hsp) );
	    mk_surf(ip);

	    /*save the first node*/
	    first_node = node_mat(nd_pt(ip,0), sreg[s1]->mat);

	    /*convolutions to figure out the edge*/
	    if ( enew < 0 ) {
		if ( nd_edg(e,enew+2) != out_node )
		    ad_edge(s[num], eadd=e, sreg[s[num]]->bnd, MAYBE, BEFORE);
		else
		    eadd = e;
	    }
	    else {
		if ( (out_node == nd_edg(e,0)) || (out_node == nd_edg(e,1)) )
		    eadd = e;
		else
		    eadd = enew;
		ad_edge(s[num], eadd, sreg[s[num]]->bnd, MAYBE, BEFORE);
	    }
	    set(edg[eadd], REGS);
	    set(edg[eadd], MARKED);
	    while( bp->edge != eadd ) {
		set(edg[bp->edge], MARKED);
		bp = bp->next;
	    }

	    /*walk around the existing region until we cross again*/
	    for(bq = bp->next;
		(inside[nF(bq)] == OUT);
		bq = bq->next) {
		e = bq->edge;
		set(edg[e],MARKED);
		ad_edge(s[num], e, sreg[s[num]]->bnd, MAYBE, BEFORE);
	    }
	    bp = bq;

	    /*bq is an edge that cross, add the possible frag*/
	    e = bq->edge;
	    out_node = nB(bq);
	    hsp = min2( get_space(pt_edg(e,0)), get_space(pt_edg(e,0)));
	    hb = l_edge(e);
	    bts = edg_crs( e, s2, c );

	    /*shorten the edge*/
	    enew = sp_edge( e, c, &ip, TRUE );
	    set_space( ip, min2(get_space(ip), hsp) );
	    mk_surf(ip);

	    /*save the last node*/
	    last_node = node_mat(nd_pt(ip,0), sreg[s1]->mat);

	    /*convolutions to figure out the edge*/
	    if ( enew < 0 ) {
		if ( nd_edg(e,enew+2) != out_node )
		    ad_edge(s[num], eadd=e, sreg[s[num]]->bnd, MAYBE, BEFORE);
		else
		    eadd = e;
	    }
	    else {
		if ( (out_node == nd_edg(e,0)) || (out_node == nd_edg(e,1)) )
		    eadd = e;
		else
		    eadd = enew;
		ad_edge(s[num], eadd, sreg[s[num]]->bnd, MAYBE, BEFORE);
	    }
	    set(edg[eadd], MARKED);
	    set(edg[eadd], REGS);

	    /*connect the regions together by walking the etch region*/

	    /*which direction do we go along the etch region?*/
	    if ( inside[nF(bts)] == IN )
		dir = 1;
	    else
		dir = 0;

	    /*begin to add sections of the etch line*/
	    len = 0.25 * ((ha<hb)?ha:hb);
	    while( bts != bte ) {
		ip = pt_nd(endpt(bts));

		if ( (pdist(ip, pt_nd(first_node)) > len) &&
		     (pdist(ip, pt_nd(last_node)) > len) ) {
		    /*make a node and add the length*/
		    mk_surf(ip);
		    if ((in1=node_mat(nd_pt(ip,0),sreg[s1]->mat)) == -1)
			in1 = mk_nd( ip, sreg[s1]->mat );
		    e1 = mk_edg(last_node, in1);
		    set(edg[e1], REGS);
		    set(edg[e1], ESURF);
		    ad_edge(s[num],e1,sreg[s[num]]->bnd,MAYBE,BEFORE);

		    last_node = in1;
		}
		bts = nexedg(bts);
	    }

	    /*finish up by connecting the last pair*/
	    e1 = mk_edg(first_node, last_node);
	    set(edg[e1], REGS);
	    set(edg[e1], ESURF);
	    set(edg[e1], MARKED);
	    ad_edge(s[num], e1, sreg[s[num]]->bnd, MAYBE, BEFORE);

	}
    }

    /*subdivide the perimeter, as needed*/
    for(i=0; i < num+1; i++) {
	ck_clock(s[i], TRUE);
    }

    return(num+1);
}



check_in(s1, s2, inside, ai, ao )
int s1, s2;
int *inside;
int *ai, *ao;
{
    struct LLedge *bp, *bnd;
    int allin, allout, cnt;
    int g;

    /*first walk the region points and test in/out*/
    allin = allout = cnt = 0;
    bnd = sreg[s1]->bnd;

    for(g=1, bp = bnd; g || bp != bnd; g = 0,bp = bp->next) {
	if ( on_bound(pt_nd(nB(bp)), s2)) {
	    inside[nB(bp)] = ON;
	    allin++;
	    allout++;
	}
	else {
	    if (pt_in_skel(cord_arr(pt_nd(nB(bp))), s2)) {
		inside[nB(bp)] = IN;
		allin++;
	    }
	    else {
		inside[nB(bp)] = OUT;
		allout++;
	    }
	}
	cnt++;
	if ( mode == ONED ) {
	    if ( on_bound(pt_nd(nF(bp)), s2)) {
		inside[nF(bp)] = ON;
		allin++;
		allout++;
	    }
	    else {
		if (pt_in_skel(cord_arr(pt_nd(nF(bp))), s2)) {
		    inside[nF(bp)] = IN;
		    allin++;
		}
		else {
		    inside[nF(bp)] = OUT;
		    allout++;
		}
	    }
	    cnt++;
	}
    }

    *ai = allin == cnt;
    *ao = allout == cnt;
}



int on_bound(p, sr)
int p;
int sr;			/*The coordinates of the points. */
{
    int f;
    float len, alph[MAXDIM];
    struct LLedge *bp, *bnd;
    void d_perp();

    /* If the perpindicular distance to the line is zero, its on the line*/
    bnd = sreg[sr]->bnd;
    for(bp = bnd, f=1; f || bp != bnd; f = 0, bp = bp->next) {
	if ( mode == ONED ) {
	    if ( pdist( p, pt_nd(nB(bp)) ) < 1.0e-8 ||
	         pdist( p, pt_nd(nF(bp)) ) < 1.0e-8 )
		return(TRUE);
	}
	else {
	    len = l_edge(bp->edge);
	    d_perp( edg[bp->edge], p, alph );

	    /*if we are closer than one angstrom*/
	    if ( fabs(alph[1] * len) < 1.0e-8 )
		if ( (alph[0] >= 0.0) && (alph[0] <= 1.0) )
		    return( TRUE );
	}
    }

    /* What a short, strange trip it's been */
    return (FALSE);
}




plot_on(sr,inside)
int sr;
int *inside;
{
    int f;
    struct LLedge *bp, *bnd;

    bnd = sreg[sr]->bnd;
    for(bp = bnd, f=1; f || bp != bnd; f = 0, bp = bp->next) {
	node_pl( nB(bp), inside[nB(bp)]+1 );
    }
}

/*-----------------PTNREG-----------------------------------------------
 * Determine whether the point (x0,y0) is within the region. Algorithm
 * 112 from CACM. This little gem works for clockwise, anticlockwise,
 * or even self-crossing regions.
 *----------------------------------------------------------------------*/
int pt_in_skel(c, sr)
float c[3];
int sr;			/*The coordinates of the points. */
{
    int flag, f;
    double xi, yi, xin, yin;
    struct LLedge *bp, *bnd;

  /*
   * If a line to infinity crosses an odd number of edges, its inside.
   */
    flag = TRUE;
    bnd = sreg[sr]->bnd;

    for(bp = bnd, f=1; f || bp != bnd; f = 0, bp = bp->next) {
	xi = cordinate( pt_edg(bp->edge,0), 0 );
	yi = cordinate( pt_edg(bp->edge,0), 1 );

	xin = cordinate( pt_edg(bp->edge,1), 0 );
	yin = cordinate( pt_edg(bp->edge,1), 1 );

	if ( mode == ONED ) {
	    if ( (c[0] <= xi) == (c[0] > xin) ) flag = !flag;
	}
	else {
	    if ((c[1] <= yi) == (c[1] > yin)) {
		if ((c[0]-xi - (c[1]-yi)*(xin-xi)/(yin-yi) ) < 0) {
		    flag = ! flag;
		}
	    }
	}
    }

    /* What a short, strange trip it's been */
    return (!flag);
}





/*
 * calculate the point at which edge e crosses region s2
 */
struct LLedge *edg_crs( e, s2, c )
int e;
int s2;
float c[MAXDIM];
{
    int e1, g;
    struct LLedge *bp, *bnd;
    float p[MAXDIM], dp[MAXDIM], q[MAXDIM], dq[MAXDIM];
    float alph[MAXDIM], lp, lq;

    p[0] = cordinate( pt_edg(e,0), 0);
    p[1] = (mode==ONED)?0.0:cordinate( pt_edg(e,0), 1);
    dp[0] = cordinate( pt_edg(e,1), 0) - p[0];
    dp[1] = (mode==ONED)?0.0:cordinate( pt_edg(e,1), 1) - p[1];
    lp = sqrt(dp[0] * dp[0] + dp[1] * dp[1]);

    bnd = sreg[s2]->bnd;
    for(bp = bnd, g=1; g || bp != bnd; g = 0, bp=bp->next) {
	e1 = bp->edge;
	q[0] = cordinate( pt_edg(e1,0), 0);
	q[1] = (mode==ONED)?0.0:cordinate( pt_edg(e1,0), 1);
	dq[0] = cordinate( pt_edg(e1,1), 0) - q[0];
	dq[1] = (mode==ONED)?0.0:cordinate( pt_edg(e1,1), 1) - q[1];
	lq = sqrt(dq[0] * dq[0] + dq[1] * dq[1]);

	if ( mode == TWOD ) {
	    if ( lil(p,dp,q,dq,alph) == 0 ) {

		/*if they cross, nice and easy*/
		if ((alph[0] >= 0.0) && (alph[0] <= 1.0) &&
		    (alph[1] >= 0.0) && (alph[1] <= 1.0) ) {
		    c[0] = alph[0] * dp[0] + p[0];
		    c[1] = alph[0] * dp[1] + p[1];
		    return( bp );
		}
		/*we have the on condition*/
		else if ( (alph[0] >= 0.0) && (alph[0] <= 1.0) ) {
		    if ( ( fabs(alph[1] * lq) < 1.0e-8 ) ||
		         ( fabs(alph[1] * lq - lq) < 1.0e-8 ) ) {
			c[0] = alph[0] * dp[0] + p[0];
			c[1] = alph[0] * dp[1] + p[1];
			return( bp );
		    }
		}
		else if ( (alph[1] >= 0.0) && (alph[1] <= 1.0) ) {
		    if ( ( fabs(alph[0] * lp) < 1.0e-8 ) ||
		         ( fabs(alph[0] * lp - lp) < 1.0e-8 ) ) {
			c[0] = alph[0] * dp[0] + p[0];
			c[1] = alph[0] * dp[1] + p[1];
			return( bp );
		    }
		}
	    }
	}
	else {
	    if (lil(p,dp,q,dq,alph) == 0) {
		if ((alph[0] >= 0.0) && (alph[0] <= 1.0) &&
		    (alph[1] >= 0.0) && (alph[1] <= 1.0) ) {
		    c[0] = alph[0] * dp[0] + p[0];
		    return( bp );
		}
	    }
	}
    }
    return(NULL);
}

