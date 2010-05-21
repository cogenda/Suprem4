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
/*   new_layer.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:21:06 */

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "impurity.h"
#ifdef CONVEX
#define hypot(a,b) sqrt((a)*(a)+(b)*(b))
#endif

#define X 0
#define Y 1
#define Z 2
#define NEUTRAL BC_OFFSET

#define vertex(t,i) tri[t]->nd[i]
#define dot(A,B) ((mode == ONED)?((A)[0]*(B)[0]):((A)[0]*(B)[0]+(A)[1]*(B)[1]))



/*-----------------NativeOxide------------------------------------------
 * Deposits oxide on bare silicon surfaces.
 * Like new_layer but different.
 *
 * Return value: whether geometry changed
 * Diagnostics:  abort on error
 *
 * notes:
 * Uses tri->nb, nd->tri
 * Insinuates oxide under nitride if nitride on bare silicon.
 * Restores all geometry when done.
 *----------------------------------------------------------------------*/
NativeOxide(thick)
    double thick;		/* That's me */
{
    int in;			/* Silicon node to deposit on */
    int addedOx=0;		/* whether anything happened */
    int ir, sr, r;
    float Normal[MAXDIM];		/* offset for new point */
    int v[MAXVRT];
    struct LLedge *bnd, *beg, *ende, *bp;
    int g;
    int tn;
    int bo, to;
    int bsp, tsp;
    int b[3];
    b[1] = b[2] = b[0] = BC_OFFSET;

    switch( mode ) {

    case ONED :
	/*find the exposed node*/
	for( in = 0; in < nn; in++) {
	    if( nd[in]->mater != Si) continue;
	    if( !exposed( in)) continue;

	    /*create an oxide node*/
	    r = mk_reg(SiO2);
	    addedOx++;
	    bo = mk_nd( nd[ in]->pt, SiO2);

	    Normal[X] = - thick;
	    bsp = SplitNode( bo, Normal, Si);
	    v[0] = bo;
	    v[1] = bsp;
	    (void)mk_ele_nd( 2, v, r);
	}
	break;

    case TWOD:
	for(ir = 0; ir < nreg; ir++) {
	    if ( (mat_reg(ir) != Si) && (mat_reg(ir) != Poly) ) continue;
	    sr = reg[ir]->sreg;

	    /*loop through all the edges in this region looking for exposed*/
	    bnd = sreg[sr]->bnd;
	    ende=NULL;
	    for(g = 1, beg = bnd; g || beg != bnd; g=0, beg = beg->next) {
		if ( ! ask(edg[beg->edge], ESURF) ) continue;

		/*find the end*/
		for(ende = beg; ask(edg[ende->edge], ESURF); ende = ende->next);

		r = mk_reg(SiO2);

		/*for every edge...*/
		for(bp = beg->prev; bp != ende; bp = bp->next) {
		    tn = nF(bp);
		    
		    /*make the oxide node here, if needed*/
		    if( (to = node_mat( tn, SiO2)) < 0)
			to = mk_nd( pt_nd(tn), SiO2);

		    (void)local_normal( tn, -2, Normal);
		    Normal[X] *= thick;
		    Normal[Y] *= thick;
		    tsp = SplitNode( to, Normal, mat_reg(ir));

		    /*make the connection to the prior node*/
		    if ( (tn != nB(beg)) || (num_nd(pt_nd((nB(bp)))) > 1) ) {
			if ( (bsp = node_mat( nB(bp), SiO2 )) < 0 )
			    bsp = mk_nd( pt_nd(nB(bp)), SiO2);
			v[0] = to; v[1] = tsp; v[2] = bsp;
			b[1] = b[2] = b[0] = BC_OFFSET;
			(void)mk_ele( 3, v, 3, b, r, FALSE);
		    }

		    if ((tn!=nB(ende)) || (num_nd(pt_nd((nF(bp->next))))>1)) {
			if ( (bsp = node_mat( nF(bp->next), SiO2 )) < 0 )
			    bsp = mk_nd( pt_nd(nF(bp->next)), SiO2);
			v[0] = to; v[1] = tsp; v[2] = bsp;
			if (is_surf(pt_nd(bsp))) b[1] = EXPOSED;
			(void)mk_ele( 3, v, 3, b, r, FALSE);
		    }
		}
		g = 0; beg = bnd->prev;
	    }
	    if ( ende != NULL ) {
		bd_connect("native oxide deposition");
		ir = -1;
	    }
	}
	break;
    }
    bd_connect("native oxide deposition");
}



