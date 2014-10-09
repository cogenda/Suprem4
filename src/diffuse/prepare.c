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
/*   prepare.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:50:10 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "diffuse.h"
#include "impurity.h"
#include "defect.h"
#include "material.h"
#include "expr.h"
#include "matrix.h"

#define Alloc(P,O,N) if(P) free(P); P = scalloc( O, N);
#define Free(P)      if(P) free(P); P=0;
#define NO_COMPUTE(A) ((A==XVEL)||(A==YVEL)||(A==DELA)||(A==Sxx)||(A==Sxy)||(A==Syy))


/************************************************************************
 *									*
 *	init_diffuse() - This routine computes the initial values	*
 *  required for the diffusion steps.					*
 *									*
 *  Original:	MEL	11/85						*
 *									*
 ************************************************************************/
init_diffuse()
{
    double *area;
    int i, vx, vy;

    vx = imptosol[XVEL];
    vy = imptosol[YVEL];

    /*if we need to, initialize velocity*/
    if ( vx != -1 ) {
	for(i = 0; i < nn; i++)
	    nd[i]->sol[vx] = nd[i]->sol[vy] = 0.0;
	point_vel();
    }

    /*alloacate some area*/
    area = salloc( double, nn );

    /*start by computing the block structure of the matrix*/
    diff_blocks();

    /*now do the rest*/
    (void)upd_diff( 0.0, area, TRUE, TRUE );

    /*perform symbolic actions on the matrices*/
    upd_matrix( );

    free(area);
}


/************************************************************************
 *									*
 *	upd_diff() - Update the coupling coefficients, edge list, bc 	*
 *  list.								*
 *									*
 *  Original:	MEL	7/90						*
 *									*
 ************************************************************************/
upd_diff( delta, area, doit,  min_ok )
double delta;
double *area;
int doit;
int min_ok;
{
    int get_connect();
    int *reorder;
    int retval = FALSE;

    /*move the points to their new locations*/
    if ( (imptosol[XVEL] != -1) || (delta != 0.0) ) {
	move_point( delta );
	geom("during update of diffusion");
    }

    if ( doit || UpdateSymbolic ) {
	if ( min_ok ) {
	    /*perform the minimum fill reorder*/
	    aoff = 10*np;
	    Alloc( ia, int, aoff);
	    if(generate( nn, 1, 1, get_connect, &ia, &aoff) < 0)
		panic("out of memory");

	    /* do it */
	    reorder = salloc(int, nn);
	    (void)min_ia_fill( ia, reorder, (verbose >= V_BARF));
	    scramble( reorder );
	    free(reorder);
	    Free( ia);
	}

	/*compute the needed ia structures*/
	ia_calc();

	/*compute the structure for the bvals and edges*/
	assem_map();

	/*update the symbolic factorization*/
	retval = TRUE;
	upd_matrix();

	/*allocate space for the new nodal structures*/
        if(min_ok) diff_alloc();

	UpdateSymbolic = 0;
    }

    /*compute the coupling coefficients structures*/
    sum_coeff( area );

    return( retval );
}




/************************************************************************
 *									*
 *	upd_matrix() - Redo the symbolic factorization.			*
 *									*
 *  Original:	MEL	7/90						*
 *									*
 ************************************************************************/
upd_matrix()
{
    int i, j, lnsol, lsol[MAXIMP];

    /*build the appropriate pointers*/
    for(i = 0; i < n_imp; i++) {
	for(j = 0; j < n_imp; j++) {
	    switch( blktype[i][j] ) {
	    case B_NONE :
		bia[i][j] = NULL;
		birow[i][j] = NULL;
		bjrow[i][j] = NULL;
		baoff[i][j] = 0;
		break;
	    case B_DIAG :
		bia[i][j] = ia_d;
		birow[i][j] = irow_d;
		bjrow[i][j] = jrow_d;
		baoff[i][j] = aoff_d;
		break;
	    case B_TRI :
	    case B_BLCK :
		bia[i][j] = ia;
		birow[i][j] = irow;
		bjrow[i][j] = jrow;
		baoff[i][j] = aoff;
		break;
	    }
	}
    }

    /*step over the blocks and decide what to factor*/
    for(i = 0; i < ss_nsol; i++) {
	for(j = 0; j < ss_nsol; j++) facblk[ss_sol[i]][ss_sol[j]] = FALSE;
	facblk[ss_sol[i]][ss_sol[i]] = TRUE;
    }

    /*prepare symbolic entries and solution arrays for the steadies*/
    do_symb(ss_nsol, ss_sol, &(ss_il), &(ss_l), &(ss_loff));

    for(lnsol = i = 0; i < tm_nsol; i++)
	if (!tm_elim[i]) lsol[ lnsol++ ] = tm_sol[i];

    /*step over the blocks and decide what to factor*/
    for(i = 0; i < lnsol; i++) {
	if ( methdata.fill == FULL )
	    for(j = 0; j < lnsol; j++) facblk[lsol[i]][lsol[j]] = TRUE;
	else if ( methdata.fill == DIAG )
	    for(j = 0; j < lnsol; j++) facblk[lsol[i]][lsol[j]] = FALSE;
	facblk[lsol[i]][lsol[i]] = TRUE;
    }

    /*prepare symbolic entries and solution arrays for the time dependents*/
    do_symb(lnsol, lsol, &(tm_il), &(tm_l), &(tm_loff));
}




