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
/*   blklu.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:44:09 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef ALLIANT
#include <cncall.h>
#endif
#include <assert.h>
#include "global.h"
#include "constant.h"
#include "sysdep.h"
#include "diffuse.h"
#include "impurity.h"
#include "matrix.h"


extern int blkmxv(), inv_blk(), fac_blk();

/************************************************************************
 *									*
 *	blkfac() _ This routine factors all the blocks.			*
 *									*
 *  Original:	MEL	11/85						*
 *									*
 ************************************************************************/
blkfac( nv, nsol, sol, il, l, loff )
int nv;				/*number of variables per block*/
int nsol;			/*number of blocks*/
int *sol;			/*the particular blocks*/
int *il;			/*l matrix descriptor*/
double *l;			/*l matrix values*/
int loff;			/*l upper triangular offset*/
{
    /*now do the hard stuff*/
    numfac(nv, sol, nsol, ia, aoff, (double *)0, il, loff, l);
}




/************************************************************************
 *									*
 *	blkbac( ) - perform a block factorization and matrix multiply.	*
 *									*
 * Original:	MEL	6/87						*
 *									*
 ************************************************************************/
app_inv( nv, nsol, sol, il, loff, l, x, ax )
int nv;
int nsol;
int *sol;
int *il;
int loff;
double *l;
double *x[];
double *ax[];
{
    register int i,j;
    double *trhs, *xsi;
    int bi;

    trhs = (double *)malloc( sizeof(double) * nv * nsol );

    /*copy the right hand side into a long vector*/
    for(i = 0; i < nsol; i++) {
	/*$dir no_recurrence*/
	xsi = x[sol[i]];
#       pragma ivdep
	for(j = 0; j < nv; j++) trhs[j * nsol + i] = xsi[j];
    }

    /*the preconditioner solve*/
#ifdef FORTRAN_VECTOR
    bi = nv * nsol;
    NUMBAC( &bi, il, &loff, l, trhs );
#else
    numbac(nv*nsol, il, loff, l, trhs );
#endif

    /*copy the right hand side into a long vector*/
    for(i = 0; i < nsol; i++) {
	xsi = x[sol[i]];
	/*$dir no_recurrence*/
#       pragma ivdep
	for(j = 0; j < nv; j++) xsi[j] = trhs[j * nsol + i];
    }

    free( trhs );

    /*compute the matrix multiply*/
    bigmxv( nsol, sol, nv, x, ax );
}




