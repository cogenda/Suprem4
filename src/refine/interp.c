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
/*   interp.c                Version 5.1     */
/*   Last Modification : 7/3/91 15:40:45 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/times.h>

#include <global.h>
#include <constant.h>
#include <geom.h>
#include <regrid.h>
#include <material.h>
#include <impurity.h>
#include "refine.h"
#include "skel.h"


/*
 * interpolate the solutions found in r2 onto those found in r1
*/
void interp( int r1, int r2)
{
    register int ie1, ie2;
    register int i,j, in, fnd;
    int *done;
    struct tms before, after;

    times(&before);

    /*initialize the done array*/
    done = salloc( int, nn );
    for(ie1 = 0; ie1 < nn; ie1++) done[ie1] = FALSE;

    /*
     This has to be done in a somewhat cludgey fashion,
     because the region structure has not been built yet for r1
    */

    /*step over all the elements in the first region*/
    ie1 = 0;
    while( done_tri(ie1) ) {

	/*if this triangle is in the region*/
	if ( reg_tri(ie1) == r1 ) {
	    for(i = 0; i < num_vert(ie1); i++) {
		in = vert_tri(ie1, i);
		if ( done[in] ) continue;

		/*now look in the second region for triangles...*/
		done[in] = TRUE;

		fnd = FALSE;
		for(j = 0; !fnd && j < num_tri_reg(r2); j++) {
		    ie2 = tri_reg(r2,j);

		    if ( pt_in_tri(cord_arr(pt_nd(in)), ie2 ) ) {
			fnd = TRUE;

			if ( mode == TWOD ) {
			    interp_2d(in, ie2);
			}
			else if (mode == ONED) {
			    interp_1d(in, ie2);
			}
		    }
		}
		if ( !fnd ) panic("node not in any triangle");
	    }
	}
	next_tri(ie1);
    }
    free(done);
    times(&after);
    print_time("total interpolation time", &before, &after);
}


interp_1d(in, ie2)
int in, ie2;
{
    register int i;
    double r, ir;
    double ans[3], new;

    r = pdist( pt_nd(in), pt_nd(vert_tri(ie2,0)) ) /
        pdist( pt_nd(vert_tri(ie2,1)), pt_nd(vert_tri(ie2,0)) );
    ir = 1.0 - r;

    /*solve the system for all solutions*/
    for(i = 0; i < n_imp; i++) {
	switch( soltoimp[i] ) {
	case B   : case Ba   :
	case P   : case Pa   :
	case Sb  : case Sba  :
	case As  : case Asa  :
	case iBe : case iBea :
	case iMg : case iMga :
	case iSe : case iSea :
	case iSi : case iSia :
	case iSn : case iSna :
	case iGe : case iGea :
	case iZn : case iZna :
	case iC  : case iCa  :
	case iG  : case iGa  :
	    ans[0] = log(sol_nd(vert_tri(ie2,0),i)+1.0);
	    ans[1] = log(sol_nd(vert_tri(ie2,1),i)+1.0);
	    new = r * ans[1] + ir * ans[0];
	    set_sol_nd(in, i, exp(new));
	    break;
	default:
	    ans[0] = sol_nd(vert_tri(ie2,0),i);
	    ans[1] = sol_nd(vert_tri(ie2,1),i);
	    new = r * ans[1] + ir * ans[0];
	    set_sol_nd(in, i, new);
	    break;
	}
    }
}


interp_2d(in, ie2)
int in, ie2;
{
    register int i, k;
    int pvt[3];
    double b[3][3], ans[3], new;

    /*compute the linear basis interpolation function*/
    for(k = 0; k < num_vert(ie2); k++) {
	b[k][0] = cordinate( pt_nd(vert_tri(ie2,k)), 0);
	b[k][1] = cordinate( pt_nd(vert_tri(ie2,k)), 1);
	b[k][2] = 1.0;
    }

    /*factor the matrix with pivoting*/
    fac3x3( b, pvt );

    /*solve the system for all solutions*/
    for(i = 0; i < n_imp; i++) {
	switch( soltoimp[i] ) {
	case B   : case Ba   :
	case P   : case Pa   :
	case Sb  : case Sba  :
	case As  : case Asa  :
	case iBe : case iBea :
	case iMg : case iMga :
	case iSe : case iSea :
	case iSi : case iSia :
	case iSn : case iSna :
	case iGe : case iGea :
	case iZn : case iZna :
	case iC  : case iCa  :
	case iG  : case iGa  :
	    ans[0] = log(sol_nd(vert_tri(ie2,0),i)+1.0);
	    ans[1] = log(sol_nd(vert_tri(ie2,1),i)+1.0);
	    ans[2] = log(sol_nd(vert_tri(ie2,2),i)+1.0);
	    sol3x3(b, pvt, ans);
	    new = ans[0] * cordinate(pt_nd(in),0) +
		  ans[1] * cordinate(pt_nd(in),1) +
		  ans[2];
	    set_sol_nd(in, i, exp(new));
	    break;
	default:
	    ans[0] = sol_nd(vert_tri(ie2,0),i);
	    ans[1] = sol_nd(vert_tri(ie2,1),i);
	    ans[2] = sol_nd(vert_tri(ie2,2),i);
	    sol3x3(b, pvt, ans);
	    new = ans[0] * cordinate(pt_nd(in),0) +
		  ans[1] * cordinate(pt_nd(in),1) +
		  ans[2];
	    set_sol_nd(in, i, new);
	    break;
	}
    }
}


#define p0 pvt[0]
#define p1 pvt[1]
#define p2 pvt[2]

fac3x3( a, pvt )
double a[3][3];
int pvt[3];
{
    register int i, i1, i2;
    double am;

    /*perform pivoting*/
    am = 0.0;
    for(i = 0; i < 3; i++)
	if ( fabs(a[i][0]) > am ) {am = fabs(a[i][0]); pvt[0] = i;}

    /*first row*/
    for(i = 1; i < 3; i++) a[p0][i] /= a[p0][0];

    /*the rest of the matrix*/
    i1 = (pvt[0]+1) %3;
    i2 = (pvt[0]+2) %3;

    a[i1][1] -= a[i1][0] * a[p0][1];
    a[i1][2] -= a[i1][0] * a[p0][2];
    a[i2][1] -= a[i2][0] * a[p0][1];
    a[i2][2] -= a[i2][0] * a[p0][2];

    if ( fabs(a[i1][1]) > fabs(a[i2][1]) && a[i1][1] != 0.0 ) {
	pvt[1] = i1;
	pvt[2] = i2;
    }
    else {
	pvt[1] = i2;
	pvt[2] = i1;
    }

    /*second row*/
    a[p1][2] /= a[p1][1];

    /*third row*/
    a[p2][2] -= a[p2][1] * a[p1][2];
}

sol3x3(a, pvt, ans)
double a[3][3];
int pvt[3];
double ans[3];
{
    double temp[3];

    /*first do the the for solve*/
    ans[p0] /= a[p0][0];
    ans[p1] = (ans[p1] - a[p1][0] * ans[p0]) / a[p1][1];
    ans[p2] = (ans[p2] - a[p2][1] * ans[p1] - a[p2][0] * ans[p0]) / a[p2][2];

    /*now the back solve*/
    temp[2] = ans[p2];
    temp[1] = ans[p1] - a[p1][2] * temp[2];
    temp[0] = ans[p0] - a[p0][1] * temp[1] - a[p0][2] * temp[2];

    ans[0] = temp[0];ans[1] = temp[1];ans[2] = temp[2];
}
