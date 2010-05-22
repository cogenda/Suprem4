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
/*   check.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:35 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include "material.h"

#define report(S,A,B,C) {if (!err_cnt++)\
		     fprintf(stderr, "Mesh failed self test %s:\n", when);\
					     fprintf(stderr, S, A, B, C);}



/*-----------------MESH_TEST--------------------------------------------
 * Hopefully print a useful error message before a bad mesh brings
 * about a core dump.
 *
 * mtest1 checks things which don't depend on the connectivity
 * mtest2 checks things which do
 *----------------------------------------------------------------------*/
mtest1(when)
    char *when;
{
    int i, j, p, n, t, e, r, tmat, gassy, err_cnt = 0, *n_in_tri;

    /*
     * There is but one way to get things right, and ah so
     * many ways to get them wrong... All we can do is scratch
     * the surface of the error space
     */

    /*Check first that things have been properly allocated*/
    for (p = 0; p < np; p++)
	if (!pt[p])
	    { report("point %d has not been allocated\n", p, 0, 0); continue; }

    for (n = 0; n < nn; n++)
	if (!nd[n])
	    { report("node %d has not been allocated\n", n, 0, 0); continue; }

    for (t = 0; t < ne; t++)
	if (!tri[t])
	    { report("element %d has not been allocated\n", t, 0, 0); continue;}

    for (e = 0; e < ned; e++)
	if (!edg[e])
	    { report("edge %d has not been allocated\n", e, 0, 0); continue;}

    for (r = 0; r < nreg; r++)
	if (!reg[r])
	    { report("region %d has not been allocated\n", r, 0, 0); continue;}



    /*No further processing if we have memory problems*/
    if (err_cnt > 0)
        panic( "after mesh self test");

    /*Self-consistency checks*/
    /*Every region must have a valid material number */
    for (i = 0; i < nreg; i++)
	if( mat_reg(i) != Si   && mat_reg(i) != Poly &&
	    mat_reg(i) != SiNi && mat_reg(i) != SiO2 &&
	    mat_reg(i) != OxNi && mat_reg(i) != PhRs &&
	    mat_reg(i) != Al   && mat_reg(i) != GaAs)
	    report("region %d has bad material code %d\n", i, mat_reg(i), 0)

    /*Check points*/
    for (p = 0; p < np; p++) {

	/*The node list should not be empty*/
	if (num_nd(p) == 0)
	    report("point %d has no associated node\n", p, 0, 0)

	/*Each node should exist*/
	for (gassy = j = 0; j < num_nd(p); j++) {
	    n = nd_pt(p,j);
	    if (n < 0 || n >= nn)
		report("point %d has a bad node %d at %d\n", p, n, j)

	    /* and link back*/
	    else {
		if (pt_nd(n) != p)
		    report("broken link node %d <== point %d\n", n, p, 0)
		if (mat_nd(n) == GAS)
		    gassy++;
	    }
	}

	/*Check for consistency of gas nodes and SURFACE flag*/
	if (is_surf(p) || is_back(p))
	    {if (!gassy) report("exposed point %d has no gas node\n", p, 0, 0)}
	else
	    if (gassy) report("unexposed point %d has gas node\n", p, 0, 0)
    }

    /*Check nodes*/
    for (n = 0; n < nn; n++) {

	/*Points should point back at nodes which point to them*/
	p = pt_nd(n);
	if (p < 0 || p >= np)
	    report("node %d has a bad point %d\n", n, p, 0)
	else {
	    for (i = num_nd(p)-1; i >= 0; i--)
		if (nd_pt(p,i) == n) break;
	    if (i < 0)
		report("broken link point %d <== %d\n", p, n, 0)
	}

	/*Someday check if the solution values have been initialized?*/
    }

    /*Check triangles*/

    /*Initialize list of nodes which are in some triangle*/
    n_in_tri = salloc( int, nn );
    for (n = 0; n < nn; n++) n_in_tri[ n] = -1;

    for (t = 0; t < ne; t++) {

	/*We have problems if two nodes are the same */
	for(i = 0; i < num_vert(t); i++)
	    for(j = i+1; j < num_vert(t); j++)
		if ( vert_tri(t,i) == vert_tri(t,j) )
		    report("triangle %d has duplicate nodes\n", t, 0, 0)

	/*Every element must have a valid region number*/
	tmat = reg_tri(t);
	if (tmat < 0 || tmat >= nreg)
	    report("triangle %d has a bad region number %d\n", t, tmat, 0)
	else
	    tmat = mat_reg(tmat);

	/*Every triangle's nodes should have the same material*/
	for (i = 0; i < num_vert(t); i++) {
	    n = vert_tri(t,i);
	    if (n < 0 || n >= nn)
		report("triangle %d has a bad node %d\n", t, n, 0)
	    else {
		n_in_tri[ n] = t;
		if( mat_nd(n) != tmat)
		    report("node %d in triangle %d has wrong material %d\n",
						    n, t, nd[ n ]->mater);
	    }
	}
    }

    /*Now check on loose nodes*/
    for (n = 0; n < nn; n++)
	if (mat_nd(n) == GAS) {
	    if (n_in_tri[ n] != -1)
		report("gas node %d is in triangle %d\n", n, n_in_tri[ n], 0)
	}
	else {
	    if (n_in_tri[ n] == -1)
		report("material node %d belongs to no triangles\n", n, 0, 0)
    }


    free(n_in_tri);

    /* That's as much as we can do right now... */
    if (err_cnt != 0) panic( "after mesh self test");
    return;
}

