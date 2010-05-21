h59339
s 00000/00000/00043
d D 2.1 93/02/03 15:10:32 suprem45 2 1
c Initial IV.GS release
e
s 00043/00000/00000
d D 1.1 93/02/03 15:08:29 suprem45 1 0
c date and time created 93/02/03 15:08:29 by suprem45
e
u
U
f e 0
t
T
I 1
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
/*   imp_lat.c                Version 5.1     */
/*   Last Modification : 7/3/91 12:08:37 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "implant.h"

/************************************************************************
 *									*
 *	imp_latr( pos, left, right, data ) - This routine calculates 	*
 *  the lateral distribution function for lateral standard deviation 	*
 *  side and distance from the right and left sides of the slice.	*
 *									*
 *  Original:	MEL	1/87						*
 *									*
 ************************************************************************/
double imp_latr(pos, data)
double pos;
struct imp_info *data;
{
    double siglat, val;

    val = exp( - (pos * pos) / (2.0 * data->lat[LATSIG] * data->lat[LATSIG]) );
	val /= data->lat[LATAREA] * 1.0e-4;
	return(val);
}
E 1
