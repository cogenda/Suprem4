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
/*   grid_upd.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:54 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "impurity.h"
#include "regrid.h"
#include "matrix.h"		/* For the old, new areas */
#define assert(x) if (!(x)) panic("assertion failed")

extern float oxide_grid;

#define PC(x) if (err = (x)) panic(err)

/* Addiction to convenience fodder */
#define tpt(T,V)   (nd[ tri[ (T)]->nd[ (V)]]->pt)
#define bedge(T,E) (tri[ (T)]->nb[ (E)] < 0 || \
		    tri[ tri[ (T)]->nb[ (E)]]->regnum != tri[ (T)]->regnum)
#define X 0
#define Y 1
#define Z 2
#define cross(A,B) ((A)[X] * (B)[Y] - (A)[Y] * (B)[X])
#define dot(A,B) ((mode == ONED)?((A)[X]*(B)[X]):((A)[X]*(B)[X]+(A)[Y]*(B)[Y]))
#define abs(X)  (((X) >= 0)? (X) : -(X))

#define vertex(t,i) tri[t]->nd[i]
#define nbrtri(t,i) tri[t]->nb[i]
#define point(n)    nd[n]->pt
#define sameRegion(a,b) ((a==b) || (a>=0)&&(b>=0)&&(reg_tri(a)==reg_tri(b)))


/*-----------------GRID_ADD---------------------------------------------
 * Add some new grid to oxide connections which are getting too long.
 * Algorithm: call new_layer().
 * Bugs: It's not guaranteed to do nothing if the grid isn't moving.
 *----------------------------------------------------------------------*/
int grid_add(dt)			/* Returns fix_conn */
double dt;
{
    int fix_conn = 0;

    /* add some new grid on the oxide side if we are really simulating it*/
    if (oxide_grid != 0)
	if ( new_layer(Si, SiO2, oxide_grid, dt)) {
	    fix_conn++;
	    UpdateSymbolic++;
	    bd_connect("Grid Addition");
	}

    return( fix_conn);
}


/*-----------------BACK_PEDAL-------------------------------------------
 * Look for triangles that will be turned over by the grid motion.
 * Provides a HARD upper limit on the time step, which is cut back
 * if necessary.
 *----------------------------------------------------------------------*/
back_pedal( dt )
     double *dt;
{
    static double worst;	/* Most severe cutback needed */
    double p[2],q[2];		/* Triangle vectors */
    double dp[2], dq[2];	/* Triangle displacements */
    double pxq, pxdq, dpxq, dpxdq; /* Terms in cross product */
    double minRoot;		/* Cutback for this triangle, its delta */
    int ie;			/* Triangle, vertex, dimension indices */
    double pA, pB, pC, root1, root2, discrim;
    int p0, p1, p2;
#   define TOL 0.05


    worst = 1;
    
    switch( mode ) {
    case TWOD :
	for (ie = 0; ie < ne; ie++) {

	    /* Unpack triangle */
	    p0 = point( vertex(ie,0));
	    p1 = point( vertex(ie,1));
	    p2 = point( vertex(ie,2));
	
	    /*compute difference terms*/
	    p[X]=xcord(p1)-xcord(p0); 	  p[Y]=ycord(p1)-ycord(p0);
	    q[X]=xcord(p2)-xcord(p0);	  q[Y]=ycord(p2)-ycord(p0);
	    dp[X] = *dt*(xvel(p1) - xvel(p0)); 
	    dp[Y] = *dt*(yvel(p1) - yvel(p0));
	    dq[X] = *dt*(xvel(p2) - xvel(p0));
	    dq[Y] = *dt*(yvel(p2) - yvel(p0));
	
	    /* Compute cross product terms */
	    pxq   = cross(  p,  q);
	    pxdq  = cross(  p, dq);
	    dpxq  = cross( dp,  q);
	    dpxdq = cross( dp, dq);

	    /* current area better be positive */
	    assert(pxq > 0);

	    /* consider the area as a function of step length */
	    /* pxq + t*(pxdq+dpxq) + t*t*(dpxdq) - TOL pxq */
	    pA = dpxdq;
	    pB = pxdq+dpxq;
	    pC = (1-TOL)*pxq ;
	
	    /* Check first if its linear */
	    minRoot = 1;
	    if( abs(pA) == 0.0 ) {
	    
		/* If the area is going down, see how fast */
		if (pB < 0)
		    minRoot = - pC/pB;
	    }

	    /* Nope, it's quadratic. Examine the roots */
	    else {

		discrim = (pB*pB - 4*pA*pC);
		if (discrim > 0) {
		    discrim = sqrt(discrim);
		    root1 = (-pB+discrim)/(2*pA);
		    if (root1 > 0 && root1 < minRoot) minRoot = root1;
		    root2 = (-pB-discrim)/(2*pA);
		    if (root2 > 0 && root2 < minRoot) minRoot = root2;
		}
	    }

	    /* Keep a record of the smallest root encountered */
	    if (minRoot < worst) {
		worst = minRoot;
	    }
	}
	break;

    case ONED :
	for (ie = 0; ie < ne; ie++) {

	    /* Unpack triangle */
	    p0 = point(vertex(ie,0));
	    p1 = point(vertex(ie,1));
	
	    /*compute difference terms*/
	    pxq = xcord(p0)-xcord(p1);
	    dp[X] = *dt * (xvel(p0) - xvel(p1)); 
	
	    /* current area better be positive */
	    assert(pxq > 0);

	    /* consider the area as a function of step length */
	    /* pxq + s*dp - TOL pxq */
	    pC = (1-TOL) * pxq;
	    pB = dp[X];

	    minRoot = 1;
	    
	    /* If the area is going down, see how fast */
	    if (pC + pB < 0)
		minRoot = - pC/pB;

	    /* Keep a record of the smallest root encountered */
	    if (minRoot < worst) {
		worst = minRoot;
	    }
	}
	break;
    }

    
    if (worst < 1) {
	if( verbose >= V_BARF)
	    printf ("Time step cut back by oxide to %g%%\n", 100*worst);
	*dt *= worst;
	fixup_t1d (worst);
    }
}


