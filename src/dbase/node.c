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
/*   node.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:21:08 */

#include <stdio.h>
#include <stdlib.h>
#include <global.h>
#include <constant.h>
#include <geom.h>
#include <material.h>	/* So we can set nmat to 0 - want this? */
#include "impurity.h"	/* So we can set n_imp to 0 */
#include "diffuse.h"	/* for the time of creation */

static int maxnd = 0;

/*-----------------ALLOC_ND -----------------------------------
 * Dumb routines to allocate storage and bump np, nn.
 * A good place to put malloc smarts.
 *---------------------------------------------------------------------*/
char *alloc_nd()
{
    int j;

    if (nn + 1 >= maxnd) {
	if ( maxnd == 0 ) {
	    maxnd = 3000;
	    nd = salloc( nd_typ *, maxnd );
	}
	else {
	    maxnd += 1000;
	    nd = sralloc( nd_typ *, maxnd, nd );
	}
    }
    nd[nn] = (nd_typ *) malloc (sizeof (nd_typ));
    if (!nd[nn]) return ("Out of storage in alloc_nd");

    nd[ nn ]->pt = -1;
    nd[ nn ]->mater = -1;
    nd[ nn ]->ltri.all = 0;
    nd[ nn ]->ltri.num = 0;
    nd[ nn ]->ltri.list = NULL;
    nd[ nn ]->edg.all = nd[nn]->edg.num = 0;
    nd[ nn ]->edg.list = NULL;
    nd[ nn ]->step = process_step;
    nd[ nn ]->time = total;
    nd[ nn ]->flags = 0;

    for (j = 0; j < MAXIMP; j++) {
	switch ( soltoimp[j] ) {
	case As:
	case B :
	case P :
	case Sb:
	case I :
	case V :
	case O2:
	case H2O:
	case T :
	case Au:
	case Ga:
	case Cs:
	case iBe:
	case iMg:
	case iSe:
	case iSi:
	case iSn:
	case iGe:
	case iZn:
	case iC :
	case iG :
	    nd[nn]->sol[j] = 1.0e5;
	    break;
	default:
	    nd[nn]->sol[j] = 0.0;
	    break;
	}
    }
    nd2tri_dirty = TRUE;
    nd2edg_dirty = TRUE;

    nn++;
    return(0);
}



/*
 * make a node at a point with the given material
 */
mk_nd( p, m )
int p, m;
{
    char *err;

    if ( (err = alloc_nd()) != NULL ) panic(err);

    /*build the connections to the node*/
    nd[nn-1]->pt = p;
    nd[nn-1]->mater = m;

    /*build the reverse pointer while we are at it*/
    add_nd_pt(p, nn-1);

    return(nn-1);
}



/*
 *	Discard all the nodes
 */
dis_nd()
{
    int i;

    /*free the node information*/
    for (i = 0; i < nn; i++)
	dis_1nd(&(nd[i]));
    nn = 0;
}




/*
 *	Discard a single node
 */
dis_1nd (n)
    struct nd_str **n;
{
    /*free the structure*/
    dis_list(&(n[0]->edg));
    dis_list(&(n[0]->ltri));
    free(n[0]);
    /*null the pointer so that we have no future problems*/
    n[0] = NULL;
}




/************************************************************************
 *									*
 *	make_nodes() - This routine generates the node list in a two 	*
 *  step method.  The first step is determine the number of nodes and 	*
 *  materials that are at each point in the mesh.  This step makes use	*
 *  of the nd array in point as a temp list.  The second pass is	*
 *  through the points to create as many nodes as are needed for the	*
 *  point.								*
 *									*
 *  Original:	MEL	10/85		(Based on the old set_sol for	*
 *					 history buffs)			*
 *									*
 ************************************************************************/
make_nodes()
{
    register int i;	/*every routine needs an i for indexing*/
    register int p;	/*point count*/
    register int t;	/*triangle count*/
    register int mat;	/*material type of the current triangle*/
    char *alloc_nd();
#   define PC(ARG) if (err=ARG) panic(err)

    /*pass 0: clear out the pt->nd arrays (slightly redundant but safe) */
    for( p = 0; p < np; p++) {
	for (i = 0; i < MAXMAT; i++) pt[ p]->nd[ i] = FALSE;
	if ( (is_surf(p)) || (is_back(p)) ) pt[p]->nd[GAS] = TRUE;
    }

    /*pass 1*/
    for(t = 0; t < ne; t++) {
	mat = mat_tri(t);

	/*for each vertex*/
	for(i = 0; i < num_vert(t); i++) {
	    /*each point needs an node for this material*/
	    pt[ vert_tri(t,i) ]->nd[ mat ] = TRUE;
	}
    }

    /*pass 2*/
    for( nn = p = 0; p < np; p++ ) {
	/*for each material marked true, add a node*/
	for(pt[p]->nn = i = 0; i < MAXMAT; i++)
	    if ( pt[p]->nd[i] ) (void)mk_nd(p, i);
    }

    /*that was easier than I expected, how about you?*/
}




/************************************************************************
 *									*
 *	node_to_tri() - This routine generates the node to triangle	*
 *  list.  It is extremely inefficient due to the (>=) nn malloc calls. *
 *									*
 *  Original:	CSR	(Somewhere in Pisces/Iggi)			*
 *  Revised:	MEL	(Made it work for the latest greatest data base)*
 *									*
 ************************************************************************/
node_to_tri()
{
    int i;
    int t;
    int nx;

    for(i = 0; i < nn; i++) dis_list( &(nd[i]->ltri) );

    /*set up the node to triangle array*/
    t = 0;
    while ( done_tri(t) ) {
	for(i = 0; i < num_vert(t); i++) {
	    nx = vert_tri(t,i);
	    add_tri_nd( nx, t );
	}
	next_tri(t);
    }

    /*backwards fucking compatability*/
    for(i = 0; i < nn; i++) set(nd[i], ND2TRI);
}
