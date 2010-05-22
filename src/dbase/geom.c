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
/*   geom.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:48 */
#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "skel.h"
#include "dbaccess.h"

#define XC(A)	cordinate(A,0)
#define YC(A)	cordinate(A,1)
#define ZC(A)	cordinate(A,2)

#define XCP(A)	cordinate(pt_nd(A),0)
#define YCP(A)	cordinate(pt_nd(A),1)
#define ZCP(A)	cordinate(pt_nd(A),2)

/*-----------------GEOM-------------------------------------------------
 * Returns a value representing the goodness of a triangle, in [-1...+1].
 * Negative values are for clockwise triangles.
 *----------------------------------------------------------------------*/
double good_tri(i,j,k)
int i,j,k;
{
#define NORM 3.464101616   /* So equilaterals come out one. */
    double x1,y1,x2,y2,x3,y3,det,dd;

    x1 = XCP(k) - XCP(j);
    y1 = YCP(k) - YCP(j);
    x2 = XCP(j) - XCP(i);
    y2 = YCP(j) - YCP(i);
    x3 = XCP(i) - XCP(k);
    y3 = YCP(i) - YCP(k);
    det = x2*y1-x1*y2;
    dd = x1*x1 + y1*y1 + x2*x2 + y2*y2 + x3*x3 + y3*y3;
    return (det*NORM/dd);
}

/*-----------------CCENTRE----------------------------------------------
 * Compute circumcentre of a triangle.
 *----------------------------------------------------------------------*/
char * ccentre (i, j, k, c, r)
int i,j,k;		/* Indices of points. */
float *c;		/* Centre. */
double       *r;    	/* Radius. */
{
    float p[MAXDIM],q[MAXDIM],dp[MAXDIM],dq[MAXDIM],alph[MAXDIM];
    double dist();
    int lil();

  /*...Center is defined by intersection of perp. bisectors of sides. */
    p[0] = 0.5 * (XC(j) + XC(i));
    p[1] = 0.5 * (YC(j) + YC(i));
    q[0] = 0.5 * (XC(k) + XC(j));
    q[1] = 0.5 * (YC(k) + YC(j));
    dp[0] = -0.5 * (YC(j) - YC(i));
    dp[1] =  0.5 * (XC(j) - XC(i));
    dq[0] = -0.5 * (YC(k) - YC(j));
    dq[1] =  0.5 * (XC(k) - XC(j));

    if (lil (p,dp,q,dq,alph))	return("Flat triangle in ccentre.");
    c[0] = p[0] + alph[0]*dp[0];
    c[1] = p[1] + alph[0]*dp[1];
    p[0]  = XC(i);
    p[1]  = YC(i);
    *r   = dist(p,c);
    return(0);
}

/*-----------------D_PERP-----------------------------------------------
 * Compute the directed perpendicular distance from edge to point.
 * (Same as lil routine, but simplifies call.)
 * Convention : if *ep is vertical, and *np is in the right half plane,
 * signed distance is positive.
 * Value returned is in units of the length of the edge.
 *----------------------------------------------------------------------*/
void d_perp(ep,n,alph)
struct edg_str *ep;
int n;
float alph[MAXDIM];
{
    float p[MAXDIM],dp[MAXDIM],q[MAXDIM],dq[MAXDIM];
    int ier,i,j;
    static char err[40];

    i = pt_nd(ep->nd[0]);
    j = pt_nd(ep->nd[1]);
    p[0] = cordinate(i,0);
    p[1] = cordinate(i,1);
    dp[0] = cordinate(j,0) - p[0];
    dp[1] = cordinate(j,1) - p[1];
    q[0] = cordinate( n, 0);
    q[1] = cordinate( n, 1);
    dq[0] = - dp[1];
    dq[1] =   dp[0];

    ier = lil(p,dp,q,dq,alph);
    if (ier) {
	sprintf(err,"Edge %d-%d has 0 length!",i,j);
	panic(err);
    }
}


double l_edge(ie)
    int ie;
{
    return (pdist(pt_edg(ie,0), pt_edg(ie,1)));
}

/*-----------------INTANG-----------------------------------------------
 * Return the internal angle of points p1-p2-p3.
 * Result in the range 0->2*PI.
 *----------------------------------------------------------------------*/
double intang(n1,n2,n3)
    int n1,n2,n3;
{
    double dx1,dy1,dx2,dy2,l1,l2,denom,cosa,sina,alph;

    dx1 = XCP(n2) - XCP(n1);
    dy1 = YCP(n2) - YCP(n1);
    dx2 = XCP(n3) - XCP(n2);
    dy2 = YCP(n3) - YCP(n2);
    l1 =  sqrt (dx1*dx1 + dy1*dy1);
    l2 =  sqrt (dx2*dx2 + dy2*dy2);
    denom = l1*l2;
    if (denom <= 0) return(MAXFLOAT);
    cosa = (dx1*dx2 + dy1*dy2) / denom;
    sina = (dx1*dy2 - dx2*dy1) / denom;

    /*Protect against rounding error*/
    if (cosa > 1) cosa = 1;
    if (cosa < -1) cosa = -1;

    alph =  acos(cosa);
    if (sina<0) alph= -alph;
    return(PI-alph);
}

/*-----------------LIL--------------------------------------------------
 * Compute the intersection of two lines in p,dp form.
 * Return 0 if ok,
 *        1 if parallel.
 *----------------------------------------------------------------------*/
int lil(p,dp,q,dq,alph)
float *p,*dp,*q,*dq,*alph;
{
    double dx,dy,det;

    dx  = p[0] - q[0];
    dy  = p[1] - q[1];
    det = dp[1]*dq[0] - dp[0]*dq[1];
    if (det == 0) {
	alph[0] = 0;
	alph[1] = 0;
	return(1);
	}
    alph[0] = (dx*dq[1] - dy*dq[0])/det;
    alph[1] = (dx*dp[1] - dy*dp[0])/det;
    return(0);
}



