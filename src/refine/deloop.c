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
/*   deloop.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:37 */

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include "refine.h"

/************************************************************************
 *									*
 *	de_loop() - find and remove loops in a region.			*
 *									*
 *  Original:	MEL	10/90						*
 *									*
 ************************************************************************/
de_loop( nop, op, eps )
int *nop;
struct line *op;
double eps;
{
    register int bp1, bp2;
    register int i;
    int numop = *nop;
    float p[MAXDIM], dp[MAXDIM], q[MAXDIM], dq[MAXDIM];
    float alph[MAXDIM], c[MAXDIM];
    float etol = 1.0e-6, etol1 = 1.0 + 1.0e-6;

    if ( mode == ONED ) return(FALSE);

    for(bp1 = 2; bp1 < numop; bp1++) {

	p[0]  = op[bp1-1].x;
	p[1]  = op[bp1-1].y;
	dp[0] = op[bp1].x - p[0];
	dp[1] = op[bp1].y - p[1];

	/*check all the prior edges w/r to this edge*/
	for(bp2 = bp1 - 2; bp2 > 0; bp2--) {

	    q[0]  = op[bp2-1].x;
	    q[1]  = op[bp2-1].y;
	    dq[0] = op[bp2].x - q[0];
	    dq[1] = op[bp2].y - q[1];

	    if ( lil(p,dp,q,dq,alph) == 0 ) {
		if ((alph[0] >= -etol) && (alph[0] <= etol1) &&
		    (alph[1] >= -etol) && (alph[1] <= etol1) ) {

		    /*compute the intersection point*/
		    c[0] = alph[0] * dp[0] + p[0];
		    c[1] = alph[0] * dp[1] + p[1];

		    numop = bp2;
		    add_toline(&numop,op,c[0],c[1],op[bp2].map, eps);
		    add_toline(&numop,op,op[bp1].x,op[bp1].y,op[bp1].map, eps);

		    for(i = bp1+1; i < *nop; i++) {
			op[numop].x = op[i].x;
			op[numop].y = op[i].y;
			op[numop].map = op[i].map;
			op[numop].p = op[i].p;
			numop++;
		    }
		    *nop = numop;
		    return(TRUE);
		}
	    }
	}
    }
    return( FALSE );
}

