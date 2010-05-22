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
/*   reduce.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:12:31 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "global.h"
#include "expr.h"

#define NEW_VEC (struct vec_str *)calloc(sizeof(struct vec_str), 1)
#define EQUIVALENCE(A, B) A->type = B->type; A->value = B->value


/************************************************************************
 *									*
 *	This file contains the code and definitions for a real		*
 *  number expression parser.  The parser is a pseudo LALR job		*
 *  modified to handle this ambiguous grammar.  The grammer rules	*
 *  are :								*
 *	expr  |  expr OP1 expr		OP1 = +,-			*
 *	      |  expr OP2 expr		OP2 = *,/			*
 *	      |  FN expr		FN = implemented functions	*
 *	      |  expr COM expr 		comma operator for functions	*
 *	      |  OP1 expr		unary minus and plus		*
 *	      |  ( expr )		paranthesis			*
 *	      |	 RCONST			real number constant		*
 *	      |  SOLVAL			solution value			*
 *									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/



/************************************************************************
 *									*
 *	expr_reduce(a, b, c, d) - This routine builds a tree out of 	*
 *  the values a, b, and c.  The routine puts in d the reduced 		*
 *  expression.  This routine works at pulling things off the stack 	*
 *  and into the tree.							*
 * 									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/
expr_reduce(a, b, c, d)
struct tok_str *a, *b, *c, *d;
{
    struct vec_str *tmp;

    /*the whole idea here is to return an expression*/
    tmp = NEW_VEC;

    /*attemp to figure out what is going on...*/
    switch ( a->type ) {
    case FN   :
    case VFN  : /*function operations*/
		EQUIVALENCE(tmp, a);
		if ( c != NULL ) {
		    tmp->left = b->value.bval;
		    tmp->right = c->value.bval;
		}
		else {
		    tmp->right = b->value.bval;
		}
		break;
    case OP1  : /*unary plus/minus*/
		EQUIVALENCE(tmp, a);
		tmp->right = b->value.bval;
		break;
    case EXPR :
		EQUIVALENCE(tmp, b);
		tmp->left = a->value.bval;
		tmp->right = c->value.bval;
		break;
    case RCONST :
    case STRING    :
    case SOLVAL : /* convert single number to an expression */
		EQUIVALENCE(tmp, a);
		break;
    default   : break;
    }
    d->type = EXPR;
    d->value.bval = tmp;
}





/************************************************************************
 *									*
 *	reduce() - This routine reduces the stack and input token.  It	*
 *  will return true if further reduction is possible, false for no 	*
 *  further reduction possible.						*
 * 									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/
reduce()
{
    struct tok_str *top, *prev, *old, *old1, *old2;

    top = &stack[st_pnt];
    prev = (st_pnt > 0)?(&stack[st_pnt-1]):NULL;
    old = (st_pnt > 1)?(&stack[st_pnt-2]):NULL;
    old1 = (st_pnt > 2)?(&stack[st_pnt-3]):NULL;
    old2 = (st_pnt > 3)?(&stack[st_pnt-4]):NULL;

    /*switch on all possible combinations of input values*/
    switch( input_token.type ) {
    case STRING    :
    case RCONST :
    case SOLVAL :   /*reduce this single value to an expression*/
		    expr_reduce(&input_token, NULL, NULL, &input_token);
		    return( TRUE );
		    break;
    case EXPR : /*look back at previous stuff to decide*/
		switch( top->type ) {
		case OP1 :  /*if preceeding operator is an operator - unary*/
			    if (prev->type & (OP1|OP2|OP3|TOPSTK|FN|VFN|LPAR)) {
				/*reduce expr | op1 expr */
				expr_reduce(top, &input_token, NULL,
							      &input_token);
				st_pnt--;
				return( TRUE );
			    }
			    break;
		case OP3 :  /*if preceeding operator is an expr, reduce*/
			    if (prev->type == EXPR) {
				/*reduce expr | expr op2 expr*/
				expr_reduce(prev, top, &input_token,
							 &input_token);
				st_pnt -= 2;
				return( TRUE );
			    }
			    break;
		default  :  break;
		}
		break;
    case OP2  : /*check back on the stack for this stuff*/
		if ((top->type == EXPR ) && (prev->type == OP2) &&
					    (old->type == EXPR)) {
		    expr_reduce(old, prev, top, old);
		    st_pnt -= 2;
		    return( TRUE );
		}
		break;
    case EOI  : /*end of input*/
    case OP1  : /*check way back for these guys*/
		if ((top->type == EXPR ) && ((prev->type == OP1)||(prev->type == OP2)) &&
					    (old->type == EXPR)) {
		    expr_reduce(old, prev, top, old);
		    st_pnt -= 2;
		    return( TRUE );
		}
		break;
    case RPAR : /* this only works if an expression is on top */
		if (top->type == EXPR) {
		    if (((prev->type==OP2)||(prev->type==OP1)) && (old->type==EXPR)) {
			expr_reduce(old, prev, top, old);
			st_pnt -= 2;
			return( TRUE );
		    }
		    /*go after two valued functions*/
		    if ( (prev->type==COM)&&(old->type==EXPR)&&
			 (old1->type==LPAR)&&((old2->type==FN)||(old2->type==VFN))) {
			 expr_reduce( old2, old, top, &input_token );
			 st_pnt -= 5;
			 return( TRUE );
		    }
		    /*single valued functions*/
		    if ( (prev->type==LPAR)&&((old->type==FN)||(old->type==VFN))) {
			 expr_reduce( old, top, NULL, &input_token );
			 st_pnt -= 3;
			 return( TRUE );
		    }
		    if (prev->type == LPAR) {
			input_token = *top;
			st_pnt -= 2;
			return( TRUE );
		    }
		}
		break;
    default   : break;
    }
    return( FALSE );

}

