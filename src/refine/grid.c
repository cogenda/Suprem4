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
/*   grid.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:42 */

#include <stdio.h>
#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include "skel.h"

/*-----------------TRIANG-----------------------------------------------
 * Triangulate a region. The core routine.
 *----------------------------------------------------------------------*/
grid(ir, rbn, bflag, spflag)
int ir;
int rbn;
int bflag;
int spflag;
{
    if ( mode == TWOD )
	triang( ir, rbn, bflag, spflag );
    else if ( mode == ONED )
	lineseg(ir, rbn );
    else 
	panic("Three dimensional regrid is not yet supported");
}



/*
 * 	Generate the line segments on a skeleton into elements
 */
lineseg( ir, rbn )
int ir;
int rbn;
{
    struct LLedge *bp;
    int i;
    int n[2];

    for(i=1, bp=sreg[ir]->bnd; i || bp != sreg[ir]->bnd; i=0, bp = bp->next) {
	n[0] = nd_edg(bp->edge,0);
	n[1] = nd_edg(bp->edge,1);
	(void)mk_ele_nd(2, n, rbn);
    }
}

