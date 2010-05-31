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
/*   deposit.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:39 */

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
#include "poly.h"

/************************************************************************
 *									*
 *	user_deposit( ) - get the parameters for deposition of a new	*
 *  material.								*
 *									*
 *  Original:	CSR	3/86						*
 *		MEL	4/87   rewrote the work portion			*
 *		LCC     4/90   add 1-D mode				*
 ************************************************************************/
user_deposit( par, param )
char *par;
int param;
{
    register int mater, ndiv, i;
    double thick;
    struct line *p;
    struct line *op;
    int nump, numop;
    int square;
    int iskel;
    extern string_offset();
    char *sfile;
    int imp, impa, sol, r;
    double conc, space;
    double temp, pressure;
    int oldnn = nn;

    sfile = NULL;
    if( InvalidMeshCheck()) return -1;

    /*increment the process step counter*/
    process_step++;
    GridSave();

#   define chosen(A) (is_specified( param, A) && get_bool( param, A))
    if (chosen ("silicon")) mater = Si;
    if (chosen ("poly")) mater = Poly;
    if (chosen ("oxide")) mater = SiO2;
    if (chosen ("nitride")) mater = SiNi;
    if (chosen ("oxynitride")) mater = OxNi;
    if (chosen ("photoresist")) mater = PhRs;
    if (chosen ("aluminum")) mater = Al;
    if (chosen ("gaas")) mater = GaAs;

    thick = 1e-4 * get_float( param, "thick");
    ndiv = get_int( param, "divisions");

    /* check for a string file for deposit */
    sfile = get_string( param, "file" );

    if (sfile == NULL)  {
	if ( is_specified( param, "space" ) )
	    space = get_float( param, "space" ) * 1.0e-4;
	else
	    space = thick / ndiv;

	/* People have been known to forget vital information */
	if (thick <= 0 || space <= 0) {
	    fprintf( stderr, "Bad deposition parameters: thick %e spacing %e\n",
		thick*1e4, space*1e4);
	    return(-1);
	}

	if ( is_specified( param, "square" ) )
	    square = get_bool( param, "square" );
	else
	    square = TRUE;
    }

    /*work out which impurity to use in the doping*/
    imp = -1;
    if      ( get_bool( param, "arsenic" ))    imp = As;
    else if ( get_bool( param, "phosphorus" )) imp = P;
    else if ( get_bool( param, "antimony" ))   imp = Sb;
    else if ( get_bool( param, "boron" ))      imp = B;
    else if ( get_bool( param, "gallium" ))    imp = Ga;
    else if ( get_bool( param, "beryllium" ))	imp = iBe;
    else if ( get_bool( param, "magnesium" ))	imp = iMg;
    else if ( get_bool( param, "selenium" ))	imp = iSe;
    else if ( get_bool( param, "isilicon" ))	imp = iSi;
    else if ( get_bool( param, "tin" ))		imp = iSn;
    else if ( get_bool( param, "germanium" ))	imp = iGe;
    else if ( get_bool( param, "zinc" ))	imp = iZn;
    else if ( get_bool( param, "carbon" ))	imp = iC;
    else if ( get_bool( param, "generic" ))	imp = iG;

    /*make a new region*/
    r = mk_reg( mater );

    /*get the concentration*/
    conc = get_float( param, "concentrat" );

    /*generate the line of the surface*/
    nump = find_surf( &p );


    /*generate the offset line*/
    if (sfile)  {
        numop = string_offset( sfile, &op );
        thick = p[0].y - op[0].y;
    }
    else
        numop = gen_offset( nump, p, &op, thick, space);

    /*build a skeleton region and go at it*/
    iskel = build_skel( nump, p, numop, op, ndiv, r, (double)thick/ndiv, square );

    free(op);

    /*do the triangulation of the skeleton region*/
    grid( iskel, r, FALSE, TRUE );

    /*remove the original skeleton*/
    free_skel(iskel);

    /*clean up mesh*/
    bd_connect( "after deposit" );

    /*deposit new nodes with dopant*/
    if ( imp > 0 ) {
	add_impurity( imp, 1.0e5, -1 );

	switch( imp ) {
	case As  : impa = Asa;  break;
	case Sb  : impa = Sba;  break;
	case B   : impa = Ba;   break;
	case P   : impa = Pa;   break;
	case iBe : impa = iBea; break;
	case iMg : impa = iMga; break;
	case iSe : impa = iSea; break;
	case iSi : impa = iSia; break;
	case iSn : impa = iSna; break;
	case iGe : impa = iGea; break;
	case iZn : impa = iZna; break;
	case iC  : impa = iCa;  break;
	case iG  : impa = iGa;  break;
	default  : impa = 0;    break;
	}
	if ( impa && (imptosol[impa] == -1)) {
	    add_impurity(impa, 1.0, -1);
	    SET_FLAGS(impa, ACTIVE);
	}

	sol = imptosol[imp];
	for(i = 0; i < num_nd_reg(r); i++)
	    set_sol_nd( nd_reg(r,i), sol, conc );
    }


    /*make sure things are okey-dokey*/
    mtest1( "after deposit" );
    mtest2( "after deposit" );

    /*if we deposited poly, calculate the initial grain size*/
    if ( mater == Poly ) {
	temp = pressure = 0.0;

	/* get parameters */
	if ( is_specified( param, "temperature" ) )
	    temp = get_float(param, "temperature");
	if ( is_specified( param, "pressure" ) )
	    pressure = get_float(param, "pressure");

	/*add the grain size*/
        add_impurity( GRN, 0.0, -1 );

	/*inititialize the grain size*/
	poly_init( oldnn, temp, pressure, thick, imp,conc,ndiv );
    }
    return(0);
}