/*-----------------NEW_LAYER--------------------------------------------
 * Inserts a thin new layer of oxide at the Si/SiO2 interface.
 *
 * Return value: whether geometry changed
 * Diagnostics:  abort on error
 *
 *----------------------------------------------------------------------*/
new_layer(Msil, Mox, oxide_grid, dt )
    int Msil, Mox;		/* Layer below, above interface */
    double oxide_grid;
    double dt;
{
    int io;			/* Oxide node being split */
    int i;
    int p;
    int e, ei, noth;
    int eperp;
    double fig;
    double magv;
    double evec[MAXDIM];
    float c[MAXDIM];
    double dis;
    int did_something = FALSE;

    /*
     * Geometry pass:
     * Loop through nodes on the oxide/silicon interface and see if their
     * local spacing is growing too large.
     */
    for( io = 0; io < nn; io++) {
	/*make sure we are at the oxide / silicon interface*/
	if( nd[ io]->mater != Mox) continue;
	if( node_mat( io, Msil) < 0) continue;
	p = pt_nd(io);
	magv = sqrt( dot(vel_arr(p), vel_arr(p)) );
	if ( magv == 0.0 ) continue;

	/*find the edge that is the most perpindicular to the velocity*/
	fig = 0.0;
	eperp = -1;
	for(i = 0; i < num_edge_nd(io); i++) {
	    e = edge_nd(io,i);
	    if ( nd_edg(e,0) == io )
		ei = 1;
	    else
		ei = 0;
	    noth = nd_edg(e,ei);
	    evec[0] = xcord(p) - xcord(pt_nd(noth));
	    evec[1] = ycord(p) - ycord(pt_nd(noth));

	    dis = dot( evec, vel_arr(p) ) / (magv * len_edg(e));

	    if ( dis > fig ) {
		fig = dis;
		eperp = e;
	    }
	}

	if ( (fig * len_edg(eperp) >= oxide_grid) &&
	     (dt * magv > 20.0e-8) ) {
	    /*split this edge*/
	    if ( nd_edg(eperp,0) == io )
		ei = 1;
	    else
		ei = 0;
	    noth = nd_edg(eperp,ei);
	    evec[0] = xcord(pt_nd(noth)) - xcord(p);
	    evec[1] = ycord(pt_nd(noth)) - ycord(p);
	    c[0] = 5.0e-8 * evec[0] / len_edg(eperp) + xcord(p);
	    c[1] = 5.0e-8 * evec[1] / len_edg(eperp) + ycord(p);
	    sp_edge(eperp, c, &p, FALSE);
	    did_something=TRUE;
	}
    }
    return(did_something);
}
	



/*-----------------SplitNode-------------------------------------------
 * This routine splits a boundary point in two, separated by a given delta.
 * The area between the two points is to be later filled with triangles
 * of material Mox, so a new node Mox is also created.
 * All the old nodes, bar the Msil one, are attached to the new point.
 * The new Mox one is attached to the old point.
 * This is so that the triangles of Mox do not have to be reconnected.
 * The Msil nodes and triangles are unaffected by the proceedings.
 * A side effect is that oxide can sneak in between nitride and silicon.
 * The return value is the new node created.
 *----------------------------------------------------------------------*/
SplitNode( aNode, v, mSil)
    int aNode;			/* Point being split as seen by mOx */
    float *v;			/* Displacement for new point */
    int mSil;			/* The other side of the interface */
{
    int i, OldPt, NewPt, *in, *inn, NewNd;
    float loc[MAXDIM];

    OldPt = nd[ aNode]->pt;
    loc[X] = pt[ OldPt]->cord[X] + v[X];
    loc[Y] = pt[ OldPt]->cord[Y] + v[Y];
    loc[Z] = pt[ OldPt]->cord[Z] + v[Z];

	/*
	 * Make a new point at the specified location
	 */
	NewPt = mk_pt( 3, loc );

	/*
	 * Transfer flags
	 */
	pt[ NewPt]->flags = pt[ OldPt]->flags;
	if( pt[ OldPt]->flags & SURFACE)
	    pt[ OldPt]->flags &= ~SURFACE;

	/*
	 * Attach old nodes to new point, except for the Msil one
	 * which moves down to the zeroth position at the old point.
	 */ 
	for (in = pt[ OldPt]->nd, inn = in + pt[ OldPt]->nn; in < inn; in++)
	    if (nd[ *in]->mater == mSil) pt[ OldPt]->nd[0] = *in;
	    else {
		pt[ NewPt]->nd[ pt[ NewPt]->nn++] = *in;
		nd[ *in]->pt = NewPt;
	    }
	pt[ OldPt]->nn = 1;

	/*
	 * Create the new oxide node and copy solution values
	 */
	NewNd = mk_nd( OldPt, nd[ aNode]->mater);
    
	for (i = 0; i < MAXIMP; i++)
	    nd[ NewNd]->sol[ i] = nd[ aNode ]->sol[i];

    return( NewNd);
}

