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
/*   eval.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:12:26 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
extern double erf(), erfc(), pow();
#include <ctype.h>
#include "global.h"
#include "constant.h"
#include "dbaccess.h"	/*for the number of nodes for vector functions*/
#include "expr.h"

extern char *get_solval(), *vfunc();


/************************************************************************
 *									*
 *	This file contains the code and definitions for a real		*
 *  number expression evaluator.  The parser is a pseudo LALR job	*
 *  modified to handle this ambiguous grammar.  The grammer rules	*
 *  are :								*
 *	expr  |  expr OP1 expr		OP1 = +,-			*
 *	      |  expr OP2 expr		OP2 = *,/			*
 *	      |  FN expr		FN = implemented functions	*
 *	      |  OP1 expr		unary minus and plus		*
 *	      |  ( expr )		paranthesis			*
 *	      |	 RCONST			real number constant		*
 *	      |  SOLVAL			solution value			*
 *									*
 *  This routine handles the parse tree as developed in reducing this	*
 *  grammar.								*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/


/************************************************************************
 *									*
 *	eval_real( expr, val ) - This routine evaluates the real number *
 *  expr contained in the parse tree expr.  If a vector variable is 	*
 *  found, it generates an error.  The value is returned in val when	*
 *  all is said and done.						*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
char *eval_real( expr, val )
struct vec_str *expr;
float *val;
{
    float lval, rval;
    float interface();
    char *err;

    /*evaluate the kids*/
    if ( expr->left != NULL )
	if ( (err = eval_real( expr->left, &lval ) ) != NULL ) return(err);

    if ( expr->right != NULL )
	if ( (err = eval_real( expr->right, &rval ) ) != NULL ) return(err);

    /*switch on the type of value contained in the passed argument*/
    switch( expr->type ) {
    case OP1 :	/*plus or minus*/
		/*check the unary case out*/
		if (expr->left == NULL) {
		    if ( expr->value.ival == '+' ) *val = rval;
		    if ( expr->value.ival == '-' ) *val = -rval;
		}
		else {
		    if ( expr->value.ival == '+' ) *val = lval + rval;
		    if ( expr->value.ival == '-' ) *val = lval - rval;
		}
		break;
    case OP2 :  /*multiplication / division*/
		if ( expr->value.ival == '*' ) *val = lval * rval;
		if ( expr->value.ival == '/' ) *val = lval / rval;
		break;
    case OP3 :  /*exponentiation*/
		if ( expr->value.ival == '^' ) *val = pow( lval, rval );
		break;
    case FN  :  /*a function specifier*/
		switch( expr->value.ival ) {
		case LOG10 :	*val = log10( rval );
				break;
		case LOG   :	*val = log( rval );
				break;
		case EXP   :	*val = exp( rval );
				break;
		case ERF   :	*val = (double)erf( rval );
				break;
		case ERFC  :	*val = (double)erfc( rval );
				break;
		case ABS   :	*val = (rval > 0.0)?(rval):(-rval);
				break;
		case SQRT  :	*val = (double)sqrt( rval );
				break;
		case SIGN  :	if ( rval > 0.0 ) *val = 1.0;
				else if ( rval < 0.0 ) *val = -1.0;
				else *val = 0.0;
				break;
		case X     :
		case Y     :
		case Z     :    if ( (expr->left == NULL) || (expr->right == NULL) )
				    return("x,y,and z functions require two arguments\n");
				else
				    *val = sol_interp( expr->value.ival, lval, rval );
				break;
		default    :    *val = interface( expr->value.ival, rval );
		}
		break;
    case RCONST :  *val = expr->value.dval;
		break;
    case VFN	:
    case SOLVAL :  return( "a vector solution value is illegal here\n" );
		break;
    case STRING    :
		return("string not allowed in expression\n");
		break;
    }
    return( NULL );
}


