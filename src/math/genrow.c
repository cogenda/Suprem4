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
/*   genrow.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:44:14 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "matrix.h"



/************************************************************************
 *									*
 *	genrow() - This routine computes the row indices so that the	*
 *  matrix can be accessed in both the symmetric fashion and by row.	*
 *									*
 *  Original:	MEL	10/87						*
 *									*
 ************************************************************************/
genrow( ia, aoff, irow, jrow )
int *ia;
int aoff;
int *irow;
int *jrow;
{
    register int i, j, indx, row, col;
    int *links;			/*linked list pointers*/
    int *locs;			/*locations*/
    int *cnum;			/*column number*/
    int el;			/*end of the list pointer*/

    /*malloc the local space*/
    links = salloc( int, ia[nn]+1+aoff );
    locs  = salloc( int, ia[nn]+1+aoff );
    cnum  = salloc( int, ia[nn]+1+aoff );

    /*initialize the links*/
    for(i = 0; i < nn; i++) links[i] = -1;

    /*run through the column list in ia and invert it*/
    el = nn;
    for(row = nn-1; row >= 0; row--) {
	for(j = ia[row]; j < ia[row+1]; j++) {
	    col = ia[j];

	    /*update the links add at beginning*/
	    links[el] = links[col];
	    links[col] = el;
	    cnum[el] = row;
	    locs[el] = j;
	    el++;
	}
    }

    /*now that we have ia inverted to a row list, make irow, jrow*/
    irow[0] = nn+1;
    indx = nn + 1;
    for(row = 0; row < nn; row++) {

	/*first merge in the linked list*/
	for( i = links[row]; i != -1 ; i = links[i] ) {
	    irow[indx] = cnum[i];
	    jrow[indx] = locs[i];
	    indx += 1;
	}

	/*now merge in any column space*/
	for( i = ia[row]; i < ia[row+1]; i++ ) {
	    irow[indx] = ia[i];
	    jrow[indx] = i + aoff;
	    indx += 1;
	}

	irow[row+1] = indx;
    }

    /*free the space and go home*/
    free( links );
    free( locs );
    free( cnum );
}