/*-----------------EXPOSED----------------------------------------------
 * Tell whether a node is on an exposed edge.
 * Algorithm: look at edges by running through n->tri
 * You might think this routine unnecessary, but remember that backside
 * nodes have gas nodes and pt->flags& SURFACE set as well.
 *----------------------------------------------------------------------*/
exposed(in)
    int in;			/* node number */
{
    int ix, ixx, jin, j, nbr;
    
    if( node_mat( in, GAS) < 0) return(0);
    
    for( ixx = 0; ixx < num_tri_nd(in); ixx++) {
	ix = tri_nd(in, ixx);
	jin = nwhich( ix, in);
	for (j = jin; (j = (j+1)%3) != jin; ) {
	    nbr = tri[ ix]->nb[ j];
	    if (nbr == EXPOSED)
		return(1);
	}
    }
    return( 0);
}
	


/*-----------------SPLIT_THICK------------------------------------------
 * A heuristic function to decide when oxide nodes need splitting
 * Returns suitable displacement of node, 0 if none.
 * Diagnostics: return 0 anyway - can't hurt to not add stuff
 *----------------------------------------------------------------------*/
extern float oxide_grid;

split_thick( n, Mox, Msil, vdisp)
    int n;
    int Mox, Msil;
    float vdisp[MAXDIM];
{
    float SilToOx[MAXDIM], OxToSil[MAXDIM], dox, rough_perp();

    /* Compute a local normal */
    if (local_normal( n, Msil, OxToSil)) return(0);
    SilToOx[ X] = -OxToSil[ X];
    SilToOx[ Y] = -OxToSil[ Y];
    vdisp[ X] = 5e-8 * SilToOx[ X];
    vdisp[ Y] = 5e-8 * SilToOx[ Y];

    /* Estimate the oxde grid spacing on both sides */
    dox  = rough_perp( nd[n]->pt, Mox, SilToOx);	if (dox < 0) return(0);

    /* Provide a little hysteresis by adding grid anywhere between 0.25 and 1*/
    /* Danger: don't want slightly less thick oxides triggering just after field oxide */
    if (dox <= 0.25*oxide_grid)
	return(-1);
    else if (dox <= oxide_grid)
	return( 0);
    else
	return (1);
}

/*-----------------ROUGH_PERP-------------------------------------------
 * Get a rough idea of the spacing in some direction at a node.
 * Algorithm: Try intersecting the direction with the node's poly.
 * 	      Use closest node to direction if there is no intersection.
 * Return value: spacing value, scaled by length of direction vector.
 * Diagnostics:  return -1 for bad input or directions way out of the material.
 *----------------------------------------------------------------------*/
