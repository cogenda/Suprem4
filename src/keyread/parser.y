%{

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
/*   parser.y                Version 4.2     */
/*   Last Modification : 3/20/91  09:25:42 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "check.h"
#include "key.h"

#ifdef MEMDEB
#define malloc MALLOC
#endif

#define FALSE 0
#define TRUE 1

%}

%start keyfile

%union  {
    int ival;
    float dval;
    char *sval;
    struct bool_exp *bval;
    }


%token <ival> CARD BG_GRP END_GRP
%token <ival> PARTYPE EOL BOOLSTART
%token <ival> GRT LES GRTE LESE OR AND EQU NOTEQU NOT PLUS TIMES
%token <dval> RCONST
%token <sval> STRING
%token <ival> INTEGER NUMB
%token <sval> NAME UNITS ERR_MSG
%token <sval> COMMENT PARNAME

%type <bval> boolean term bexp
%type <ival> id

%left OR PLUS
%left AND TIMES
%left EQU NOTEQU
%left GRT LES GRTE LESE
%left NOT



%%

keyfile
    :	parameter
    |	keyfile parameter
    ;

parameter
    :	identifier err_msg boolean EOL
	{
	    /*add the pointer to the top of the boolean tree*/
	    current[0]->bexp = $3;

	    /*everything has parsed okey-dokey, bump the pointer*/
	    current++;
	}
    |	COMMENT
	{
	    /*ignore the sucker*/
	}
    |   BG_GRP
	{
	    /*push everything down on the stack*/
	    current = (struct par_str **)push(current);
	}
    |	END_GRP
	{
	    /*pop a layer off and finish off the pointer list*/
	    current = (struct par_str **)pop();
	    /*if we pop a NULL, it is time to quit*/
	    if (current == NULL)
		return(0);
	}
    |	error EOL
	{
	    /*ignore the line in error*/
	    fprintf(stderr, "error in parsing the input key file\n");
	}
    ;



identifier
    :	id NAME deflt
	{
	    if (strlen($2) >= 12)
		*($<sval>2 + 11) = '\0';
	    strcpy(current[0]->name, $2);
	    current[0]->units = NULL;
	    current[0]->type = $1;
	}
    |	id NAME deflt UNITS
	{
	    if (strlen($2) >= 12)
		*($<sval>2 + 11) = '\0';
	    strcpy(current[0]->name, $2);
	    current[0]->units = $4;
	    current[0]->type = $1;
	}
    |	id NAME deflt NUMB
	{
	    if (strlen($2) >= 12)
		*($<sval>2 + 11) = '\0';
	    strcpy(current[0]->name, $2);
	    current[0]->units = NULL;
	    current[0]->type = $1 | ($4 & INDEX);
	}
    |	id NAME deflt UNITS NUMB
	{
	    if (strlen($2) >= 12)
		*($<sval>2 + 11) = '\0';
	    strcpy(current[0]->name, $2);
	    current[0]->units = $4;
	    current[0]->type = $1 | ($5 & INDEX);
	}
    ;


id
    :	CARD
	{
	    $$ = COMM;
	}
    |   PARTYPE
	{
	    $$ = $1;
	}
    ;


deflt
    :	'='  RCONST
	{
	    current[0]->def.dval = $2;
	}
    |   '='  STRING
	{
	    current[0]->def.sval = $2;
	}
    |	'=' INTEGER
	{
	    current[0]->def.ival = $2;
	}
    |	/*empty case, zero everything*/
	{
	    current[0]->def.ival = 0;
	}
    ;


err_msg
    :	ERR_MSG
	{
	    current[0]->err_msg = $1;
	}
    |	/*empty case, give a default message*/
	{
	    current[0]->err_msg = NULL;
	}
    ;

boolean
    :	BOOLSTART bexp
	{
	    $$ = $2;
	}
    |   /*empty*/
	{
	    $$ = NULL;
	}
    ;


bexp
    :	'(' bexp ')'
	{
	    $$ = $2;
	}
    |   bexp GRT bexp
	{
	    val_str t;
	    t.ival = '>';
	    $$ = node(OPER, t, $1, $3);
	}
    |   bexp LES bexp
	{
	    val_str t;
	    t.ival = '<';
	    $$ = node(OPER, t, $1, $3);
	}
    |   bexp GRTE bexp
	{
	    val_str t;
	    t.ival = 'g';
	    $$ = node(OPER, t, $1, $3);
	}
    |   bexp LESE bexp
	{
	    val_str t;
	    t.ival = 'l';
	    $$ = node(OPER, t, $1, $3);
	}
    |   bexp OR bexp
	{
	    val_str t;
	    t.ival = '|';
	    $$ = node(OPER, t, $1, $3);
	}
    |   bexp AND bexp
	{
	    val_str t;
	    t.ival = '&';
	    $$ = node(OPER, t, $1, $3);
	}
    |   bexp EQU bexp
	{
	    val_str t;
	    t.ival = '=';
	    $$ = node(OPER, t, $1, $3);
	}
    |   bexp NOTEQU bexp
	{
	    val_str t;
	    t.ival = 'n';
	    $$ = node(OPER, t, $1, $3);
	}
    |   NOT bexp
	{
	    val_str t;
	    t.ival = '!';
	    $$ = node(OPER, t, NULL, $2);
	}
    |   bexp PLUS bexp
	{
	    val_str t;
	    t.ival = '+';
	    $$ = node(OPER, t, $1, $3);
	}
    |   bexp TIMES bexp
	{
	    val_str t;
	    t.ival = '*';
	    $$ = node(OPER, t, $1, $3);
	}
    |   term
	{
	    $$ = $1;
	}
    ;


term
    :	RCONST
	{
	    val_str t;
	    t.dval = $1;
	    $$ = node(REAL, t, NULL, NULL);
	}
    |	PARNAME
	{
	    val_str t;
	    t.sval = $1;
	    $$ = node(PARVAL, t, NULL, NULL);
	}
    |	INTEGER
	{
	    val_str t;
	    t.dval = (float)$1;
	    $$ = node(REAL, t, NULL, NULL);
	}
    ;

%%

#include "lexical.c"



/************************************************************************
 *									*
 *	node(type, val, left, right) - this routine builds the boolean	*
 *  expression node of a tree with the given value.  It mallocs the 	*
 *  space and assigns the data.						*
 *									*
 ************************************************************************/
struct bool_exp *node(type, val, left, right)
int type;
val_str val;
struct bool_exp *left, *right;
{
    struct bool_exp *t;

    t = (struct bool_exp *)malloc( sizeof(struct bool_exp) );
    t->value = val;
    t->type = type;
    t->left = left;
    t->right = right;
    return(t);
}

int yyerror() { return (1); }

int yywrap(void) { return (1); }
