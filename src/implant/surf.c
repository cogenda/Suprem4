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
/*   surf.c                Version 2.1     */
/*   Last Modification : 2/3/93 15:10:37 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sysdep.h"
#include "constant.h"
#include "global.h"
#include "geom.h"
#include "material.h"
#include "implant.h"
#include "refine.h"

double tr[MAXDIM][MAXDIM];

#define XTRAN(x, y) (mode==ONED)?(x / tr[1][1]):(x*tr[0][0] + y*tr[0][1])
#define YTRAN(x, y) (mode==ONED)?(0.0):(x*tr[1][0] + y*tr[1][1])

/************************************************************************
 *									*
 *	make_surf() - This routine finds and orders a surface list	*
 *  for use in implantation.						*
 *									*
 *  Original:	MEL	12/86						*
 *									*
 ************************************************************************/
make_surf( v, ang, lat )
struct surf_info **v;
double ang;
double lat;
{
    register int i,j,k,in;   /*every routine needs these for indexing*/
    struct line *s;		/*surface list*/
    int ns;			/*and number thereof */
    struct surf_info *cur, *prev, *tmp, lft, rht;
    float x, px =0.0;
    int dbl, doed;
    float mval[2*MAXMAT];
    int mat[2*MAXMAT];
    int mnuml, mnumr;
    float atop;
    int id;

    /*build the translation matrix*/
    tr[0][0] = cos(ang);	tr[0][1] = sin(ang);	tr[0][2] = 0.0;
    tr[1][0] = -sin(ang);	tr[1][1] = cos(ang);	tr[1][2] = 0.0;
    tr[2][0] = 0.0;		tr[2][1] = 0.0;		tr[2][2] = 1.0;

    /* generate a surface description */
    ns = find_surf( &s );

    /*before angular rotation, get the materials at left and right*/
    lft.rht[0] = lft.lft[0] = pt[ s[0].p ]->cord[0] + ERR;
    get_mat( &lft, TRUE );
    rht.lft[0] = rht.rht[0] = pt[ s[ns-1].p ]->cord[0] - ERR;
    get_mat( &rht, TRUE );

    /*translate the mesh onto the new angle coordinates*/
    for(i = 0; i < np; i++) {
	for(j = 0; j < mode; j++) {
	    pt[i]->cordo[j] = pt[i]->cord[j];
	}
	pt[i]->cord[0] = XTRAN(pt[i]->cordo[0], pt[i]->cordo[1]);
	pt[i]->cord[1] = YTRAN(pt[i]->cordo[0], pt[i]->cordo[1]);
    }

    if ( mode == ONED ) {
	cur = scalloc( struct surf_info, 1 );
	*v = cur;
	cur->next = NULL;
	get_mat( cur, FALSE );
	return;
    }

    if ( mode != ONED ) {
	/*copy the line into the surface info structure*/
	cur = scalloc( struct surf_info, 1 );
	*v = cur;
	prev = cur;
	x = pdist( s[0].p, s[1].p );
	for(i = 0; i < mode; i++) {
	    cur->rht[i] = pt[ s[0].p ]->cord[i];
	    cur->lft[i] = lat * (pt[ s[0].p ]->cord[i] - pt[ s[1].p ]->cord[i])
			    / x + pt[ s[0].p ]->cord[i];
	}
    }

    /*build the list*/
    for(i = 1; i < ns; i++) {

	cur = scalloc( struct surf_info, 1 );

	for(j = 0; j < mode; j++) {
	    cur->lft[j] = pt[ s[i-1].p ]->cord[j];
	    cur->rht[j] = pt[ s[i].p ]->cord[j];
	}

	/*set up linked list*/
	prev->next = cur;
	prev = cur;

    }

    if (mode != ONED ) {
	cur = scalloc( struct surf_info, 1 );
	prev->next = cur;
	cur->next = NULL;
	x = pdist( s[ns-2].p, s[ns-1].p );
	for(i = 0; i < mode; i++) {
	    cur->rht[i] = lat*(pt[s[ns-1].p]->cord[i]-pt[s[ns-2].p]->cord[i]) / x
			   + pt[ s[ns-1].p]->cord[i];
	    cur->lft[i] = pt[ s[ns-1].p ]->cord[i];
	}
    }

    prev = NULL;
    if (mode==ONED)
	id = 0;
    else
	id = 1;

    cur = *v;
    while ( cur != NULL ) {
	if ( prev == NULL ) {
	    atop = 0.5 * (cur->lft[id] + cur->rht[id]);
	    cur->nmat = lft.nmat;
	    for(j = 0; j < lft.nmat; j++) {
		cur->top[j] = lft.top[j] - lft.top[0] + atop;
		cur->bot[j] = lft.bot[j] - lft.top[0] + atop;
		cur->mat[j] = lft.mat[j];
	    }
	}
	else if (cur->next == FALSE) {
	    cur->nmat = rht.nmat;
	    atop = 0.5 * (cur->lft[id] + cur->rht[id]);
	    for(j = 0; j < rht.nmat; j++) {
		cur->top[j] = rht.top[j] - rht.top[0] + atop;
		cur->bot[j] = rht.bot[j] - rht.top[0] + atop;
		cur->mat[j] = rht.mat[j];
	    }
	}
	else if ( cur->lft[0] >= cur->rht[0] ) {
	    cur->nmat = 0;
	}
	else {
	    get_mat( cur, FALSE );
	}
	prev = cur;
	cur = cur->next;
    }
}



