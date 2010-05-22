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
 *************************************************************************/
/*   do_action.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:28:04 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <unistd.h>
#include <sysdep.h>

#include "shell.h"
#include "check.h"
#include "global.h"

/************************************************************************
 *									*
 *	do_source(file, redir, back) - this routine handles the 	*
 *  sourcing of an input file.  The file name is given in file, the	*
 *  redirect file name is in redir, and the if the job should be in the *
 *  background is indicted by back					*
 *									*
 *	Original	Mark E. Law		Sept, 1984		*
 *									*
 ************************************************************************/
do_source(file, redir, back, reperr)
char *file;	/*the file to be sourced*/
char *redir;	/*the filename for redirection, if any*/
int back;	/*flag for backgorund execution*/
int reperr;	/*report errors in the file open?*/
{
    FILE *tinf, *tsrc;
    FILE tout, *t1;
    char *redirect;
    char *oldbuf;

    redirect = NULL;

    /*if we have not been given a file, save a lot of hassle*/
    if ( file == NULL ) {
	fprintf(stderr, "must specify a file to source\n");
	return;
    }

    /*save the old input file pointer, set up the new*/
    tinf = in_file;
    tsrc = file_parse(file, "r");

    /*if a legal file get specified*/
    if (tsrc != NULL) {
	/*save the old output file pointer, set up the new*/
	tout = *stdout;
	if (redir != NULL) {
	    t1 = file_parse(redir, "a");
	    redirect = redir;
	    *stdout = *t1;
	}

	/*save the old buffer away*/
	oldbuf = salloc(char, strlen(buffer+bufptr) + 1 );
	strcpy( oldbuf, buffer+bufptr );

	/*null the input buffer*/
	bufptr = 0;
	buffer[bufptr] = '\0';

	/*if background job, handle it*/
	in_file = tsrc;
	if (back) {
	    if (fork() == 0) {
		/*save this location*/
		while( yyparse() != -1) ;
		exit(0);
	    }
	}
	else {
	    while( yyparse() != -1) ;
	}
	fclose( tsrc );
	in_file = tinf;

	/*restore the buffer*/
	strcpy( buffer, oldbuf );
	bufptr = 0;
	sfree(oldbuf);

    }
    else if ( reperr )
	fprintf(stderr, "Could not find file %s to open\n", file);

    /*clean up the output buffer*/
    if (redirect != NULL) {
	fclose(stdout);
	*stdout = tout;
	sfree(redirect);
    }
    return;
}


/************************************************************************
 *									*
 *	do_string(str, rdir, back) - this routine handles the execution *
 *  of a string of input.  It is called by loops and bracket sets.	*
 *									*
 *	Original	Mark E. Law		Oct, 1986		*
 *									*
 ************************************************************************/
do_string(instr, redir, back)
char *instr;	/*the file to be sourced*/
char *redir;	/*the background file name if any*/
int back;	/*flag for backgorund execution*/
{
    FILE tout, *t1;
    int len;
    char *redirect;
    char *oldbuf;

    redirect = NULL;

    /*save the old output file pointer, set up the new*/
    tout = *stdout;
    if (redir != NULL) {
	t1 = file_parse(redir, "a");
	redirect = redir;
	*stdout = *t1;
    }

    /*if background job, handle it*/
    if (back) {
	if (fork() == 0) {

	    /*make sure there is enough space*/
	    if ( (len = (strlen( instr ) + 3) ) > supbln ) {
		supbln = len;
		supbuf = sralloc(char, len * sizeof(int), supbuf );
	    }

	    /*copy the string int line size pieces into the buffer*/
	    strcpy( supbuf, instr );
	    strcat( supbuf, "\n\001" );
	    supbpt = 0;

	    /*parse it*/
	    while( yyparse() != -1) ;

	}
    }
    else {
	/*save the old buffer away*/
	oldbuf = salloc(char , strlen(supbuf+supbpt) + 1 );
	strcpy( oldbuf, supbuf+supbpt );

	/*make sure there is enough space*/
	if ( (len = (strlen( instr ) + 3) ) > supbln ) {
	    supbln = len;
	    supbuf = sralloc(char, len * sizeof(int), supbuf );
	}

	/*copy the string int line size pieces into the buffer*/
	strcpy( supbuf, instr );
	strcat( supbuf, "\n\001" );
	supbpt = 0;

	/*parse it*/
	while( yyparse() != -1) ;

	/*restore the buffer*/
	strcpy( supbuf, oldbuf );
	supbpt = 0;
	sfree(oldbuf);
    }

    /*clean up the output buffer*/
    if (redirect != NULL) {
	fclose(stdout);
	*stdout = tout;
	sfree(redirect);
	}
}


