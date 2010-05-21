/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *************************************************************************/
/*   dev_solve.c                Version 5.2     */
/*   Last Modification : 7/3/91 15:44:20  */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "diffuse.h"
#include "impurity.h"
#include "material.h"
#include "matrix.h"
#include "device.h"

/*the amount of over solve needed for this inner loop*/
#define NEWT 1.0e-2

extern dev_setup();


/************************************************************************
 *									*
 *	dev_solve( ) -  This routine computes the device operating	*
 *  point.								*
 *									*
 *  Original:	MEL	12/84						*
 *									*
 ************************************************************************/
dev_solve( area , movie)
double *area;
char *movie;
{
    register int k, i, sk;
    double rhsnm[MAXIMP];

    /*compute the boundary condition info*/
    bval_compute( 300.0, new, 0.0 );

    /*load the setup structure*/
    cs.old_del = 0.0;
    cs.new_del = 0.0;
    cs.temp = 0.0;
    cs.new = new;
    cs.newa = newa;
    cs.nco = area;
    cs.type = SS;
    cs.sol = dev_sol;
    cs.nsol = dev_nsol;
    cs.elim = dev_elim;
    cs.il = dev_il;
    cs.l = dev_l;
    cs.loff = dev_loff;

    /*load the values*/
    for(k = 0; k < dev_nsol; k++) {
	sk = dev_sol[k];
	for(i = 0; i < nn; i++) new[sk][i] = nd[i]->sol[sk];
    }

    dev_setup( rhsnm );

    /*now call the real solve routine*/
    k = soldif( TRUE , "DEV", dev_setup );

    /*save away the calculated values*/
    for(k = 0; k < dev_nsol; k++) {
	sk = dev_sol[k];
	for(i = 0; i < nn; i++) nd[i]->sol[sk] = new[sk][i];
    }
}




/************************************************************************
 *									*
 *	do_setup( ) - This routine is a simple cover function to 	*
 * do a setup for the matrices in question.				*
 *									*
 *  Original:	MEL	5/88						*
 *									*
 ************************************************************************/
dev_setup( rhs2 )
double *rhs2;		/*the two norm of the right hand side*/
{
    register int i, j, imp, bi, bs;

    /*zero everything we can think of*/
    zero_setup(&cs);


    /*compute the active concentration*/
    for(i=0; i < nn; i++) net[i] = 0.0;
    for ( j = 0; j < n_imp; j++ ) {
	switch ( (imp = soltoimp[j]) ) {
	case Asa   :
	case Ba    :
	case Sba   :
	case Pa    :
	case iBea  :
	case iMga  :
	case iSea  :
	case iSia  :
	case iSna  :
	case iGea  :
	case iZna  :
	case iCa   :
	case iGa   :
	    if (IS_ACCEPTOR(imp)) {
		for(i = 0; i < nn; i++) net[i] -= sol_nd(i, j);
	    } else {
		for(i = 0; i < nn; i++) net[i] += sol_nd(i, j);
	    }
	    break;

	default    :
	    break;
	}
    }

    if (IS_PSEUDO(H)) {
	bi = imptosol[H];
	for(i=0; i<nn; i++) {
	    if ( IS_SEMI(mat_nd(i)) ) {
		net[i] += new[bi][i];
		dnet[bi][i] = 1.0;
	    }
	    else {
		dnet[bi][i] = 0.0;
		new[bi][i] = 1.0;
		net[i] = 0.0;
	    }
	}
    }
    if (IS_PSEUDO(N)) {
	bi = imptosol[N];
	for(i=0; i<nn; i++) {
	    if ( IS_SEMI(mat_nd(i)) ) {
		net[i] -= new[bi][i];
		dnet[bi][i] = - 1.0;
	    }
	    else {
		dnet[bi][i] = 0.0;
		new[bi][i] = 1.0;
		net[i] = 0.0;
	    }
	}
    }

    /*compute band edges*/
    for(i = 0; i < nn; i++) {
	Ec[i] = Econ( mat_nd(i) );
	Ev[i] = Eval( mat_nd(i) );
    }

    /*compute the matrix blocks and boundary conditions*/
    for(bi = 0; bi < cs.nsol; bi++) {
	bs = cs.sol[bi];

	/*do all the diffusing terms*/
	setup_2d(bs, cs.temp, cs.nsol, cs.sol, NULL, NULL, cs.new, cs.newa, 
		       equil, cs.nco, bval, bcnum );
    }

    for(bi = 0; bi < cs.nsol; bi++) {
	bs = cs.sol[bi];
	for(i = 0; i < nn; i++) rhs[bs][i] = newb[bs][i];
    }

    /*compute the norms*/
    for(bi = 0; bi < cs.nsol; bi++) {
	bs = cs.sol[bi];
	/*check the right hand side and scaling*/
	rhs2[bs] = 0.0;
	for(i = 0; i < nn; i++) {
	    if ( rhs[bs][i] < 1.0e18 )
		rhs2[bs] += rhs[bs][i] * rhs[bs][i];
	    else
		rhs2[bs] += 1.0e18;
	}
    }
}
