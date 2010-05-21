h06386
s 00000/00000/00170
d D 2.1 93/02/03 15:25:08 suprem45 2 1
c Initial IV.GS release
e
s 00170/00000/00000
d D 1.1 93/02/03 15:24:15 suprem45 1 0
c date and time created 93/02/03 15:24:15 by suprem45
e
u
U
f e 0
t
T
I 1

/*************************************************************************
 *									 *
 *   Original : MEL         Stanford University        Sept, 1984	 *
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   get.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:41:08 */

#include <stdio.h>
#include "global.h"
#include "check.h"

/************************************************************************
 *									*
 *	This file contains a series of routines to help the user obtain *
 *  the value of parameters.  It does virtually no error checking.  It  *
 *  assumes the user can and will ask for everything.			*
 *									*
 ************************************************************************/


/************************************************************************
 *									*
 *	get_value( par, name, value ) - this routine searches the 	*
 *  parameter list par for parameter name and returns its value.	*
 *  This routine returns true if the parameter was specified.  This is  *
 *  used in the is_specified routine.					*
 *									*
 *	Original:	Mark E. Law	Oct, 1984			*
 * 									*
 ************************************************************************/
get_value( par, name , value)
struct par_str **par;
char *name;
val_str *value;
{
    int i;
    register char *s, *ss;
    
    /*check each parameter*/
    for( ; par[0] != NULL; par++ ) {

	if (*(par[0]->name) == *name) {
	    for (s=par[0]->name, ss=name; *ss == *s && *s; s++, ss++);
	    if  ( !*ss || !*s ) {
		/*found it go home*/
		*value =  par[0]->value;
		return ( par[0]->type & SPECIFIED );
	    }
	}
	/*check any subparameters...*/
	if (par[0]->param != NULL) {
	    if ( (i = get_value (par[0]->param, name, value)) >= 0)
		return( i );
	}
    }
    value->ival = 0;
    return(-1);
}



/************************************************************************
 *									*
 *	get_bool(param, name) - this routine returns the boolean that   *
 * matches the name passed out of the list of parameters param.		*
 *									*
 *	Original:	Mark E. Law	Oct, 1984			*
 *									*
 ************************************************************************/
get_bool(par, name)
int par;
char *name;
{
    val_str value;

    if (get_value(((struct par_str *)par)->param, name, &value) != -1)
	return( value.ival );
	
    return(0);
}

/************************************************************************
 *									*
 *	get_string(param, name) - this routine returns the boolean that *
 * matches the name passed out of the list of parameters param.		*
 *									*
 *	Original:	Mark E. Law	Oct, 1984			*
 *									*
 ************************************************************************/
char *get_string(par, name)
int par;
char *name;
{
    val_str value;

    if (get_value(((struct par_str *)par)->param, name, &value) != -1)
	return( value.sval );
    return(NULL);
}

/************************************************************************
 *									*
 *	get_int(param, name) - this routine returns the boolean that   	*
 * matches the name passed out of the list of parameters param.		*
 *									*
 *	Original:	Mark E. Law	Oct, 1984			*
 *									*
 ************************************************************************/
get_int(par, name)
int par;
char *name;
{
    val_str value;

    if (get_value(((struct par_str *)par)->param, name, &value) != -1)
	return( value.ival );
    return(0);
}



/************************************************************************
 *									*
 *	get_float(param, name) - this routine returns the boolean that 	*
 * matches the name passed out of the list of parameters param.		*
 *									*
 *	Original:	Mark E. Law	Oct, 1984			*
 *									*
 ************************************************************************/
float get_float(par, name)
int par;
char *name;
{
    val_str value;

    if (get_value(((struct par_str *)par)->param, name, &value) != -1)
	return( value.dval );
    return(0);
}




/************************************************************************
 *									*
 *	is_specified(param, name) - this routine returns a boolean that *
 * indicates if the user specified the parameter or it has a default	*
 * value.								*
 *									*
 * Original:	MEL	1/85						*
 *									*
 ************************************************************************/
is_specified(par, name)
int par;
char *name;
{
    int temp;
    val_str value;

    if ((temp=get_value(((struct par_str *)par)->param, name, &value)) != -1)
	return( temp );
    return(0);
}
E 1
