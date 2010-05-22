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
/*   dispose.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:42 */

#include <stdio.h>
#include <stdlib.h>
#include <global.h>
#include <constant.h>
#include <geom.h>
#include <material.h>	/* So we can set nmat to 0 - want this? */
#include "impurity.h"	/* So we can set n_imp to 0 */
#include "diffuse.h"	/* for the time of creation */


/************************************************************************
 *									*
 *	dis_all() - this routine frees all the currently used 		*
 *  memory for the mesh information.					*
 *									*
 ************************************************************************/
dis_all()
{
    int i;
    while( nsreg > 0 ) free_skel(0);
    dis_pt();
    dis_nd();
    dis_tri();
    dis_edg();
    dis_reg();
    n_imp = 0;
    for (i = 0; i < MAXIMP; i++) soltoimp[i] = imptosol[i] = -1;
    MeshInvalidate();
}

#include <regrid.h>	/* Until we move the macros from regrid.h to geom.h */

/*-----------------WASTE------------------------------------------------
 * To standardize global grid removals
 *----------------------------------------------------------------------*/
int waste()
{
    int ie, save_ne = ne, *new_tri,
	in, save_nn = nn, *new_nd,
	ip, save_np = np, *new_pt,
	    *new_edg,
	ir, save_nreg = nreg, *new_reg,
	j;
    tri_typ *tswap; nd_typ *nswap; pt_typ *pswap;
    struct reg_str *rswap;

    new_tri = salloc( int, ne );
    new_nd = salloc( int, nn );
    new_pt = salloc( int, np );
    new_edg = salloc( int, ned );
    new_reg = salloc( int, nreg );

    /*
     * Remove dead wood - all removal is done here.
     * First we actually remove the objects, then fix pointers to them.
     * EVERYTHING THAT IS INDEXED BY NODE POINT OR TRIANGLE CHANGES
     * HERE
     */
    need_waste = FALSE;
    for (ne = 0, ie = 0; ie < save_ne; ie++)
	if (dead_tri (ie)) {
	    new_tri[ie] = -1; dis_1tri (&tri[ie]);
	    }
	else {
	    new_tri [ie] = ne;
	    tswap = tri[ ie]; tri[ie] = NULL; tri[ ne++] = tswap;
	    }

    for (nn = 0, in = 0; in < save_nn; in++)
	if (dead_nd (in)) {
	    new_nd[in] = -1; dis_1nd (&nd[in]);
	    }
	else {
	    new_nd[in] = nn;
	    nswap = nd[ in]; nd[ in] = NULL; nd[ nn++] = nswap;
	    }

    for (np = 0, ip = 0; ip < save_np; ip++)
	if (dead_pt (ip)) {
	    new_pt[ip] = -1; dis_1pt (&pt[ip]);
	    }
	else {
	    new_pt[ip] = np;
	    pswap = pt[ ip]; pt[ ip] = NULL; pt[np++] = pswap;
	    }

    for (nreg = 0, ir = 0; ir < save_nreg; ir++)
	if (dead_reg (ir)) {
	    new_reg[ir] = -1; dis_1reg (&reg[ir]);
	    }
	else {
	    new_reg[ir] = nreg;
	    rswap = reg[ ir];
	    reg[ ir] = NULL;
	    reg[nreg++] = rswap;
	    }


    /*fix up the triangle structures*/
    for (ie = 0; ie < ne; ie++) {
	/*nodes*/
	for (j=0; j < num_vert(t); j++) {
	    if ((tri[ie]->nd[j] = new_nd[ tri[ie]->nd[j] ]) < 0)
		panic("pointer to zombie node");
	}
	neigh_dirty = TRUE;
	for(j = 0; j < num_face(ie); j++) {
	    if ( tri[ie]->nb[j] > 0 )
		if ((tri[ie]->nb[j] = new_tri[ tri[ie]->nb[j] ]) < 0)
		    clr( tri[ie], NEIGHB );
	}

	/*region*/
	if ((tri[ie]->regnum = new_reg[ tri[ie]->regnum ]) < 0)
	    panic("pointer to zombie region");
    }

    /*fix up the node structures*/
    for (in = 0; in < nn; in++) {
	/*parent point*/
	if ((nd[in]->pt = new_pt[ nd[in]->pt ]) < 0)
	    panic("pointer to zombie point");
    }


    /*fix up the point structure*/
    for (ip = 0; ip < np; ip++) {
	/*nodes*/
	for (j=0; j < pt[ip]->nn; j++) {
	    if ((pt[ip]->nd[j]= new_nd[ pt[ip]->nd[j] ]) < 0)
		panic("pointer to zombie node");
	}
    }

    free( new_tri );
    free( new_nd );
    free( new_pt );
    free( new_edg );
    free( new_reg );
}


lose_impurity( imp)
     int imp;
{
    int i, j, sol = imptosol[ imp ];

    if( sol != -1 ) {
	for( j = sol; j < n_imp-1; j++) {
	    for (i = 0; i < nn; i++)
		nd[ i ]->sol[ j ] = nd[ i ]->sol[ j+1 ];
	    soltoimp[ j ] = soltoimp[ j+1 ];
	    imptosol[ soltoimp[ j ] ] = j;
	}
	n_imp--;
	imptosol[imp] = -1;
	soltoimp[n_imp] = -1;
    }
}


/*-----------------MeshValid--------------------------------------------
 * Try not to crash due to absence of mesh.
 *----------------------------------------------------------------------*/
MeshValid()
{
    /* This is as good a guess as any. */
    /* Maybe someday we'll keep a global flag */
    return( ne != 0 && np != 0 && nn != 0);
}

MeshInvalidate()
{
    ne = 0;
    np = 0;
    nn = 0;
}

InvalidMeshCheck()
{
    if( !MeshValid()) {
	fprintf( stderr, "No mesh defined!\n");
	return(-1);
    }
    else
	return(0);
}


