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
 *     Copyright c 1991 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *									 *
 *************************************************************************/
/*   parser.y                Version 5.1     */
/*   Last Modification : 7/3/91 08:28:19 */

/************************************************************************
 *									*
 *	yyparse - this yacc file specifies the grammer used in the 	*
 *  parser.  The return value is -1 for end of input, 2 for continue    *
 *  with a prompt, and 0 for continue with no prompt.			*
 *									*
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "sysdep.h"


static char *tp[10];
static int te[10];

#ifdef MEMDEB
#define malloc MALLOC
#endif

#include "shell.h"

#define YYRETURN(A)        { return(A); }

%}

/*decalre a start symbol for the shell program*/
%start line

/*declare a union of types for the lex return value*/
%union  {
    int ival;
    char *sval;
    }

%token <sval> PARAMETER COMMAND NAME LIST
%token <ival> SOURCE EOL ENDFILE QUIT BACK REDIRECT BANG HELP
%token <ival> BG_GRP END_GRP DELIMIT DEFINE COMMENT UNDEF
%token <ival> SET UNSET FOR END CHDIR

%type <ival> line endline
%type <sval> rdir parameter act2

%%


line
    :	COMMAND parameter rdir endline
	{
	    char *cmd = $1, *par = $2, *red = $3;
	    int back = $4;

	    do_echo();

	    /*do no take the action if storing*/
	    if (depth == -1) {
		/*call the command setup routine*/
		if ((par == NULL) || (interactive))
		    do_command(cmd, par, TRUE, red, (back & BACKGROUND) );
		else
		    do_command(cmd, par, FALSE, red, (back & BACKGROUND) );
		}

	    /*free the used malloc space*/
	    free(cmd);
	    if (par != NULL) free(par);
	    if (red != NULL) free(red);

	    if (depth == -1) {
		YYRETURN( back & PROMPT );
	    }
	}
    |	SOURCE parameter rdir endline
	{
	    char *par = $2, *red = $3;
	    int back = $4;

	    do_echo();

	    /*do not take the action if storing*/
	    if (depth == -1) {
		do_source(par, red, ( back & BACKGROUND), TRUE );

	        if ( (back & PROMPT) && (back & BACKGROUND) ) {
		    YYRETURN( PROMPT );
		}
		else {
		    YYRETURN( ~ PROMPT );
		}
	    }
	}
    |   HELP parameter rdir endline
	{
	    char *par = $2, *red = $3;
	    int back = $4;

	    FILE tout, *t1;
	    do_echo();

	    /*do the redirection if necessary*/
	    tout = *stdout;
	    if (red != NULL)  {
		t1 = file_parse(red, "a");
		*stdout = *t1;
	    }

	    help( par );

	    if (red != NULL) {
		fclose(stdout);
		*stdout = tout;
	    }

	    /*free the space*/
	    if (par != NULL) free(par);
	    if (red != NULL) free(red);

	    if (depth == -1) YYRETURN( back & PROMPT );
	}
    |	UNDEF  parameter endline
	{
	    char *par = $2;
	    int back = $3;

	    do_echo();

	    /*do no take the action if storing*/
	    if (depth == -1) {
		/*do the undefintion*/
		if (par != NULL)
		    if ( undef_macro(par, &macro) == -1)
			fprintf(stderr, "can not find macro %s\n", par);
		}
	    if (par != NULL) free(par);
	    if (depth == -1)
		YYRETURN( back & PROMPT );
	}
    |	DEFINE parameter endline
	{
	    char *par = $2;
	    int back = $3;

	    do_echo();

	    /*do no take the action if storing*/
	    if (depth == -1) {
		/*handle the definitions and results*/
		if (par == NULL)
		    dump_macro(macro);
		else
		    define_macro(par, &macro);
		}

	    if (par != NULL) free(par);
	    if (depth == -1)
		YYRETURN( back & PROMPT );
	}
    |	BANG parameter rdir endline
	{
	    char *par = $2, *red = $3;
	    int back = $4;
	    char str[160];
	    int status, pid, w;

	    do_echo();

	    str[0] = '\0';

	    /*do no take the action if storing*/
	    if (depth == -1) {
		/*if no command give a shell*/
		if (par == NULL)
		    strcpy(str, "csh");
		else {
		    strcpy(str, par);
		    free(par);
		    }

		/*if a redirect add that on*/
		if (red != NULL) {
		    strcat(str, red);
		    free(red);
		    }

		/*add background on*/
		if (back & BACKGROUND)
		    strcat(str, " & ");

		/*exec a shell to handle the request*/
		if ((pid = vfork()) == 0) {
		    if (execl("/bin/sh", "sh", "-c", str, 0) == -1)
			printf("error number %d\n", 127);
		    _exit(127);
		}

		/*wait for the process to finish*/
		while ((w = wait(&status)) != pid && w != -1);

	    }
	    if (depth == -1)
		YYRETURN( PROMPT );
	}
    |	FOR NAME LIST act1 commands END act2 rdir endline
	{
	    char *body;
	    char *list;
	    char *name;
	    char *rdir;
	    char *value;
	    char str[160];
	    int endline;
	    int i;
	    extern char *list_parse();

	    echo_bufptr = 0;

	    prompt = tp[depth];
	    echoall = te[depth];
	    depth--;
	    if ( depth == -1 ) {
		/*we need to save the names, because parsing will
		  destroy the current set*/
		body = $7;
		list = $3;
		name = $2;
		rdir = $8;
		endline = $9;

		/*strip the "end" off*/
		for(i = strlen(body)-1; body[i] != 'e'; i--);
		body[i] = '\0';

		for( value = list_parse( &list, TRUE ); value != NULL;
		     value = list_parse( &list, FALSE) ) {
		    /*basic plan is to use macro processor to define strings*/
		    strcpy(str, name);
		    strcat(str, " ");
		    strcat(str, value);
		    free(value);
		    define_macro(str, &macro);

		    do_string( body, rdir, ( endline & BACKGROUND ) );
		}


		/*remove the entry from the macro table*/
		(void)undef_macro(name, &macro);

		if (body != NULL) free(body);
		if (list != NULL) free(list);
		if (name != NULL) free(name);
		if (rdir != NULL) free(rdir);

		if ( (endline & PROMPT) || (endline & BACKGROUND) ) {
		    YYRETURN( PROMPT );
		}
		else {
		    YYRETURN( ~ PROMPT );
		}
	    }
	}
    |	BG_GRP act1 commands END_GRP act2 rdir endline
	{
	    char *body = $5, *red = $6;
	    int back = $7;
	    char *s;
	    int i;

	    echo_bufptr = 0;

	    prompt = tp[depth];
	    echoall = te[depth];
	    depth--;
	    /*do no take the action if storing*/
	    if (depth == -1)  {
		s = body;

		/*strip off the trailing }*/
		for(i = strlen(s)-1; s[i] != '}'; i--);
		s[i] = ';';

		do_string(body, red, ( back & BACKGROUND) );

		if (body != NULL) free(body);

		if ( (back & PROMPT) || (back & BACKGROUND) ) {
		    YYRETURN( PROMPT );
		}
		else {
		    YYRETURN( ~ PROMPT );
		}
	    }
	}
    |	COMMENT parameter endline
	{
	    int back = $3;
	    do_echo();

	    if (depth == -1)
		YYRETURN( back & PROMPT );
	}
    |	SET parameter endline
	{
	    char *par = $2;
	    int back = $3;
	    do_echo();

	    /*do no take the action if storing*/
	    if (depth == -1)
		do_set(par, TRUE);
	    /*free up the wasted space*/
	    if (par != NULL) free(par);
	    if (depth == -1)
		YYRETURN( back & PROMPT );
	}
    |	UNSET parameter endline
	{
	    char *par = $2;
	    int back = $3;
	    do_echo();

	    /*do no take the action if storing*/
	    if (depth == -1)
		do_set(par, FALSE);
	    /*free up the wasted space*/
	    if (par != NULL) free(par);
	    if (depth == -1)
		YYRETURN( back & PROMPT );
	}
    |	CHDIR parameter endline
	{
	    char *par = $2;
	    int back = $3;
	    char *s;

	    do_echo();

	    /*do not take the action if storing*/
	    if (depth == -1) {
		s = par;
		if ( s == NULL ) {
		    if (chdir( getenv("HOME") ))
			fprintf( stderr, "%s: No such file or directory\n", s);
		}
		else {
		    while ( isspace( *s ) ) s++;
		    if (chdir( s ))
			fprintf( stderr, "%s: No such file or directory\n", s);
		}
	    }
	    /*free up the wasted space*/
	    if (par != NULL)
		free(par);
	    if (depth == -1)
		YYRETURN( back & PROMPT );
	}
    |	endline
	{
	    echo_bufptr = 0;

	    if (depth == -1)
		YYRETURN( $1 );
	}
    |	ENDFILE
	{
	    echo_bufptr = 0;

	    if (depth == -1)
		YYRETURN(-1);
	}
    |	QUIT
	{
	    do_echo();

	    if (depth == -1)
		YYRETURN(-1);
	}
    |	error endline
	{
	    do_echo();

	    fprintf(stderr, "illegal input\n");
	    if (depth == -1)
		YYRETURN( $2 & PROMPT );
	}
    ;


