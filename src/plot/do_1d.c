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
/*   do_1d.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:38:48 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "plot.h"
#include "material.h"
extern d_compar();

#define ABS(x)  (((x)>0.0)?(x):(-(x)))

/* A doubly linked list to represent the boundary */
typedef struct b_str { struct b_str *left, *right; int ie, j; } b_typ;

/************************************************************************
 *									*
 *	do_1d( ptype, val, bound, out ) - This routine plots the one 	*
 * dimensional data on the already initialized screen.  It plots either	*
 * in ptype at value val.  bound is a flag for drawing material bounds	*
 * on the screen.  If out is non-NULL, then the data is printed instead	*
 *									*
 * Original:	MEL	1/85						*
 *									*
 ************************************************************************/
do_1d( ptype, val, data, mat1, mat2, byarc )
int ptype;			/*type of one d cross section*/
float val;			/*the value in x or y*/
struct d_str data[];
int mat1, mat2;			/* two sides of an interface */
int byarc;			/* whether to go by arclength or x */
{
    register int t, i, j, nx, px;
    int num, in, count = 0, l[3];
    int vmax;
    float arr[MAXVRT][3];  /*coordinates of the surface/volume*/
    float p[2][3];	  /*coordinates of the line*/
    float sign;
    float tmp, xo, xn, yo, yn;
    char a[80], b[80];
    b_typ **seeds, **FindItf(), *ttt;

    /* Looking for x-y cross-sections */
    if ( (ptype == XSEC) || (ptype == YSEC) || (ptype == ZSEC) ) {
	/*initialize for small dimension case*/
	for(i = 0; i < MAXVRT; i++) arr[i][0] = arr[i][1] = arr[i][2] = val;

	/*initialize the storage locations*/
	switch( ptype ) {
	case XSEC : l[0] = 2; l[1] = 0; l[2] = 1; break;
	case YSEC : l[0] = 0; l[1] = 2; l[2] = 1; break;
	case ZSEC : l[0] = 0; l[1] = 1; l[2] = 2; break;
	}

	/*step through all the triangles*/
	for( t = 0; t < ne; t++ ) {
	    if (!leaf (tri[t])) continue;

	    /*step through each triangle index*/
	    for( i = 0; i < nvrt; i++ ) {
		nx = tri[t]->nd[i];
		px = nd[ nx ]->pt;

		for(j = 0; j < mode; j++)
		    arr[i][l[j]] = pt[ px ]->cord[j];
		for(j = mode; j < 2; j++)
		    arr[i][l[j]] = pt[ px ]->cord[j];
		arr[i][l[2]] = z[ nx ];
	    }

	    /*now that we have all that done, calculate the line*/
	    num = intersect(arr, val, p);
	    for(j = 0; j < num; j++) {
		/*should probably save and sort these guys*/
		data[count].x = p[j][0];
		data[count].y = p[j][1];
		data[count].mat = reg_tri(t);
		count++;
	    }
	}

        /*sort the data*/
        qsort(data, count, sizeof(struct d_str), d_compar);

	/*eliminate the duplicates*/
	sprintf(a, "%16e\t%16e", data[0].x, data[0].y);

	for(vmax = count, i = count = 1; i < vmax; i++) {
	    sprintf(b, "%16e\t%16e", data[i].x, data[i].y);

	    if ( (strcmp(a, b) != 0) || (data[count-1].mat != data[i].mat) ) {
		data[count].x = data[i].x;
		data[count].y = data[i].y;
		data[count].mat = data[i].mat;
		count++;
		strcpy(a, b);
	    }
	}

        /*check to make sure the materials are contiguous*/
        for(i = 1; i < count - 1; i++) {
	    if ( fabs(data[i].x - data[i-1].x) < 1.0e-10 ) {
		if ( data[i].mat != data[i+1].mat ) {
		    tmp = data[i-1].mat; data[i-1].mat = data[i].mat; data[i].mat = tmp;
		    tmp = data[i-1].y; data[i-1].y = data[i].y; data[i].y = tmp;
		    tmp = data[i-1].x; data[i-1].x = data[i].x; data[i].x = tmp;
		}
	    }
	}

	/*convert regions back into materials*/
	for(i = 0; i < count; i++) data[i].mat = mat_reg(data[i].mat);
    }

    /* Looking for material interfaces */
    else if ( ptype == BND ) {
	seeds = FindItf( mat1, mat2);

	/* Set up the data array by accumulating distance */
	count = 0;
	for (i = 0; seeds[i]; i++) {

	    /* Leftmost point is special */
	    in = tri[ seeds[ i]->ie]->nd[ (seeds[i]->j+2)%3];
	    xo = pt[ nd[ in]->pt]->cord[0];
	    yo = pt[ nd[ in]->pt]->cord[1];
	    data[ count].x = xo;
	    data[ count].y = z[ in];
	    data[ count].mat = nd[ in]->mater;
	    count++;

	    /* A heuristic to make both sides of an interface look the same */
	    sign = 1;
	    in = tri[ seeds[ i]->ie]->nd[ (seeds[ i]->j+1)%3];
	    if( pt[ nd[ in]->pt]->cord[0] < xo) sign = -1;

	    /* Then all the right points */
	    for(ttt = seeds[ i]; ttt; ttt = ttt->right) {
		in = tri[ ttt->ie]->nd[ (ttt->j+1)%3];
		xn = pt[ nd[ in]->pt]->cord[0];
		yn = pt[ nd[ in]->pt]->cord[1];
		if( byarc)
		    data[ count].x = data[ count-1].x + sign * hypot( xn-xo, yn-yo);
		else
		    data[ count].x = xn;
		data[ count].y = z[ in];
		data[ count].mat = nd[ in]->mater;
		count++;
		xo = xn; yo = yn;
	    }
	}
	free( seeds);
    }

    /*return the number of data points*/
    return( count );
}


