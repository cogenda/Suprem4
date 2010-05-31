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
/*   plot_2d.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:39:03 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "impurity.h"
#include "material.h"
#include "geom.h"
#include "regrid.h"
#include "plot.h"

#define XCORD(A) pt[ A ]->cord[0]
#define YCORD(A) pt[ A ]->cord[1]


void draw_vornoi();
void draw_points (float );
void draw_stress ( float vleng, float smax, int ccol, int tcol);
void draw_flow( float vleng, float vmax, int col);


/************************************************************************
 *									*
 *	plot_2d(par, param ) - this routine is the father of the 	*
 *  plot_2d software.  It calls appropriate routines to set up 		*
 *  and do plots.							*
 *									*
 *  Original :	MEL, CSR, MRP	11, 1984 (modeled on pisces2)		*
 *									*
 ************************************************************************/
int plot_2d( char *par, int param )
{
    int i;
    int boundary;
    int grid;
    int clear;
    int fill;
    static int line_bound=1, line_grid=1, line_com=3, line_ten=4;
    int vornoi, diamonds;
    int stress;
    static float txmin, txmax, tymin, tymax;
    float vleng, vmax;
    char pbuf[100];
    int deb;
    int axis;

    if ( InvalidMeshCheck()) return( -1);

    deb = pl_debug;
    pl_debug = FALSE;

    if ( mode == ONED ) {
	fprintf( stderr, "plot.2d is illegal in one dimension\n" );
	return ( -1 );
    }

    /*get the card parameters*/
    boundary = get_bool(param, "boundary");
    grid     = get_bool(param, "grid");
    vornoi   = get_int(param, "vornoi");
    diamonds = get_bool(param, "diamonds");
    stress   = get_bool(param, "stress");
    clear    = get_bool(param, "clear");
    axis     = get_bool(param, "axis");
    fill     = get_bool(param, "fill");
    if(	is_specified( param, "line.bound")) line_bound = get_int(param, "line.bound");
    if(	is_specified( param, "line.grid")) line_grid = get_int(param, "line.grid");
    if(	is_specified( param, "line.com")) line_com = get_int(param, "line.com");
    if(	is_specified( param, "line.ten")) line_ten = get_int(param, "line.ten");
    if( is_specified( param, "vmax"))    vmax    = get_float(param, "vmax");
    if( is_specified( param, "vleng"))    vleng    = 1e-4* get_float(param, "vleng");
    else if (stress) {
	fprintf(stderr, "A vector length must be specified!\n");
	stress = 0;
    }


    dev_lmts(&txmin, &txmax, &tymin, &tymax);

    if ( is_specified( param, "x.min" ) )
	txmin     = get_float(param, "x.min") * 1e-4;
    if ( is_specified( param, "x.max" ) )
	txmax     = get_float(param, "x.max") * 1e-4;
    if ( is_specified( param, "y.min" ) )
	tymin     = get_float(param, "y.min") * 1e-4;
    if ( is_specified( param, "y.max" ) )
	tymax     = get_float(param, "y.max") * 1e-4;

    /* have a heart */
    if (txmin > txmax) {float swap = txmin; txmin = txmax; txmax = swap;}
    if (tymin > tymax) {float swap = tymin; tymin = tymax; tymax = swap;}

    if (clear) xgClear();

    if (!fill) {

	/* Center the requested area in the window */

	float dx = txmax-txmin, dy=tymax-tymin, cenwin;
	if (dx > dy) {
	    cenwin = 0.5*(tymin+tymax);
	    tymax = cenwin + 0.5*dx;
	    tymin = cenwin - 0.5*dx;
	} else {
	    cenwin = 0.5*(txmin+txmax);
	    txmax = cenwin + 0.5*dy;
	    txmin = cenwin - 0.5*dy;
	}
    }

    /* Check the results to avoid disaster */
    if( txmin >= txmax || tymin >= tymax) {
	fprintf( stderr, "bad bounds in plotting: x(%f:%f) y(%f:%f)\n",txmin, txmax, tymin, tymax);
	return(-1);
    }

    yflip = TRUE;
    /*set some options*/
    xgSetScale(1.0e4, -1.0e4);

    xgLogAxis(FALSE, FALSE);
    if (axis)
	xgSetBounds(txmin*1.0e4, txmax*1.0e4, -tymax*1.0e4, -tymin*1.0e4);
    xgAxisLabels("x in microns", "y in microns", title);

    xgNewSet();

    if (grid) {
	xgSetName("grid");
	for(i = 0; i < ned; i++) edge_pl(i);
    }

    if (vornoi) {
	draw_vornoi();
    }

    if (boundary) {
	material(line_bound);
    }

    if (diamonds) {
	draw_points(0.005*(xmax-xmin+ymax-ymin));
    }

    if (stress)
	draw_stress( vleng, vmax, line_com, line_ten);

    if( get_bool( param, "flow"))
	draw_flow( vleng, vmax, line_com);

    /*clean up plotting and post it out*/
    xgUpdate(0);
    pl_debug = deb;

    return(0);

}

