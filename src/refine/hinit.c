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
/*   hinit.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:43 */

#include <math.h>
#include "global.h"
#include "constant.h"
#include "skel.h"
#include "dbaccess.h"

/*-----------------HINIT------------------------------------------------
 * Initialize the local spacing for each node.
 *----------------------------------------------------------------------*/
int hinit(ir, sp)
int ir;
double sp;
{
    int f, p, i, n;
    struct LLedge *bp;
    double l_edge();

#define min2(A,B) (A < B)?A:B

    for(f=1, bp = sreg[ir]->bnd; (bp != sreg[ir]->bnd)||f; f=0, bp = bp->next) {
	n = nB(bp);
	p = pt_nd(n);

	/*for each edge, spacing is set to sp*/
	set_space( p, sp );
	for(i = 0; i < num_edge_nd(n); i++) {
	    set_space(p,min2(get_space(p),len_edg(edge_nd(n,i))));
	}
    }
}
