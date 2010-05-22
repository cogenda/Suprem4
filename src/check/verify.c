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
/*   verify.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:12:38 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "global.h"
#include "sysdep.h"
#include "check.h"
extern val_str bool_check();
extern char *parse_expr(), *eval_real();


/************************************************************************
 *									*
 *	parse_real(str, dval) - this routine parses the string into the *
 *  real number dval.  It tries to trap errors before calling sscanf.	*
 *  This routine returns a pointer to the end of the real number, or 	*
 *  NULL if an error occured.						*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
char *parse_real(str, dval)
char *str;
float *dval;
{
    char *s;
    int leading = FALSE, decimal = FALSE;

    s = str;

    if (s == NULL)
	return(NULL);

    /*skip over leading spaces*/
    while ( isspace( *s ) ) s++;

    /*handle any leading + or -*/
    if (( *s == '+' ) || ( *s == '-' ))
	s++;

    /*check off any leading digits*/
    for ( leading = isdigit( *s ); isdigit( *s ); s++);

    /*following any leading digits can be a decimal place*/
    if ( *s == '.' ) {
	s++;	/*skip over the decimal place*/
	/*skip over any decimal place digits*/
	for ( decimal = isdigit( *s ); isdigit( *s ); s++);
    }

    /*if we do not have either leading digits or decimal digits, error*/
    if ( ! (leading || decimal) )
	return( NULL );

    /*if the next char is an e, we have an exponential portion*/
    if ( *s == 'e' ) {
	s++;	/*skip over the exponent character*/
	/*skip over the exponent sign, if any*/
	if ( (*s == '+') || (*s == '-') ) s++;
	/*skip over the exponent digits*/
	while ( isdigit( *s ) ) s++;
    }

    sscanf(str, "%e", dval);
    return( s );
}




/************************************************************************
 *									*
 *	name_check(pars, name, ret) - this routine recursively checks   *
 * the list in pars.  If it finds a name it quits and returns a pointer *
 * to the matching parameter.						*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
#define Substring(X,Y) ((*(X)==*(Y)) && (substring(X,Y)))
name_check(pars, name, ret)
struct par_str **pars, **ret;
char *name;
{
    int ambig;

    for( ; *pars != NULL; pars++ ) {

	/*check the name in this parameter*/
        if (Substring(pars[0]->name, name)) {
	    if (ret[0] != NULL) {
		fprintf(stderr, "ambiguous parameter - %s\n", name);
		return( -2 );
	    }
	    ret[0] = pars[0];
	}
	/*if there are sub params, check them now*/
	if (pars[0]->param != NULL) {
	    if ((ambig = name_check(pars[0]->param, name, ret)) == -2) {
		return( ambig );
	    }
	}
    }
    /*return no match found*/
    if (ret[0] == NULL)
	return(-1);
    else
	return(0);
}



#include "expr.h"