/*-----------------DRAW_VORNOI------------------------------------------
 * Draw the Vornoi tesselation. Boundary segments are done in material();
 * points can be done in draw_points().
 *----------------------------------------------------------------------*/
void draw_vornoi()
{
	int ie, nj, j, j1, j2;
	float ci[2], cj[2];
	double ri, rj;
	extern char *ccentre();

#       define nop(J,I) (nd[ tri[I]->nd[J] ]->pt)
#	define ngh(J,I) (tri[I]->nb[J])
#	define lx(J,I)  (pt[ nd[ tri[I]->nd[J] ]->pt ]->cord[0])
#	define ly(J,I)  (pt[ nd[ tri[I]->nd[J] ]->pt ]->cord[1])

	xgNewSet();
	xgSetName("vornoi");
	for (ie=0; ie < ne; ie++)
	{
	    if (!leaf (tri[ie])) continue;
	    if (ccentre (nop(0,ie), nop(1,ie), nop(2,ie), ci, &ri) == NULL ) {
		for (j=0; j < 3; j++)
		{
		    nj = ngh (j, ie);
		    if (nj < 0) {
			j1 = (j+1)%3;
			j2 = (j+2)%3;
			xgNewGroup();
			xgPoint ( 0.5*(lx(j1,ie)+lx(j2,ie)) ,
				0.5*(ly(j1,ie)+ly(j2,ie)) );
			xgPoint (ci[0], ci[1]);
		    }
		    else {
			if (ccentre (nop (0, nj), nop(1,nj), nop(2,nj), cj, &rj) == NULL) {
			    xgNewGroup();
			    xgPoint (cj[0], cj[1]);
			    xgPoint (ci[0], ci[1]);
			}
		    }
		}
	    }
	}
}

void draw_points (float delt)
{
    int ip;

    xgNewSet();
    xgSetName("points");
    xgSetMark(1);
    for (ip = 0; ip < np; ip++) {
	xgNewGroup();
	xgPoint (pt[ip]->cord[0], pt[ip]->cord[1]);
    }

}

/*-----------------DRAW_STRESS------------------------------------------
 *----------------------------------------------------------------------*/
