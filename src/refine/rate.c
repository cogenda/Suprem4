/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   rate.c                Version 5.1     */
/*   Last Modification : 7/3/91  15:40:48 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "impurity.h"
#include "regrid.h"
#include "refine.h"
extern float area_tri();

/*define some macros to work on the triple point handling*/
#define TRIPLE 0x4
#define gettrip(p)   (pt[p]->flags & TRIPLE)
#define settrip(p)   pt[p]->flags = (pt[p]->flags | TRIPLE)
#define unsettr(p)   pt[p]->flags = (pt[p]->flags & ~TRIPLE)



/************************************************************************
 *									*
 *	phys_etch( rate, time ) - This routine provides a physical etch *
 *  of the surface of the wafer.  This code is leveraged off the 	*
 *  exisiting oxide code.						*
 *									*
 *  Original:	MEL	5/88						*
 *									*
 ************************************************************************/
phys_etch( rate, time )
double *rate;		/*the etch rate as a function of material*/
double time;
{
    double total=0.0, dt, dtmax, maxr=0.0;
    register int xsol, ysol;
    register int i,j;
    int *isgrow;
    int prio[MAXMAT], *done;

    /*figure the material priorities - fastest is the highest*/
    for(i = 0; i < MAXMAT; i++) prio[i] = 0;
    for(i = 0; i < MAXMAT; i++) {
	if ( rate[i] > maxr ) maxr = rate[i];
	for(j = 0; j < MAXMAT; j++) if ( rate[i] > rate[j] ) prio[i]++;
    }


    /*get some temporary space*/
    done = salloc( int, nn );
    isgrow = salloc( int, nn );

    /*add impurities for the x and y velocity*/
    add_impurity( XVEL, 0.0, -1 );
    add_impurity( YVEL, 0.0, -1 );
    xsol = imptosol[XVEL];
    ysol = imptosol[YVEL];

    /*zero the initial velocities and interface flags*/
    for(i = 0; i < nn; i++) nd[i]->sol[xsol] = nd[i]->sol[ysol] = 0.0;
    for(i = 0; i < np; i++) {
	isgrow[i] = FALSE;
	unsettr( i );
    }

    /*process only twenty angstroms at a time*/
    dtmax = time;
    dt = dtmax;

    /*now that we have that done, proceed to do the etch*/
    while( total < time ) {

	if ( total + dt > time ) dt = time - total;

	/*zero out the appropriate array*/
	for(j = 0; j < np; j++) isgrow[j] = done[j] = FALSE;

	/*compute the velocity for all the exposed nodes*/
	compute_rate( rate, prio );

	/*save away the point velocities in a managable way*/
	point_vel( );

	/*check for grid looping*/
	if (DetectLoop()) printf("delooping the string\n");

	/*cut the time step back if needed*/
	back_pedal( &dt );

	ChooseKillNodes( dt );

	/*move the points to the new locations*/
	move_point( dt );

	/*update the computation time*/
	total += dt;
	dt = dtmax;

	/*grid subtraction*/
	grid_sub( dt );
    }

    /*free the local storage*/
    free( done );
    free( isgrow );
    lose_impurity( XVEL );
    lose_impurity( YVEL );
}




/************************************************************************
 *									*
 *	compute_rates() This routine computes the etch rate at each	*
 *  point in the mesh.							*
 *									*
 ************************************************************************/
