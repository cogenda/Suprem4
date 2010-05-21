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
/*   geometry.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:49 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"	/* For nmat, mattyp, SiO2 etc */
#include "matrix.h"	/* For UpdateSymbolic */

#define report(S,A,B,C) {if (!err_cnt++)\
		     fprintf(stderr, "Mesh failed self test %s:\n", when);\
					     fprintf(stderr, S, A, B, C);}


/************************************************************************
 *									*
 *	geom() - Holding Function for geometry calculations.		*
 *									*
 ************************************************************************/
geom(when)
char *when;
{
    int t, e;

    for(t = 0; t < ne; t++) {
	if ( ask(tri[t], CHPFIX) ) {
	    clr( tri[t], CHPFIX );
	    clr( tri[t], GEOMDN );
	}
	for(e = 0; e < num_edge(t); e++) clr( edg[edg_ele(t,e)], GEOMDN );
    }

    if (flip()) UpdateSymbolic++;
    while( done_tri(t) ) {
	do_geom(t);
	next_tri(t);
    }

    /*sum the coefficients into the edge array*/
    sum_edge();

    repair_obtuse();

    /*write out some data for the user*/
    if (verbose >= V_NORMAL) {
	if ( when != NULL ) printf("Mesh statistics %s:\n", when);
	printf("    Points = %4d\t", np);
	printf("Nodes = %4d\t\n", nn);
    }
}




/************************************************************************
 *									*
 *	sum_edge() - This routine computes the length of an edge, and	*
 *  the coupling coefficient of each edge.				*
 *									*
 ************************************************************************/
sum_edge()
{
    int e;

    for(e = 0; e < ned; e++) {
#ifdef FOO
	if ( ! ask(edg[e], GEOMDN ) ) {
	    set(edg[e], GEOMDN);
#endif
	    edg[e]->cpl = gimme_ehed( e );
	    edg[e]->len = dist( cord_arr(pt_edg(e,0)), 
				cord_arr(pt_edg(e,1)) );
#ifdef FOO
	}
#endif
    }
}


/*-----------------GimmeEhed--------------------------------------------
 * Calculate the coupling coefficient contribution from one side of
 * a triangle.
 *----------------------------------------------------------------------*/
float gimme_ehed( it )
int it;
{
    int t, e;
    float l, ehed_tri();

    /*for all elements connected to this edge*/
    l = 0.0;
    for(t = 0; t < num_tri_edg(it); t++) {
	e = ewhich(tri_edg(it,t), it);
	l += ehed_tri(tri_edg(it,t), e);
    }
    return(l);
}




/************************************************************************
 *									*
 *	This routine computes the ehed of a given triangle edge.	*
 *									*
 *      Original     	Mark E. Law		2/90			*
 *									*
 ************************************************************************/
float ehed_tri(t, e)
int t, e;
{
#ifdef FOO
    if (!ask(tri[t],GEOMDN)) do_geom(t);
#else
    if (!ask(tri[t],CHPFIX)) do_geom(t);
#endif

    return( tri[t]->ehed[e] );
}




/************************************************************************
 *									*
 *	This routine computes the geometry for a given specific element *
 *									*
 *      Original     	Mark E. Law		2/90			*
 *									*
 ************************************************************************/
do_geom(t)
int t;
{
    set(tri[t], GEOMDN);
    switch(mode) {
    case ONED :
	seg_geom(t);
	break;
    case TWOD :
	tri_geom(t);
	break;
    }

}





/************************************************************************
 *									*
 *	tri_geom() - This routine calculates the coupling parameters 	*
 *  for the passed triangle.						*
 *									*
 *      Original     	C. Price		8/04/80 		*
 *                      Mark R. Pinto		04/84			*
 *			Mark E. Law		10/84	(C version)	*
 *			Mark E. Law		11/84   (d value store)	*
 *									*
 ************************************************************************/
