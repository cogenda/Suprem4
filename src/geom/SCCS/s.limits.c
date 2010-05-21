h58163
s 00000/00000/00048
d D 2.1 93/02/03 14:59:54 suprem45 2 1
c Initial IV.GS release
e
s 00048/00000/00000
d D 1.1 93/02/03 14:58:25 suprem45 1 0
c date and time created 93/02/03 14:58:25 by suprem45
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
/*   limits.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:34:04 */

#include <stdio.h>
#include "global.h"
#include "constant.h"
#include "geom.h"


/************************************************************************
 *									*
 *	dev_lmts(dxmin, dxmax, dymin, dymax) - this routine calculates  *
 *  the limits of the device.						*
 *									*
 *    Original : CSR           Stanford University        Nov, 1983 	*
 *    Revision : MEL	       (C translation)		  Oct, 1984	*
 *									*
 ************************************************************************/
dev_lmts(dxmin, dxmax, dymin, dymax)
float *dxmin, *dxmax, *dymin, *dymax;
{
    register float *val;
    register struct pt_str **p;

    /*initialize max and mins to first values**/
    *dxmin = *dxmax = pt[0]->cord[0];
    *dymin = *dymax = pt[0]->cord[1];

    /*loop through all the points*/
    for(p = pt; *p != NULL; p++) {
	val = p[0]->cord;
	*dxmin = (*dxmin < *val) ? *dxmin : *val;
	*dxmax = (*dxmax > *val) ? *dxmax : *val;
	val++;	/*advance to y coordinate*/
	*dymin = (*dymin < *val) ? *dymin : *val;
	*dymax = (*dymax > *val) ? *dymax : *val;
    }
}


E 1
