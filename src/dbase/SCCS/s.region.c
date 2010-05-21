h03004
s 00000/00000/00126
d D 2.1 93/02/03 14:33:37 suprem45 2 1
c Initial IV.GS release
e
s 00126/00000/00000
d D 1.1 93/02/03 14:30:34 suprem45 1 0
c date and time created 93/02/03 14:30:34 by suprem45
e
u
U
f e 0
t
T
I 1
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
/*   region.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:21:12 */

#include <stdio.h>
#include <global.h>
#include <constant.h>
#include <geom.h>
#include <material.h>	/* So we can set nmat to 0 - want this? */


mk_reg(mat)
int mat;
{
    if (nreg+1 > MAXREG) panic ("Too many edges generated");
    reg[nreg] = (reg_typ *) malloc (sizeof (reg_typ));
    if (!reg[nreg]) panic ("Out of storage in alloc_region");

    reg[nreg]->mater = mat; 

    reg[nreg]->edg.num = reg[nreg]->edg.all = 0; reg[nreg]->edg.list = NULL;
    reg[nreg]->tri.num = reg[nreg]->tri.all = 0; reg[nreg]->tri.list = NULL;
    reg[nreg]->fac.num = reg[nreg]->fac.all = 0; reg[nreg]->fac.list = NULL;
    reg[nreg]->nd.num = reg[nreg]->nd.all = 0; reg[nreg]->nd.list = NULL;
    reg[nreg]->sreg = -1;
    reg[nreg]->flags = 0;

    nreg++;
    reg_dirty = TRUE;
    return(nreg-1);
}



dis_reg()
{
    int i;
    for (i=0; i < nreg; i++)
	dis_1reg(&(reg[i]));
    nreg=0;
}

dis_1reg (r)
    struct reg_str **r;
{
	/*free the rest of the structure*/
	if (r[0]->tri.num != 0 ) dis_list(&(r[0]->tri));
	if (r[0]->edg.num != 0 ) dis_list(&(r[0]->edg));
	if (r[0]->fac.num != 0 ) dis_list(&(r[0]->fac));
	if (r[0]->nd.num != 0 ) dis_list(&(r[0]->nd));
	if (r[0]->sreg != -1 ) free_skel(r[0]->sreg);
	free(r[0]);

	/*null the pointer so that we have no future problems*/
	r[0] = NULL;
}



/*
 * 	This routine constructs a single simply connected regions.
 */
build_reg( r )
int r;
{
    register int e, t, in, i, nnb, nb;
    int *test;
    
    test = salloc( int, nn );

    /*throw away the old stuff*/
    dis_list(&(reg[r]->tri));
    dis_list(&(reg[r]->edg));
    dis_list(&(reg[r]->fac));
    dis_list(&(reg[r]->nd));

    /*first generate a list of all the edges contained in each region*/
    for(e = 0; e < ned; e++) {
	if ( reg_tri( tri_edg(e,0) ) == r ) add_edg_reg(r, e);
    }
    for(i = 0; i < nn; i++) test[i] =FALSE;

    /*clear the exposed region flag*/
    clr( reg[r], EXPOS );

    /*now build the unordered region boundary description*/
    t = 0; 
    while( done_tri(t) ) {
	if (reg_tri(t) == r) {
	    for(i = 0; i < num_vert(t); i++) test[vert_tri(t,i)] = TRUE;

	    add_tri_reg(r, t);
	    nnb = num_face(t);
	    for(i = 0; i < nnb; i++) {
		if ( is_face_exp(t, i) ) set(reg[r], EXPOS);
		nb = neigh_fc(t, i);
		if ( (nb < 0) || (reg_tri(nb) != r) ) add_bnd_reg(r,t,i);
	    }
	}
	next_tri(t);
    }

    /*build the node list*/
    for(in = 0; in < nn; in++) {
	if (test[in]) add_nd_reg(r, in);
    }

    free(test);
    /*build and save the skeleton region*/
    reg[r]->sreg = skel_reg(r);
}

E 1
