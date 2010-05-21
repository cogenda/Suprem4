h49209
s 00000/00000/00050
d D 2.1 93/02/03 14:33:23 suprem45 2 1
c Initial IV.GS release
e
s 00050/00000/00000
d D 1.1 93/02/03 14:30:27 suprem45 1 0
c date and time created 93/02/03 14:30:27 by suprem45
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
/*   dimen.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:41 */

#include "global.h"
#include "constant.h"
#include "impurity.h"
#include "geom.h"


/************************************************************************
 *									*
 *	set_dim( dim ) - This routine initiailizes the dimensionality	*
 *									*
 *  Original:	MEL	10/84						*
 *									*
 ************************************************************************/
set_dim( dim )
int dim;
{
    switch( dim ) {
    case 1 :
       mode = ONED;
       nvrt = 2;
       nedg = 2;
       oxide_model = O_VERTICAL;
       break;
    case 2 :
       mode = TWOD;
       nvrt = 3;
       nedg = 3;
       break;
    default :
	panic("three dimensions are net yet supported");
    }
}
E 1