/************************************************************************
 *									*
 *	eval_vec( expr, val ) - This routine evaluates the real number	*
 *  expr contained in the parse tree expr.  The value is returned in 	*
 *  val when all is said and done.					*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
char *eval_vec( expr, val )
struct vec_str *expr;
float *val;
{
    float *lval, *rval;
    float tmp;
    char *err = NULL;
    int i;

    lval = salloc( float, nn );
    rval = salloc( float, nn );

    /*evaluate the kids*/
    if ( expr->type != VFN ) {
	if ( expr->left != NULL )
	    if ( (err = eval_vec( expr->left, lval ) ) != NULL ) {
		sfree(lval);
		sfree(rval);
		return(err);
	    }
	if ( expr->right != NULL )
	    if ( (err = eval_vec( expr->right, rval ) ) != NULL ) {
		sfree(lval);
		sfree(rval);
		return(err);
	    }
    }

    /*switch on the type of value contained in the passed argument*/
    switch( expr->type ) {
    case OP1 :	/*plus or minus*/
		/*check the unary case out*/
		if (expr->left == NULL) {
		    if ( expr->value.ival == '+' )
			for(i = 0; i < nn; i++) val[i] = rval[i];
		    if ( expr->value.ival == '-' )
			for(i = 0; i < nn; i++) val[i] = - rval[i];
		}
		else {
		    if ( expr->value.ival == '+' )
			for(i = 0; i < nn; i++) val[i] = lval[i] + rval[i];
		    if ( expr->value.ival == '-' )
			for(i = 0; i < nn; i++) val[i] = lval[i] - rval[i];
		}
		break;
    case OP2 :  /*multiplication / division*/
		if ( expr->value.ival == '*' )
			for(i = 0; i < nn; i++) val[i] = lval[i] * rval[i];
		if ( expr->value.ival == '/' )
		    for(i = 0; i < nn; i++) val[i] = lval[i] / rval[i];
		break;
    case OP3 :  /*exponentiation*/
		if ( expr->value.ival == '^' )
		    for(i = 0; i < nn; i++) val[i] = pow( lval[ i], rval[ i]);
		break;
    case FN  :  /*a function specifier*/
		switch( expr->value.ival ) {
		case LOG10 :	for(i=0; i<nn; i++) val[i] = log10(rval[i]);
				break;
		case LOG   :	for(i=0; i<nn; i++) val[i] = log(rval[i]);
				break;
		case EXP   :	for(i=0; i<nn; i++) val[i] = exp(rval[i]);
				break;
		case ERF   :	for(i=0; i<nn; i++) val[i] = erf(rval[i]);
				break;
		case ERFC  :	for(i=0; i<nn; i++) val[i] = erfc(rval[i]);
				break;
		case ABS   :	for(i=0; i<nn; i++)
				    val[i]=(rval[i]>0.0)?(rval[i]):(-rval[i]);
				break;
		case SQRT  :	for(i=0; i<nn; i++) val[i] = (double)sqrt( rval[i] );
				break;
		case SIGN  :	for(i=0; i<nn; i++)  {
				    if ( rval[i] > 0.0 ) val[i] = 1.0;
				    else if ( rval[i] < 0.0 ) val[i] = -1.0;
				    else val[i] = 0.0;
				}
				break;
		case X     :
		case Y     :
		case Z     :    if ( (expr->left == NULL) || (expr->right == NULL) )
				    err = "x,y,and z functions require two arguments\n";
				else {
				    tmp = sol_interp( expr->value.ival, *lval, *rval );
				    for(i=0; i <nn; i++) val[i] = tmp;
				}
				break;
		}
		break;

    case RCONST :
		for(i = 0; i < nn; i++) val[i] = expr->value.dval;
		break;

    case SOLVAL :
		err = get_solval( val, expr->value.ival );
		break;

    case VFN    :
		if ( (err = vfunc( rval, expr->value.ival, expr )) == NULL)
		   for(i=0; i<nn; i++) val[i] = rval[i];
		break;

    case STRING    :
		err = "string not allowed in expression";
		break;
    }
    sfree(lval);
    sfree(rval);
    return( err );
}



/************************************************************************
 *									*
 *	islogexp( expr ) - This routine returns true if the expression	*
 *  starts with a log base 10.  It is used by the plotting software.	*
 *									*
 *  Original:	MEL	12/85						*
 *									*
 ************************************************************************/
islogexp( expr )
struct vec_str *expr;
{
    return( (expr->type == FN) && (expr->value.ival == LOG10) );
}




/************************************************************************
 *									*
 *	free_expr - This is a memory disposer.				*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
free_expr( t )
struct vec_str *t;
{
    if ( t == NULL ) return;
    free_expr( t->left );
    free_expr( t->right );
    if (t->type == STRING) sfree(t->value.sval);
    sfree(t);
}