/* for the perennially broken dbx */
do_str( instr)
     char *instr;
{
    do_string( instr, NULL, 0);
}



/************************************************************************
 *									*
 *	do_command(name, param, intr, file, back) - this procedure 	*
 *  handles the parsing of the data presented.  Name is the name of the *
 *  command to be parsed and executed.  Param is the remainder of the   *
 *  line which describes the parameters for the command.  Intr is a 	*
 *  flag which specifies if the command is to be done in interactive	*
 *  mode.  File is a filename for any possible redirection of output.	*
 *  Back is a flag for backgrounding the command.			*
 *									*
 *	Original	Mark E. Law		Sept, 1984		*
 *									*
 ************************************************************************/
do_command(name, param, intr, file, back)
char *name;	/*the name of the command*/
char *param;	/*the command parameters*/
int intr;	/*the interactive flag*/
char *file;	/*the filename for redirect, NULL if no redirect*/
int back;	/*the flag for background of the command*/
{

    FILE tout, *t1;
    int index;

    /*get proc parses the string associated with command*/
    index = get_proc(name);

    /*if command is -1, an illegal input was typed*/
    if (index != -1) {

	tout = *stdout;
	if (file != NULL)  {
	    t1 = file_parse(file, "a");
	    *stdout = *t1;
	}

	if (back) {
	    /*background the process*/
	    if (fork() == 0) {
		/*in the child, do the command*/
		do_exec( param, FALSE, index, noexecute );
		exit(0);
	    }
	}
	else
	    do_exec( param, intr, index, noexecute );


	if (file != NULL) {
	    fclose(stdout);
	    *stdout = tout;
	}
    }
    else {
	char *sh = (char *)getenv("SHELL");
	char *str;
	int pid, w;
	int status;

	if (file != NULL)
	    if (param != NULL)
		str = malloc( strlen(file)+strlen(name)+strlen(param)+20 );
	    else
		str = malloc( strlen(file)+strlen(name)+20 );
	else
	    if (param != NULL)
		str = malloc( strlen(name)+strlen(param)+20 );
	    else
		str = malloc( strlen(name)+20 );

	if (sh == NULL) sh = "/bin/sh";

	strcpy(str, name);
	if (param != NULL) strcat(str, param);

	/*if a redirect add that on*/
	if (file != NULL) {
	    strcat(str, ">");
	    strcat(str, file);
	}

	/*add background on*/
	if (back) strcat(str, "&");

	/*exec a shell to handle the request*/
	if ((pid = vfork()) == 0) {
	    if (execl(sh, sh, "-c", str, 0) == -1)
		printf("error number %d\n", 127);
	    _exit(127);
	}

	/*wait for the process to finish*/
	while ((w = wait(&status)) != pid && w != -1);
    }
}



/************************************************************************
 *									*
 *	do_exec(par, intr, index, noexec ) -  This routine does 	*
 *  parameter checks and looks at the noexec and intr flags to do any	*
 *  preprocessing that may be needed.   If all goes well,  it calls 	*
 *  the appropriate routine to do the action.  				*
 *									*
 *  Original:	MEL	3/85	(gearing up for Pisces2)		*
 *									*
 ************************************************************************/
do_exec(par, intr, index, no_exec)
char *par;
int intr;
int index;
int no_exec;
{

    (void)check_x();

    if (check(par, cards[command[index].param]) == -1) {
	fprintf(stderr, "errors detected on command input\n");
	return;
    }
    else if ( no_exec )
	fprintf(stderr, "no error in %s command input\n",
						   command[index].name);

    if ( intr )
	printf("invoked interactively\n");

    /*if not in no exec mode, do the work*/
    if (! no_exec) {
	command[index].func( par, cards[command[index].param] );
    }
    return;

}