d_compar(f1, f2)
struct d_str *f1, *f2;
{
    if ( f1->x > f2->x )
	return( 1 );
    else if ( f1->x < f2->x )
	return( -1 );
    else if ( f1->mat > f2->mat )
	return( 1 );
    else if ( f1->mat < f2->mat )
	return( -1 );
    else
	return( 0 );
}

/*-----------------FindItf---------------------------------------------
 * Return an array of linked lists representing the interfaces between
 * two materials.
 * Orientation is such that leftmost node is first for mat1=Si, mat2=Ox.
 *----------------------------------------------------------------------*/
b_typ **
FindItf( mat1, mat2)
    int mat1, mat2;
{
    int i, ns=0, ie, j, Ms=10;
    b_typ *ttt, **seeds, *AddItfEdge(), **touched;

    seeds = salloc( b_typ *,  Ms);
    touched = salloc( b_typ *, 3*ne);

    for (i = 0; i < 3*ne; i++) touched[i] = 0;

    /* Find seed edges and form doubly linked lists*/
    for (ie = 0; ie < ne; ie++) {
	for (j = 0; j < 3; j++) {
	    if (touched[ 3*ie+j]) continue;
	    if (ttt = AddItfEdge( ie, j, mat1, mat2, touched)) {
		for (seeds[ ns] = ttt; seeds[ ns]->left; seeds[ ns] = seeds[ ns]->left)
		    ;
		if (ns++ >= Ms-1) {
		    Ms *= 2;
		    seeds = sralloc( b_typ *, Ms, seeds);
		}
	    }
	}
    }
    seeds[ ns] = 0;
    free( touched);
    return( seeds);
}

/*-----------------AddItfEdge-----------------------------------------
 * Recursive routine to add one edge and its neighbors and
 * their neighbors and ...
 *----------------------------------------------------------------------*/
b_typ *
AddItfEdge( ie, j, mat1, mat2, touched)
    int ie, j, mat1, mat2;
    b_typ **touched;
{
    b_typ *new;
    int je, oje, k, kk, ib;

    /* Is this edge desired? */
    if( ie < 0 || mat_reg(reg_tri(ie)) != mat1) return(0);
    ib = tri[ ie]->nb[ j];
    if (((ib < 0) && (ib == mat2)) ||
	((ib >= 0) && (mat_reg(reg_tri(ib)) == mat2)))
    {
	/* Yes, is it already there? */
	if (touched[ 3*ie+j])
	    return( touched[ 3*ie+j]);

	/* Otherwise create it */
	new = salloc( b_typ, 1);
	new->ie = ie;
	new->j  = j;
	touched[ 3*ie+j] = new;

	/* Look clockwise, then anticlockwise */
	trotate( ie, (j+2)%3, 1, &je, &oje, &k, &kk);
	new->left = AddItfEdge( oje, 3-k-kk, mat1, mat2, touched);
	trotate( ie, (j+1)%3, 0, &je, &oje, &k, &kk);
	new->right = AddItfEdge( oje, 3-k-kk, mat1, mat2, touched);

	return( new);
    }
    else return(0);
}

