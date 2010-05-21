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
#include "gplot.h"
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
