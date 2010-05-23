/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   elast.c                Version 3.6     */
/*   Last Modification : 9/18/88  13:29:27 */

/*-----------------ELAST_GROWTH-----------------------------------------*
 * An attempt at an elastic oxide growth model
 * Original: CSR Apr 86
 * This effort has no justification whatsoever except raw speed.
 * Elastic is a misnomer = this is a viscous flow model without incompressiblity
 *----------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "material.h"


#define IAJA(R,C) iaja(ia, aoff, R, C)
#define X 0
#define Y 1

float elstif[ 6][ 6];

elast_growth( temp, dt)
    float temp;
    double dt;
{
    int n, sXVEL, sYVEL, i, j, ie, v, w, s, t, get_elasconn();
    int col, row, lcol, lrow, nat_p, where, *fix, *nx, *nx1, p;
    int *ia=0, *il, aoff, loff;
    struct tms before, after;
    double *a, *l, *rhs;
    double mult;
    float ln[ 2];		/* Velocity at a point */

    /* Get symbolic arrays */
    times( &before);

    /* Count # of real variables */
    for (j = 0, i = 0; i < nn; i++) if (nd[i]->mater != Si) j++;

    /*
     * Allocate an initialize A map.
     * 2n variables, about 6*2 neighbors each, symmetry, 6=>7 for slop
     */
    aoff = (2*np) + (2*j)*7;
    ia = salloc( int, aoff);
    if( generate( np, 2, 0, get_elasconn, &ia, &aoff)) {
	fprintf( stderr, "Insufficient core for oxide flow solution\n");
	return;
    }
    a  = scalloc( double, 2*np + aoff+2);


    /* smart new Ssymfac reallocs if necessary */
    loff = 2*np + 20*aoff;
    il = scalloc( int, loff);
    assert( !symfac( 2*np, (int *)0, 1, ia, aoff, &il, &loff));

    l  = scalloc( double, 2*np+loff+2);
    rhs = scalloc( double, 2*np);
    aoff = loff = 0;

    times(&after);
    print_time("Compressible matrix symbolism", &before, &after);

    times(&before);

    /* Set up boundary conditions: */
    fix = scalloc( int, 2*np);

    /* Dummy up silicon nodes - a speed hack*/
    for (n = 0; n < nn; n++) {
	if( nd[ n]->mater == Si) {
	    p = nd[ n]->pt;
	    a[ 2*p + X] = 1;	fix[ 2*p + X] = 1;
	    a[ 2*p + Y] = 1;	fix[ 2*p + Y] = 1;
	}
    }

    /* Interface displacements */
    sXVEL = imptosol[ XVEL];
    sYVEL = imptosol[ YVEL];
    for (n = 0; n < nn; n++) {

	/* Find oxide nodes with underlying Si node */
	if ((nd[ n]->mater == SiO2) &&
	    ((node_mat (n, Si)) >= 0)) {

	    /* Compute the normal velocity here */
	    Onvel( n, ln);

	    /* Store away in right hand side */
	    p = nd[ n]->pt;
	    rhs[2*p + X] = - (1 - 1/alpha[SiO2][Si]) * ln[ X];
	    rhs[2*p + Y] = - (1 - 1/alpha[SiO2][Si]) * ln[ Y];

	    /* Mark these displacements as known */
	    fix[ 2*p +X] = 1;
	    fix[ 2*p +Y] = 1;
	    a[   2*p +X] = 1;
	    a[   2*p +Y] = 1;
	}
    }

    /* Also XVEL is zero on natural boundaries */
    for (ie = 0; ie < ne; ie++) {
	for (s = 0; s < 3; s++) {
	    if (tri[ ie]->nb[ s] != BC_OFFSET) continue;

	    for (j = 1; j <= 2; j++) {
		nat_p = nd[ tri[ ie]->nd[ (s+j)%3]]->pt;
		fix[ 2*nat_p +X] = 1;
		a  [ 2*nat_p +X] = 1;
		rhs[ 2*nat_p +X] = 0;
	    }
	}
    }

    /* Set up global stiffness matrix */
    for (ie = 0; ie < ne; ie++) {

	if( mat_reg(reg_tri(ie)) == Si) continue;

	elem_stiff( ie);

	for (v = 0; v < 3; v++)
	    for (s = X; s <= Y; s++) {
		lrow = 2*v + s;
		row = 2*nd[ tri[ ie]->nd[v]]->pt + s;

		/* Skip boundary rows */
		if (fix[ row]) continue;

		for (w = 0; w < 3; w++)
		    for (t = X; t <= Y; t++) {
			lcol = 2*w + t;
			col = 2*nd[ tri[ ie]->nd[ w]]->pt + t;

			/* Boundary columns end up in rhs */
			if( fix[ col]) {
			    rhs[ row] -= elstif[ lrow][ lcol] * rhs[ col];
			    continue;
			}

			/* Symmetry */
			if (lcol > lrow) continue;

			where = IAJA( row, col);
			a[ where] += elstif[ lrow][ lcol];
		    }
	    }
    }

    times(&after);
    print_time("Compressible matrix assembly", &before, &after);

    times(&before);

    /* Go solve the problem */
    numfac( 2*np, (int*)0, 1, ia, aoff, a, il, loff, l);
    numbac( 2*np, il, loff, l, rhs);

    times(&after);
    print_time("Compressible matrix solution", &before, &after);


    /* Transfer the answer into permanent storage */
    for (p = 0; p < np; p++) {
	for (nx = pt[ p]->nd, nx1 = nx + pt[ p]->nn; nx < nx1; nx++) {
	    mult = (nd[ *nx]->mater != Si)? 1.0 : 1/(1-alpha[SiO2][Si]);
	    nd[ *nx]->sol[ sXVEL] = rhs[ 2*p +X] *mult;
	    nd[ *nx]->sol[ sYVEL] = rhs[ 2*p +Y] *mult;
	}
    }
    free(il); free(ia); free(rhs); free(l); free(a); free(fix);

    /* shut the door when you leave */
    return;
}