/************************************************************************
 *									*
 *	get_mat() - This routine finds the material indices for a given	*
 *  surface point.							*
 *									*
 *  Original:	MEL	12/86						*
 *									*
 ************************************************************************/
get_mat( cur, angle )
struct surf_info *cur;
int angle;
{
    int j, k;
    float mval[2*MAXMAT], tf;
    int mat[2*MAXMAT], ti;
    int mnum;
    float x[3];

    get_edge(mval,mat,&mnum,Y_SLICE, 0.5*(cur->lft[0] + cur->rht[0]) );

    /*order from top to bottom each material*/
    for(j = 0; j < mnum; j+=2) {
	if ( mval[j] > mval[j+1] ) {
	    /*swap them*/
	    ti = mat[j+1];  mat[j+1] = mat[j];   mat[j] = ti;
	    tf = mval[j+1]; mval[j+1] = mval[j]; mval[j] = tf;
	}
    }

    /*order the pairs of materials*/
    for(j = 0; j < mnum-2; j+=2) {
	for(k = j+2; k < mnum; k+=2) {
	    if ( mval[k] < mval[j] ) {
		/*swap them*/
		ti = mat[k];  mat[k] = mat[j];   mat[j] = ti;
		tf = mval[k];  mval[k] = mval[j];   mval[j] = tf;
		ti = mat[k+1];  mat[k+1] = mat[j+1];   mat[j+1] = ti;
		tf = mval[k+1];  mval[k+1] = mval[j+1];   mval[j+1] = tf;
	    }
	}
    }

    if ( angle ) {
	switch( mode ) {
	case ONED :
	    x[0] = mval[0] / tr[1][1];
	    break;
	default :
	    x[0] = cur->top[0] * tr[1][0] + mval[0] * tr[1][1];
	    break;
	}

	for( j = 1; j < mnum; j++ )
	    mval[j] = (mval[j] - mval[0]) / tr[0][0] + x[0];
	mval[0] = x[0];
    }

    /*load the material offsets*/
    for(cur->nmat = j = 0; j < mnum; j+= 2) {
	cur->top[cur->nmat] = mval[j];
	cur->bot[cur->nmat] = mval[j+1];
	cur->mat[cur->nmat] = mat[j];
	cur->nmat++;
    }

}



/************************************************************************
 *									*
 *	free_surf() - This routine finds and orders a surface list	*
 *  for use in implantation.						*
 *									*
 *  Original:	MEL	12/86						*
 *									*
 ************************************************************************/
free_surf( v )
struct surf_info **v;
{
    struct surf_info *cur, *next;

    cur = *v;

    while( cur != NULL ) {
	next = cur->next;
	free( cur );
	cur = next;
    }
}