/************************************************************************
 *									*
 *	Snumfac - This routine does a sparse LDU factorization of the	*
 *  matrix a.
 *									*
 *  Reference:  Crout's Algorithm					*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
numfac(n, sol, nsol, ia, aoff, a, il, loff, l)
int n;
int *sol;
int nsol;
int ia[];
int aoff;
double *a;
int *il;
int loff;
double l[];
{
    double utmp;
    double ltmp;
    register int k;
    int first_nbr, nxti;
    register int jmin, jmax, j, i ;
    register int endloop;
    register int bi;
    register int vi;
    int *lrow, *lstr;
    double *row, *col;

    /*allocate the local space required*/
    lrow = salloc(int, nsol * n);	/*linked lit of rows being processed*/
    lstr = salloc(int, nsol * n);	/*list of initial element pointers*/
    row = salloc(double, nsol * n); 	/*row work space*/
    col = salloc(double, nsol * n); 	/*column work space*/

    /*initialize the linked list of rows and start points*/
    /*$dir no_recurrence*/
    for(k = 0; k < n*nsol; k++) {
	lrow[k] = -1;
	lstr[k] = il[k];
	col[k] = row[k] = 0.0;
    }

    /*for each variable we have to work on*/
    for(vi = 0; vi < n; vi++) {
	for( bi = 0; bi < nsol; bi++ ) {

	    k = vi * nsol + bi;

	    /*clear all the values we may need*/
	    endloop = il[k+1];

	    /*$dir no_recurrence*/
	    for(j = il[k]; j < endloop; j++) col[ il[j] ] = row[ il[j] ] = 0.0;
	    col[k] = row[k] = 0.0;

	    /*figure the positions in the matrix*/
	    /*if not given sol assume a is one long piece*/
	    if( sol != 0 )
		get_coldata( vi, bi, nsol, sol, ia, aoff, col, row );
	    else {
		col[k] = a[k];
		endloop = ia[k+1];
		/*$dir no_recurrence*/
		for(j = ia[k]; j < endloop; j++) {
		    col[ ia[j] ] = a[j];
		    row[ ia[j] ] = a[j + aoff];
		}
	    }

	    /*speaking of gawdawful ugly hacks....*/
	    l[k] = col[k];
	    if ( l[k] == 0.0 ) l[k] = 1.0;

	    /*lrow linked list contains the rows/cols that need subtracting*/
	    nxti = lrow[k];

	    while ( (i = nxti) != -1) {
		nxti = lrow[i];
		jmin = lstr[i];
		jmax = il[i+1];

		/*set up the temp multipliers of the row/col to be removed*/
		ltmp = l[ jmin ] / l[i];
		utmp = l[ jmin + loff ] / l[i];

		/*handle the diagonal term*/
		l[k] -= l[jmin] * l[jmin + loff] / l[i];
		jmin++;

		if (jmin >= jmax) continue;

		/*$dir no_recurrence*/
		for(j = jmin; j < jmax; j++) {
		    row[il[j]] -= ltmp * l[j + loff];
		    col[il[j]] -= utmp * l[j];
		}

		lstr[i] = jmin;
		lrow[i] = lrow[ il[jmin] ];
		lrow[ il[jmin] ] = i;
	    }

	    endloop = il[k+1];
	    /*$dir no_recurrence*/
	    l[k] = 1.0 / l[k];
	    for(j = il[k]; j < endloop; j++) {
		l[j] = col[ il[j] ] * l[k];
		l[j + loff] = row[ il[j] ] * l[k];
	    }
	    if( (lstr[k] = il[k]) < il[k+1] ) {
		first_nbr = il[ lstr[k]];
		lrow[k] = lrow[ first_nbr ];
		lrow[ first_nbr ] = k;
	    }
	}
    }
    free( lrow );
    free( lstr );
    free( row );
    free( col );
}