/*-----------------Grid Removal-----------------------------------------
 * Some info describing node removal.  These are stored in the point
 * structure so that we can choose what nodes to kill
 * deep in the oxidation code, but come back at the end of a diffuse
 * and do the actual removal then.
 *----------------------------------------------------------------------*/

/* Store info on what to kill in the flags field.  Positive integers only! */
#define BFACMASK 0xff00
#define BFACN 8
#define PKILLMASK 0xff0000
#define PKILLN 16
#define getbfac(p)   ((pt[p]->flags & BFACMASK)>>BFACN)
#define setbfac(p,n) pt[p]->flags = (pt[p]->flags &~BFACMASK)|(n<<BFACN)
#define getkill(p)   ((pt[p]->flags & PKILLMASK)>>PKILLN)
#define setkill(p,n) pt[p]->flags = (pt[p]->flags &~PKILLMASK)|(n<<PKILLN)

/*-----------------GRID_SUB---------------------------------------------
 * Requires: tri->nb
 * Provides: tri->nb, nd->tri
 * This routine kills the nodes marked by ChooseKillNodes.
 * Then it cleans up the data structure as necessary.
 *----------------------------------------------------------------------*/
grid_sub(dt)
double dt;
{
    int ie, ip;
    int test = FALSE;

    if( test = CknMcLen( dt, oxide_Ltol, oxide_Ltol, 5.0e-8))
	(void)CknMcLen( dt, oxide_Etol, oxide_Etol, 5.0e-8);

    if ( test ) {
	for(ie = 0; ie < ned; ie++)
	    if ( ask( edg[ie], KILL_LATER) ) {rem_edg( ie ); test = TRUE;}

	bd_connect("after removing silicon nodes");
	UpdateSymbolic++;
    }

    if( test = CknMcObt( dt, oxide_Ltol, oxide_Ltol))
	(void)CknMcObt( dt, oxide_Etol, oxide_Etol);

    if ( test ) {
	for(ip = 0; ip < np; ip++)
	    if ( ask( pt[ip], KILL_LATER) ) {rem_pt( ip ); test = TRUE;}

	bd_connect("after removing silicon nodes");
	UpdateSymbolic++;
    }

    return;
}



/*-----------------ChooseKillNodes---------------------------------------
 * This routine decides which nodes are to go.
 * All the ugly heuristics are here.
 * Requires: tri->nb, nd->tri (down in HackNodes)
 *----------------------------------------------------------------------*/
ChooseKillNodes( dt)
    double dt;			/* time step */
{
    return;
}

/*-----------------CknMcCoy---------------------------------------------
 * The real thing.
 *----------------------------------------------------------------------*/
CknMcLen( dt, sepTol, obTol, abstol)
    double dt;			/* time step */
    double sepTol, obTol;	/* Tolerance for separation, obtuseness */
    double abstol;
{     
    int ie, Kills = 0; 
    int t, e, n;
    int pl[2];
    float lrel[MAXDIM];
    float BadObtuse();
    
#define pvel(A,B) pt[A]->vel[B]

    /*
     * Go: Look for nodes with approachment problems.
     * Closeness and obtuseness are handled differently, because
     * nodes can get arbitrarily close  without no damage.
     * Eventually back_pedal will choose a timestep so that the triangle
     * they are both in will shrink by 50% in one timestep and then
     * they will annhilate.
     * On the contrary, it is not tolerable to let a triangle gradually
     * get obscenely obtuse.  Even if a 150 degree triangle is only
     * losing 1% of its perpendicular height each timestep, something
     * has to be done.
     */
    
    /* Pass 1:  look for nodes that are getting too close together. */

    /* Walk on triangle edges */
    for( ie = 0; ie < ned; ie++ ) {

	/* Get the nodes and points at the ends */
	/* 1~right 2~left when nbr is above and j below (y up)*/
	pl[0] = pt_edg(ie,0);
	pl[1] = pt_edg(ie,1);

	if ( ask(edg[ie], KILL_LATER ) ) continue;

	/* Get relative velocity, displacement (vectors point to 1)*/
	lrel[ X] = xcord(pl[0])-dt*pvel(pl[0],X)-xcord(pl[1])+dt*pvel(pl[1],X);
	lrel[ Y] = ycord(pl[0])-dt*pvel(pl[0],Y)-ycord(pl[1])+dt*pvel(pl[1],Y);
	lrel[ Z] = zcord(pl[0])-dt*pvel(pl[0],Z)-zcord(pl[1])+dt*pvel(pl[1],Z);

	/* If the separation decrease by no more than half, let it be */
	if( (len_edg(ie)*len_edg(ie) < (1.0-sepTol)*dot(lrel, lrel)) ||
	    (len_edg(ie) < abstol) ) {
	    set( edg[ie], KILL_LATER );
	    Kills++;
	    if ( mode == TWOD ) {
		if ( num_tri_edg(ie) == 1 ) {
		    t = tri_edg(ie,0);
		    e = ewhich(t,ie);
		    n = neigh_fc(t,e);
		    if ( n >= 0 ) {
			e = twhich(n,t);
			set( edg[edg_ele(n,e)], KILL_LATER );
			Kills++;
		    }
		}
	    }
	}
    }
    return( Kills);
}

