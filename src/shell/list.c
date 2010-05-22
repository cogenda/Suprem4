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
/*   list.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:28:11 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "global.h"


/************************************************************************
 *									*
 *	list_parse( str ) - This routine strips off the parts of a list	*
 *  and returns a pointer to the argument portions.  It is used in 	*
 *  for loop parsing and macro argument expansion.  It modifies the 	*
 *  so that repeated calls strip off the next argument.			*
 *									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/
char *list_parse( s, first )
char **s;
int first;
{
    char *value, *str;
    int count, i;

    /*if we are on the first pass preprocess for stepping loops*/
    if (first)
	loop_check(s);

    str = s[0];

    /*if str has zero length, return NULL*/
    if ( strlen(str) == 0)
	return(NULL);

    /*step over any leading parens*/
    if ( *str == '(' )
	strcpy(str, str+1);

    while ( isspace( *str ) ) strcpy(str, str+1);

    if( *str == 0) return(NULL);

    /*find the end of the argument*/
    for(count = 0;
	     (str[count] != ' ') && (str[count] != ')') && (str[count] != ',');
	count++);

    /*malloc off space to hold the result*/
    value = salloc(char , strlen(str) + 1);

    /*copy the string in*/
    for(i = 0; i < count; i++) value[i] = str[i];
    value[i] = '\0';

    /*update the list value*/
    strcpy(str, (str + count + 1) );

    if ( strlen(value) == 0)
	return(NULL);
    else
	return( value );
}



/************************************************************************
 *									*
 *	loop_check(str) - This routine attempts to check for loop 	*
 *  constructs.  The form is start to end step size, where start, end	*
 *  and size are all floating point numbers.				*
 *									*
 *  Original:	MEL	4/85						*
 *									*
 ************************************************************************/
loop_check(str)
char **str;
{
    float start, stop, step;
    float index;
    int length;
    int pos;
#define SIZE 17

    /*check to see if we have float to float type of line*/
    if (sscanf(str[0], "(%e to %e", &start, &stop) != 2) {
	/*the string does not look right at all*/
	return;
    }

    /*if we got here, everything is all right*/
    if (sscanf(str[0], "(%e to %e step %e", &start, &stop, &step) != 3)
	step = 1.0;

    /*build a new string*/
    length = (stop - start) / step * SIZE + 2*SIZE;
    if (length < SIZE) length = SIZE;
    str[0] = sralloc(char , length, str[0] );

    strcpy(str[0], "(");
    if ( step < 0 ) {
	for(pos = 0, index = start; index >= stop; index += step, pos += SIZE)
	    sprintf(&str[0][pos+1], "%16e ", index);
    }
    else {
	for(pos = 0, index = start; index <= stop; index += step, pos += SIZE)
	    sprintf(&str[0][pos+1], "%16e ", index);
    }

    strcat(str[0], ")");
}