/************************************************************************
 *									*
 *	Ssymfac - This routine computes the fill and the sparse pattern	*
 *  of the LDU factorization of the matrix whose sparse pattern is	*
 *  containde in a.							*
 *									*
 *  Reference:  Crout's Algorithm					*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
symfac(n, sol, nsol, ia, aoff, pil, loff)
int n;
int *sol;
int nsol;
int *ia;
int aoff;
int **pil;
int *loff;
{
    register int *aj, *ajE;
    register int *il, *icol, ilkp1;
    register int j;
    int i, k, vi, nxti, first_nbr, jmin, jmax, ilmax = *loff, last_nbr;
    int bi;
    int *lrow, *lstr;

    /*allocate local storage space*/
    lrow = salloc(int, nsol*n);	/*linked lit of rows being processed*/
    lstr = salloc(int, nsol*n);	/*list of initial element pointers*/
    icol = salloc(int, nsol*n);
    il = *pil;

    /*initialize the linked list of rows and start points*/
    for(k = 0; k < n*nsol; k++) {
	lrow[k] = -1;
	icol[k] = lstr[k] = 0;
    }

    il[0] = nsol*n + 1;

    /*for each variable to be solved*/
    for(vi = 0; vi < n;  vi++) {
	for( bi = 0; bi < nsol; bi++ ) {

	    /*compute the il index of this variable*/
	    k = vi * nsol + bi;

	    ilkp1 = il[k];

	    /*get the a matrix positions*/
	    /*if not given sol assume ia is one long piece*/
	    if( sol != 0 )
		last_nbr = get_locs( vi, bi, nsol, sol, ia, aoff, icol );
	    else {
#pragma asis
		for(aj = &ia[ ia[k]], ajE = &ia[ia[k+1]]; aj < ajE; aj++)
		    icol[ *aj ] = 1;
		last_nbr = aj[-1];
	    }

	    /*lrow linked list contains the rows/cols that need subtracting*/
	    nxti = lrow[k];

	    while ( (i = nxti) != -1) {
		nxti = lrow[i];
		assert(il[lstr[i]] == k);
		jmin = lstr[i] + 1;
		jmax = il[i+1];

		if (jmin >= jmax) continue;

#pragma asis
		for(aj = &il[jmin], ajE = &il[jmax]; aj < ajE; aj++) icol[*aj] = 1;
		if( aj[-1] > last_nbr) last_nbr = aj[-1];

		lstr[i] = jmin;
		lrow[i] = lrow[ il[jmin] ];
		lrow[ il[jmin] ] = i;
	    }

	    /*copy the values out of the scratch space*/
	    for(aj = &icol[k + 1], ajE = &icol[ last_nbr]; aj <= ajE; aj++) {
		if (*aj) {
		    j = aj - icol;

		    il[ ilkp1++ ] = j;

		    if (ilkp1 >= ilmax-10) {
			/* Ooops. Sure hope the caller malloc'ed il... */
			ilmax *= 1.5;
			il = *pil = sralloc( int, ilmax, *pil );
		    }
		    *aj = 0;
		}
	    }
	    icol[k] = 0;

	    /*add k to its first neighbor's linked list*/
	    if ((lstr[k] = il[k]) < ilkp1) {
		first_nbr = il[ lstr[k]];
		lrow[k] = lrow[ first_nbr ];
		lrow[ first_nbr ] = k;
	    }
	    il[k+1] = ilkp1;
	}
    }

    il[nsol*n] = il[nsol*n-1];
    *loff = il[nsol*n] - n*nsol - 1;

    free(lrow);
    free(lstr);
    free(icol);

    return(0);
}



/************************************************************************
 *									*
 *	Snumbac - This routine performs a for/back solve loop on the	*
 *  factored matric contained in l and with the rhs as passed.  The 	*
 *  answer is returned in the rhs vector.				*
 *									*
 *  Reference:  Crout's Algorithm					*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
numbac(n, il, off, l, rhs )
int n;		/*the number of equations*/
int il[];	/*the array description of l*/
int off;	/*the offset to the upper triangular part*/
double l[];	/*the factored array*/
double rhs[];	/*the right hand side vector*/
{
    register int i, j;
    register int endloop;
    double a;

    /*let's do the for solve*/
    for(i = 0; i < n; i++) {
	endloop = il[i+1];
	/*$dir no_recurrence*/
	a = rhs[i];
	for(j = il[i]; j < endloop; j++) {
	    rhs[il[j]] -= l[j] * a;
	}
    }

    /*$dir no_recurrence*/
#pragma ivdep
    for(i = 0; i < n; i++) rhs[i] *= l[i];

    /*do the for back solves as required to get answer*/
    for(i = n-1; i >= 0; i--) {
	/*$dir no_recurrence*/
	endloop = il[i+1];
	a = rhs[i];
	for(j = il[i]; j < endloop; j++) {
	    a -= rhs[il[j]] * l[j+off];
	}
	rhs[i] = a;
    }
}


/* These are redundant but I can't find the other copies right now. */
dzero( a, n)
    double *a;
    int n;
{
    int i;

    /*$dir no_recurrence*/
    for (i = 0; i < n; i++) a[i] = 0;
}

dcopy( a, b, n)
    double *a, *b;
    int n;
{
    int i;

    /*$dir no_recurrence*/
    for (i = 0; i < n; i++) b[i] = a[i];
}