mtest2( when)
    char *when;
{
    int i, j, p, n, t, e, tn, err_cnt = 0, *is_exposed;

#ifdef DEBUG
    /* Unless we are really paranoid, assume mtest1 has already been called */
    mtest1( when);
#endif

    /* Check nodes */
    for (n = 0; n < nn; n++) {

	/*Material nodes should be in some triangle, gas nodes shouldn't*/
	if (mat_nd(n) == GAS) {
	    if (num_tri_nd(n) != 0)
		report("gas node %d points to %d triangles\n",n,num_tri_nd(n),0);
	}
	else {
	    if (num_tri_nd(n) == 0)
		report( "material node %d has no linked triangles\n", n, 0, 0)

	    /*Triangles should point back at nodes which point at them*/
	    else
		for (i = 0; i < num_tri_nd(n); i++) {
		    t = tri_nd(n, i);
		    if (t < 0 || t >= ne)
			report("node %d has a bad triangle %d at %d\n", n, t, i)
		    else {
			for (j = num_vert(t); j >= 0; j--)
			    if (vert_tri(t,j) == n) break;
			if (j < 0)
			  report("broken link triangle %d <== node %d\n",t,n,0)
		    }
		}

	    /*check edges*/
	    if (num_edge_nd(n) == 0)
		report( "material node %d has no linked edges\n", n, 0, 0)

	    /*Edges should point back at nodes which point at them*/
	    else
		for (i = 0; i < num_edge_nd(n); i++) {
		    t = edge_nd(n, i);
		    if (t < 0 || t >= ned)
			report("node %d has a bad edge %d at %d\n", n, t, i)
		    else {
			if ( (nd_edg(t,0) != n) && (nd_edg(t,1) != n) )
			  report("broken link edge %d <== node %d\n",t,n,0)
		    }
		}

	}
    }

    /* Check edges */
    for(e = 0; e < ned; e++) {

	/*make sure nodes point to edge*/
	for(j = 0; j < 2; j++ ) {
	    n = nd_edg(e,j);
	    for (i = 0; (i < num_edge_nd(n)) && (edge_nd(n,i) != e); i++);
	    if (i == num_edge_nd(n))
		report("broken link node %d <== edge %d\n",n,e,0)
	}

	/*make sure triangles point to edges*/
	for(j = 0; j < num_tri_edg(e); j++) {
	    t = tri_edg(e, j);
	    for(i = 0; (i < num_edge(t)) && (edg_ele(t,i) != e); i++);
	    if ( i == num_edge(n) )
		report("broken link triangle %d <== edge %d\n",t,e,0)
	}

    }

    /* Check elements */

    /* Initialize list of points which are on a gas edge */
    is_exposed = salloc(int, np);
    for (p = 0; p < np; p++) is_exposed[ p] = 0;

    t = 0;
    while( done_tri(t) ) {

	/*nodes better point back*/
	for (i = 0; i < num_vert(t); i++) {
	    n = vert_tri(t,i);
	    for (j = 0; j < num_tri_nd(n); j++)
		if( tri_nd(n,j) == t) break;
	    if (j == num_tri_nd(n))
		report("broken link node %d <== triangle %d\n", n, t, 0)
	}

	/*edges had better point back*/
	for (i = 0; i < num_edge(t); i++) {
	    e = edg_ele(t,i);
	    for (j = 0; j < num_tri_edg(e); j++)
		if( tri_edg(e,j) == t) break;
	    if (j == num_tri_edg(e))
		report("broken link edge %d <== triangle %d\n", e, t, 0)
	}

	/*edges had better point back*/

	/*The neighbors better be neighborly*/
	for (i = 0; i < num_face(t); i++) {
	    tn = neigh_fc(t, i);

	    /* Neighbor triangle case */
	    if (tn >= 0) {
		if (tn == t)
		    report("triangle %d is its own %d neighbor\n",t,i,0)
		else
		    if (!tnabor (t, tn, &n, &p))
		      report("connection %d of triangle %d is wrong\n", i, t, 0)
		for (j = 0; j < num_face(tn); j++)
		    if (neigh_fc(tn,j) == t) break;
		if (j == num_face(tn))
		    report("broken neighbor connection %d <== %d\n", tn, t, 0)
	    }

	    /*No neighbor case*/
	    /*For non-neutral edges, there should be a gas node at each end*/
	    else if (tn != BC_OFFSET)
		for (j = 0; j < num_nd_fc(t,i); j++ ) {
		    n = nd_face_ele(t,j,i);
		    p = pt_nd(n);
		    if (! (is_surf(p)||is_back(p)))
		      report("triangle %d lacks gas node at vertex %d\n",t,i,0)
		    is_exposed[ p]++;
		}
	}

	next_tri(t);
    }

    /* Finish the check on surface<->exposed edge*/
    for (p = 0; p < np; p++) {
	if (ask(pt[p], SKELP)) continue;
	if ((is_surf(p)||is_back(p)) && ! is_exposed[ p ])
	    report("exposed point %d lacks an exposed triangle edge\n", p, 0, 0)
    }
    free(is_exposed);


    /* if we haven't already dropped core :-) */
    if (err_cnt != 0) panic( "after mesh self test");
    return;
}
