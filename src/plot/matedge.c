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
/*   matedge.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:38:55 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"




/************************************************************************
 *									*
 *	mat_edges( dir, value ) - This routine plots the edges of 	*
 *  materials that cross the appropriate line for a 1d plot.		*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
mat_edges( dir, value, ymin, ymax )
int dir;
float value;
float ymin, ymax;
{
    int i;
    float mval[MAXMAT];		/*array of the crossing locations*/
    int mat[MAXMAT];		/*material that is crossed*/
    int mnum;			/*number of materials that are crossed*/

    /*the next step is to find the crossing values*/
    get_edge(mval, mat, &mnum, dir, value);

    /*now draw the vertical lines at the crossings*/
    for(i = 0; i < mnum; i += 2 ) {
	xgNewSet();
	xgSetName(MatNames[mat_reg(mat[i])]);
	xgNewGroup();
	xgPoint( mval[i], ymin );
	xgPoint( mval[i], ymax );
    }
}




/************************************************************************
 *									*
 *	get_edge( lots of stuff ) - This routine is the meat and	*
 *  potatoes of the finding of material interfaces for the plots and	*
 *  implant code.  It searches through the polygon descriptions of the	*
 *  materials and adds a value whenever there is a crossing.		*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
get_edge(mval, mat, mnum, dir, value)
float mval[];		/*the returned intersection points*/
int mat[];		/*the returned materials crossed*/
int *mnum;		/*the number of materials crossed*/
int dir;		/*the direction along which to find the crossings*/
float value;		/*the value for the constant line*/
{
    register int i;
    register int j, k, l, e;
    register int tstart;
    struct LLedge *bp, *bnd;
    float c1[3];
    float c2[3];
    float tf, mn, mx;
    int dup_cnt;

    /*search each region, one by one*/
    for(*mnum = 0, i = 0; i < nreg; i++) {
	tstart = *mnum;
	
	switch(mode) {
	case TWOD :
	    /*search each regions edges*/
	    for(j = 1, bp = bnd = sreg[reg[i]->sreg]->bnd; j || bp != bnd;
		j = 0, bp = bp->next ) {

		e = bp->edge;

		c1[0] = xcord( pt_edg(e,0) );
		c1[1] = ycord( pt_edg(e,0) );
		c2[0] = xcord( pt_edg(e,1) );
		c2[1] = ycord( pt_edg(e,1) );

		/*check and see if the line crosses*/
		if ( dir ) {
		    if ( ((c1[0] < value) && (c2[0] >= value)) ||
			 ((c1[0] <= value) && (c2[0] > value)) ||
			 ((c1[0] > value) && (c2[0] <= value)) ||
			 ((c1[0] >= value) && (c2[0] < value)))  {
			mat[ *mnum ] = i;
			/*interpolate the value*/
			if (c1[0] == c2[0]) 
			    mval[ *mnum ] = c1[1];
			else
			    mval[ *mnum ] = (value-c1[0])/(c2[0]-c1[0]) * 
						      (c2[1]-c1[1]) + c1[1];
			(*mnum)++;
		    }
		}
		else {
		    if ( ((c1[1] < value) && (c2[1] >= value)) ||
			 ((c1[1] <= value) && (c2[1] > value)) ||
			 ((c1[1] > value) && (c2[1] <= value)) ||
			 ((c1[1] >= value) && (c2[1] < value)))  {
			mat[ *mnum ] = i;
			/*interpolate the value*/
			if (c1[1] == c2[1]) 
			    mval[ *mnum ] = c1[0];
			else
			    mval[ *mnum ] = (value-c1[1])/(c2[1]-c1[1]) * 
						      (c2[0]-c1[0]) + c1[0];
			(*mnum)++;
		    }
		}
	    }
	    break;

	case ONED :
	    mn = MAXFLOAT;
	    mx = -MAXFLOAT;
	    for(j = 1, bp = bnd = sreg[reg[i]->sreg]->bnd; j || bp != bnd;
		j = 0, bp = bp->next ) {
		e = bp->edge;
		c1[0] = xcord( pt_edg(e,0) );
		mn = (mn < c1[0])?mn:c1[0];
		mx = (mx > c1[0])?mx:c1[0];
		c2[0] = xcord( pt_edg(e,1) );
		mn = (mn < c2[0])?mn:c2[0];
		mx = (mx > c2[0])?mx:c2[0];
	    }

	    mat[ *mnum ] = i;
	    mval[ *mnum ] = mn;
	    (*mnum)++;
	    mat[ *mnum ] = i;
	    mval[ *mnum ] = mx;
	    (*mnum)++;
	    break;
	}

	/*sort the material crossing into ascending order*/
	for(k = tstart; k < *mnum; k++) {
	    for(l = k+1; l < *mnum; l++) {
 		if (mval[k] > mval[l]) {
		    tf = mval[l]; mval[l] = mval[k]; mval[k] = tf;
		}
	    }
	}

	/*check for crossing at a point*/
	for(dup_cnt = 0, k = tstart; k < *mnum -1 ; k++) {
	    if ( fabs(mval[k+1] - mval[k]) < 1.0e-10 ) dup_cnt++;
	}

	/*four cases - duplicates even/odd, number even/odd*/
	if ( ((dup_cnt % 2) == 0) && ((*mnum - tstart) % 2 == 0) ) {
	    /*remove one of each*/
	    for(k = tstart; k < *mnum -1 ; k++) {
		if ( fabs(mval[k+1] - mval[k]) < 1.0e-10 ) {
		    /*move everything down*/
		    for(l = k; l < *mnum - 1; l++) mval[l] = mval[l+1];
		    *mnum -= 1;
		}
	    }	
	}
	else if ( ((dup_cnt % 2) == 1) && ((*mnum - tstart) % 2 == 0) ) {
	    /*cross at a point, remove both duplicates*/
	    for(k = tstart; k < *mnum -1 ; k++) {
		if ( fabs(mval[k+1] - mval[k]) < 1.0e-10 ) {
		    /*move everything down*/
		    for(l = k; l < *mnum - 1; l++) mval[l] = mval[l+1];
		    *mnum -= 1;
		}
	    }	
	}
	else if ( ((dup_cnt % 2) == 1) && ((*mnum - tstart) % 2 == 1) ) {
	    /*remove one of each*/
	    for(k = tstart; k < *mnum -1 ; k++) {
		if ( fabs(mval[k+1] - mval[k]) < 1.0e-10 ) {
		    /*move everything down*/
		    for(l = k; l < *mnum - 1; l++) mval[l] = mval[l+1];
		    *mnum -= 1;
		}
	    }	
	}
	else if ( ((dup_cnt % 2) == 0) && ((*mnum - tstart) % 2 == 1) ) {
	    /*remove one of each*/
	    for(k = tstart; k < *mnum -1 ; k++) {
		if ( fabs(mval[k+1] - mval[k]) < 1.0e-10 ) {
		    /*move everything down*/
		    for(l = k; l < *mnum - 1; l++) mval[l] = mval[l+1];
		    *mnum -= 1;
		}
	    }	
	}


    }
}

