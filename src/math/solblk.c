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
/*   solblk.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:44:19 */

#include <stdio.h>
#include <stdlib.h>
#ifdef convexvc
#include <fastmath.h>
#else
#include <math.h>
#endif
#include "global.h"
#include "constant.h"
#include "sysdep.h"
#include "diffuse.h"
#include "impurity.h"
#include "matrix.h"
extern double norm2();

#define BLKERR 1.0e-2

/************************************************************************
 *									*
 *	solve_blocks( bunch of stuff ) - This routine operates on the	*
 *  blocks to solve the data.  It currently uses a gauss-seidel block	*
 *  method to get the results.  I hope to include an iccg method at the	*
 *  block level as well.						*
 *									*
 *  Original:	MEL	11/85						*
 *              LCC      6/90 (mxv_tri for 1-D. mxv_diag for B_DIAG.)   *
 *									*
 ************************************************************************/
solve_blocks( nv, sol, nsol, elim, il, l, loff, wrkspc, factor )
int nv;
int *sol;
int nsol;
int *elim;
int *il;
double *l;
int loff;
double *wrkspc[];
int factor;
{
    register int i, si, sj;
    register int bi, bj;
    int lsol[MAXIMP], lnsol;
    double *tmp;
    double *aij, *ri, *wi, *wj;

    if ( elim != NULL) {
	tmp = salloc(double, ia[nv]+aoff);

	/*for each block that can be eliminated painlessly*/
	for( lnsol = bi = 0; bi < nsol; bi++ ) {
	    if ( ! elim[bi] ) {
		lsol[lnsol++] = sol[bi];
	    }
	    else {
		si = sol[bi];

		/*zero the solution vector for each*/
		/*$dir no_recurrence*/
		dzero( wrkspc[si], nv );

		/*for each row we are solving*/
		for( bj = 0; bj < nsol; bj++ ) {
		    if ( bj == bi ) continue;
		    sj = sol[bj];

		    if ( blktype[sj][si] == B_NONE ) continue;

		    /*compute ajk = ajk - aji *ajk / aii*/
		    elim_blk_row( nv, nsol, sol, sj, si, tmp );
		}
	    }
	}
	free(tmp);
    }
    else {
	lnsol = nsol;
	for(i = 0; i < nsol; i++) lsol[i] = sol[i];
    }

    if ( lnsol > 0 ) {
	/*noactor anything that needs it*/
	if ( factor )
	    blkfac( nv, lnsol, lsol, il, l, loff );

	/*solve the blocks appropriately*/
	cg_block(nv, lnsol, lsol, il, loff, l, wrkspc, factor);
    }


    if ( elim == NULL ) return;

    /*compute the value of the pre - eliminated variables*/
    for(bi = nsol-1; bi >= 0; bi--) {
	if ( elim && (elim[bi]) ) {
	    si = sol[bi];
	    ri = rhs[si];

	    /*for each row variable*/
	    for(bj = 0; bj < nsol; bj++) {
		if ( elim[bj] ) continue;
		sj = sol[bj];
		if (sj == si ) continue;

		if ( blktype[si][sj] != B_NONE ) {
		    /*$dir no_recurrence*/
		    aij = a[si][sj];
		    wj = wrkspc[sj];
#                   pragma ivdep
		    for( i = 0; i < nv; i++ ) ri[i] -= aij[i] * wj[i];
		}
	    }
	    /*$dir no_recurrence*/
	    wi = wrkspc[si];
	    for(i = 0; i < nv; i++) wi[i] = ri[i] / a[si][si][i];
	}
    }
}



