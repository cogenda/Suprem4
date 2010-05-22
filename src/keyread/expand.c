
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
/*   expand.c                Version 4.1     */
/*   Last Modification : 8/18/89  16:35:50 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "check.h"
#include "key.h"

/************************************************************************
 *									*
 *	This file contains the routines that parse down the references  *
 *  to other variables in the boolean expressions.  The routines follow *
 *  a recursive path to check all the parameters and to check each valid*
 *  boolean expression.  The parameters are modified from the 		*
 *  name.subname form to a string of numbers.  Each character in the    *
 *  string represents the ith paramater at that level.  For example,	*
 *  "\005\008\002\000" represents the second parameter of the sub 	*
 *  parameters of the eighth sub parameter of the fifth card.		*
 *  Or the record could be referenced by :				*
 *  (cards[4].param[7]->param[1]) C requires a -1 to the indices	*
 *									*
 ************************************************************************/


/************************************************************************
 *									*
 *	expand( par ) - This routine recursivly travels throughout the  *
 *  parameters and calls bool_expand on each boolean expression.	*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
expand(param)
struct par_str **param;
{

    while ( param[0]->name[0] != '\0' ) {
	/*process this parameter*/
	if ( param[0]->bexp != NULL )
	    bool_expand( param[0]->bexp );

	/*does this sucker have sub parameters??*/
	if (param[0]->param != NULL) {
	    depth++;
	    expand( param[0]->param );
	    depth--;
	}

	param++;
	if (depth == 0)
	    cardnum++;
    }
}



/************************************************************************
 *									*
 *	bool_expand( bexp ) - this routine does postfix traversal of 	*
 *  the tree and examines each parameter.  If it needs to be expanded,  *
 *  it calls par_expand.						*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
bool_expand( bexp )
struct bool_exp *bexp;
{
    /*check the type to see if it needs expansion*/
    if ( bexp->type == PARVAL )
	if (par_expand( bexp ) == -1) {
	    fprintf(stderr, "can not expand parameter %s\n", bexp->value.sval);
	}

    /*do the sons and daughters*/
    if ( bexp->left != NULL )
	bool_expand( bexp->left );
    if ( bexp->right != NULL )
	bool_expand( bexp->right );
}



/************************************************************************
 *									*
 *	compare(s1, s2) - this routine returns the number of characters *
 *  s1 and s2 have in common.  						*
 *									*
 *	Original	Mark E. Law		Sept, 1984		*
 *									*
 ************************************************************************/
compare(s1, s2)
char *s1, *s2;
{
    int i;

    for(i = 0;  (*s1 == *s2) && (*s1) && (*s2); s1++, s2++, i++);
    return(i);
}


/************************************************************************
 *									*
 *	par_expand(bexp) - this routine checks the card deck for the    *
 *  list of parameters to try and expand the parameter name given.	*
 *  A number of things are tried.  First, if no period is in the name,  *
 *  the parameters in the current card are checked.  Then the card names*
 *  are checked.  If there is a period, then the path is attempted.	*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
par_expand(bexp)
struct bool_exp *bexp;
{
    char *s;
    struct par_str *temp;
    int best;
    struct par_str *match;

    /*check to see if there is a . in the name*/
    for ( s = bexp->value.sval; ( *s != '%' ) && ( *s ); s++ );

    /*if there is no period, handle by checking locals*/
    if ( *s == '\0' ) {
	/*check all of the cards below this one*/
	best = compare( bexp->value.sval, cards[ cardnum ]);
	match = (best > 0) ? cards[ cardnum ] : NULL;
	/*check all the sub pointers*/
	s = bexp->value.sval;
	if (check_list(s, cards[cardnum]->param, &best, &match, TRUE)==-1)
	    return(-1);
	/*match is a pointer to the fit*/
	strcpy( bexp->value.sval, match->value.sval );
    }
    else {
	/*check all the top cards for the first part, etc*/
	match = NULL;
	s = (char *)strtok(bexp->value.sval, "%");
	do {
	    best = 0;
	    if ( match == NULL ) {
		/*search the top levels*/
		if ( check_list (s, cards, &best, &match, FALSE ) == -1)
		    return( -1 );
	    }
	    else {
		if ( check_list(s, match->param, &best, &match, FALSE ) == -1)
		    return( -1 );
	    }
	} while ((match != NULL) && (( s = (char *)strtok(NULL,".")) != NULL));
	if (match == NULL)
	    return(-1);
	/*match is a pointer to the fit*/
	strcpy( bexp->value.sval, match->value.sval );
    }
    return( 0 );
}


/************************************************************************
 *									*
 *	check_list(s, pars, best, match) - this routine checks the list *
 *  parameters for the name s and returns the length of the match and   *
 *  the best fit fo the match.						*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
check_list(s, pars, best, match, recurs)
char *s;		/*pattern to be matched*/
struct par_str **pars;  /*parameters to check*/
int *best;		/*length of the match*/
struct par_str **match; /*the best fit pointer*/
int recurs;
{
    int temp;
    static int ambig;

    /*loop through all the parameters*/
    for( ; pars[0]->name[0] != '\0'; pars++) {

	temp = compare( s, pars[0]->name );
	if (temp > *best) {
	    match[0] = pars[0];
	    *best = temp;
	    ambig = FALSE;
	}
	else if (temp == *best)
		ambig = TRUE;

	/*does this sucker have sub params?*/
	if (recurs)
	    if (pars[0]->param != NULL)
		check_list(s, pars[0]->param, best, match, TRUE);
    }
    if ((ambig == TRUE) || (*best == 0))
	return( -1 );
    return(0);
}






/************************************************************************
 *									*
 *	make_depth( param ) - this builds the depth string for each  	*
 *  parameter.  							*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
make_depth(param)
struct par_str **param;
{

    while ( param[0]->name[0] != '\0' ) {
	/*process this parameter*/
	param[0]->value.sval = (char *)malloc( 20 );
	strcpy( param[0]->value.sval, depthstr);

	/*does this sucker have sub parameters??*/
	if (param[0]->param != NULL) {
	    depth++;
	    *(depthstr + depth) = '\001';
	    make_depth( param[0]->param );
	    *(depthstr + depth) = '\000';
	    depth--;
	}

	/*increment the depthstr characters along*/
	(*(depthstr + depth))++;
	param++;
    }
}

