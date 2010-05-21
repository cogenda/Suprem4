h12384
s 00000/00000/00056
d D 2.1 93/02/03 15:29:34 suprem45 2 1
c Initial IV.GS release
e
s 00056/00000/00000
d D 1.1 93/02/03 15:28:46 suprem45 1 0
c date and time created 93/02/03 15:28:46 by suprem45
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
/*   option.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:38:58 */

#include <stdio.h>
#include <math.h>
#include "global.h"

/************************************************************************
 *									*
 *	This file implements the option card.  It currently only has	*
 *  one parameter, terminal.  It should in some future day ave more 	*
 *  configuration options present.					*
 *  									*
 *  Now it does - CSR Sep 86						*
 *									*
 ************************************************************************/



/************************************************************************
 *									*
 *	option( par, param ) - this card handles config	parameters for 	*
 *  the system.								*
 *									*
 *  Original:	MEL	10/84						*
 *									*
 ************************************************************************/
option( par, param )
char *par;
int param;
{

#   define CHOSEN(x) (is_specified( param, x) && get_bool( param, x))
    /*how much barfola the user wants*/
    if( CHOSEN("quiet")) verbose = V_QUIET;
    if( CHOSEN("normal")) verbose = V_NORMAL;
    if( CHOSEN("chat")) verbose = V_CHAT;
    if( CHOSEN("barf")) verbose = V_BARF;
    
    return(0);
}
E 1
