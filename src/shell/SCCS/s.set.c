h37035
s 00000/00000/00080
d D 2.1 93/02/03 15:37:22 suprem45 2 1
c Initial IV.GS release
e
s 00080/00000/00000
d D 1.1 93/02/03 15:35:48 suprem45 1 0
c date and time created 93/02/03 15:35:48 by suprem45
e
u
U
f e 0
t
T
I 1
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
/*   set.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:28:17 */

#include <stdio.h>
#include <ctype.h>
#include "global.h"
#include "shell.h"

/************************************************************************
 *									*
 *	do_set(par,on) - this routine sets the variables for local 	*
 *  shell action.  If par == NULL, the routine dumps those variables.	*
 *  Otherwise, the variable is set on or off depending on the on flag.	*
 *									*
 *	Original	Mark E. Law		Sept, 1984		*
 *									*
 ************************************************************************/
do_set(par, on)
char *par;
int on;
{
    
    char *s;
    
    /*first test for print or set mode*/
    if ( par == NULL ) {
	if (interactive)
	    printf("interactive\n");
	if (noexecute)
	    printf("noexecute\n");
	if (echoall)
	    printf("echoall\n");
	printf("prompt\t%s\n", prompt);
	return;
	}
    
    /*handle the parsing of the others*/

    /*skip leading white space*/
    while ( isspace( *par ) ) par++;

    /*begin to parse things off the front*/
    if ( substring("interactive", par))
	interactive = on;
    if ( substring("noexecute", par))
	noexecute = on;
    if ( substring("echo", par))
	echoall = on;
    if ( substring(par, "prompt")) {
	par += strlen("prompt");

	/*strip white space*/
	while ( isspace ( *par )) par++;

	/*do we have a leading quote??*/
	if ( (*par == '"') || (*par == '\'') ) {
	    par++;
	    for(s = par; ( *s && (*s != '"') && (*s != '\'') ); s++);
	    if ( (*s == '"') || (*s == '\'') ) *s = '\0';
	}

	/*prompt is the rest of the line*/
	sfree(prompt);
	prompt = salloc(char, strlen(par) + 1);
	strcpy(prompt, par);
	}
    return;
}
    

E 1