/************************************************************************
 *									*
 *	build_skel() - Build a skeleton out of the surface line and	*
 *  the offset outer line.						*
 *									*
 *  Original:	MEL	4/87						*
 *									*
 ************************************************************************/
build_skel( nump, p, numop, op, div, rn, sp, square )
int nump;
struct line *p;
int numop;
struct line *op;
int div;
int rn;
double sp;
int square;
{
    register int i, j;
    int ir;
    int mat = mat_reg(rn);
    int node, n1, gasnd, matnd;
    int e, ln, ip, e1, ef;
    float c[3];
    int sq;

#define min3(A,B,C) (A<B)?((B<C)?B:C):((A<C)?A:C)
#define min2(A,B) ((A<B)?(A):(B))

    /*create the initial skeleton shape*/
    ir = cr_sreg(mat);

    /*go through all the points on the surface and fix the nodes there*/
    ln = -1;
    for( i = 0; i < nump; i++ ) {
	/*find the gas node and maybe the mater node*/
	for( matnd = -1, j = 0; j < num_nd(p[i].p); j++ ) {
	    node = pt[ p[i].p ]->nd[j];
	    if ( mat_nd(node) == GAS )   gasnd = j;
	    if ( mat_nd(node) == mat ) matnd = j;
	}

	/*mark the point as non surface*/
	clr_surf( p[i].p );

	/*if we have no node of mater - change the gas node*/
	if ( matnd == -1 ) {
	    node = nd_pt(p[i].p, gasnd);
	    nd[ node ]->mater = mat;
	    p[i].p = node;
	}
	else {
	    /*destroy the gas node*/
	    n1 = nd_pt(p[i].p, gasnd);
	    fing_nd( n1 );

	    /*save the material node*/
	    node = nd_pt(p[i].p, matnd);

	    /*fix the node list up*/
	    sub_nd_pt(n1, p[i].p);

	    /*store the material node number at p*/
	    p[i].p = node;
	}

	/*build the skeleton by adding edges*/
	if ( ln != -1 ) {
	    /*find the correct edge to add...*/
	    for(ef = -1, e1 = 0; e1 < num_edge_nd(ln) && ef == -1; e1++) {
		e = edge_nd(ln, e1);
		if ( (nd_edg(e,0) == node) || (nd_edg(e,1) == node) ) ef = e;
	    }
	    if ( ef == -1 )
		ef = mk_edg(ln, node);
	    else
		clr( edg[ef], ESURF );

	    ad_edge( ir, ef, sreg[ir]->bnd, MAYBE, BEFORE );
	    set_space(pt_nd(node), len_edg(ef));
	    set_space(pt_nd(ln), min2(len_edg(ef),get_space(pt_nd(ln))));
	}
	else {
	    set_space(pt_nd(node), sp);
	}
	ln = node;
    }

    /*now go up the separator...*/
    for(i = 0; i < div-1; i++) {
	/*figure the coordinates*/
	c[0] = p[nump-1].x + (op[numop-1].x - p[nump-1].x) * (i + 1) / div;
	c[1] = p[nump-1].y + (op[numop-1].y - p[nump-1].y) * (i + 1) / div;

	if ((mode != ONED) && square && (c[0] > p[nump-1].x))
	    c[0] = p[nump-1].x;

	/*make a point and a node*/
	ip = mk_pt( 2, c );
	set_space(ip, sp);
	node = mk_nd( ip, mat );

	/*make the edge and add it*/
	e = mk_edg(ln, node);
	ad_edge( ir, e, sreg[ir]->bnd, MAYBE, BEFORE );
	set_space(pt_nd(node), len_edg(e));
	set_space(pt_nd(ln), min2(len_edg(e),get_space(pt_nd(ln))));

	ln = node;
    }

    /*now walk the offset line*/
    for(i = numop-1; i >= 0; i--) {
	c[0] = op[i].x;
	c[1] = op[i].y;

	if ((mode != ONED) && square && ((c[0] - p[nump-1].x) > 1.0e-8) ) {
	    c[0] = p[nump-1].x;
	    ip = mk_pt( 2, c );
	    sq = FALSE;
	}
	else if ( (mode != ONED) && square && (p[0].x - c[0] > 1.0e-8) ) {
	    c[0] = p[0].x;
	    ip = mk_pt( 2, c );
	    sq = FALSE;
	}
	else {
	    ip = mk_pt( 2, c );
	    set_surf(ip);
	    node = mk_nd( ip, GAS );
	    sq = TRUE;
	}

	node = mk_nd( ip, mat );

	/*make the edge and add it*/
	e = mk_edg(ln, node);
	if ( (mode != ONED) && sq ) set(edg[e], ESURF);
	ad_edge( ir, e, sreg[ir]->bnd, MAYBE, BEFORE );
	set_space(pt_nd(node), len_edg(e));
	set_space(pt_nd(ln), min2(len_edg(e),get_space(pt_nd(ln))));
	ln = node;
    }

    if ( mode == TWOD ) {
	/*finally back down the divider*/
	for(i = div-1; i > 0; i--) {
	    /*figure the coordinates*/
	    c[0] = p[0].x + (op[0].x - p[0].x) * i / div;
	    c[1] = p[0].y + (op[0].y - p[0].y) * i / div;

	    if ((mode != ONED) && square && (c[0] < p[0].x))
		c[0] = p[0].x;

	    /*make a point and a node*/
	    ip = mk_pt( 2, c );
	    set_space(ip, sp);
	    node = mk_nd( ip, mat );

	    /*make the edge and add it*/
	    e = mk_edg(ln, node);
	    ad_edge( ir, e, sreg[ir]->bnd, MAYBE, BEFORE );
	    set_space(pt_nd(node), len_edg(e));
	    set_space(pt_nd(ln), min2(len_edg(e),get_space(pt_nd(ln))));
	    ln = node;
	}

	/*and connect the region*/
	e = mk_edg(ln, p[0].p);
	ad_edge( ir, e, sreg[ir]->bnd, MAYBE, BEFORE );
    }

    /*fix up the region nice*/
    ck_clock(ir, TRUE);

    return(ir);
}