/************************************************************************
 *									*
 *	diff_alloc() - This routine dose all the prepass calculations	*
 *  for the diffusion code.						*
 *									*
 *  Original:	MEL	11/85						*
 *									*
 ************************************************************************/
diff_alloc( )
{
    int i;
    int nc[MAXIMP];

    /*do the symbolic stuff as defined on the symbolic card*/
    for(i = 0; i < n_imp; i++) nc[i] = TRUE;
    for(i = 0; i < tm_nsol; i++) nc[tm_sol[i]] = FALSE;
    for(i = 0; i < ss_nsol; i++) nc[ss_sol[i]] = FALSE;
    arr_alloc(nc);
}





/************************************************************************
 *									*
 *	ia_calc() - This routine computes the ia indices.		*
 *									*
 *  Original:	MEL	11/85						*
 *									*
 ************************************************************************/
ia_calc( )
{
    register int i, ij;
    register int p, n, j;
    int get_connect();

    /*first get the old geometry pulled together*/
    if ( ClockTri( FALSE )) printf("bad initial grid\n");

    /*allocate space for the old matrix connectivity*/
    aoff = 10 * np;
    Alloc( ia, int, aoff);
    if ( generate( nn, 1, 0, get_connect, &ia, &aoff ) < 0 )
	panic("out of memory");
    aoff = ia[nn] - ia[0];

    /*make the row list for B.C. manipulation*/
    if ( irow != NULL ) free( irow );
    if ( jrow != NULL ) free( jrow );
    irow = salloc( int, (ia[nn]+1+aoff) * 2 );
    jrow = salloc( int, (ia[nn]+1+aoff) * 2 );
    genrow( ia , aoff, irow, jrow );

    /*get enough space*/
    Alloc( ia_d, int, (3*nn -2*np) + 1 );

    /*build an extended diagonal list*/
    ij = ia_d[0] = nn+1;
    for(i = 0; i < nn; i++) {
	p = nd[i]->pt;
	for(j = 0; j < pt[p]->nn; j++) {
	    n = pt[p]->nd[j];
	    if ( n > i ) ia_d[ij++] = n;
	}
	ia_d[i+1] = ij;
    }
    aoff_d = ia_d[nn] - ia_d[0];

    /*generate the row description*/
    if ( irow_d != NULL ) free( irow_d );
    if ( jrow_d != NULL ) free( jrow_d );
    irow_d = salloc( int, (ia_d[nn]+1+aoff_d) * 2 );
    jrow_d = salloc( int, (ia_d[nn]+1+aoff_d) * 2 );
    genrow( ia_d , aoff_d, irow_d, jrow_d );
}




/************************************************************************
 *									*
 *	diff_blocks() - This routine computes the block structure of 	*
 *  the matrix.								*
 *									*
 *  Original:	MEL	7/90						*
 *									*
 ************************************************************************/
