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
/*   couple.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:38 */

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include "constant.h"
#include "global.h"
#include "geom.h"
#include "material.h"
#include "matrix.h"
#include "impurity.h"

/************************************************************************
 *									*
 *	sum_coeff() - This routine generates the coefficient array for	*
 *  the current matrix and triangle structure.				*
 *									*
 *  Original:	MEL	11/85						*
 *									*
 ************************************************************************/
sum_coeff( area )
double *area;
{
    register int t, i, j, s, n1, n2, m1, m2;
    int mat, nl;
    int ncnt;

    /* zero the summing array */
    for(i = 0; i < nn; i++) area[i] = 0.0;

    /*for each triangle*/
    for( t = 0; t < ne; t++ ) {
	if (!leaf (tri[t])) continue;

	/*for each vertex/side*/
	for ( i = 0; i < nvrt; i++ ) {
	    
	    /*get the node number*/
	    n1 = tri[t]->nd[i];

	    /*add the area in*/
	    area[n1] += tri[t]->earea[i];
	}
    }

    if ( mode == ONED ) ncnt = 1;
    else if ( mode == TWOD ) ncnt = 3;

    /*for each boundary condition*/
    for(i = 0; i < bcnum; i++) {
	n1 = bval[i].nx[0];
	n2 = bval[i].nx[1];
	m1 = nd[n1]->mater;
	m2 = nd[n2]->mater;
	if ( m1 == GAS ) {
	    m1 = m2; m2 = GAS;
	    j = n1; n1 = n2; n2 = j;
	}
	bval[i].cpl = 0.0;

	for(j = 0; j < num_tri_nd(n1); j++) {
	    t = tri_nd(n1,j);

	    for(s = 0; s < nedg; s++) {
		if ( tri[t]->nb[s] < 0 ) {
		    if ((tri[t]->nb[s] == BC_OFFSET + 2) && (m2 == GAS))
			bval[i].cpl += tri[t]->d[s];
		}
		else if (mat_reg( reg_tri(tri[t]->nb[s]) ) == m2)
		    bval[i].cpl += tri[t]->d[s];
	    }
	}
    }
}


#ifdef DEBCPL
#include "impurity.h"
dose_calc()
{
    register int ie, j, ni, n;
    double dose[MAXIMP];
    double ar;

    for(ni = 0; ni < n_imp; ni++) dose[ni] = 0.0;

    for( ie = 0; ie < ne; ie++ ) {

	for(j = 0; j < 3; j++) {
	    n = tri[ie]->nd[j];
	    ar = tri[ie]->earea[j];

	    for(ni = 0; ni < n_imp; ni++) 
		dose[ni] += nd[n]->sol[ni] * ar;
	}
    }

    for( ni = 0; ni < n_imp; ni++ )
	printf("Impurity %d dose %e\n", soltoimp[ni], dose[ni] );
}
#endif

