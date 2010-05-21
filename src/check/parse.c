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
/*   parse.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:12:30 */

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "global.h"
#include "expr.h"


/************************************************************************
 *									*
 *	This file contains the code and definitions for a real		*
 *  number expression parser.  The lexical analyzer is a test		*
 *  routine and nothing more.  The parser is a pseudo LALR job		*
 *  modified to handle this ambiguous grammar.  The grammer rules	*
 *  are :								*
 *	expr  |  expr OP1 expr		OP1 = +,-			*
 *	      |  expr OP2 expr		OP2 = *,/			*
 *	      |  FN expr 		FN = implemented functions	*
 *	      |  FN LPAR expr COM expr RPAR  two headed function	*
 *	      |  OP1 expr		unary minus and plus		*
 *	      |  ( expr )		paranthesis			*
 *	      |	 RCONST			real number constant		*
 *	      |  SOLVAL			solution value			*
 *	      |  STRING       		string argument			*
 *									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/



/************************************************************************
 *									*
 *	detect_error() - This routine uses the stack and input token	*
 *  to determine whether or not an error occurs.  If the input token 	*
 *  can not legally follow the top of stack token, than this routine	*
 *  returns true.  (Translates as: an error will cause this routine to	*
 *  return true.)							*
 *									*
 *  Original:	MEL	2/85						*
 *  									*
 ************************************************************************/
detect_error()
{
    switch( stack[st_pnt].type ) {
    case OP1  : if ( ! input_token.type & (VFN | FN | LPAR | RCONST | SOLVAL)) {
		    if ( input_token.type == OP1 ) {
			if ( stack[st_pnt-1].type != EXPR )
			    return( TRUE );
		    }
		    else 
			return( TRUE );
		}
		break;
    case TOPSTK :
    case OP3  :
    case OP2  :
    case COM  :
    case LPAR :
    case VFN  :
    case FN   : if (! input_token.type & (VFN|FN | OP1 | LPAR | RCONST|SOLVAL))
		    return( TRUE );
		break;
    case EXPR :
    case RCONST :
    case SOLVAL :
    case STRING  :
    case RPAR : if ( ! input_token.type & (EOI | RPAR | OP1 | OP2 | OP3 ))
		    return( TRUE );
		break;
    case EOI  : break;
    default   : return( TRUE );
    }
    return( FALSE );
}




/************************************************************************
 *									*
 *	parse_vec( str, out ) - This routine parses a vactor and 	*
 *  stores it in the vector location given by out.  This parse tree	*
 *  is used later by evaluate to determine the values at each node 	*
 *  point.								*
 * 									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/
char *parse_expr( str, out ) 
char *str;
struct vec_str **out;
{
    
    /*make the return null to handle error conditions*/
    out[0] = NULL;

    /*initialize the stack*/
    st_pnt = 0;
    stack[st_pnt].type = TOPSTK;
    stack[st_pnt].value.ival = 0;

    /*repeat until an error or end of input encountered*/
    do {
	
	/*get an input token for parsing*/
	if (lexical(str, &input_token) == -1)
	    return("unrecognized strings in expression\n");

	/*check for an error*/
	if (detect_error() ) 
	    return("grammar rule violated in expression\n");

	/*else reduce it until we can't no more*/
	while( reduce() );

	/*push the now reduced input token onto the top of stack*/
	stack[ ++st_pnt ]  = input_token;

    } while ( input_token.type != EOI );

    /*make sure we were able to fully reduce it all*/
    if ((st_pnt != 2) || (stack[0].type != TOPSTK) ||
	(stack[1].type != EXPR) || (stack[2].type != EOI))
	return("grammar rule violated in expression\n");
    
    /*set up the pointers*/
    out[0] = stack[1].value.bval;
    return( NULL );
}