diff_blocks()
{
    register int si, i, j, k, sj, sk;
    int imp, imp2;
    int lnsol, lsol[MAXIMP];
    int tstelm;
    int blck = (mode==ONED)?(B_TRI):(B_BLCK);

    /*set up each block*/
    for(i = 0; i < n_imp; i++) {

	/*non mobile species are diagonal blocks*/
	if ( IS_MOBILE( soltoimp[ i ] ) ) blktype[i][i] = blck;
	else blktype[i][i] = B_DIAG;

	/*initialize the block types*/
	imp = soltoimp[i];
	switch( imp ) {
	case iBe :
	case iMg :
	case iSe :
	case iSi :
	case iSn :
	case iGe :
	case iZn :
	case iC  :
	case iG  :
	case As :
	case B  :
	case Sb :
	case P  :   for(j = 0; j < n_imp; j++) {
			imp2 = soltoimp[j];
			switch( imp2 ) {
			case I   :
			case V   :
			case Psi :  blktype[i][j] = blck;
				    break;
			default  :  blktype[i][j] = B_NONE;
				    break;
			}
			if (i == j) blktype[i][j] = blck;
		    }
		    break;

	case I  :
	case V  :   for(j = 0; j < n_imp; j++) {
			imp2 = soltoimp[j];
			switch( imp2 ) {
			case iBe :
			case iMg :
			case iSe :
			case iSi :
			case iSn :
			case iGe :
			case iZn :
			case iC  :
			case iG  :
			case As  :
			case B   :
			case Ga  :
			case Sb  :
			case P   :  if ( methdata.defmodel == FULLCPL )
					blktype[i][j] = blck;
				    else
					blktype[i][j] = B_NONE;
				    break;
			case I   :
			case V   :  if ( methdata.defmodel == FULLCPL )
					blktype[i][j] = blck;
				    else
					blktype[i][j] = B_DIAG;
				    break;
			case Psi :  if ( methdata.defmodel == FERMI )
					blktype[i][j] = B_DIAG;
				    else
					blktype[i][j] = blck;
				    break;
			default  :  blktype[i][j] = B_NONE;
				    break;
			}
			if ((i == j) && (methdata.defmodel != FERMI))
			    blktype[i][j] = blck;
		    }
		    break;

	case T  :   for(j = 0; j < n_imp; j++) {
			imp2 = soltoimp[j];
			switch( imp2 ) {
			case T   :
			case I   :
			case Psi :
			case V   :  blktype[i][j] = B_DIAG;
				    break;
			default  :  blktype[i][j] = B_NONE;
				    break;
			}
			if (i == j) blktype[i][j] = B_DIAG;
		    }
		    break;

	case Psi :  for(j = 0; j < n_imp; j++) {
			imp2 = soltoimp[j];
			switch( imp2 ) {
			case iBe :
			case iMg :
			case iSe :
			case iSi :
			case iSn :
			case iGe :
			case iZn :
			case iC  :
			case iG  :
			case As  :
			case B   :
			case Ga  :
			case Sb  :
			case P   :
			case I   :
			case V   :  blktype[i][j] = B_DIAG;
				    break;
			default  :  blktype[i][j] = B_NONE;
				    break;
			}
			if (i == j) blktype[i][j] = blck;
		    }
		    if ( IS_PSEUDO(Psi) ) blktype[i][i] = B_DIAG;
		    break;

	case H2O :
	case O2 :
		    for(j = 0; j < n_imp; j++) {
			blktype[i][j] = B_NONE;
		    }
		    blktype[i][i] = blck;
		    break;

	case Ga :
	case Cs :
	case Au :   for(j = 0; j < n_imp; j++) {
			imp2 = soltoimp[j];
			switch( imp2 ) {
			case Psi :  blktype[i][j] = blck;
				    break;
			default  :  blktype[i][j] = B_NONE;
				    break;
			}
			if (i == j) blktype[i][j] = blck;
		    }
		    break;

	default  :  for(j = 0; j < n_imp; j++) blktype[i][j] = B_NONE;
		    blktype[i][i] = blck;

	}
    }


    /*compute the steady state variables*/
    for(ss_nsol = i = 0; i < n_imp; i++) {
	j = soltoimp[i];
	if ( (j == H2O) || (j == O2) )
	    ss_sol[ ss_nsol++ ] = i;
    }

    /*compute the time dependent variables*/
    for(tm_nelim = tm_nsol = i = 0; i < n_imp; i++) {
	si = soltoimp[i];
	if ( IS_DIFFUSE(si) && (!IS_STEADY(si) || IS_LOCKED(si) ) ) {
	    tm_sol[ tm_nsol++ ] = i;
	}
    }

    /*if any of these variables are being eliminated...*/
    for(lnsol = i = 0; i < tm_nsol; i++) {
	si = tm_sol[i];

	/*if any of these variables is all diagonal across a row*/
	for(tstelm = TRUE, j = 0; j < tm_nsol; j++) {
	    sj = tm_sol[j];
	    tstelm = tstelm && ((blktype[si][sj]==B_DIAG)||(blktype[si][sj]==B_NONE));
	}

	if ( tstelm )
	    tm_elim[i] = TRUE;
	else {
	    tm_elim[i] = FALSE;
	    lsol[ lnsol++ ] = si;
	}
    }

    /*block promote...*/
    for( i = 0; i < tm_nsol; i++) {
	si = tm_sol[i];
	if ( tm_elim[ i ] ) {
	    /*for each row*/
	    for(j = 0; j < tm_nsol; j++) {
		sj = tm_sol[j];
		if ( blktype[sj][si] == B_NONE ) continue;
		/*for each item*/
		for(k = 0; k < tm_nsol; k++) {
		    sk = tm_sol[k];
		    if (blktype[si][sk] == B_NONE) continue;

		    if ((blktype[sj][si] == B_DIAG) && (blktype[sj][sk] == B_NONE)) {
			blktype[sj][sk] = B_DIAG;
			bia[sj][sk] = ia_d;
			birow[sj][sk] = irow_d;
			bjrow[sj][sk] = jrow_d;
			baoff[sj][sk] = aoff_d;
		    }
		    if ( (blktype[sj][si] == blck) ) {
			blktype[sj][sk] = blck;
			bia[sj][sk] = ia;
			birow[sj][sk] = irow;
			bjrow[sj][sk] = jrow;
			baoff[sj][sk] = aoff;
		    }
		}
	    }
	}
    }
}





