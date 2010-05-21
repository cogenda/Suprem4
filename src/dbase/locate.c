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
/*   locate.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:21:01 */

#include <stdio.h>
#include <global.h>
#include <constant.h>
#include <geom.h>


/*-----------------TWHICH, PWHICH---------------------------------------
 * Find which neighbor/node/pt of a triangle is the given neighbor/node/pt.
 * Would be macros were C perfect. Returns 3 if anything went wrong.
 *----------------------------------------------------------------------*/
twhich (in, wanted)
    int in, wanted;
{
    int j;
    for (j=0; j<nedg; j++)
	if (tri[in]->nb[j] == wanted)
	    break;
    return(j);
}

nwhich (in, wanted)
    int in, wanted;
{
    int j;
    for (j=0; j<nvrt; j++)
	if (tri[in]->nd[j] == wanted)
	    break;
    return(j);
}

ewhich(it, wanted)
int it, wanted;
{
    int j;
    for (j=0; j< num_edge(it); j++) if (edg_ele(it,j) == wanted) break;
    return(j);
}


/*-----------------NODE_MAT---------------------------------------------
 * Find the node under a particular node which is the right material.
 * -1 for bad.
 *----------------------------------------------------------------------*/
node_mat (in, wanted)
    int in, wanted;
{
    int ip, nx;
    ip = pt_nd(in);

    for (nx = 0; nx < num_nd(ip); nx++)
	if ( mat_nd(nd_pt(ip,nx)) == wanted ) return( nd_pt(ip,nx) );

    return(-1);
}

