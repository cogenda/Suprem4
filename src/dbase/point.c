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
/*   point.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:21:10 */

#include <stdio.h>
#include <stdlib.h>
#include <global.h>
#include <constant.h>
#include <geom.h>
#include <material.h>	/* So we can set nmat to 0 - want this? */
#include "impurity.h"	/* So we can set n_imp to 0 */
#include "diffuse.h"	/* for the time of creation */

static int maxpt = 0;

/*-----------------ALLOC_PT-----------------------------------
 * Dumb routines to allocate storage and bump np, nn.
 * A good place to put malloc smarts.
 *---------------------------------------------------------------------*/
char *alloc_pt()
{
    int j;

    if (np + 1 >= maxpt) {
	if ( maxpt == 0 ) {
	    maxpt = 3000;
	    pt = salloc( pt_typ *, maxpt );
	}
	else {
	    maxpt += 1000;
	    pt = sralloc( pt_typ *, maxpt, pt );
	}
    }
    pt[np] = (pt_typ *) malloc (sizeof (pt_typ));
    if (!pt[np]) return ("Out of storage in alloc_pt");

    pt[np]->cord[0] = pt[np]->cordo[0] = MAXFLOAT;
    pt[np]->cord[1] = pt[np]->cordo[1] = MAXFLOAT;
    pt[np]->cord[2] = pt[np]->cordo[2] = MAXFLOAT;

    pt[np]->vel[0] = pt[np]->ovel[0] = 0.0;
    pt[np]->vel[1] = pt[np]->ovel[1] = 0.0;
    pt[np]->vel[2] = pt[np]->ovel[2] = 0.0;

    set_space(np, MAXFLOAT);
    pt[ np ]->flags = 0;
    pt[ np ]->nn = 0;

    for (j = 0; j < MAXMAT; j++)
	pt[ np]->nd[ j] = -1;

    np++;
    return(0);
}


/*
 *  Create a point out of the minimal information required for it.
 */
mk_pt( nc, cord )
int nc;
float *cord;
{
    char *err;
    int i;

    if ((err = alloc_pt()) != NULL) panic( err );

    for(i = 0; i < nc; i++) pt[np-1]->cord[i] = cord[i];

    return( np-1 );
}




/*
 * destroy all points
 */
dis_pt()
{
    int i;
    for (i=0; i < np; i++)
	dis_1pt(&(pt[i]));
    np = 0;
}


/*
 * destroy a single pooint
 */
dis_1pt (p)
    struct pt_str **p;
{
	/*free the rest of the structure*/
	free(p[0]);

	/*null the pointer so that we have no future problems*/
	p[0] = NULL;
}




/************************************************************************
 *									*
 *	pt_to_node() - This routine assumes the nodes have been created *
 *  using a structure read and then generates the point to node list 	*
 *									*
 *  Original:	MEL	4/86						*
 *									*
 ************************************************************************/
pt_to_node()
{
    int i;

    /*clear the number of nodes list*/
    for(i = 0; i < np; i++) pt[i]->nn = 0;

    /*step through the node list and build the pt to node stuff*/
    for(i = 0; i < nn; i++) add_nd_pt( pt_nd(i), i );
}