/************************************************************************
 *									*
 *	arr_alloc() - This routine allocates memory for the arrays used	*
 *  in the linear solver for diffusion.					*
 *									*
 *  Original:	MEL	4/85						*
 *									*
 ************************************************************************/
arr_alloc( no_compute )
int *no_compute;
{
    int i, j;
    int byte = 0;

    Alloc( net, double,  nn );
    Alloc( noni, double,  nn );

    /*allocate space for the arrays*/
    for(byte = 0, i = 0; i < n_imp; i++) {

	/*solution variable as a function of time*/
	Alloc( old[i], double,  nn );
	Alloc( mid[i], double,  nn );
	Alloc( new[i], double,  nn );
	byte += sizeof(double) * 3 * nn;

	if ( no_compute[ i ] ) continue;

	/*active concentrations*/
	Alloc( olda[i], double,  nn );
	Alloc( mida[i], double,  nn );
	Alloc( newa[i], double,  nn );
	byte += sizeof(double) * 3 * nn;

	/*right hand side values*/
	Alloc( rhs[i], double,  nn );
	Alloc( oldb[i], double,  nn );
	Alloc( newb[i], double,  nn );
	byte += sizeof(double) * 3 * nn;

	/*active, equilibrium, and derivitives*/
	Alloc( equil[i], double,  nn );
	Alloc( dequ[i], double,  nn );
	Alloc( dnet[i], double,  nn );
	byte += sizeof(double) * 3 * nn;

	for(j = 0; j < n_imp; j++) {

	    if ( no_compute[ j ] ) continue;

	    Alloc( dact[i][j], double,  nn );
	    byte += sizeof(double) * nn;

	    if ( blktype[i][j] != B_NONE ) {
		Alloc( a[i][j],  double,  (bia[i][j][nn] + baoff[i][j] + 1));
		byte += sizeof(double) * (bia[i][j][nn] + baoff[i][j] + 1);
	    }
	}
    }

    if (verbose >= V_CHAT)
	printf("diffuse  allocation %d bytes\n", byte);
}


/************************************************************************
 *									*
 *	assem_map() - This routine computes the edges and bvals.	*
 *									*
 *  Original:	MEL	7/90						*
 *									*
 ************************************************************************/
assem_map( )
{
    register int i, j, k;
    register int nx1, nx2, m;
    register int aoff = ia[nn] - ia[0];

    bcnum = 0;
    Alloc( bval, struct bound_str, nn );

    for(i = 0; i < nn; i++) {
	for(j = ia[i]; j < ia[i+1]; j++) {

	    if ( nd[i]->pt == nd[ia[j]]->pt ) {
		/*generate a boudary condition entry*/
		/*make material one be silicon*/
		if ( nd[ia[j]]->mater == Si ) {
		    /*get the node and arrays into easy access*/
		    nx1 = bval[bcnum].nx[0] = ia[j];
		    nx2 = bval[bcnum].nx[1] = i;
		    bval[bcnum].loc[0][0] = nx1;
		    bval[bcnum].loc[0][1] = j;
		    bval[bcnum].loc[1][0] = j + aoff;
		    bval[bcnum].loc[1][1] = nx2;
		}
		else {
		    /*get the node and arrays into easy access*/
		    nx1 = bval[bcnum].nx[0] = i;
		    nx2 = bval[bcnum].nx[1] = ia[j];
		    bval[bcnum].loc[0][0] = nx1;
		    bval[bcnum].loc[0][1] = j + aoff;
		    bval[bcnum].loc[1][0] = j;
		    bval[bcnum].loc[1][1] = nx2;
		}
		bval[bcnum].mat[0] = nd[nx1]->mater;
		bval[bcnum].mat[1] = nd[nx2]->mater;

		bcnum++;
	    }
	    else {
		/*generate an entry for the edge*/
		/*find the edge....*/
		nx1 = -1;
		for(k = 0; nx1 == -1 && k<num_edge_nd(i); k++) {
		    m = edge_nd(i,k);
		    if ((nd_edg(m,0)==ia[j]) || (nd_edg(m,1)==ia[j])) nx1 = m;
		}
		edg[nx1]->l = j;
		edg[nx1]->u = j + aoff;
		if (nd_edg(nx1,0) > nd_edg(nx1,1)) {
		    m = nd_edg(nx1,0);
		    nd_edg(nx1,0) = nd_edg(nx1,1);
		    nd_edg(nx1,1) = m;
		}
	    }
	}
    }
}




