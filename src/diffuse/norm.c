/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   norm.c                Version 5.1     */
/*   Last Modification : 7/3/91  10:50:07 */

#include <stdio.h>
#include <math.h>

/************************************************************************
 *									*
 *	norm( arr, n ) - This routine calculates the norm of the array	*
 *  of doubles arr with length n.  Initially, it is set up to handle 	*
 *  a single norm, the two norm.  Later, it should be configured	*
 *  to do several different norms based on a passed parameter.		*
 *									*
 *  Original:	MEL	12/84						*
 *									*
 ************************************************************************/
double norm( arr, n )
double *arr;
int n;
{
    double f;
    int i;

    f = 0.0;

    for(i = 0; i < n; i++)  {
	f += arr[i] * arr[i];
    }

    f = sqrt( f / n );
    return( f );
}
    