void draw_stress ( float vleng, float smax, int ccol, int tcol)
{
    int in;
    float t1, t2, rho, p1, p2, sxx,syy,sxy, ang, dx, dy, cx, cy, delt;

    delt = vleng;

    if( imptosol[ Sxx] == -1) {
	fprintf(stderr, "That value is not available for plotting\n");
	return;
    }

    xgNewSet();
    xgSetName("Stress Field");

    /* First work out max principal stress */
    if (smax == 0) {
	for (in = 0; in < nn; in++) {
	    sxx = nd[in]->sol[ imptosol[Sxx]];
	    sxy = nd[in]->sol[ imptosol[Sxy]];
	    syy = nd[in]->sol[ imptosol[Syy]];
	    t1 = 0.5*(sxx+syy);
	    t2 = 0.5*(sxx-syy);
	    rho = sqrt(t2*t2 + sxy*sxy);
	    p1 = t1 + rho;	if (p1 < 0) p1 = -p1;
	    p2 = t1 - rho;  if (p2 < 0) p2 = -p2;
	    if (p1 > smax) smax = p1;
	    if (p2 > smax) smax = p2;
	}
    }
    if (smax == 0) return;

    /* Repeat and scale */
    for (in = 0; in < nn; in++) {
	sxx = nd[in]->sol[ imptosol[Sxx]];
	sxy = nd[in]->sol[ imptosol[Sxy]];
	syy = nd[in]->sol[ imptosol[Syy]];
	if (sxx == 0 && syy == 0 && sxy == 0) continue;
	t1 = 0.5*(sxx+syy);
	t2 = 0.5*(sxx-syy);
	rho = sqrt(t2*t2 + sxy*sxy);
	p1 = t1 + rho;
	p2 = t1 - rho;
	ang = (t2 != 0)? 0.5*atan2(sxy,t2): 0;
	if (fabs( p1) > smax || fabs(p2) > smax) continue;
	dx = 0.5*delt*cos(ang);
	dy = 0.5*delt*sin(ang);
	cx = pt[ nd[in]->pt]->cord[0];
	cy = pt[ nd[in]->pt]->cord[1];
	xgNewGroup();
	xgPoint (cx - dx * p1/smax, cy - dy *p1/smax);
	xgPoint (cx + dx * p1/smax, cy + dy *p1/smax);
	xgNewGroup();
	xgPoint (cx - dy * p2/smax, cy + dx *p2/smax);
	xgPoint (cx + dy * p2/smax, cy - dx *p2/smax);
    }


}

/*-----------------DRAW_FLOW--------------------------------------------
 * Little arrows flowing in the stream...
 *----------------------------------------------------------------------*/
void draw_flow( float vleng, float vmax, int col)
{
    int in;
    float vel, vx, vy, dx, dy, cx, cy, delt;

    delt = vleng;

    if( imptosol[ XVEL] == -1) {
	fprintf( stderr, "That value is not avaiable for plotting\n");
	return;
    }

    xgNewSet();
    xgSetName("Flow Field");

    /* First work out max velocity */
    if (vmax == 0) {
	for (in = 0; in < nn; in++) {
	    vx = nd[ in]->sol[ imptosol[XVEL]];
	    vy = nd[ in]->sol[ imptosol[YVEL]];
	    vel = hypot( vx, vy);
	    if (vel > vmax) vmax = vel;
	}
    }
    if (vmax == 0) return;

    /* Repeat and scale */
    for (in = 0; in < nn; in++) {

	/* Silicon nodes are confusing to the viewer*/
	if(nd[ in]->mater == Si) continue;

	vx = nd[ in]->sol[ imptosol[XVEL]];
	vy = nd[ in]->sol[ imptosol[YVEL]];
	vel = hypot( vx, vy);
	if (vel > vmax) continue;
	if (vel == 0) continue;
	dx = 0.5*delt*vx/vmax;
	dy = 0.5*delt*vy/vmax;
	cx = pt[ nd[in]->pt]->cord[0];
	cy = pt[ nd[in]->pt]->cord[1];
	xgNewGroup();
	xgPoint (cx - dx , cy - dy );
	xgPoint (cx + dx , cy + dy );
	/* the arrow head */
	xgPoint( cx + dx + (dy-dx)/5, cy + dy + (-dy-dx)/5);
	xgNewGroup();
	xgPoint( cx + dx , cy + dy );
	xgPoint( cx + dx + (-dy-dx)/5, cy + dy + (-dy+dx)/5);

    }

}
