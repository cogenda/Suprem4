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
/*   split.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:12:34 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "global.h"
#include "check.h"

/************************************************************************
 *									*
 *	split(line, argv) - this routine takes the parameter line and   *
 *  splits it into parameters and arguments for them.  The parameters   *
 *  are returned in the pointer to pointers array.			*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
split(line, argv, parnames)
char *line;	/*the input line to broken*/
char **argv;	/*the return parameter value pairs*/
int parnames;	/*are there leading parameter names*/
{
    int i;
    char *s;
    int parcnt;

    i = 0;

    /*eat any leading white space up*/
    while ( isspace( *line ) ) line++;

    /*repeat until we are at end of line*/
    while ( *line != '\0' ) {

	/*allocate space for the pair*/
	argv[i] = (char *)malloc( strlen(line)+1 );

	/*eat a parameter name - non blanks followed by a blank or =*/
	if ( parnames ) {
	    for(s = argv[i]; (*line!='\0')&&(*line!='=')&&(!isspace(*line)); line++)
		*s++ = *line;
	}
	else s = argv[i];

	/*handle a possible assignement*/
	if ( *line == ' ' )
	    /*eat any leading white space up*/
	    while ( isspace( *line ) ) line++;

	if ( (*line == '=') || !parnames ) {
	    /*add the parameter assignment*/
	    if (parnames) *s++ = *line++;  /*add the equal sign in*/

	    /*eat white space after the equal*/
	    while ( (*line != '\0') && isspace( *line ) ) line++;

	    /*handle differently if we found a ", (, or something else*/
	    switch( *line ) {
	    case '"' :	/*skip over the quote we found*/
			line++;
			/*read until we find the end quote*/
			while( ( *line) && ( *line != '"' ) )  *s++ = *line++;
			if ( *line != '"' ) {
			    fprintf(stderr, "unmatched quotes in input\n");
			    return( -1 );
			}
			line++;
			break;
	    case '\'':	/*skip over the quote we found*/
			line++;
			/*read until we find the end quote*/
			while( ( *line) && ( *line != '\'' ) )  *s++ = *line++;
			if ( *line != '\'' ) {
			    fprintf(stderr, "unmatched quotes in input\n");
			    return( -1 );
			}
			line++;
			break;
	    case '(' :  /*add one to the paren count read until matched set*/
			parcnt = 1;
			*s++ = *line++;
			while( ( *line ) && ( parcnt != 0 ) ) {
			    if ( *line == '(' ) parcnt++;
			    if ( *line == ')' ) parcnt--;
			    *s++ = *line++;
			}
			if ( parcnt != 0 ) {
			    fprintf(stderr, "unmatched parenthesis in input\n");
			    return( -1 );
			}
			break;
	    default  :  /*now eat until a non space*/
			while ( ( *line != '\0') && ! isspace( *line ) )
			    *s++ = *line++;
	    }
	}
	/*add a terminator to the string*/
	*s = '\0';
	/*advance the argument pointer*/
	i++;

	/*eat any white space before the next parameter*/
	while ( isspace( *line )) line++;
    }
    /*terminate the list of arguments*/
    argv[i] = NULL;
    return( 0 );
}