elim_blk_row( nv, nsol, sol, sj, si, tmp )
int nv, nsol, *sol;
int sj, si;
double *tmp;
{
    register int tao, i, j, bk, sk;
    int *tia;
    register double *aji, *aii, *ajk, *aik;

    /*compute the multiplier term*/
    tia = bia[sj][si];
    tao = baoff[sj][si];

    /*compute aji / aii*/
    aji = a[sj][si];
    aii = a[si][si];
    switch( blktype[sj][si] ) {
    case B_NONE : return;


    case B_DIAG :
	for(i = 0; i < nv; i++) tmp[i] = aji[i] / aii[i];
	/*compute rhs(j) = rhs(j) - rhs(i) * aji / aii*/
	mxv_diag( nv, -1.0, tmp, rhs[si], rhs[sj] );
	break;

    case B_TRI :
#ifdef FOO
	for(i = 0,j=tia[0]; i < nv-1; i++,j++) {
	    tmp[i] = aji[i] / aii[i];
	    tmp[j] = aji[j] / aii[i];
	    tmp[j+tao] = aji[j+tao] / aii[tia[j]];
	}
	tmp[nv-1] = aji[nv-1] / aii[nv-1];
	/*compute rhs(j) = rhs(j) - rhs(i) * aji / aii*/
	mxv_tri( nv, tia, tao, -1.0, tmp, rhs[si], rhs[sj] );
	break;
#endif

    case B_BLCK :
	for(i = 0; i < nv; i++) {
	    tmp[i] = aji[i] / aii[i];
	    for(j = tia[i]; j < tia[i+1]; j++) {
		tmp[j] = aji[j] / aii[i];
		tmp[j+tao] = aji[j+tao] / aii[tia[j]];
	    }
	}
	/*compute rhs(j) = rhs(j) - rhs(i) * aji / aii*/
	mxv( nv, tia, tao, -1.0, tmp, rhs[si], rhs[sj] );
	break;
    }

    /*and each entry in the column*/
    for( bk = 0; bk < nsol; bk++) {
	sk = sol[bk];

	tia = bia[sj][si];
	tao = baoff[sj][si];

	/*a(j)(k) = a(j)(k) - tmp * a(i)(k)*/
	ajk = a[sj][sk];
	aik = a[si][sk];

	if ( blktype[si][sk] == B_NONE) continue;

	if (sk == si)
	    dzero( ajk, tia[nv] + tao );
	else {
	    switch( blktype[sj][si] ) {
	    case B_DIAG :
		for(i = 0; i < nv; i++,j++) ajk[i] -= tmp[i] * aik[i];
		break;
	    case B_TRI :
#ifdef FOO
		for(i = 0,j=tia[0]; i < nv; i++,j++) {
		    ajk[i] -= tmp[i] * aik[i];
		    ajk[j] -= tmp[j] * aik[i];
		    ajk[j+tao] -= tmp[j+tao] * aik[tia[j]];
		}
		break;
#endif
	    case B_BLCK :
		for(i = 0; i < nv; i++) {
		    ajk[i] -= tmp[i] * aik[i];
		    for(j = tia[i]; j < tia[i+1]; j++) {
			ajk[j] -= tmp[j] * aik[i];
			ajk[j+tao] -= tmp[j+tao] * aik[tia[j]];
		    }
		}
		break;
	    }
	}
    }
}

/************************************************************************
 *									*
 *	cg_block( nv, b_blk, ans ) - Thsi routine uses a 	*
 *  conjugate residual technique to solve the blocks.			*
 *									*
 *  Original:	MEL	4/85						*
 *									*
 ************************************************************************/