tri_geom(ie)
int ie;			/*the triangle number*/
{
    int i,j,k;
    float xij,yij,xjk,yjk,xki,yki;	/*separation terms*/
    float xi,yi,xj,yj,xk,yk;		/*coordinate terms*/
    float disq,djsq,dksq,di,dj,dk;	/*distances*/
    float ai,aj,ak;			/*area terms*/
    float s,s4,r,rsq,den;	/*temps and misc.*/

    /*assume the son of gun is clockwise*/
    /*get the point numbers*/
    i = vert_tri(ie,0);
    j = vert_tri(ie,1);
    k = vert_tri(ie,2);

    /*get the coordinate terms*/
    xi = pt[ pt_nd(i) ]->cord[0];	yi = pt[ pt_nd(i) ]->cord[1];
    xj = pt[ pt_nd(j) ]->cord[0];	yj = pt[ pt_nd(j) ]->cord[1];
    xk = pt[ pt_nd(k) ]->cord[0];	yk = pt[ pt_nd(k) ]->cord[1];

    /*get the separation terms*/
    xij = xi - xj; 			yij = yi - yj;
    xjk = xj - xk; 			yjk = yj - yk;
    xki = xk - xi; 			yki = yk - yi;

    /*calculate the denominator for the coupling*/
    den = xki * yjk - yki * xjk;
    if (den == 0) {
	char buf[120];
	sprintf(buf, "triangle %d is flat\n", ie);
	panic(buf);
    }

    /*triangle area is one half of this value*/
    s = -0.5 * den;

    /*if area is less than 0, reorder the points and repeat*/
    if (s < 0.0) {
	    panic("triangles are not clock wise, data base corrupted");
    } 

    /*we want one half of the cotangent*/
    den = 0.5 / den;

    /*calculate those coupling coeeficients*/
    tri[ie]->ehed[0] = (xki * xij + yki * yij) * den;
    tri[ie]->ehed[1] = (xij * xjk + yij * yjk) * den;
    tri[ie]->ehed[2] = (xjk * xki + yjk * yki) * den;

    /*compute the area weighting*/
    disq = xjk * xjk + yjk * yjk;
    djsq = xki * xki + yki * yki;
    dksq = xij * xij + yij * yij;
    di = sqrt(disq);
    dj = sqrt(djsq);
    dk = sqrt(dksq);

    /*store away the half the side distances in the triangle array*/
    tri[ie]->d[0] = di * 0.5;
    tri[ie]->d[1] = dj * 0.5;
    tri[ie]->d[2] = dk * 0.5;

    if ((di == 0.0) || (dj == 0.0) || (dk == 0.0)) 
	panic("zero length side in triangle %d\n");

    /*calculate area, radius for circle and tangents*/
    s4 = s * 4.0;
    r = di * dj * dk / s4;
    rsq = r * r;

    /*calculate area assuming the acute angle case from the original*/
    ai = 0.250 * di * sqrt(fabs(rsq - 0.25 * disq));
    aj = 0.250 * dj * sqrt(fabs(rsq - 0.25 * djsq));
    ak = 0.250 * dk * sqrt(fabs(rsq - 0.25 * dksq));
    tri[ie]->earea[0] = aj + ak;
    tri[ie]->earea[1] = ak + ai;
    tri[ie]->earea[2] = ai + aj;
}



/************************************************************************
 *									*
 *	geom1d() - This routine calculates the geometry properties	*
 *  for the new 1d elements.						*
 *									*
 *      Original     	Mark E. Law		2/90			*
 *									*
 ************************************************************************/
seg_geom( ie )
{
    register int i, j;	/*the node numbers*/
    float xi, xj, xij;	/*the coordinate info*/
    float d, di;	/*the length of side*/


    /*get the node numbers*/
    i = tri[ie]->nd[0];
    j = tri[ie]->nd[1];

    /*get the coordinate terms*/
    xi = pt[ nd[i]->pt ]->cord[0];
    xj = pt[ nd[j]->pt ]->cord[0];

    xij = xi - xj;

    /*if area is less than 0, reorder the points and repeat*/
    if (xij < 0.0) {
	panic("triangles are not clock wise, data base corrupted");
    }

    /*calculate the side length*/
    d = xi - xj;
    di = 1.0 / d;

    /*calculate those coupling coeeficients*/
    tri[ie]->ehed[0] = di;
    tri[ie]->ehed[1] = di;

    /*set a uniform side length scaling*/
    tri[ie]->d[0] = 1.0;
    tri[ie]->d[1] = 1.0;

    tri[ie]->earea[0] = 0.5 * d;
    tri[ie]->earea[1] = 0.5 * d;

}