CknMcObt( dt, sepTol, obTol)
    double dt;			/* time step */
    double sepTol, obTol;	/* Tolerance for separation, obtuseness */
{     
    int ie, j, skip, Kills = 0; 
    int ip, ip1, ip2;
    float BadObtuse(), tt;
    
    /*
     * Go: Look for nodes with approachment problems.
     * Closeness and obtuseness are handled differently, because
     * nodes can get arbitrarily close  without no damage.
     * Eventually back_pedal will choose a timestep so that the triangle
     * they are both in will shrink by 50% in one timestep and then
     * they will annhilate.

    /* Pass2: look for obtuse triangles that are getting worse.*/
    if ( mode == TWOD ) {
	for (ie = 0; ie < ne; ie++) {
	    skip = FALSE;
	    /*if any edge is marked for deletion, than skip it*/
	    for(j = 0; j < num_edge(ie); j++) 
		skip = skip ||  ask( edg[edg_ele(ie,j)], KILL_LATER ) ;
	    for(j = 0; j < num_vert(ie); j++) 
		skip = skip ||  ask( pt[pt_nd(vert_tri(ie,j))], KILL_LATER ) ;

	    if ( skip ) continue;

	    for(j = 0; j < 3; j++) {
		ip = pt_nd( vert_tri(ie,j) );
		ip1 = pt_nd( vert_tri(ie,(j+1)%3) );
		ip2 = pt_nd( vert_tri(ie,(j+2)%3) );

		/*we do not do this for boundary nodes!*/
		if ( pt[ip]->nn != 1 ) continue;

		/* Skip it if nothing happening. */
		if( pt[ip]->vel[X] == 0 && pt[ip]->vel[Y] == 0 &&
		    pt[ip1]->vel[X] == 0 && pt[ip1]->vel[Y] == 0 &&
		    pt[ip2]->vel[X] == 0 && pt[ip2]->vel[Y] == 0) continue;

		/* Check if the triangle looks bad after update */
		if((tt=BadObtuse(xcord(ip), ycord(ip),
				 xcord(ip1), ycord(ip1),
				 xcord(ip2), ycord(ip2)))
		       > 0.5) continue;

		/* Yes it does.  But maybe it's getting better */
		if( BadObtuse((float)(xcord(ip )-dt*pt[ip ]->vel[X]),
			      (float)(ycord(ip )-dt*pt[ip ]->vel[Y]),
			      (float)(xcord(ip1)-dt*pt[ip1]->vel[X]),
			      (float)(ycord(ip1)-dt*pt[ip1]->vel[Y]),
			      (float)(xcord(ip2)-dt*pt[ip2]->vel[X]),
			      (float)(ycord(ip2)-dt*pt[ip2]->vel[Y])) < tt) continue;


		set(pt[ip], KILL_LATER);
		Kills++;
	    }
	    
	}/*end of triangles */
    }
    
    return( Kills);
}
   
/*-----------------BadObtuse--------------------------------------------
 * Look for a bad obtuse angle at ip subtented by ip1, ip2
 *----------------------------------------------------------------------*/
float BadObtuse( x,y, x1,y1, x2,y2)
    float x,y,x1,y1,x2,y2;
{
    float d1[2], d2[2], len0, len1, len2, area2, pheight, bad;
    extern double fmin(), fmax();
    
    if ( mode == ONED ) return( 1.0 );

    d1[X] = x1 - x;
    d1[Y] = y1 - y;
    d2[X] = x2 - x;
    d2[Y] = y2 - y;

    /* If it's not obtuse period, forget it */
    if( dot( d1, d2) >= 0) return(1.0);

    len0 = hypot( d1[X] - d2[X], d1[Y] - d2[Y]);
    len1 = hypot( d1[X], d1[Y]);
    len2 = hypot( d2[X], d2[Y]);
    area2 = cross( d1,  d2);
    pheight = area2 / len0;

    bad = fmax( pheight/ len1, pheight/len2);

    return( bad);
}

