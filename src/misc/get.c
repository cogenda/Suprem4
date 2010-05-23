
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
#include <stdlib.h>

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
int get_value( par, name , value)
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
int get_bool(struct par_str * par, char *name)
{
    val_str value;

    if (get_value(par->param, name, &value) != -1)
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
char *get_string(struct par_str * par, char *name)
{
    val_str value;

    if (get_value(par->param, name, &value) != -1)
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
int get_int(struct par_str * par, char *name)
{
    val_str value;

    if (get_value(par->param, name, &value) != -1)
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
float get_float(struct par_str * par, char *name)
{
    val_str value;

    if (get_value(par->param, name, &value) != -1)
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
int is_specified(struct par_str * par, char *name)
{
    int temp;
    val_str value;

    if ((temp=get_value(par->param, name, &value)) != -1)
	return( temp );
    return(0);
}
