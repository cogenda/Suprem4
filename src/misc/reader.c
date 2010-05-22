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
/*   reader.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:41:16 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>  /*not neeeded for lib version*/
#include "global.h"
#include "check.h"
#include "key.h"
extern int errno;

/************************************************************************
 *									*
 *	read.c - The routines defined here read the unformatted key     *
 *  key file data in to the structure defined for the parameters.  It   *
 *  tries to do this in an optimal way w.respect to memory useage.	*
 *  									*
 ************************************************************************/

static level = 0;

/************************************************************************
 *									*
 *	read_list(par, fd ) - this routine is the outer level routine	*
 *  for unformatted key file reads.					*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/

read_list(par, fd)
struct par_str **par;
FILE *fd;
{
    static int temp;
    int eof;

    while ( TRUE ) {
	/*begin by reading the level of the next command*/
	if ( (eof = fread(&temp, sizeof(int),1,fd)) != 1) {
	    /*only place an eof can appear*/
	    if ( eof == 0 )
		return( 1 );
	    else
		return( eof );
	}

	if ( temp == level ) {
	    /*we are at the same level*/
	    par++;
	    /*allocate space for par*/
	    par[0] = (struct par_str *)calloc(1, sizeof(struct par_str));
	    if ( (eof = read_param( par[0], fd )) != 1)
		return( eof );
	}
	else {
	    /*we are going somewhere, either up or down*/
	    if ( temp < level )
		return(1);

	    /*set it up for a depth move*/
	    level++;
	    par[0]->param = (struct par_str **)calloc(NUMPAR+1, sizeof(int *));
	    /*allocate space for the first one*/
	    par[0]->param[0]=(struct par_str *)calloc(1,sizeof(struct par_str));

	    /*read the rest of that one of the current record*/
	    if (( eof = read_param( par[0]->param[0], fd )) != 1)
		return( eof );

	    /*read all the sub little parameters*/
	    if (( eof = read_list( &(par[0]->param[0]), fd)) != 1)
		return( eof );
	    level--;

	    /*if we are still below where we need to be pop*/
	    if (temp < level)
		return(1);

	    /*upon return, we ahve read level of the higher one, finish it*/
	    par++;
	    /*allocate space for par*/
	    par[0] = (struct par_str *)calloc(1, sizeof(struct par_str));
	    if ( (eof = read_param( par[0], fd )) != 1)
		return(eof);
	}
    }
}



/************************************************************************
 *									*
 *	read_param(par, fd) - this routine reads a single parameter 	*
 *  record from the file and saves it in par.				*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
read_param(par, fd)
struct par_str *par;
FILE *fd;
{
    char buffer[1024];
    int eof, i;
    char c;

    /*begin by reading all of the fixed length crap*/
    if ( (eof = fread(par->name, 12, 1, fd)) != 1)
	return( eof );
    if ( (eof = fread(&par->type, sizeof(int), 1, fd)) != 1)
	return(eof);

    /*read in the default based on type data*/
    switch( par->type & ~ MASK ) {
    case REAL :	if ( (eof = fread(&par->def.dval,sizeof(float),1,fd) != 1))
		    return( eof );
		break;
    case COMM :
    case BOOL :
    case CHOICE :
    case INT  :	if ( (eof = fread(&par->def.ival, sizeof(int),1, fd) != 1))
		    return( eof );
		break;
    case STR  :	for(c = 'a', i = 0; c != '\0'; i++) {
		    if (( eof  = c = getc(fd)) == EOF)
			return( eof );
		    buffer[i] = c;
		}
		if (i > 1) {
		    par->def.sval = (char *)malloc(strlen(buffer) + 1);
		    strcpy(par->def.sval, buffer);
		}
		break;
    default   : return(-1);
    }
    par->value = par->def;

    /*read in the units and the error message*/
    for(c = 'a', i = 0; c != '\0'; i++) {
	if (( eof = c = getc( fd)) == EOF)
	    return( eof );
	buffer[i] = c;
    }
    if (i > 1) {
	par->units = (char *)malloc(strlen(buffer) + 1);
	strcpy(par->units, buffer);
    }
    for(c = 'a', i = 0; c != '\0'; i++) {
	if (( eof = c = getc( fd)) == EOF)
	    return( eof );
	buffer[i] = c;
    }
    if (i > 1) {
	par->err_msg = (char *)malloc(strlen(buffer) + 1);
	strcpy(par->err_msg, buffer);
    }

    if ((eof = read_boolean(&(par->bexp), fd)) != 1)
	    return( eof );
    return(1);
}




/************************************************************************
 *									*
 *	read_boolean(bexp, fd) - this orutine reads the boolean 	*
 *  expression from the unformatted key file.				*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
read_boolean(bexp, fd)
struct bool_exp **bexp;
FILE *fd;
{
    int temp,i;
    int eof;
    char c;
    char buffer[20];

    /*read out the type*/
    if (( eof = fread(&temp, sizeof(int), 1, fd)) != 1)
	return( eof );

    /*check for the expression case*/
    if (temp == 0)
	return(1);

    /*malloc space for the boolean we have*/
    bexp[0] = (struct bool_exp *)calloc(1, sizeof(struct bool_exp) );
    bexp[0]->type = temp;

    /*write out the value based on the type of data stored*/
    switch ( bexp[0]->type ) {
    case OPER   : /*write out the four byte descriptor*/
		  if ((eof = fread(&(bexp[0]->value.ival), sizeof(int), 1, fd))
								    != 1)
		      return(eof);
		  break;
    case REAL   : /*write out the real number*/
		  if ((eof = fread(&(bexp[0]->value.dval),sizeof(float), 1,fd))
								    != 1)
		      return( eof );
		  break;
    case PARVAL	: /*read in the parameter value descriptor*/
		  for(c = 'a', i = 0; c != '\0'; i++) {
		      if (( eof = c = getc( fd)) == EOF)
			  return( eof );
		      buffer[i] = c;
		  }
		  bexp[0]->value.sval = (char *)malloc(strlen(buffer) + 1);
	          strcpy(bexp[0]->value.sval, buffer);
		  break;
    default	: return( -1 );
		  break;
    }

    /*read the left kid if needed*/
    if ((bexp[0]->type == OPER) && (bexp[0]->value.ival != '!')) {
	if (( eof = read_boolean( &(bexp[0]->left), fd)) != 1)
	    return( eof );
    }
    if ( bexp[0]->type == OPER ) {
	if (( eof = read_boolean( &(bexp[0]->right), fd)) != 1)
	    return( eof );
    }
    return(1);
}
