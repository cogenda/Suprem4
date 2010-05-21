h10985
s 00000/00000/00070
d D 2.1 93/02/03 14:47:01 suprem45 2 1
c Initial IV.GS release
e
s 00070/00000/00000
d D 1.1 93/02/03 14:41:25 suprem45 1 0
c date and time created 93/02/03 14:41:25 by suprem45
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
 *************************************************************************/
/*   species.c                Version 5.1     */
/*   Last Modification : 7/3/91  10:50:21 */

#include <stdio.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "diffuse.h"
#include "impurity.h"
#include "matrix.h"


/************************************************************************
 *									*
 *	This file contains definitions of routines which are used in	*
 * the specie dependent routines.  They are mostly data base access 	*
 * routines written so that a user may modify the model routines, and 	*
 * not have to understand the structure of the data base.		*
 *									*
 ************************************************************************/



/************************************************************************
 *									*
 *	clear_row(n, imp) - This zeroes the row for the node n.  	*
 *  It is useful for fixed boundary conditions.				*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
clear_row(n, sol)
int n;
int sol;
{
    register int j,k;

    for(j = 0; j < n_imp; j++) {
	
	switch( blktype[sol][j] ) {

	case B_NONE :
	    break;

	case B_DIAG :
	    a[sol][j][n] = 0.0;
	    for(k = irow_d[n]; k < irow_d[n+1]; k++) a[sol][j][jrow_d[k]] = 0.0;
	    break;

	case B_TRI :
	case B_BLCK :
	    a[sol][j][n] = 0.0;
	    for(k = irow[n]; k < irow[n+1]; k++) a[sol][j][jrow[k]] = 0.0;
	    break;
	}
    }

    newb[sol][n] = 0.0;
}



E 1
