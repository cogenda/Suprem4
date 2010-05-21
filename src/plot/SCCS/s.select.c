h17550
s 00000/00000/00100
d D 2.1 93/02/03 15:29:37 suprem45 2 1
c Initial IV.GS release
e
s 00100/00000/00000
d D 1.1 93/02/03 15:28:49 suprem45 1 0
c date and time created 93/02/03 15:28:49 by suprem45
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
/*   select.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:39:07 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "sysdep.h"
#include "constant.h"
#include "material.h"
#include "plot.h"
#include "expr.h"
#include "dbaccess.h"


/************************************************************************
 *									*
 *	sel_var( par, param ) - this routine sets up a solution vector 	*
 *  for the plot commands that need one.				*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
sel_var( par, param )
char *par;
int param;
{
    char *zstr;
    struct vec_str *zexp;
    char *err;
    char *tmp;

    zstr = get_string( param, "z" );
    znn = nn;
    z = salloc( float, nn );

    if ( is_specified( param, "label" ) ) {
	if ( label != NULL ) free( label );
	tmp = get_string( param, "label" );
	label = (char *)malloc( strlen(tmp) + 1 );	
	strcpy( label, tmp );
    }
    else {
	if ( label != NULL ) free( label );
	label = (char *)malloc( strlen( zstr ) + 1 );
	strcpy( label, zstr );
    }

    if ( is_specified( param, "title" ) ) {
	if ( title != NULL ) free( title );
	tmp = get_string( param, "title" );
	title = (char *)malloc(strlen(tmp) + strlen(VERSION) + strlen(" - ") + 1);
	strcpy(title, tmp );
	strcat(title, " - ");
	strcat(title, VERSION );
    }
    else {
	if ( title != NULL ) free( title );
	title = (char *)malloc(strlen(VERSION) + 1);
	strcpy(title, VERSION );
    }

    if ( is_specified( param, "temp" ) ) {
	last_temp = get_float( param, "temp" ) + 273.0;
	/*compute the sislicon material constants*/
	comp_mat( last_temp );
    }


    /*attempt to parse the string*/
    if ( (err = (char *)parse_expr(zstr, &zexp)) != NULL) {
	fprintf(stderr, "%s\n", err);
	return(-1);
    }

    /*evaluate the vector expression*/
    if ( (err = (char *)eval_vec( zexp, z )) != NULL) {
	fprintf(stderr, "%s\n", err);
	return(-1);
    }

    sel_log = islogexp( zexp );

    free_expr( zexp );
    return(0);
}
E 1
