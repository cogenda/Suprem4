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
/*   make_db.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:21:02 */

#include <stdio.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"	/* For nmat mattyp GAS */


/************************************************************************
 *									*
 *	This file contains routines that help maintain the data base.	*
 *  There are three main sets of routines.  The first builds the data	*
 *  base elements from scratch, which is typically done only during 	*
 *  the mesh initialization.  The second builds the connectivity pieces	*
 *  from the fundamental set of pieces.  The third carefully(?) updates	*
 *  the data base in a minmimal way.  					*
 *									*
 ************************************************************************/

/************************************************************************
 *									*
 *	create_db() - This routine builds the fundamental set of nodes,	*
 *  points, and elements.  Everything else is constructed from this set.*
 *  It then builds the connectivity non fundamental pieces of data.	*
 *									*
 *  Original:	MEL	9/90						*
 *									*
 ************************************************************************/
create_db( mk_nodes )
int mk_nodes;
{
    int i;

    /*make edge boundaries self-consistent with the points*/
    bc_dirty = FALSE; 
    make_bc();

    /*if we have no nodes read in*/
    if (mk_nodes) {
	/*first order of business is to create the nodes*/
	make_nodes();
	/*next, change the triangle to point data to triangle to nodes*/
	tripts = FALSE; 
	tri_to_node();
    }
    else {
	/*make Gas nodes for points if they need them and don't have them*/
	for(i = 0; i < np; i++)
	    if ( is_surf(i) && (node_mat(nd_pt(i,0), GAS)==-1) ) mk_nd(i, GAS);

	/*get the point to node lists set up*/
	pt_to_node();
    }

    /*next, change the triangle to point data to triangle to nodes*/
    if (tripts) {tripts = FALSE; tri_to_node();}

    /*next build the connectivity pieces that are not fundamental*/
    bd_connect("Mesh Creation");
}



/************************************************************************
 *									*
 *	bd_connect() - This rebuilds the connectivity list from scratch *
 *									*
 *  Original:	MEL	9/90						*
 *									*
 ************************************************************************/
bd_connect(when)
char *when;
{

    int r;

    /*free all existing skeletons*/
    while( nsreg > 0 ) free_skel(0);

    /*clean up any dead stuff first*/
    if ( need_waste ) {need_waste = FALSE; waste();}

    /*make sure the damn triangles are clockwise ordered*/
    if (clkws_dirty) {clkws_dirty = FALSE; clock_tri();}

    /*next, generate the node to triangle lists*/
    node_to_tri();

    /*finally, generate the neighbor lists*/
    if (neigh_dirty) {neigh_dirty = FALSE; nxtel();}

    /*generate edges*/
    build_edg();

    /*build geometry*/
    geom( when );
    geom_dirty = FALSE;

    /*generate regions*/
    for(r = 0; r < nreg; r++) build_reg(r);

    /*check for consistency*/
    mtest1(when);
    mtest2(when);
}