float rough_perp( p, matl, dir)
    int p;		/* Point */
    int matl;		/* + material = node */
    float dir[MAXDIM];	/* direction */
{
    int n, it, t, j, k, ot, o1, o2;
    float p_ot[MAXDIM], o1_o2[MAXDIM], dlen, dlen2, olen2, cos2, cosd, xn[MAXDIM];
    float dinter = 0, proj;

    /* Find the matl node at p */
    n = node_mat( pt[ p]->nd[0], matl);
    if (n == -1) return(-1);

    dlen = sqrt( dlen2 = dot( dir, dir));
    if (dlen == 0) return(-1);

    /* For each triangle containing that node */
    for (it = 0; it < num_tri_nd( n); it++) {
	t = tri_nd( n, it);

	/* Get the position of this node */
	j = nwhich( t, n);

	switch( mode ) {
	case ONED :
	    o1 = nd[ tri[ t]->nd[ (j+1)%nvrt]]->pt;
	    o1_o2[ X] = pt[ o1]->cord[ X] - pt[ p]->cord[X];

	    if ( o1_o2[X] * dir[X] > 0.0 ) {
		dinter = fabs(o1_o2[X]);
	    }
	    break;

	case TWOD :

	    o1 = nd[ tri[ t]->nd[ (j+1)%3]]->pt;
	    o2 = nd[ tri[ t]->nd[ (j+2)%3]]->pt;

	    /* The vector along that side */
	    o1_o2[ X] = pt[ o2]->cord[ X] - pt[ o1]->cord[X];
	    o1_o2[ Y] = pt[ o2]->cord[ Y] - pt[ o1]->cord[Y];

	    /* No point in looking for an intersection if the
	       side is parallel to the direction vector. */
	    olen2 = dot( o1_o2, o1_o2);
	    cosd  = dot( dir,   o1_o2);
	    cos2  = cosd*cosd/(dlen2*olen2);
	    if (cos2 > 0.9999) continue;

	    /* Compute the intersection - shouldn't fail.*/
	    assert(!lil (pt[ p]->cord, dir, pt[ o1]->cord, o1_o2, xn));

	    /* If the intersection is in the segment */
	    if (xn[1] > -0.01 && xn[1] < 1.01)
		/* and it's in the positive direction */
		if (xn[0] > 0)
		    /* save it */
		    dinter = xn[0];
	    break;
	}
    }

    /* If we got an intersection, we're outta of here */
    if (dinter > 0)
	return( dinter);

    /* No intersection. The point must be on the boundary and the
       direction vector points out of the material. What does the
       local spacing in that direction mean? Not much unless the
       direction vector is fairly close to the interface direction.
       Then we would say the distance to the point along the boundary
       closest to the direction is some measure of the spacing.
       So try the maximum projection of each nbr point on the direction*/

    /* Each neighbor point */
    for (it = 0; it < num_tri_nd( n); it++) {
	t = tri_nd( n, it);
	j = nwhich( t, n);
	for (k = 1; k <= 2; k++) {
    	    ot = nd[ tri[ t]->nd[ (j+k)%3]]->pt;
	    
	    /* The vector from p to other */
	    p_ot[ X] = pt[ ot]->cord[ X] - pt[ p]->cord[X];
	    p_ot[ Y] = pt[ ot]->cord[ Y] - pt[ p]->cord[Y];

	    /* Project it on the direction if it's reasonably close */
	    olen2 = dot( p_ot, p_ot);
	    cosd = dot( p_ot, dir);
	    if (cosd > 0) {
		cos2 = cosd*cosd / (dlen2*olen2);
		if (cos2 > 0.75) {
		    proj = cosd / dlen2;
		    if (proj > dinter) dinter = proj;
		}
	    }
	}
    }

    /* Hopefully we got a good projection (normalized above) */
    if (dinter > 0) return( dinter);

    /* I quit */
    else {
	fprintf( stderr, "Warning: unknown spacing for point %d[%d] in direction (%e, %e)\n",
		p, matl, dir[0], dir[1]);
	return(-1);
    }
}



/*-----------------LOCAL_NORMAL-----------------------------------------
 * Get some sort of idea of the interface normal at a node.
 * Given matl > 0, uses only edges facing that material
 * Given matl == GAS (or -1 for backward compatibility), only exposed edges
 * Given matl == -2 any edge that isn't reflecting
 * Algorithm: bisects the angle, by averaging the normalized
 * surface vectors.  Not prestissimo but it works.
 * Returns 0 for success, 1 for no normal (probably bad interface)
 * Also has an option to boost corner normals to help grid smoothness.
 *----------------------------------------------------------------------*/