compute_rate( rate, prio )
double *rate;	/*etch rate of each material*/
int *prio;	/*the material priority*/
{
    struct line *s;
    double lastv[MAXDIM];
    double newv[MAXDIM], perp[MAXDIM], dir[MAXDIM], dis;
    int ns;
    register int i, j, p, p1, p2, n, m;
    int t1, m1, t2, m2;
    int xsol = imptosol[XVEL];
    int ysol = imptosol[YVEL];

    ns = find_surf( &s );

    /*first fix up any unfixed triple points*/
    for(i = 0; i < ns; i++) {
	p = s[i].p;
	if ((pt[p]->nn > 2) && (!gettrip(p))) {
	    settrip(p);
	    triple_fix( p, s[i-1].p );
	    s[i].p = np-1;
	    s[i].map = s[i-1].map;
	    triple_fix( p, s[i+1].p );
	    for(j = ns-1; j > i; j--) s[j+1].p = s[j].p;
	    ns++;
	    s[i+1].p = np-1;
	    s[i+1].map = s[i+2].map;
	    trip_tri(p, &t1, &m1, &t2, &m2);
	    if ( prio[m1] > prio[m2] )
		rate_trip(p, rate[m1], t1, t2);
	    else
		rate_trip(p, rate[m2], t1, t2);
	}
	else if ( gettrip(p) ) {
	    if ( pt[p]->nn > 2 ) {
		trip_tri(p, &t1, &m1, &t2, &m2);
		if ( prio[m1] > prio[m2] )
		    rate_trip(p, rate[m1], t1, t2);
		else
		    rate_trip(p, rate[m2], t1, t2);
		for(j = i; j < ns-1; j++) s[j].p = s[j+1].p;
		ns--;
	    }
	    else {
		unsettr(p);
		for(j = 0; j < pt[p]->nn; j++) {
		    n = pt[p]->nd[j];
		    m = nd[n]->mater;
		    if ( m != GAS ) s[i].map = m;
		}
	    }
	}
	else {
	    if ( pt[p]->nn != 2 ) printf("triple handler error - l1\n");
	    for(j = 0; j < pt[p]->nn; j++) {
		n = pt[p]->nd[j];
		m = nd[n]->mater;
		if ( m != GAS ) s[i].map = m;
	    }
	}
    }

    switch(mode) {
    case ONED :
	for(i = 0; j < pt[s[0].p]->nn; j++) {
	    n = pt[p]->nd[j];
	    nd[n]->sol[xsol] = rate[s[0].map];
	}
	break;

    case TWOD :
	/*for each surface segment, figure the etch rate*/
	for(i = 0; i < ns-1; i++) {
	    p1 = s[i].p;
	    p2 = s[i+1].p;
	    dis = 0.0;

	    /*sanity checks....*/
	    if ( (pt[p1]->nn != 2) || (pt[p2]->nn != 2) )
		printf("error in triple handler - l2\n");

	    if ( s[i].map == s[i+1].map ) {
		dir[0] = pt[p2]->cord[0] - pt[p1]->cord[0];
		dir[1] = pt[p2]->cord[1] - pt[p1]->cord[1];
		dis = sqrt(dir[0]*dir[0] + dir[1]*dir[1]);
		perp[0] = - dir[1] / dis;
		perp[1] = dir[0] / dis;

		/*compute the rate of the nodes on this edge*/
		lastv[0] = newv[0];
		lastv[1] = newv[1];
		newv[0] = rate[s[i].map] * perp[0];
		newv[1] = rate[s[i].map] * perp[1];

		if ( i == 0 ) {
		    for(j = 0; j < pt[p1]->nn; j++) {
			n = pt[p1]->nd[j];
			nd[n]->sol[xsol] = 0.0;
			nd[n]->sol[ysol] = rate[s[i].map];
		    }
		}
		else if ( s[i-1].map != s[i].map ) {
		    for(j = 0; j < pt[p1]->nn; j++) {
			n = pt[p1]->nd[j];
			nd[n]->sol[xsol] = newv[0] * perp[0];
			nd[n]->sol[ysol] = newv[1] * perp[1];
		    }
		}
		else {
		    for(j = 0; j < pt[p1]->nn; j++) {
			n = pt[p1]->nd[j];
			nd[n]->sol[xsol] = 0.5 * (newv[0] + lastv[0]) * perp[0];
			nd[n]->sol[ysol] = 0.5 * (newv[1] + lastv[1]) * perp[1];
		    }
		}
	    }
	    else {
		for(j = 0; j < pt[p1]->nn; j++) {
		    n = pt[p1]->nd[j];
		    nd[n]->sol[xsol] = newv[0] * perp[0];
		    nd[n]->sol[ysol] = newv[1] * perp[1];
		}
	    }
	}
	p2 = s[ns-1].p;
	for(j = 0; j < pt[p2]->nn; j++) {
	    n = pt[p1]->nd[j];
	    nd[n]->sol[xsol] = 0.0;
	    nd[n]->sol[ysol] = rate[s[i].map];
	}
	break;
    }
}


/************************************************************************
 *									*
 *	compute_rates() This routine computes the etch rate at each	*
 *  point in the mesh.							*
 *									*
 ************************************************************************/
rate_trip( p, rate, t1, t2 )
int p;
double rate;		/*the etch rate as a function of material*/
int t1;
{
    int i, fj, nb;
    int p1, p2;
    int xsol = imptosol[XVEL];
    int ysol = imptosol[YVEL];
    double dir[MAXDIM];

    fj = -1;
    for(i = 0; i < nedg; i++)
	if ( tri[t1]->nb[i] == t2 ) fj = i;

    /*get the points on either end of the shared segment*/
    p1 = nd[ tri[t1]->nd[(fj+1)%nedg] ]->pt;
    p2 = nd[ tri[t1]->nd[(fj+2)%nedg] ]->pt;
    if (p == p2) { p2 = p1; p1 = p; }

    /*compute the direction along the segment*/
    dir[0] = pt[p2]->cord[0] - pt[p1]->cord[0];
    dir[1] = pt[p2]->cord[1] - pt[p1]->cord[1];

    for(i = 0; i < pt[p]->nn; i++) {
	nb = pt[p]->nd[i];
	nd[nb]->sol[xsol] = rate * dir[0];
	nd[nb]->sol[ysol] = rate * dir[1];
    }
}
