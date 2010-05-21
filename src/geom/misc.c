/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   misc.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:34:05 */

#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"

/************************************************************************
 *									*
 *	dist( a     ,  b     ) - this function returns the distance 	*
 *  between two points. if b is null, just return magnitude.            *
 *									*
 ************************************************************************/
double dist ( a, b)
float a[MAXDIM], b[MAXDIM];
{
    float tx, ty;

    tx = a[0];
    ty = a[1]; 
    if (b) {
	tx -= b[0];
	ty -= b[1]; 
    } 
    switch( mode ) {
    case ONED :
	return( fabs(tx) );
    case TWOD :
	return ( sqrt( tx*tx + ty*ty ) );
    }
    return(0.0);
}
    

/************************************************************************
 *									*
 *	vol_ele(t) - this routine returns the volume of the element	*
 *									*
 ************************************************************************/
float vol_ele(t)
int t;
{
    switch(mode) {
    case ONED :
	return( fabs( cordinate(pt_nd(vert_tri(t,0)),0) - 
		      cordinate(pt_nd(vert_tri(t,1)),0) ) );
	break;
    case TWOD :
	return( area_tri(
		      cordinate(pt_nd(vert_tri(t,0)),0),
		      cordinate(pt_nd(vert_tri(t,0)),1),
		      cordinate(pt_nd(vert_tri(t,1)),0),
		      cordinate(pt_nd(vert_tri(t,1)),1),
		      cordinate(pt_nd(vert_tri(t,2)),0),
		      cordinate(pt_nd(vert_tri(t,2)),1) ) );
	break;
    }
    return(0.0);
}

/************************************************************************
 *									*
 *	area_tri(a1,a2,b1,b2,c1,c2) - this routine returns the area of 	*
 *  the triangle formed by the coordintes passed.			*
 *									*
 ************************************************************************/
float area_tri(a1,a2,b1,b2,c1,c2)
float a1,a2,b1,b2,c1,c2;
{
    return( (b1*(c2-a2) + c1*(a2-b2) + a1*(b2-c2) ) * 0.5 );
}



/************************************************************************
 *									*
 *	pt_in_tri(x,y,tr) - this routine determins if the pt x,y is in  *
 *  element number tr.							*
 *									*
 ************************************************************************/
pt_in_tri(c,tr)
float c[3];
int tr;
{
    float tx1, tx2, tx3, ty1, ty2, ty3;
    float abtol, artol, tsign;
    float x = c[0];
    float y = c[1];

    switch( mode ) {
    case ONED :
	if ( (c[0] < cordinate(pt_nd(vert_tri(tr,0)),0)) != 
	     (c[0] <= cordinate(pt_nd(vert_tri(tr,1)),0)) )
	    return(TRUE);
	else
	    return(FALSE);
	break;
    case TWOD :
	/*get triangle coordinates*/
	tx1 = cordinate( pt_nd( vert_tri(tr,0) ), 0);
	ty1 = cordinate( pt_nd( vert_tri(tr,0) ), 1);
	tx2 = cordinate( pt_nd( vert_tri(tr,1) ), 0);
	ty2 = cordinate( pt_nd( vert_tri(tr,1) ), 1);
	tx3 = cordinate( pt_nd( vert_tri(tr,2) ), 0);
	ty3 = cordinate( pt_nd( vert_tri(tr,2) ), 1);

	/*calculate tolerances*/
	abtol = 1e-04;
	artol = area_tri(tx1,ty1, tx2,ty2, tx3,ty3);
	tsign = artol/fabs(artol);
	artol = fabs(artol);
	artol = artol * abtol;

	/*begin the testing*/
	if ( (tsign * area_tri( tx1,ty1, tx2,ty2, x,y )) <= -artol)
	    return( FALSE );
	if ( (tsign * area_tri( tx2,ty2, tx3,ty3, x,y )) <= -artol)
	    return( FALSE );
	if ( (tsign * area_tri( tx3,ty3, tx1,ty1, x,y )) <= -artol)
	    return( FALSE );
    
	/*if all that failed, retun true*/
	return( TRUE );
    }
    return(-1);
}


