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
/*   element.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:45 */

#include <stdio.h>
#include <stdlib.h>
#include <global.h>
#include <constant.h>
#include <dbaccess.h>
#include <material.h>	/* So we can set nmat to 0 - want this? */

static int maxtri = 0;

/*
 * Initialize the triangle structure
 */
char *alloc_tri()
{
    int j;

    if (ne + 1 >= maxtri) {
	if ( maxtri == 0 ) {
	    maxtri = 3000;
	    tri = salloc( tri_typ *, maxtri );
	}
	else {
	    maxtri += 1000;
	    tri = sralloc( tri_typ *, maxtri, tri );
	}
    }
    tri[ne] = (tri_typ *) calloc (1, sizeof (tri_typ));
    if (!tri[ne]) return ("Out of storage in alloc_tri");

    for (j = 0; j < MAXVRT; j++) {
	tri[ ne ]->nd [ j ] = -1;
	tri[ ne ]->earea[j] = MAXFLOAT;
	tri[ ne ]->sig[ j ] = 0;
    }
    for (j = 0; j < MAXSID; j++) {
	tri[ ne ]->nb [ j ] = -1;
	tri[ ne ]->ehed[j] = tri[ ne ]->d[j] = MAXFLOAT;
	tri[ ne ]->sig[ j ] = 0;
    }

    tri[ne]->fath  = NO_TRIANGLE;
    tri[ne]->son   = NO_TRIANGLE;
    tri[ne]->level = 0;
    tri[ne]->flags = 0;
    geom_dirty = TRUE;
    clkws_dirty = TRUE;
    neigh_dirty = TRUE;

    ne++;
    return(0);
}


/*
 * make an element with vertices and neighbor information
 */
mk_ele( nv, v, nb, b, r, flag )
int nv, *v;
int nb, *b;
int r;
int flag;
{
    char *err;
    register int i, j, p, t;

    if ((err = alloc_tri()) != NULL) panic( err );

    for(i = 0; i < nv; i++) {
	tri[ne-1]->nd[i] = v[i];
	if (!flag) add_tri_nd(v[i], ne-1);
    }
    for(i = 0; i < nb; i++) tri[ne-1]->nb[i] = b[i];
    tri[ne-1]->regnum = r;
    if ( reg[r] ) add_tri_reg(r, ne-1);

    if (flag) {
	tripts = TRUE;
	set(tri[ne-1], TRIPTS);
    }
    else
	clr(tri[ne-1], TRIPTS);

    /*make sure points correspond to edge codes*/
    t = ne - 1;
    for (j=0; j < num_face(t); j++) {
	if ( is_face_bck(t,j) ) {
	    i = num_nd_fc(t,j);
	    if ( flag )
		for(p = 0; p < i; p++) set_back(nd_face_ele(t,j,p));
	    else
		for(p = 0; p < i; p++) set_back(pt_nd(nd_face_ele(t,j,p)));
	}
	if ( is_face_exp(t,j) ) {
	    i = num_nd_fc(t,j);
	    if ( flag )
		for(p = 0; p < i; p++) set_surf(nd_face_ele(t,j,p));
	    else
		for(p = 0; p < i; p++) set_surf(pt_nd(nd_face_ele(t,j,p)));
	}
    }
    return( ne-1 );
}


/*
 * mk an element out of points
 */
mk_ele_pt( nv, v, reg )
int nv, *v;
int reg;
{
    char *err;
    int i;
    int nb = (mode==ONED)?2:3;

    if ((err = alloc_tri()) != NULL) panic( err );

    for(i = 0; i < nv; i++) tri[ne-1]->nd[i] = v[i];
    for(i = 0; i < nb; i++) tri[ne-1]->nb[i] = -1024;
    tri[ne-1]->regnum = reg;
    tripts = TRUE;
    set(tri[ne-1], TRIPTS);
    bc_dirty = TRUE;
    clr(tri[ne-1], GEOMDN);

    return( ne-1 );
}


/*
 * mk an element out of nodes
 */
mk_ele_nd( nv, v, r )
int nv, *v;
int r;
{
    char *err;
    int i;
    int nb = (mode==ONED)?2:3;

    if ((err = alloc_tri()) != NULL) panic( err );

    for(i = 0; i < nv; i++) {
	tri[ne-1]->nd[i] = v[i];
	add_tri_nd(v[i], ne-1);
    }
    for(i = 0; i < nb; i++) tri[ne-1]->nb[i] = -1024;
    tri[ne-1]->regnum = r;
    if ( reg[r] ) add_tri_reg(r, ne-1);
    clr(tri[ne-1], TRIPTS);
    bc_dirty = TRUE;

    return( ne-1 );
}




/*
 * discard all triangles
 */
dis_tri()
{
    int i;

    for (i=0; i < ne; i++)
	dis_1tri(&(tri[i]));
    ne = 0;
}


/*
 * discard a single triangle
 */