/*-----------------ELEM_STIFF-------------------------------------------*
 * Compute stiffness matrix for a constant strain triangle.
 * A Practical Intro to FEM p. 38 (Y.K. Cheung & M.F. Yeo)
 * CSR Apr 86
 *----------------------------------------------------------------------*/
elem_stiff( ie)
    int ie;	/* Triangle number */
{
    int nic1, nic2, nic3, mat, i, j;
    float x1, y1, x2, y2, x3, y3, area, C12, C1, C2;
    float bi, ci, bj, cj, bm, cm;
    float Ym, Pr;

    /* Initialize */
    for (i = 0; i < 6; i++)
	for (j = 0; j < 6; j++)
	    elstif[ i][ j] = 0;

    /* What material are we dealing with ? */
    mat = mat_reg( reg_tri(ie));

    /* For now, ignore silicon */
    if (mat == Si) {
	for (i = 0; i < 6; i++)
	    elstif[ i][ i] = 1.0;
	return;
    }
    Ym = E[ mat];
    Pr = pr[ mat];

    /* Stiffness coeffts */
    C1 = Ym * (1 - Pr) / ((1 + Pr)*(1 - 2*Pr));
    C2 = Pr / (1 - Pr);
    C12 = C1*(1-C2)/2;

    /* Get element geometry */
    nic1 = nd[ tri[ ie]->nd[ 0]]->pt;
    nic2 = nd[ tri[ ie]->nd[ 1]]->pt;
    nic3 = nd[ tri[ ie]->nd[ 2]]->pt;
    x1 = pt[nic1]->cord[0];
    y1 = pt[nic1]->cord[1];
    x2 = pt[nic2]->cord[0];
    y2 = pt[nic2]->cord[1];
    x3 = pt[nic3]->cord[0];
    y3 = pt[nic3]->cord[1];

    area = (x2*y3 - x3*y2 - x1*(y3-y2) + y1*(x3-x2))/2;

    bi = y2-y3;
    ci = x3-x2;
    bj = y3-y1;
    cj = x1-x3;
    bm = y1-y2;
    cm = x2-x1;

    elstif[0][0] = C1*bi*bi + C12*ci*ci;
    elstif[1][0] = (C1*C2 + C12)*bi*ci;
    elstif[1][1] = C1*ci*ci + C12*bi*bi;
    elstif[2][0] = C1*bi*bj + C12*ci*cj;
    elstif[2][1] = C1*C2*bj*ci + C12*bi*cj;
    elstif[2][2] = C1*bj*bj + C12*cj*cj;
    elstif[3][0] = C1*C2*bi*cj + C12*bj*ci;
    elstif[3][1] = C1*ci*cj + C12*bi*bj;
    elstif[3][2] = (C1*C2 + C12)*bj*cj;
    elstif[3][3] = C1*cj*cj + C12*bj*bj;
    elstif[4][0] = C1*bi*bm + C12*ci*cm;
    elstif[4][1] = C1*C2*bm*ci + C12*bi*cm;
    elstif[4][2] = C1*bj*bm + C12*cj*cm;
    elstif[4][3] = C1*C2*bm*cj + C12*bj*cm;
    elstif[4][4] = C1*bm*bm + C12*cm*cm;
    elstif[5][0] = C1*C2*bi*cm + C12*bm*ci;
    elstif[5][1] = C1*ci*cm + C12*bi*bm;
    elstif[5][2] = C1*C2*bj*cm + C12*bm*cj;
    elstif[5][3] = C1*cj*cm + C12*bj*bm;
    elstif[5][4] = (C1*C2 + C12)*bm*cm;
    elstif[5][5] = C1*cm*cm + C12*bm*bm;

    for (i = 0; i < 5; i++)
	for (j = i+1; j < 6; j++)
	    elstif[ i][ j] = elstif[ j][ i];

    for (i = 0; i < 6; i++)
	for (j = 0; j < 6; j++)
	    elstif[ i][ j] *= 1/(4*area);

}

/*-----------------GET_ELASCONN-----------------------------------------
 * A connection routine which returns point-to-point connections.
 * Connections from silicon triangles are ignored so that the
 * silicon part of the matrix is just a big identity.
 *----------------------------------------------------------------------*/
get_elasconn( ind, taken, num)
     int ind;			/* which node */
     short *taken;		/* the list of connected nodes */
     int *num;			/* the number of neighbors found, initially max. */
{
    int avail;
    int *nx, *nx1, tx, *j, *jj;
    tri_typ *at; pt_typ *ap;

    avail = *num; *num = 0;

    /*tiptoe through the triangles at this point's nodes*/
    ap = pt[ ind];
    for (nx = ap->nd, nx1 = nx + ap->nn; nx < nx1; nx++) {

	for (tx = 0; tx < num_tri_nd(*nx); tx++) {

	    at = tri[tri_nd(*nx,tx)];

	    if (mat_reg(at->regnum) != Si) {

		/*include all the points of that triangle*/
		for (j = at->nd, jj = j + 3; j < jj; j++) {
		    taken[ (*num) ++] = nd[ *j]->pt;
		    if ( (*num) >= avail) return( -1);
		}

	    } else {
		/* Silicon triangles only provide the self connection */
		taken[ (*num) ++] = ind;
		if( (*num) >= avail) return(-1);
	    }
	}
    }

    clean_list( taken, num);

    return( 0);
}

