/*************************************************************************
 *									 *
 *   Original : MEL         Stanford University        Oct, 1984	 *
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   man.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:41:09 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "sysdep.h"

/************************************************************************
 *									*
 *	man( par, param ) - this routine reads the first thing 		*
 *  its input line and sees if it can find a helpfile by that name.  If *
 *  it does, it opens the file and pipes it to more.			*
 *									*
 *	Original	Mark E. Law		Oct. 16, 1984		*
 *									*
 *	Caution - This routine will have to be modified for non UNIX	*
 *  systems.								*
 *									*
 ************************************************************************/
man( par, param )
char *par;
int param;
{
    FILE *help, *more;
    char *s;
    char filename[80];
    int c;
    char *dir;

    /*figure out where to look for the manual directory*/
    if (( dir = (char *)getenv("MANDIR") ) == NULL)
	dir = MANDIR;

    strcpy(filename, dir);
    strcat(filename, "/");

    if (par == NULL)
	strcat(filename, "suprem.h");
    else {
	/*skip over leading white space*/
	while ( isspace ( *par )) par++;

	/*skip over non blanks*/
	for( s = par; ( ! isspace( *s ) && ( *s != '\0' ) ); s++) ;
	*s = '\0';

	strcat(filename, par);
	strcat(filename, ".h");
    }

    /*open a file to read from*/
    if ((help = fopen( filename, "r" )) == NULL) {
	fprintf(stderr, "can not find help for %s\n", par);
	return(-1);
    }

    /*open up the command more to write to*/
    more = (FILE *)popen( PAGER, "w" );

    while ((c = getc(help)) != EOF)
	putc(c, more);

    fclose(help);
    pclose(more);
    return(0);
}