cg_block( nv, nsol, sol, il, loff, l, ans, initial )
int nv;		/*the number of variables*/
int nsol;
int *sol;
int *il;
int loff;
double *l;
double **ans;	/*the answer vector*/
int initial;
{
    register int i, j;
    register int si;
    register int str, curr, b;
    int count;
    double nm[MAXIMP], nm2=0.0, err;
    double maxnorm;
    double t, s;
    double alpha, beta[MAXBACK];
    double rap[MAXBACK], normap[MAXBACK];
    double numer[MAXBACK];

    if ( initial ) {
	/*compute an initial guess for the simulation*/
	for( nm2 = 0.0, i = 0; i < nsol; i++ ) {
	    si = sol[i];
	    /*$dir no_recurrence*/
	    for(j = 0; j < nv; j++) {
		ans[si][j] = rhs[si][j];
		nm2 += 1.0e-10 * rhs[si][j] * rhs[si][j];
	    }
	}
	nm2 = 1.0e5 * sqrt( nm2 );

	/*compute the inverse of the rhs and ax*/
	app_inv( nv, nsol, sol, il, loff, l, ans, r );

	/*compute initial directions and and residual*/
	err = 0.0;
	for(i = 0; i < nsol; i++) {
	    si = sol[i];
	    /*$dir no_recurrence*/
	    for(j = 0; j < nv; j++) {
		p[0][si][j] = r[si][j] = rhs[si][j] - r[si][j];
		err += r[si][j] * r[si][j];
	    }
	}
	err = sqrt(err);
	if ( err > nm2 ) return;
    }
    else {
	for(nm2 = 0.0, i = 0; i < nsol; i++) {
	    si = sol[i];

	    /*$dir no_recurrence*/
	    for(j = 0; j < nv; j++) {
		ans[si][j] = 0.0;
		p[0][si][j] = r[si][j] = rhs[si][j];
		nm2 += rhs[si][j] * rhs[si][j];
	    }
	}
	nm2 = sqrt(nm2);
    }

    /*compute ap*/
    bigmxv( nsol, sol, nv, p[0], ap[0] );

    count=str=curr=0;
    while ( count < methdata.blkitlim ) {

	/*figure out which backing vector we are calculating*/
	str = curr + 1;
	if ( str == methdata.back ) str = 0;

	/*compute an alpha factor*/
	for(normap[curr] = rap[curr] = 0.0, i = 0; i < nsol; i++) {
	    si = sol[i];
	    alpha_body( nv, ap[curr][si], r[si], &t, &s );
	    normap[curr] += t;
	    rap[curr] += s;
	}

	if ( normap[curr] == 0.0 ) return;
	alpha = rap[curr] / normap[curr];

	/*calculate new answer value and residuals*/
	for(maxnorm = 0.0, i = 0; i < nsol; i++)  {
	    si = sol[i];
#ifdef FORTRAN_VECTOR
	    UPDATE_BODY(&nv, &alpha, ans[si], p[curr][si], ap[curr][si],
			      qinvr[si], r[si], &t);
#else
	    update_body(nv, alpha, ans[si], p[curr][si], ap[curr][si],
			      qinvr[si], r[si], &t);
#endif
	    maxnorm += t;
	    nm[si] = sqrt( t );
	}
	maxnorm = sqrt(maxnorm);

	if ( verbose >= V_BARF)
	    printf("iteration %d error %e\n", count, maxnorm);

	if ( (maxnorm < (nm2*1.0e-8)) || (maxnorm < 1.0e-4)) return;

	/*compute qinvr*/
	app_inv( nv, nsol, sol, il, loff, l, qinvr, aqinvr );

	/*now calculate beta*/
	for(b = 0; b <= curr; b++ ) {
	    numer[b] = 0.0;
	    for(i = 0; i < nsol; i++) {
		si = sol[i];
		beta_calc( nv, aqinvr[si], ap[b][si], &t );
		numer[b] += t;
	    }
	    beta[b] = - numer[b] / normap[b];
	}

	/*update the current direction vectors*/
	for(i = 0; i < nsol; i++) {
	    si = sol[i];
	    upd_p( nv, p[str][si], p[curr][si], qinvr[si],
		       ap[str][si], ap[curr][si], aqinvr[si],  beta[curr] );
	}

	for(b = 0; b < curr; b++) {
	    for( i = 0; i < nsol; i++ ) {
		si = sol[i];
		hist_upd( nv, p[str][si], p[b][si], ap[str][si], ap[b][si], beta[b] );
	    }
	}
	count++;
	curr = str;
    }
}





alpha_body( nv, ap, r, normap, rap)
int nv;
double *ap, *r;
double *normap, *rap;
{
    register int j;
    double t1, t2;

    t1 = t2 = 0.0;
    /*$dir no_recurrence*/
    for(j = 0; j < nv; j++) {
	t1 += ap[j] * ap[j];
	t2 += ap[j] * r[j];
    }
    *normap = t1;
    *rap = t2;
}


update_body(nv, alpha, ans, p, ap, qinvr, r, nm)
int nv;
double alpha, *ans, *p, *ap, *qinvr, *r, *nm;
{
    register int j;
    double t3;

    t3 = 0.0;

    /*$dir no_recurrence*/
    for(j = 0; j < nv; j++) {
	ans[j] += alpha * p[j];
	r[j] -= alpha * ap[j];
	qinvr[j] = r[j];
	t3 += r[j] * r[j];
    }
    *nm = t3;
}


beta_calc(nv, aqinvr, ap, norm)
int nv;
double *aqinvr, *ap;
double *norm;
{
    register int j;
    double t;

    t = 0.0;
    /*$dir no_recurrence*/
    for(j = 0; j < nv; j++) {
	t += aqinvr[j] * ap[j];
    }
    *norm = t;
}


upd_p( nv, p1, p2, qinvr, ap1, ap2, aqinvr, beta )
int nv;
double *p1, *p2, *qinvr;
double *ap1, *ap2, *aqinvr;
double beta;
{
    register int j;

    /*$dir no_recurrence*/
#   pragma ivdep
    for(j = 0; j < nv; j++) {
	p1[j]  =  qinvr[j] + beta * p2[j];
	ap1[j] = aqinvr[j] + beta * ap2[j];
    }
}


hist_upd( nv, p1, p2, ap1, ap2, beta )
int nv;
double *p1, *p2, *ap1, *ap2;
double beta;
{
    register int j;

    /*$dir no_recurrence*/
#   pragma ivdep
    for(j = 0; j < nv; j++) {
	p1[j] +=  beta * p2[j];
	ap1[j] += beta * ap2[j];
    }
}



