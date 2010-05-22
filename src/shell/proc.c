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
/*   proc.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:28:16 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

/*here be stuff that should live in .h some day*/
#define FALSE 0
#define TRUE 1

/************************************************************************
 *									*
 *   get_proc(name) - this routine will have to be heavily modified	*
 *  when the genii like structure comes on line for the SUPREM4 	*
 *  parser.  The routine takes name and parses it down to the function	*
 *  to be called.							*
 *									*
 *	Original	Mark E. Law		Sept, 1984		*
 *									*
 ************************************************************************/
get_proc(name)
char *name;
{
    int test;
    int match, best_match;
    int best;
    int length;
    int ambig;

    length = strlen(name);

    /*compare all the names in the command table to this name*/
    best = -1;
    ambig = FALSE;
    best_match = 0;
    for(test = 0; command[test].name[0] != '\0'; test++) {
	match = substring(command[test].name, name);
	if (match == best_match)
	    ambig = TRUE;
	if ((match > best_match) && (match == length)) {
	    best_match = match;
	    best = test;
	    ambig = FALSE;
	    }
	}

    /*now use the best fit as the executable command*/
    if (best == -1)
	return(-1);
    else
	if (ambig) {
	    fprintf(stderr, "the command is ambiguous\n");
	    return(-1);
	}
	else
	    return(best);
}


/************************************************************************
 *									*
 *	substring(s, ss) - tests if ss is a substring of s. If it is,   *
 *	the number of characters in common is returned.			*
 *	It is intended to replace compare, which requires a comparison  *
 *      with strlen(ss) every time it is used.				*
 *									*
 *	Original	Conor S. Rafferty	Oct, 1986		*
 *									*
 ************************************************************************/
substring(s, ss)
    char *s, *ss;
{
    char *ass;

    for (ass=ss; (*s == *ss) && *ss; s++, ss++) ;

    if (!*ss) return (ss-ass);
    return (0);
}



/************************************************************************
 *									*
 *	file_parse(name, type) - this routine strips blanks off of name *
 *  and attempts to open up a file with that name.  Type indicates how  *
 *  the file should be opened.  It returns NULL if the file can not be  *
 *  opened.							 	*
 *									*
 *	Original	Mark E. Law		Sept, 1984		*
 *									*
 ************************************************************************/
FILE *file_parse(name, type)
char *name;
char *type;
{
    char *end_str;

    /*strip off leading blanks*/
    for( ; (*name == ' ') || (*name == '\t'); name++);

    /*strip off trailing blanks*/
    for(end_str = name; *end_str; end_str++);
    end_str--;
    if( (*end_str == ' ') || (*end_str == '\t') ) {
	for(end_str-- ; (*end_str == ' ') || (*end_str == '\t'); end_str--);
	*(end_str + 1) = '\0';
    }

    /*open the file and return*/
    return( fopen(name, type) );
}

