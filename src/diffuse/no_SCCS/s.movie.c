h19417
s 00000/00000/00061
d D 2.1 93/02/03 14:46:55 suprem45 2 1
c Initial IV.GS release
e
s 00061/00000/00000
d D 1.1 93/02/03 14:41:22 suprem45 1 0
c date and time created 93/02/03 14:41:22 by suprem45
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
/*   movie.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:50:04 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "shell.h"

/************************************************************************
 *									*
 *	make_movie( string, time ) - This routine executes the commands *
 *  in the string passed.  The time elapses is in time.			*
 *									*
 *  Original:	MEL	11/86						*
 *									*
 ************************************************************************/
make_movie( commands, time )
char *commands;		/*the commands*/
float time;		/*the length of time to take this diffusion/oxidation*/
{
    char buffer[512];

    if ( commands == NULL ) return;

    /*define a macro string for the current time*/
    if ( time < 1.0 ) {
	sprintf(buffer, "tlab %7.3fms", time * 1000.0 );
	define_macro( buffer, &macro );
    }
    else if ( time < 60.0 ) {
	sprintf(buffer, "tlab %7.3fs", time );
	define_macro( buffer, &macro );
    }
    else if ( time < 3600.0 ) {
	sprintf(buffer, "tlab %7.3fm", time / 60.0 );
	define_macro( buffer, &macro );
    }
    else {
	sprintf(buffer, "tlab %7.3fh", time / 3600.0 );
	define_macro( buffer, &macro );
    }

    sprintf(buffer, "time %g", time);
    define_macro( buffer, &macro );

    /*make it fancy later with the time, but right now just execute*/
    do_string( commands, NULL, 0 );

    /*undo the macro*/
    (void)undef_macro( "time", &macro );
    (void)undef_macro( "tlab", &macro );
    check_x();
}
E 1