local_normal (n, matl, ln)
    int n;		/* Node we care about */
    int matl;		/* Facing material, can be GAS */
    float ln[MAXDIM];	/* Returned vector */
{
    int it, t, j, nbr, head, tail, is_refl=0, p1, p2, v, nvec=0;
    float arrow[MAXDIM], tmp, lln, len, refl[MAXDIM], cosf, save[MAXDIM];

    switch( mode ) {
    case ONED :
	ln[0] = 1.0;
	ln[1] = 0.0;
	return(0);
	break;

    case TWOD:
	ln[ 0] = ln[ 1] = 0;
	cosf = 1.0;

	/* First figure if the node is on a reflecting boundary */
	/* Look at the edges that involve this node */
	for( it = 0; it < num_tri_nd( n); it++) {
	    t = tri_nd( n, it);
	    v = nwhich( t, n);
	    for( j = v; (j = (j+1)%3) != v; )
		if( tri[ t]->nb[ j] == NEUTRAL) {
		    /*Store the perpendicular to the boundary */
		    /*If there is more than one, well they should be parallel */
		    is_refl++;
		    p1 = nd[ n]->pt;
		    p2 = nd[ vertex( t, 3-j-v)]->pt;
		    refl[X] = pt[ p1]->cord[Y] - pt[ p2]->cord[Y];
		    refl[Y] = pt[ p2]->cord[X] - pt[ p1]->cord[X];
	    }
	}
	    
	/* Go thru triangle sides at the node */
	for( it = 0; it < num_tri_nd( n); it++) {
	    t = tri_nd( n, it);
	    v = nwhich( t, n);
	    for( j = v; (j = (j+1)%3) != v; ) {

		/* Only want interface sides */
		nbr = tri[ t]->nb[ j];
		if (matl > GAS){
		    if (nbr < 0 || mat_reg(reg_tri(nbr)) != matl) continue;}
		else if (matl == GAS || matl == -1) {
		    /*Not just any negative neighbor, would get 45o at corners*/
		    if (nbr != EXPOSED) continue;
		}
		else if (matl == -2) {
		    if (nbr == NEUTRAL ||
			(nbr >= 0 && mat_reg(reg_tri(nbr)) == nd[n]->mater))
			continue;
		}


		/* Get the vector along the interface */
		head = nd[ tri[ t]->nd[ (j+1)%3]]->pt;
		tail = nd[ tri[ t]->nd[ (j+2)%3]]->pt;
		arrow[ 0] = pt[ head]->cord[ 0] - pt[ tail]->cord[ 0];
		arrow[ 1] = pt[ head]->cord[ 1] - pt[ tail]->cord[ 1];

		/* Rotate it 90 degrees, INTO material matl */
		tmp = arrow[ 0];
		arrow[ 0] = -arrow[ 1];
		arrow[ 1] =  tmp;

		/* Normalize and add it into ln */
		len = hypot( arrow[0], arrow[1]);
		arrow[0] /= len; arrow[1] /= len;
		ln[ 0] += arrow[ 0];
		ln[ 1] += arrow[ 1];

		/* Also keep a copy for the "cos factor" */
		if( norm_style != 1) continue;
		++nvec;
		if( nvec == 1) {
		    save[0] = arrow[0];
		    save[1] = arrow[1];
		} else if (nvec == 2) {
		    cosf = fabs( dot( save, arrow)); /* just normalized 'em */
		    assert( cosf <= 1.001);
		} else {
		    fprintf( stderr, "local_normal: more than two edges at a node?\n");
		}
	    }
	}

	/*Now subtract out the component perpendicular to the reflection axis*/
	if( is_refl) {
	    cosf = 1;
	    tmp = dot(ln, refl) / dot( refl, refl);
	    ln[X] -= tmp*refl[X];
	    ln[Y] -= tmp*refl[Y];
	}

	/* Normalize */
	lln = (float) hypot( (double) ln[ 0], (double) ln[ 1]);
	if (lln != 0) {
	    ln[ 0] /= lln;  ln[ 1] /= lln;
	    if( norm_style == 1) {
		cosf = sqrt( 0.5* (1 + cosf ));
		ln[ 0] /= cosf; ln[ 1] /= cosf;
	    }
	    return( 0);
	}
	else
	    return(1);
    }
    return(1);
}

/* Enough already */
dlocal_normal (n, matl, dln)
    int n; int matl; double dln[2];
{
    int oops;
    float ln[MAXDIM];
    if( !(oops = local_normal( n, matl, ln))) {
	dln[0] = ln[0];
	dln[1] = ln[1];
    }
    return( oops);
}
	




test_ln(i,m)
    int i, m;
{
    float ln[MAXDIM];
    (void)local_normal(i,m,ln);
    printf("local normal at node %d is [%f, %f]\n", i, ln[0], ln[1]);
}
