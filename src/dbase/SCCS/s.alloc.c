h03637
s 00000/00000/00046
d D 2.1 93/02/03 14:33:20 suprem45 2 1
c Initial IV.GS release
e
s 00046/00000/00000
d D 1.1 93/02/03 14:30:26 suprem45 1 0
c date and time created 93/02/03 14:30:26 by suprem45
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
/*   alloc.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:34 */

#include <stdio.h>
#include <global.h>
#include <constant.h>
#include "dbaccess.h"	/* So we can set n_imp to 0 */
#include "impurity.h"	/* So we can set n_imp to 0 */
#include "diffuse.h"	/* for the time of creation */

/*-----------------ADD_IMPURITY-----------------------------------------
 * Adds a new impurity to the solution set...
 *----------------------------------------------------------------------*/
add_impurity( imp, background, mat)
     int imp;			/* The impurity number */
     float background;		/* A default value to give nodes */
     int mat;			/* If default is only to be given to some */
{
    int i, sol;
    
    if ( imptosol[ imp ] == -1) {
	soltoimp[ n_imp ] = imp;
	imptosol[ imp ] = n_imp;
	sol = n_imp++;
	for( i = 0; i < nn; i++ )
	    if( mat < 0 || nd[ i ]->mater == mat)
		nd[ i ] -> sol[ sol ] = background;
    }
}


E 1