dis_1tri (t)
    struct tri_str **t;
{
    /*free the structure*/
    free(t[0]);
    /*null the pointer so that we have no future problems*/
    t[0] = NULL;
}


/************************************************************************
 *									*
 *	make_bc() - This routine marks the edge codes based on point	*
 *  flags.								*
 *									*
 *  Original:	MEL	9/90						*
 *					 				*
 *									*
 ************************************************************************/
make_bc()
{
    register int ie, jf, nf, nfp, fp;
    register int bck, srf;

    /*check every element*/
    for(ie = 0; ie < ne; ie++) {

	if (ask(tri[ie], TRIPTS)) {
	    /*check every element facet*/
	    nf = num_face(ie);
	    for(jf = 0; jf < nf; jf++) {

		/*for each point on this facet*/
		nfp = num_nd_fc(ie, jf);
		bck = TRUE;
		srf = TRUE;
		for(fp = 0; fp < nfp; fp++) {
		    bck = bck && is_back( (nd_face_ele(ie, jf, fp)) );
		    srf = srf && is_surf( (nd_face_ele(ie, jf, fp)) );
		}
		if ( bck ) {set_face_bck(ie, jf); clr(tri[ie], NEIGHB);}
		if ( srf ) {set_face_exp(ie, jf); clr(tri[ie], NEIGHB);}
	    }
	}
	else {
	    /*check every element facet*/
	    nf = num_face(ie);
	    for(jf = 0; jf < nf; jf++) {

		/*for each point on this facet*/
		nfp = num_nd_fc(ie, jf);
		bck = TRUE;
		srf = TRUE;
		for(fp = 0; fp < nfp; fp++) {
		    bck = bck && is_back( pt_nd(nd_face_ele(ie, jf, fp)) );
		    srf = srf && is_surf( pt_nd(nd_face_ele(ie, jf, fp)) );
		}
		if ( bck ) {set_face_bck(ie, jf); clr(tri[ie], NEIGHB);}
		if ( srf ) {set_face_exp(ie, jf); clr(tri[ie], NEIGHB);}
	    }
	}
    }
}





/************************************************************************
 *									*
 *	tri_to_nod() - This routine is entered with the triangle vertex	*
 *  list being points rather than nodes.  Foreach triangle, the point's	*
 *  node list is checked to find the matching material node.		*
 *  This works out to be a bit of a thrash...				*
 *									*
 *  Original:	MEL	10/85						*
 *									*
 ************************************************************************/
tri_to_node()
{
    int t;	/*triangle counter*/
    int i;	/*vertex counter*/
    int p;	/*point number*/
    int n;	/*node counter*/
    int mat;	/*material of the triangle in question*/

    /*foreach triangle*/
    for(t = 0; t < ne; t++) {
	if ( ! ask( tri[t], TRIPTS ) ) continue;

	/*get the material in a local*/
	mat = mat_tri(t);

	/*foreach vertex*/
	for(i = 0; i < num_vert(t); i++) {

	    /*get the point number in a local*/
	    p = vert_tri(t,i);
	    for(n = 0; n < num_nd(p); n++) {
		if ( mat == mat_nd( nd_pt(p,n) ) )
		    set_vert_tri(t, i, nd_pt(p,n) );
	    }
	}
	clr( tri[t], TRIPTS );
    }
    /*whew!*/
}




/************************************************************************
 *									*
 *	nxtel - this routine generates the neighbor triangle arrays     *
 *      It calls p2t to pre-compute point-to-triangle info.             *
 *									*
 *    Original : C.H.Price     Stanford University        May, 1982	*
 *    Revision : CSR           Stanford University        Nov, 1983	*
 *									*
 ************************************************************************/
nxtel()
{
    register int jelema, n, in;
    register int i, p, j;
    int nsidea, nsideb, jelemb;

    /*this routine does not check or build the external edge codes*/
    make_bc();

    /*set up the neighbor triangle list using the nd data for info*/
    jelema = 0;
    while ( done_tri(jelema) ) {

	if ( !ask(tri[jelema], NEIGHB)) {
	    set(tri[jelema], NEIGHB);
	    /*check each of the vertices connect list*/
	    for(i = 0; i < num_vert(jelema); i++) {

		p = pt_nd( vert_tri(jelema, i) );

		/*for all the nodes at this point*/
		for (in = 0; in < num_nd(p); in++) {
		    n = nd_pt(p, in);

		    /*for all the triangles at this node*/
		    for(j = 0; j < num_tri_nd(n); j++) {

			jelemb = tri_nd(n, j);

			/*do not check this triangle or higher numbered ones*/
			if (jelemb != jelema) {
			    if (tnabor (jelema, jelemb, &nsidea, &nsideb)) {

				/*make the entries*/
				tri[jelema]->nb[nsidea] = jelemb;
				tri[jelemb]->nb[nsideb] = jelema;
			    }
			}
		    }  /*end of triangle list for this node*/
		}/*end of nodes at this vertex*/
	    }  /*end of the vertices to check*/
	}

	next_tri(jelema);
    }   /*end of the triangles*/
}
