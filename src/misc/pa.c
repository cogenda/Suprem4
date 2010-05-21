
/*----------------------------------------------------------------------
**  Copyright 1987 by
**  The Board of Trustees of the Leland Stanford Junior University
**  All rights reserved.
**
**  This routine may not be used without the prior written consent of
**  the Board of Trustees of the Leland Stanford University.
**----------------------------------------------------------------------
**/

/* "pa" :  (printarray) Print the specified range of a double precision
 *	   array.
 *
 * Written: Stephen E. Hansen (30 apr 87)
 *
 *  Calling sequence:
 *   (void)pa(dp, first, last)
 *
 *  Parameters (input):
 *   double	 dp		The array to be output.
 *   int	 first		The index of the first point to be output.
 *   int	 last		The index of the last point to be output.
 *
 *  Parameters (Output): (None.)
 *
 *  Notes:
 *
 */

#include <stdio.h>

static	 int r0 = -1 ;
static	 int r1 = -1 ;

void
par(first, last)
	int	 first ;
	int	 last ;
	{
	r0 = first ;
	r1 = last ;
	}

void
pa(dp, first, last)
	double	*dp ;
	int	 first, last ;
	{
	int	 i = 0 ;
	if (i) par(i, i) ;

	if (r0 >= 0) first = r0 ;
	if (r1 >= 0) last = r1 ;
	for (i = first ; i <= last ; i++) {
		printf("**[%d] = %G\n", i, dp[i]) ;
		}
	}

