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
/*   intersect.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:38:52 */

#include "global.h"
#include "constant.h"

/************************************************************************
 *									*
 *	intersect(a, val, x) - This routine finds the line at cord[3] 	*
 *  == val in the triangle formed by the points a[0], a[1], and c[2].   *
 *  Each point is made in three space, the line is a constant value of 	*
 *  the third coordinate.  The value found is returned in x.  The	*
 *  routine returns the number of crosses.				*
 *									*
 *  Original: 	MEL	1/85 						*
 *									*
 ************************************************************************/
intersect(a, val, x)
float a[MAXVRT][3]; 		/*the coordinates of the triangle*/
float val;		/*the constant value being searched for*/
float x[2][3];		/*the intersection points*/
{
    int s1, s2;		/*side pointers*/
    int i;		/*side counters*/
    int ans;		/*the answer counter*/
    float tau;		/*answer temps*/


    switch ( mode ) {
    case TWOD :
	if ( ( (a[0][2] > val) && (a[1][2] > val) && (a[2][2] > val) ) ||
	     ( (a[0][2] < val) && (a[1][2] < val) && (a[2][2] < val) ) )
	     return( 0 );
	
	for(ans = i = 0; i < 3; i++) {
	    s1 = (i + 1) % 3;
	    s2 = (i + 2) % 3;

	    /*check for an intersection on this side*/
	    if (((a[s1][2] <= val) || (a[s2][2] < val)) &&
		((a[s1][2] >= val) || (a[s2][2] > val)))	{
	
		/*if the side values are equal, return that*/
		if (a[s1][2] == a[s2][2]) {
		    x[0][0] = a[s1][0];   x[0][1] = a[s1][1];   x[0][2] = a[s1][2];
		    x[1][0] = a[s2][0];   x[1][1] = a[s2][1];   x[1][2] = a[s2][2];
		    return( 2);
		}

		/*else, calculate an intersection*/
		tau = ( val - a[s1][2] ) / ( a[s2][2] - a[s1][2] );
		x[ans][0] = (a[s2][0] - a[s1][0]) * tau + a[s1][0];
		x[ans][1] = (a[s2][1] - a[s1][1]) * tau + a[s1][1];
		x[ans][2] = val;
		ans++;
	    }
	}
	return( ans );
	break;

    case ONED :
	for(i = 0; i < 3; i++) { 
	    x[0][i] = a[0][i]; 
	    x[1][i] = a[1][i];
	}
	return( 2 );
	break;
    
    }
    return(0);
}
