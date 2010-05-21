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
/*   check.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:12:24 */

#include <stdio.h>
#include "global.h"
#include "check.h"


/************************************************************************
 *									*
 *	This file contains routines which check the error booleans for  *
 * a parameter list.							*
 *									*
 ************************************************************************/


/************************************************************************
 *									*
 *	bool_check - this routine is a recursive evaluator of the tree  *
 *  representing a boolean expression.					*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
val_str bool_check( bexp, typ )
struct bool_exp *bexp;
int *typ;
{
    
    val_str left, right;
    val_str temp;
    int ltyp, rtyp;
    struct par_str *par;
    char *s;

    /*if this node has a left child, evaluate it*/
    if (bexp->left != NULL)
	left = bool_check( bexp->left, &ltyp );
    /*if there is a right child, evaluate it*/
    if (bexp->right != NULL)
	right = bool_check( bexp->right, &rtyp );

    /*switch on the type of operator this is*/
    switch ( bexp->type )  {
    case OPER :    /*switch dependent on the operator type*/
		   switch( bexp->value.ival ) { 
		   case '>' : if (ltyp == INT) left.dval  = left.ival;
		              if (rtyp == INT) right.dval = right.ival;
			      temp.ival = left.dval > right.dval ;
			      break;
		   case '<' : if (ltyp == INT) left.dval  = left.ival;
		              if (rtyp == INT) right.dval = right.ival;
			      temp.ival = left.dval < right.dval ;
			      break;
		   case 'g' : if (ltyp == INT) left.dval  = left.ival;
		              if (rtyp == INT) right.dval = right.ival;
			      temp.ival = left.dval >= right.dval ;
			      break;
		   case 'l' : if (ltyp == INT) left.dval  = left.ival;
		              if (rtyp == INT) right.dval = right.ival;
			      temp.ival = left.dval <= right.dval ;
			      break;
		   case '+' : if (ltyp == INT) left.dval  = left.ival;
		              if (rtyp == INT) right.dval = right.ival;
			      temp.ival = left.dval + right.dval ;
			      break;
		   case '*' : if (ltyp == INT) left.dval  = left.ival;
		              if (rtyp == INT) right.dval = right.ival;
			      temp.ival = left.dval * right.dval ;
			      break;
		   case '|' : temp.ival = left.ival || right.ival ;
			      break;
		   case '&' : temp.ival = left.ival && right.ival ;
			      break;
		   case '=' : temp.ival = left.ival == right.ival ;
			      break;
		   case 'n' : temp.ival = left.ival != right.ival ;
			      break;
		   case '!' : temp.ival = ! right.ival ;
			      break;
		   }
		   if (typ != NULL) *typ = INT;
		   break;
    case INT    :  /*return the real value*/
		   temp.ival = bexp->value.ival;
		   if (typ != NULL) *typ = INT;
		   break;
    case REAL   :  /*return the real value*/
		   temp.dval = bexp->value.dval;
		   if (typ != NULL) *typ = REAL;
		   break;
    case PARVAL :  /*return the parameter value*/
		   s = bexp->value.sval;
		   par = cards[ *s - 1 ];
		   s++;
		   while ( *s != '\0' ) {
		       par = par->param[ *s - 1 ];
		       s++;
		   }
		   temp = par->value;
		   if (typ != NULL) *typ = par->type & ~ MASK;
		   break;
    default	:  panic("What type am I supposed to check???\n");
		   break;
    }
    return( temp );
}


/************************************************************************
 *									*
 *	error_check( par, bool ) - this routine does the recursive	*
 *  error checking on all specified parameters.	 Bool is a parameter    *
 *  which indicates if the parent was specified.			*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
error_check(par, bool)
struct par_str **par;
int bool;
{
    val_str temp;
    int err_ret;
    int i, mult, sing, tmp;
    
    err_ret = 0;
    /*check all the parameters in the list*/
    for(; par[0] != NULL; par++) {

	/*check switches here*/
	if (par[0]->type & CHOICE) {
	    /*only one of the sub parameters may be true and specified*/
	    for(mult = sing = i = 0; par[0]->param[i] != NULL; i++) {
		tmp = (par[0]->param[i]->type & SPECIFIED);
		/*if it is a boolean, make sure it was set true*/
		if ( par[0]->param[i]->type & BOOL )
		    tmp = tmp && (par[0]->param[i]->value.ival); 
		if (tmp && mult) err_ret = -1;
		if (tmp) sing = mult = TRUE;
	    }
	    if (err_ret == -1)
		fprintf(stderr, "%s\n", par[0]->err_msg);

	    /*if no switch set, set up a default*/
	    if (! sing) 
		if ((i = par[0]->def.ival) > 0) 
		    if (par[0]->param[i-1]->type & BOOL) 
			par[0]->param[i-1]->value.ival = TRUE;
	}

	/*check it only if it is a specified variable*/
	if (par[0]->type & SPECIFIED) {
	    if ( par[0]->bexp != NULL )
		temp = bool_check( par[0]->bexp, NULL );
	    else 
		temp.ival = FALSE;
	    if (temp.ival) {
		/*print the error message*/
		fprintf(stderr, "%s\n", par[0]->err_msg);
		err_ret = -1;
	    }
	    if (! bool ) {
		fprintf(stderr, "%s is an illegal sub parameter\n", par[0]->name);
		err_ret = -2;
	    }
	}

	/*check sub parameters*/
	if ((par[0]->param != NULL) && !(par[0]->type & CHOICE) && !err_ret) {
	    if ((err_ret = error_check(par[0]->param,par[0]->value.ival)) < 0) {
		if (err_ret == -2) 
		    fprintf(stderr, "%s has to be true for sub parameters to be legal\n",
			    par[0]->name);
		err_ret = -1;
	    }
	}
    }
    return( err_ret );
}


/************************************************************************
 *									*
 *	deflt_check( par ) - this routine checks to see if the default	*
 *  or zero would be a more appropriate value for the parameter.	*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
deflt_check(par) 
struct par_str **par;
{
    val_str temp;
    
    /*check all the parameters in the list*/
    for(; par[0] != NULL; par++) {
	/*only do this if the parameter was not specified*/
	if ((par[0]->type & SPECIFIED) == 0) {
	    /*check the default value*/
	    par[0]->value = par[0]->def;

	    if (par[0]->bexp != NULL)
		temp = bool_check( par[0]->bexp, NULL );
	    else
		temp.ival = FALSE;

	    if (temp.ival ) {
		par[0]->value.ival = 0;
	    }
	}

	/*check sub parameters*/
	if (par[0]->param != NULL)
	    deflt_check(par[0]->param);
    }
}




/************************************************************************
 *									*
 *	assign_deflt( par ) - this routine gives every parameter not    *
 *  specified the default value for that parameter.			*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
assign_deflt( par )
struct par_str **par;
{
    /*assign for all the parameters in the list*/
    for(; par[0] != NULL; par++) {
	/*only do this if the parameter was not specified*/
	if ((par[0]->type & SPECIFIED) == 0) {
	    /*assign the default value*/
	    par[0]->value = par[0]->def;

	}

	/*assign sub parameters*/
	if (par[0]->param != NULL)
	    assign_deflt(par[0]->param);
    }
}


