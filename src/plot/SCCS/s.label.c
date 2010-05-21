h65131
s 00001/00001/00052
d D 2.1 93/02/03 15:29:31 suprem45 2 1
c Initial IV.GS release
e
s 00053/00000/00000
d D 1.1 93/02/03 15:28:45 suprem45 1 0
c date and time created 93/02/03 15:28:45 by suprem45
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
/*   label.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:38:53 */

#include <stdio.h>
#include <math.h>
D 2
#include <local/gplot.h>
E 2
I 2
#include "gplot.h"
E 2
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "plot.h"
#include "material.h"

/************************************************************************
 *									*
 *	draw_lab() - This routine moves to the indicated spot and draws *
 *  a label at that location.						*
 *									*
 *  Original:	MEL	1/87						*
 *									*
 ************************************************************************/
draw_lab( par, param )
char *par;
int param;
{
    char *label;
    float x,y;

    /*get the card parameters*/
    label = get_string(param, "label");
    
    if( is_specified( param, "x") && is_specified( param, "y")) {
	x = get_float(param, "x");
	y = get_float(param, "y");

	xgLabel(label, x, y);
    }
    xgUpdate(FALSE);
}
E 1