/************************************************************************
 *									*
 *	repair_obtuse() - this routine applies the Price fix to	broken	*
 *  triangles.								*
 *									*
 *      Original     	C. Price		8/04/80 		*
 *                      Mark R. Pinto		04/84			*
 *			Mark E. Law		10/84	(C version)	*
 *			Mark E. Law		11/84   (d value store)	*
 *									*
 ************************************************************************/
repair_obtuse ()
{
    int e, nt, it;

    /*scan the edges for those that have negative coupling*/
    for(e = 0; e < ned; e++) {
	if ( cpl_edg(e) < 0.0 ) {
	    /*fixing the sucker*/
	    for(it = 0; it < num_tri_edg(e); it++) {
		nt = tri_edg(e,it);
		if ( tri[ nt]->ehed[ ewhich(nt,e) ] < 0 ) {
		    set( tri[ nt ], CHPFIX );
		    chp_fix( nt );
		}
	    }
	    edg[e]->cpl = gimme_ehed( e );
	}
    }
}




/************************************************************************
 *									*
 *	chp_fix -	Apply CHP's fix to irreparably obtuse triangles	*
 *									*
 ************************************************************************/
#include <assert.h>
chp_fix( obe )
int obe;
{
    int i,j,k;
    float xij,yij,xjk,yjk,xki,yki;	/*separation terms*/
    float xi,yi,xj,yj,xk,yk;		/*coordinate terms*/
    float disq,djsq,dksq,di,dj,dk;	/*distances*/
    float s,s4,den;	/*temps and misc.*/
    float tan[3];

    /*get the point numbers*/
    i = tri[obe]->nd[0];
    j = tri[obe]->nd[1];
    k = tri[obe]->nd[2];

    /*get the coordinate terms*/
    xi = pt[ nd[i]->pt ]->cord[0];	yi = pt[ nd[i]->pt ]->cord[1];
    xj = pt[ nd[j]->pt ]->cord[0];	yj = pt[ nd[j]->pt ]->cord[1];
    xk = pt[ nd[k]->pt ]->cord[0];	yk = pt[ nd[k]->pt ]->cord[1];

    /*get the separation terms*/
    xij = xi - xj; 			yij = yi - yj;
    xjk = xj - xk; 			yjk = yj - yk;
    xki = xk - xi; 			yki = yk - yi;

    /*calculate the denominator for the coupling*/
    den = xki * yjk - yki * xjk;

    /*triangle area is one half of this value*/
    s = -0.5 * den;

    /*we want one half of the cotangent*/
    den = 0.5 / den;

    /*compute the area weighting*/
    disq = xjk * xjk + yjk * yjk;
    djsq = xki * xki + yki * yki;
    dksq = xij * xij + yij * yij;
    di = sqrt(disq);
    dj = sqrt(djsq);
    dk = sqrt(dksq);

    /*store away the half the side distances in the triangle array*/
    tri[obe]->d[0] = di * 0.5;
    tri[obe]->d[1] = dj * 0.5;
    tri[obe]->d[2] = dk * 0.5;

    /*calculate area, radius for circle and tangents*/
    s4 = s * 4.0;

    /*Compute the local tangents*/
    tan[0] = djsq + dksq - disq;
    tan[1] = dksq + disq - djsq;
    tan[2] = disq + djsq - dksq;

    /*figure out which side is the bad one*/
    if      (tri[obe]->ehed[0] <= 0) i = 0; 
    else if (tri[obe]->ehed[1] <= 0) i = 1; 
    else if (tri[obe]->ehed[2] <= 0) i = 2;
    else panic("redo_geom: bad arguments");
    j = (i+1)%3; k = (i+2)%3;

    tan[j] = s4 / tan[j];
    tan[k] = s4 / tan[k];

    di = 2.0 * tri[obe]->d[i];
    dj = 2.0 * tri[obe]->d[j];
    dk = 2.0 * tri[obe]->d[k];

    /*do the CHP fix for side lengths and areas*/
    tri[ obe] ->ehed[ i] = 0;
    tri[ obe] ->ehed[ j] = 0.5 * tan[k];
    tri[ obe] ->ehed[ k] = 0.5 * tan[j];
    
    tri[ obe] ->earea[ j] = 0.125 * dk * tan[j] * dk;
    tri[ obe] ->earea[ k] = 0.125 * dj * tan[k] * dj;
    tri[ obe] ->earea[ i] = s - tri[obe]->earea[j] - tri[obe]->earea[k];
}
