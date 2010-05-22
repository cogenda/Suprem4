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
/*   edge.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:44 */

#include <stdio.h>
#include <stdlib.h>
#include <global.h>
#include <constant.h>
#include <dbaccess.h>

#define MIN2( A, B ) ( (A)<(B) )?(A):(B)

static int maxedg = 0;

/*
 * Create an edge from scratch
 */
mk_edg(n1, n2)
int n1, n2;
{
    if (ned + 1 >= maxedg) {
	if ( maxedg == 0 ) {
	    maxedg = 3000;
	    edg = salloc( edg_typ *, maxedg );
	}
	else {
	    maxedg += 1000;
	    edg = sralloc( edg_typ *, maxedg, edg );
	}
    }
    edg[ned] = (edg_typ *) malloc (sizeof (edg_typ));
    if (!edg[ned]) panic ("Out of storage in alloc_edge");

    edg[ned]->nd[0] = n1;
    edg[ned]->nd[1] = n2;

    edg[ned]->len = ndist(n1,n2);

    edg[ned]->cpl = MAXFLOAT;
    edg[ned]->flags = 0;

    edg[ned]->skel.all = 0;
    edg[ned]->skel.num = 0;
    edg[ned]->skel.list = NULL;

    edg[ned]->ele.all = 0;
    edg[ned]->ele.num = 0;
    edg[ned]->ele.list = NULL;

    add_edge_nd( n1, ned );
    add_edge_nd( n2, ned );

    clr(edg[ned], GEOMDN);
    ned++;
    return(ned-1);
}


/*
 * This routine printy prints an edge
 */
pr_edg(e)
int e;
{
    printf("Nodes\t%d\t%d\n", nd_edg(e,0), nd_edg(e,1) );
    printf("Elements=\n");
    pr_list( &(edg[e]->ele) );
    printf("Coupling\t%e\n", cpl_edg(e));
    printf("Length\t%e\n", len_edg(e));
    if (ask(edg[e], GEOMDN)) printf("Geometry Done\n");
}



dis_edg()
{
    int i;

    for(i = 0; i < ned; i++) {
	dis_1edg(&(edg[i]));
    }
    ned = 0;
}


dis_1edg (e)
    struct edg_str **e;
{
	/*free the rest of the structure*/
	dis_list(&(e[0]->ele));
	free(e[0]);

	/*null the pointer so that we have no future problems*/
	e[0] = NULL;
}




/*this will not work in three dimensions, I need connectivity data*/
build_edg()
{
    register int ie, n1, n2, en, nb, nnb, i;

    dis_edg();
    for(i = 0; i < nn; i++) dis_list(&(nd[i]->edg));
    ned = 0;
    geom_dirty = TRUE;

    /*now the hard part, getting elements to point in*/
    switch(mode) {
    case ONED :
	ie = 0;
	while( done_tri(ie) ) {
	    n1 = vert_tri(ie, 0);
	    n2 = vert_tri(ie, 1);
	    en = mk_edg(n1, n2);
	    set_edg_ele(ie, 0, en);
	    add_ele_edg(en, ie);
	    next_tri(ie);
	}
	break;

    case TWOD :
	ie = 0;
	while( done_tri(ie) ) {
	    nnb = num_face(ie);
	    for(i = 0; i < nnb; i++) {
		n1 = nd_face_ele(ie,i,0);
		n2 = nd_face_ele(ie,i,1);
		nb = neigh_fc(ie, i);
		if ( nb >= 0 ) {
		    if ( same_mat(nb, ie) && (ie < nb)) {
			en = mk_edg(n1, n2);
			set_edg_ele(ie, i, en);
			add_ele_edg(en, ie);
			set_edg_ele(nb, twhich(nb, ie), en);
			add_ele_edg(en, nb);
		    }
		    else if ( !same_mat(nb, ie) ) {
			en = mk_edg(n1, n2);
			set_edg_ele(ie, i, en);
			add_ele_edg(en, ie);
		    }
		}
		else {
		    en = mk_edg(n1, n2);
		    set_edg_ele(ie, i, en);
		    add_ele_edg(en, ie);
		    if ( nb == EXPOSED ) set(edg[en], ESURF );
		    if ( nb == BACKSID ) set(edg[en], EBACK );
		}
	    }
	    next_tri(ie);
	}
	break;
    }
}