/************************************************************************
 *									*
 *	verify(name, pars, value) - this routine takes the parameter	*
 *  specification in name and compares it to the list of parameters in  *
 *  pars.  The found value is returned in value.			*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
int verify(name, pars, value, parloc)
char *name;
struct par_str **pars;
val_str *value;
struct par_str **parloc;
{
    char *par_name, *par_value, *loc;
    int err;
    char *s;
    struct vec_str *rexp;

    /*split the name into its halves*/
    loc = (char *)index( name, '=' );
    if ( loc != NULL ) {
	*loc = '\0';
	par_name = name;
	par_value = loc + 1;
    }
    else {
	par_name = name;
	par_value = NULL;
    }
    parloc[0] = NULL;

    /*see if we can find the name in the list of names*/
    if ((err = name_check( pars, par_name, parloc)) < 0) {
	if (err == -1)
	    fprintf(stderr, "parameter %s does not exist for this command\n", par_name);
	return(-1);
    }

    /*make sure the par_value is cool for the type of parameter*/
    switch(parloc[0]->type & ~MASK) {
    case REAL : if ( (s = parse_expr(par_value, &rexp)) != NULL ) {
		    free_expr( rexp );
		    fprintf(stderr, "%s", s);
		    return( -1 );
		}
		else if ( (s = eval_real( rexp, &(value->dval))) != NULL) {
		    free_expr( rexp );
		    fprintf(stderr, "%s", s);
		    return(-1);
		}
		free_expr( rexp );
		break;
    case INT  : if (par_value == NULL) {
		    fprintf(stderr, "%s is not a legal integer\n", par_value);
		    return(-1);
		}
		s = par_value;
		if ( *s == '-' ) s++;
		for( ; isdigit(*s); s++);
		if ( *s != '\0' ) {
		    fprintf(stderr, "%s is not a legal integer\n", par_value);
		    return(-1);
		}
		sscanf(par_value, "%d", &(value->ival));
		break;
    case STR  : if (par_value == NULL) {
		    fprintf(stderr, "no character string given for %s\n", par_name);
		    return(-1);
		}
		value->sval = (char *)malloc( strlen(par_value) + 1);
		strcpy(value->sval, par_value);
		break;
    case BOOL : /*several conditions*/
		/*lowercase par_value for ease*/
		if (par_value != NULL)
		    for(s = par_value; *s != '\0'; s++)
			*s = isupper(*s)?tolower(*s):(*s);

		if (par_value == NULL)
		    value->ival = TRUE;
		else if (substring("on", par_value))
			 value->ival = TRUE;
		else if (substring("off", par_value))
			 value->ival = FALSE;
		else if (substring("true", par_value))
			 value->ival = TRUE;
		else if (substring("false", par_value))
			 value->ival = FALSE;
		else {
		    fprintf(stderr, "%s is not a legal boolean\n", par_value);
		    return(-1);
		    }
		break;
    }
    return(0);
}



/************************************************************************
 *									*
 *	check(str, par) - this routine checks and splits the parameters	*
 *  passed to it.  It parses the parameters and puts the value in the 	*
 *  parameter record.							*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
check(str, par)
char *str;
struct par_str *par;
{
    char *argv[ NUMPAR ];
    struct par_str *indx;
    val_str value;
    int err_ret;
    int i;

    err_ret = 0;
    for(i = 0; i < NUMPAR; i++)
	argv[i] = NULL;

    /*make sure card can be specified at this point*/
    if ( par->bexp != NULL ) {
	value = bool_check( par->bexp, NULL );
	if (value.ival) {
	    fprintf(stderr, "%s\n", par->err_msg);
	    err_ret = -1;
	}
    }

    /*make sure all card params are unspecified at this point*/
    if (par->param != NULL) unspecify( par->param );

    /*ship everything off*/
    if ( str != NULL ) if (split(str, argv, TRUE) == -1) return( -1 );

    /*check out the results with respect to the param list*/
    if ( par->param != NULL ) {
	for(i = 0; argv[i] != NULL; i++) {
	    if (verify(argv[i], par->param, &value, &indx) == -1)
		err_ret = -1;
	    else  {
		indx->type = indx->type | SPECIFIED;
		indx->value = value;
	    }
	}
    }

    /*now check non-specified params and use defaults if appropriate*/
    if ( par->param != NULL ) {
	assign_deflt( par->param );
	deflt_check(par->param);
    }

    /*now we need to check the parameter error trees*/
    /*do this for specified parameters first*/
    if ( par->param != NULL )
	if (error_check(par->param, TRUE) == -1) err_ret = -1;

    /*set the top card to zero*/
    if (err_ret == 0) {
	par->value.ival = TRUE;
	par->type = par->type & ~ SPECIFIED;
    }
    for(i = 0; argv[i] != NULL; i++) sfree(argv[i]);

    /*return error status*/
    return( err_ret );
}



/************************************************************************
 *									*
 *	unspecify( par ) - this routine turns off the specify flag in   *
 * each record.								*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
unspecify( par )
struct par_str **par;
{
    for(; par[0] != NULL; par++) {

	par[0]->type = par[0]->type & ~ SPECIFIED;

	/*if sub parameters, recursively handle them*/
	if (par[0]->param != NULL)
	    unspecify( par[0]->param );
    }
}
