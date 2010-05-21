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
/*   etch.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:40 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <global.h>
#include <constant.h>
#include <geom.h>
#include <regrid.h>
#include <material.h>
#include <impurity.h>
#include "refine.h"
#include "skel.h"

/*some useful macros for getting at the parameters*/
#define CHOSEN(A) (is_specified( param, A) && get_bool( param, A))
#define FETCH(N,V,S) V = S*get_float( param, N)
#define FETCHD(N,V,S,D) if(is_specified(param,N)) V = S*get_float( param, N);\
			else V = D;

#define MOVE( A, B, C ) {pt[A]->cordo[0] = pt[A]->cord[0]=B;\
			 pt[A]->cordo[1] = pt[A]->cord[1]=C;\
			 inside[A]=TRUE;}

#define DIS(p1,p2) hypot(pt[p1]->cord[0]-pt[p2]->cord[0], pt[p1]->cord[1]-pt[p2]->cord[1])

/*A macro for point in the etch region*/
#define IN_ETCH( AX, AY, B ) ptnreg( (AX), (AY), (B)->num, (B)->cord )

/*-----------------USER_ETCH--------------------------------------------
 * User interface to etching routine.
 *----------------------------------------------------------------------*/
user_etch( par, param)
    char *par;
    int param;
{
    register int i;
    static int mater_save;
    int mater;
    int f;
    float x;
    float xma, xmi, yma, ymi, thick;
    double rate[MAXMAT], time;
    static float *p[MAXDIM+1];	/*list of points on the etch boundary*/
    static int nump;
    static int maxp;
    struct line *surf;
    int nums;
    char *sfile;
    extern string_etch();

    sfile = NULL;
    if( InvalidMeshCheck()) return -1;
    GridSave();

    /* check for a string file for deposit */
    sfile = get_string( param, "file" );

    mater = -1;
    if (CHOSEN ("silicon")) mater = Si;
    if (CHOSEN ("poly")) mater = Poly;
    if (CHOSEN ("oxide")) mater = SiO2;
    if (CHOSEN ("oxynitride")) mater = OxNi;
    if (CHOSEN ("nitride")) mater = SiNi;
    if (CHOSEN ("photoresist")) mater = PhRs;
    if (CHOSEN ("aluminum")) mater = Al;
    if (CHOSEN ("gaas")) mater = GaAs;

    /*get the limits of the given material*/
    xmi = xma = cordinate(pt_nd(0),0);
    ymi = yma = cordinate(pt_nd(0),1);
    for(i = 1; i < nn; i++) {
	if ( xmi > cordinate(pt_nd(i),0) ) xmi = cordinate(pt_nd(i),0);
	if ( xma < cordinate(pt_nd(i),0) ) xma = cordinate(pt_nd(i),0);
	if ( ymi > cordinate(pt_nd(i),1) ) ymi = cordinate(pt_nd(i),1);
	if ( yma < cordinate(pt_nd(i),1) ) yma = cordinate(pt_nd(i),1);
    }

    /*work out the type of the etch specified*/
    if ( CHOSEN("left") || CHOSEN("right")) {

	/* 1-D dosen't have this option */
        if ( mode != ONED ) {

	    p[0] = salloc(float, 4);
	    p[1] = salloc(float, 4);
	    p[3] = salloc(float, 4);
	    /*get the endpoints of the line*/
	    FETCH ( "p1.x", p[0][0], 1e-4 );
	    FETCHD( "p1.y", p[1][0], 1e-4 , 2*ymi - yma);
	    FETCHD( "p2.x", p[0][1], 1e-4 , p[0][0]);
	    FETCHD( "p2.y", p[1][1], 1e-4 , 2*yma - ymi);
	    FETCHD( "spacing", p[3][0], 1e-4, -1.0 );
	    FETCHD( "spacing", p[3][1], 1e-4, -1.0 );

	    /*figure out which way to go*/
	    if ( get_bool( param, "left" ) )
		x = 2.0 * xmi - xma;
	    else if ( get_bool( param, "right" ) )
		x = 2.0 * xma - xmi;

	    /*set up the final sides*/
	    p[0][2] = x;
	    p[1][2] = p[1][1];
	    p[3][2] = xma - xmi;
	    p[0][3] = x;
	    p[1][3] = p[1][0];
	    p[3][3] = xma - xmi;

	    nump = 4;
	    etch( p, nump, mater );

	    sfree(p[0]);
	    sfree(p[1]);
	}
	else {
	    fprintf(stderr, "\n\? NO [left|right] options in 1-D! \n\n");
	}
    }
    else if ( CHOSEN( "dry" ) ) {
	/*how much do we take off*/
	thick = get_float( param, "thick" ) * 1.0e-4;

	/*find the surface line*/
	nums = find_surf( &surf );

	p[0] = salloc( float, nums+10 );
	p[1] = salloc( float, nums+10 );

	/*build the etch region*/
	p[0][0] = xmi - (xma - xmi);
	if ( mode == TWOD ) p[1][0] = ymi - (yma - ymi);

	/*now add the offset surface line*/
	for(i = 0; i < nums; i++) {
	   if ( mode == ONED ) p[0][i+1] = surf[i].x + thick;
	   else {
		p[0][i+1] = surf[i].x;
		p[1][i+1] = surf[i].y + thick;
	   }
	}

	/*add the last to complete the rectangle*/
	if ( mode == ONED ) {
	   nump = nums+1;
	}
	else if ( mode == TWOD ) {
	   p[0][i+1] = xma + (xma - xmi);
	   p[1][i+1] = ymi - (yma - ymi);
	   nump = nums+2;
	}

	process_step++;

	etch( p, nump, mater );
	sfree(p[0]);
	sfree(p[1]);

    }
    else if ( CHOSEN( "start" ) ) {

	mater_save = -1;
        if ( mode != ONED ) {
	    /*initialize the etch region*/
	    maxp = 500;
	    p[0] = salloc( float, maxp );
	    p[1] = salloc( float, maxp );
	    p[3] = salloc( float, maxp );
	    nump = 1;
	    FETCH( "x", p[0][0], 1e-4 );
	    FETCH( "y", p[1][0], 1e-4 );
	    FETCHD( "spac", p[3][0], 1e-4, -1.0 );
	}
	else {
	    fprintf(stderr,"\n\?NO [start|continue|done] options in 1-D!\n");
	}
	if ( mater != -1 ) mater_save = mater;
    }
    else if ( CHOSEN( "continue" ) ) {

        if ( mode != ONED ) {

	    if ( nump >= maxp ) {
                maxp += 500;
		p[0] = sralloc( float, maxp, p[0] );
		p[1] = sralloc( float, maxp, p[1] );
		p[3] = sralloc( float, maxp, p[1] );
	    }
	    /*continue defining the etch region*/
	    FETCH( "x", p[0][nump], 1e-4 );
	    FETCH( "y", p[1][nump], 1e-4 );
	    FETCHD( "spac", p[3][nump], 1e-4, -1.0 );
	    nump++;
	}
        else {
	    fprintf(stderr,"\n\?NO [start|continue|done] options in 1-D!\n");
	}
	if ( mater != -1 ) mater_save = mater;
    }
    else if ( CHOSEN( "done" ) ) {

        if ( mode != ONED ) {
	    if ( mater != -1 ) mater_save = mater;
	    mater = mater_save;
	    mater_save = -1;

	    /*finish defining the etch region*/
	    if ( nump >= maxp ) {
		maxp += 500;
		p[0] = sralloc( float, maxp, p[0] );
		p[1] = sralloc( float, maxp, p[1] );
	    }
	    FETCH( "x", p[0][nump], 1e-4 );
	    FETCH( "y", p[1][nump], 1e-4 );
	    FETCHD( "spac", p[3][nump], 1e-4, MAXFLOAT );
	    nump++;
	    etch( p, nump, mater );
	    free(p[0]);
	    free(p[1]);
	}
	else {
	    fprintf(stderr,"\n\?NO [start|continue|done] options in 1-D!\n");
	}

    }
    else if ( CHOSEN( "all" ) ) {
	for(i = 0; i < nreg; i++)
	    if ( mat_reg(i) == mater ) rem_reg(i);
	bd_connect("after etch");
    }
    else if ( CHOSEN( "physical" ) ) {
	for( i = 0; i < MAXMAT; i++ ) rate[i] = 0.0;
	FETCHD( "r.silicon", rate[Si], 1.0e-4, 0.0 );
	FETCHD( "r.oxide", rate[SiO2], 1.0e-4, 0.0 );
	FETCHD( "r.oxynitride", rate[OxNi], 1.0e-4, 0.0 );
	FETCHD( "r.nitride", rate[SiNi], 1.0e-4, 0.0 );
	FETCHD( "r.poly", rate[Poly], 1.0e-4, 0.0 );
	FETCHD( "r.photoresist", rate[PhRs], 1.0e-4, 0.0 );
	FETCHD( "r.aluminum", rate[Al], 1.0e-4, 0.0 );
	FETCHD( "r.gaas", rate[GaAs], 1.0e-4, 0.0 );
	FETCH( "time", time, 1.0 );

	phys_etch( rate, time );
    }
    else if (sfile)
	string_etch(sfile, mater);

    return(0);
}