commands
    :	commands line
    |	line
    ;


endline
    :	EOL
	{  $$ = ~BACKGROUND & PROMPT; }
    |	BACK EOL
	{  $$ = BACKGROUND | PROMPT; }
    |	DELIMIT
	{  $$ = 0; }
    |   BACK DELIMIT
	{  $$ = BACKGROUND; }
    ;

rdir
    :	REDIRECT
	{
	    fprintf(stderr, "no file for redirection!\n");
	    $$ = NULL;
	}
    |	REDIRECT PARAMETER
	{
	    $$ = $2;
	}
    |	/* empty */
	{
	    $$ = NULL;
	}
    ;

parameter
    :	PARAMETER
	{
	    $$ = $1;
	}
    |	/* empty */
	{
	    $$ = NULL;
	}
    ;


act1
    : 	/* empty for immediate reduce*/
	/*this is used for grouping startup and loop startup*/
	{

	    /*increment the nesting depth for storage*/
	    depth++;

	    /*malloc off length if not done*/
	    if (store[depth] == NULL) {
		store_len[depth] = 1024;
		store[depth] = (char *)malloc(store_len[depth]);
		}
	    /*zero pointer*/
	    store_ptr[depth] = 0;

	    tp[depth] = prompt;
	    prompt = "> ";
	    te[depth] = echoall;
	    echoall = FALSE;

	}
    ;


act2
    :	/*this is another dummy state to handle the end of a bracket set*/
	{
	    char *s;

	    *( store[depth] + store_ptr[depth]) = '\0';

	    s = (char *)malloc( strlen( store[depth] ) + 1 );
	    strcpy(s, store[depth]);

	    $$ = s;
	}
    ;


%%


int yyerror()
{ return 1; }
