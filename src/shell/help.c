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
/*   help.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:28:06 */

#include <stdio.h>
#include <ctype.h>
#include "global.h"
#include "check.h"
#include "shell.h"


/************************************************************************
 *									*
 *	help() - This command dumps information about the parameters.	*
 *									*
 *  Original:	MEL	1/87						*
 *									*
 ************************************************************************/
help( par )
char *par;
{
    register int i, indx;
    register int endstr;
    register char *s;

    /*if no name given*/
    if ( par == NULL ) {
	/*print the data for all the cards*/
	printf("%-12s\t%s\n", "Name", "Description");

	/*for each command*/
	for(i = 0; command[i].name[0] != '\0'; i++) {
	    indx = command[i].param;
	    if ( cards[indx]->units != NULL ) 
		printf("%-12s\t%s\n", command[i].name, cards[indx]->units );
	    else
		printf("%-12s\t%s\n", command[i].name, " " );
	}
	printf("For more information\n");
	printf("Type help <cmd> or man <cmd>\n");
    }
    else {
	/*split the string up to non blank pieces*/

	/*for all the non blank pieces*/
	for(endstr = FALSE, s = par;  !endstr;  s = par) {
	    
	    /*skip leading white space*/
	    while ( isspace( *par ) && *par ) par++;
	    s = par;

	    /*find the end of the string*/
	    while ( !isspace( *par ) && *par ) par++;

	    if ( *par ) {
		*par++ ='\0';
		while ( isspace( *par ) && *par ) par++;
	    }

	    if ( ! *par ) endstr = TRUE;

	    /*find the specified command*/
	    i = get_proc( s );

	    /*do the help on this string*/
	    if ( i != -1 ) {
		print_help( NULL, cards[command[i].param]->param );
		printf("For more help type man %s\n", command[i].name );
	    }
	}
    }
}


/************************************************************************
 *									*
 *	print_help( com ) - Print the help for the given command.		*
 *									*
 *  Original:	MEL	1/87						*
 *									*
 ************************************************************************/
print_help( space, param )
char *space;
struct par_str **param;
{
    register struct par_str *pars;
    register int i;
    register int indx;

    /*if no parameters, it takes a string*/
    if ( param == NULL ) {
	printf("string parameter\n");
	return;
    }

    /*recursively print out each parameter*/
    for( indx = 0; param[indx] != NULL; indx++ ) {
	pars = param[indx];
	if ( space != NULL ) printf( space );
	/*print the data dependent on the type of value*/
	switch( pars->type & ~MASK ) {
	case REAL   :   printf("float %s = %5g", pars->name, pars->def.dval );
			if ( pars->units != NULL ) printf("\t%s\n", pars->units );
			else printf("\n");
		        break;
	case INT    :   printf("int %s = %4d", pars->name, pars->def.ival );
			if ( pars->units != NULL ) printf("\t%s\n", pars->units );
			else printf("\n");
		        break;
	case STR    :   if ( pars->def.sval )
			    printf("string %s = %s", pars->name, pars->def.sval );
			else
			    printf("string %s", pars->name );
			if ( pars->units != NULL ) printf("\t%s\n", pars->units );
			else printf("\n");
		        break;
	case BOOL   :   if ( pars->def.ival )
			    printf("boolean %s = %1s",pars->name, "T");
			else
			    printf("boolean %s = %1s",pars->name, "F");
			if ( pars->units != NULL ) printf("\t%s\n", pars->units );
			else printf("\n");
		        break;
	case CHOICE :   i = pars->def.ival - 1;
		        if ( (pars->param != NULL) && (pars->param[i] != NULL) )
			    printf("switch %s = %-12s", pars->name, pars->param[i]->name);
			else
			    printf("switch %s = %-12s", pars->name, "   " );
			if ( pars->units != NULL ) printf("\t%s\n", pars->units );
			else printf("\n");
			printf("Choose one of:\n");
			break;
	}

	/*do any sub parameters*/
	if ( pars->param != NULL ) {
	    if ( space == NULL )
		print_help( "    ", pars->param );
	    else 
		print_help( "\t", pars->param );
	}

    }
}