/*-----------------ETCH-------------------------------------------------
 *----------------------------------------------------------------------*/
etch( p, nump, mater )
float *p[MAXDIM+1];
int nump;
int mater;
{
    int i, e;
    int ir, node, ip, ln, fn;
    int sr;
    int r, rn, s[MAXREG];
    int ns;
    float c[MAXDIM];
    struct LLedge *bp;
    int f, n;

    for(r = 0; r < nreg; r++) clr(reg[r], ETCHED);

    /*for every material that we are worried about...*/
    for(r = 0; r < nreg; r++) {
	if ( (mater != -1) && (mat_reg(r) != mater) ) continue;

	/*only etch exposed regions*/
	if ( !ask( reg[r], EXPOS ) ) continue;

	if ( ask(reg[r], ETCHED) ) continue;
	set(reg[r], ETCHED);

	/*build a skeleton region out of the list of points*/
	ir = cr_sreg(-1);

	/*loop through all the points that were passed*/
	ln = -1;
	for(i = 0; i < nump; i++) {
	    c[0] = p[0][i];
	    c[1] = p[1][i];

	    /*make a point and a node*/
	    ip = mk_pt( 2, c );
	    if ( p[3] != NULL && (p[3][i] > 0.0) )
		set_space(ip, p[3][i]);
	    else
		set_space(ip, MAXFLOAT);
	    node = mk_nd( ip, mat_reg(r) );
	    set(pt[ip], SKELP);
	    if (i == 0) fn = node;

	    /*make the edge and add it*/
	    if ( ln != -1 ) {
		e = mk_edg(ln, node);
		ad_edge( ir, e, sreg[ir]->bnd, MAYBE, BEFORE );
	    }
	    ln = node;
	}
	e = mk_edg(ln, fn);
	if (mode != ONED ) ad_edge( ir, e, sreg[ir]->bnd, MAYBE, BEFORE );
#ifdef DEBUG
	reg_pl(ir);
#endif
#define min2(A,B) (A < B)?A:B
	ck_clock(ir, TRUE);
	for(f=1, bp=sreg[ir]->bnd; (bp != sreg[ir]->bnd)||f; f=0, bp=bp->next) {
	    n = nB(bp);
	    ip = pt_nd(n);
	    for(i = 0; i < num_edge_nd(n); i++) {
		set_space(ip,min2(get_space(ip),len_edg(edge_nd(n,i))));
	    }
	}

	/*build a skeleton from the material we are worried about*/
	sr = reg[r]->sreg;;
	hinit(sr, MAXFLOAT);
	ck_clock(sr, TRUE);
	ns = sub_skel(sr, ir, s);
	if (ns == -1) {
	    rem_reg(r);
	}
	else if ( ns == 0) {
	}
	else {

	    for(i = 0; i < ns; i++) {
#ifdef DEBUG
		reg_pl(s[i]);
#endif
		rn = mk_reg( mat_reg(r) );
		reg[rn]->thick = reg[r]->thick;
		set(reg[rn], ETCHED);
		hinit( s[i], MAXFLOAT );
		grid( s[i], rn, TRUE, TRUE );
		interp( rn, r );
	    }
	    while(num_tri_reg(r) > 0) rem_ele(tri_reg(r,0), FALSE);
	    fing_reg(r);
	}

	/*clear the skeleton etch region*/
	for(i = 0; i < ne; i++) clr( tri[i], NEIGHB );

	/*mark as dead all the points in the skeleton*/
	for(i = 1, bp = sreg[ir]->bnd;
	    i || bp != sreg[ir]->bnd; i = 0, bp = bp->next) {
	    if (num_tri_nd(nB(bp)) == 0) rem_1nd(nB(bp));
	}
	if ( mode == ONED ) {
	    bp = bp->prev;
	    if (num_tri_nd(nF(bp)) == 0) rem_1nd(nF(bp));
	}

	/*this step renumbers the regions - have to start over...*/
	if ( ns != 0 ) r = -1;
	bd_connect("after etch of a single region");
    }
    /*clear the temporary skeleton marker*/
    for(i = 0; i < np; i++) clr(pt[i], SKELP);
}




