
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
/*   write.c                Version 4.1     */
/*   Last Modification : 8/18/89  16:35:37 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "check.h"
#include "key.h"

/************************************************************************
 *									*
 *	write.c - The routines defined here dump the card structure as  *
 *  read from the key file to an unformatted file for later use.	*
 *  The method for dumping the hierarchy is recursive because I don't   *
 *  want anyone to be able to understand it. (Myself included)		*
 *  									*
 ************************************************************************/


/************************************************************************
 *									*
 *	write_list(param, fd) - this writes out each of the parameters  *
 *  in the list passed to it.  If there are sub parameters, it 		*
 *  increments depth and calls itself on the sub parameters.		*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
write_list(param, fd)
struct par_str **param;
int fd;
{

    while ( param[0]->name[0] != '\0' ) {
	/*process this parameter*/
	if (write_param( param[0], fd ) == -1)
	    return(-1);

	/*does this sucker have sub parameters??*/
	if (param[0]->param != NULL) {
	    depth++;
	    if (write_list( param[0]->param, fd ) == -1)
		return(-1);
	    depth--;
	}

	/*advance the counter*/
	param++;
    }
}


/************************************************************************
 *									*
 *	write_boolean(bexp) - this routine stores the boolean 		*
 *  expression in a post fix notation.  It is, once again, recursive    *
 *  because of traditional reasons.					*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
write_boolean(bexp, fd)
struct bool_exp *bexp;
int fd;
{

    /*first write out the current structure*/
    write(fd, &(bexp->type), sizeof(int));

    /*write out the value based on the type of data stored*/
    switch ( bexp->type ) {
    case OPER   : /*write out the four byte descriptor*/
		  if (write(fd, &bexp->value.ival, sizeof(int)) == -1)
		      return(-1);
		  break;
    case REAL   : /*write out the real number*/
		  if (write(fd, &bexp->value.dval, sizeof(float)) == -1)
		      return( -1 );
		  break;
    case PARVAL	: /*write out the descriptor string*/
		  if (write(fd,bexp->value.sval,strlen(bexp->value.sval)+1)==-1)
		      return(-1);
		  break;
    default	: return( -1 );
		  break;
    }

    /*write out the left and right kids*/
    if ( bexp->left != NULL ) {
	if ( write_boolean( bexp->left, fd) == -1)
	    return(-1);
    }
    if ( bexp->right != NULL ) {
	if ( write_boolean( bexp->right, fd) == -1)
	    return(-1);
    }
    return(0);
}


/************************************************************************
 *									*
 *	write_param(par, fd) - this routine writes out the single parameter *
 *  passed to it.							*
 *									*
 *	Original	Mark E. Law		Oct, 1984		*
 *									*
 ************************************************************************/
write_param(par, fd)
struct par_str *par;
int fd;
{
    char nullstr[2];
    int i;

    nullstr[0] = '\0';
    i = 0;
    /*first write out the current depth*/
    if ( write(fd, &depth, sizeof(int)) == -1)
	return(-1);

    /*begin by writing out all of the fixed length crap*/
    if (write(fd, par->name, 12) == -1)
	return(-1);
    if (write(fd, &par->type, sizeof(int)) == -1)
	return(-1);

    /*write out the default based on type data*/
    switch( par->type & ~MASK ) {
    case REAL :	if (write(fd, &par->def.dval, sizeof(float)) == -1)
		    return(-1);
		break;
    case COMM :
    case BOOL :
    case CHOICE :
    case INT  :	if (write(fd, &par->def.ival, sizeof(int)) == -1)
		    return(-1);
		break;
    case STR  :	if (par->def.sval != NULL) {
		    if (write(fd, par->def.sval, strlen(par->def.sval)+1) == -1)
			return(-1);
		}
		else {
		    if (write(fd, nullstr, 1) == -1)
			return(-1);
		}
		break;
    default   : return(-1);
    }

    /*write out the units and the error message*/
    if (par->units != NULL) {
	if (write(fd, par->units, strlen(par->units)+1) == -1)
	    return(-1);
    }
    else {
	if (write(fd, nullstr, 1) == -1)
	    return(-1);
    }
    if (par->err_msg != NULL) {
	if (write(fd, par->err_msg, strlen(par->err_msg)+1) == -1)
	    return(-1);
    }
    else {
	if (write(fd, nullstr, 1) == -1)
	    return(-1);
    }

    /*write out the boolean expression*/
    if (par->bexp != NULL) {
	if (write_boolean(par->bexp, fd) == -1)
	    return(-1);
    }
    else {
	if (write(fd, &i, sizeof(int)) == -1)
	    return(-1);
    }
    return(0);
}



