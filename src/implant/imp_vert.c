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
/*   imp_vert.c                Version 5.1     */
/*   Last Modification : 7/3/91 12:08:45 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "implant.h"

/************************************************************************
 *									*
 *	imp_vert() - This function returns a value between 0 and 1 	*
 *  corresponding to the value of the distribution function at the 	*
 *  given depth.							*
 *									*
 *  Original:	MEL	1/87						*
 *									*
 ************************************************************************/
double imp_vert(depth, dat)
double	depth ;
struct imp_info	*dat;
{
    double val;

    if ( imp_model == PEARS ) {
	if ( depth < dat->maxz ) {
	    val = prson(depth - dat->vert[PRS_RP], dat);
	}
	else 
	    val = 0.0;
    }
    else if ( imp_model == GAUSS ) {
	if ( depth < dat->maxz ) {
	    val = (depth - dat->vert[GUS_RP]) / dat->vert[GUS_SIG];
	    val = exp( - val * val * 0.5 );
	}
	else val = 0.0;
    }
    return( val );
}


double func(l, pt, dat)
double l[MAXDIM];
double pt[MAXDIM];
struct imp_info	*dat;
{
    return( imp_vert((l[0] - pt[0])*1.0e4, dat) * imp_latr((l[1] - pt[1])*1.0e4, dat) );
}



double simp_int( l, a, b, eps, dat )
double l[MAXDIM];
double a[MAXDIM];        
double b[MAXDIM];        
double eps;
struct imp_info *dat;
{
    int i;
    double s, sbar;
    double ab[MAXDIM], ab3[MAXDIM], a3b[MAXDIM];
    double h = b[1] - a[1];
    double fa, fab, fb, fa3, fb3;
    double integ = 0.0;

    for(i = 0; i < mode; i++) {
	ab[i] = 0.5 * (a[i] + b[i]);
	ab3[i] = 0.75*a[i]+0.25*b[i];
	a3b[i] = 0.75*b[i]+0.25*a[i];
    }

    /*Evaluate using the adaptive Simpson's rule*/
    fa = func(l, a, dat);
    fab = func(l, ab, dat);
    fb = func(l, b, dat);
    fa3 = func(l, ab3, dat);
    fb3 = func(l, a3b, dat);

    s = h * (fa + 4.0 * fab + fb) / 6.0;
    sbar = h * (fa + 4.0*fa3 + 2*fab + 4*fb3 + fb) / 12.0;

    if ( fabs(s - sbar) > 15.0 * eps ) {
	integ = simp_int(l, a, ab, eps, dat) + simp_int(l, ab, b, eps, dat);
    }
    else
	integ = sbar;

    return(integ);
}


double evaluate(l, lft, rht, dat)
double l[MAXDIM];  
double lft[MAXDIM];        
double rht[MAXDIM];        
struct imp_info *dat;
{
    double a[MAXDIM], b[MAXDIM];
    double val;

    /*This is confusing due to a mismatch - parallel to beam is
      cooridinate 0, perp is cord 0*/
    a[0] = lft[1] - 0.5 * (lft[1] + rht[1]);
    a[1] = lft[0];
    b[0] = rht[1] - 0.5 * (lft[1] + rht[1]);
    b[1] = rht[0];

    if ( mode == ONED ) {
	val = imp_vert(l[0]*1.0e4, dat);
    }
    else if ( mode == TWOD ) {
	val = simp_int(l, a, b, 1.0e-12, dat);
    }
    return(val);
}
