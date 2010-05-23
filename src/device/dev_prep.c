/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *************************************************************************/
/*   dev_prep.c                Version 5.2     */
/*   Last Modification : 7/3/91 15:44:19  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <math.h>

#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "device.h"
#include "impurity.h"
#include "matrix.h"

#define Alloc(P,O,N) if(P) free(P); P = scalloc( O, N);
#define Free(P)      if(P) free(P); P=0;
#define NO_COMPUTE(A) ((A!=Psi)&&(A!=N)&&(A!=H)&&(A!=CKT))


/************************************************************************
 *									*
 *	do_prep() - This routine does the necessary pre calc and array	*
 *  allocation for solving the device equations.			*
 *									*
 *  Original:	MEL	7/85						*
 *									*
 ************************************************************************/
dev_prep(double *area)
{
    int i;
    int get_connect();
    int *reorder;
    struct tms before, after;
    int nc[MAXIMP];
    int lnsol, lsol[MAXIMP];

    /* temporary L+U version of ia.  Malloc should split this in half later*/
    times(&before);
    aoff = 9*np;
    Alloc( ia, int, aoff);
    if( generate( nn, 1, 1, get_connect, &ia, &aoff) < 0) panic("out of memory");

    /* do it */
    reorder = salloc(int, nn);
    (void)min_ia_fill( ia, reorder, (verbose >= V_BARF));
    scramble( reorder );
    free(reorder);
    times(&after);
    print_time("minimum fill reorder", &before, &after );
    Free( ia);

    /*compute the ia indices*/
    ia_calc( );

    /* device connections set up */
    dev_net();

    /*compute the structure for the bvals and edges*/
    assem_map();

    for(lnsol = 0, i = 0; i < dev_nsol; i++)
	if ( !dev_elim[i] ) lsol[lnsol++] = dev_sol[i];

    /*do the symbolic stuff as defined on the symbolic card*/
    do_symb( lnsol, lsol, &(dev_il), &(dev_l), &(dev_loff) );

    for(i = 0; i < n_imp; i++) nc[i] = NO_COMPUTE(soltoimp[i]);
    arr_alloc(nc);

    sum_coeff( area );
}




/************************************************************************
 *									*
 *	dev_net( ) - Generate the device description and circuit 	*
 *  description for the device solution.				*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
dev_net()
{
    register int Psol, Nsol, Hsol;
    register int i, j, si, sj;

    /*set up the relevant blocks*/
    dev_nsol = 3;
    Psol = dev_sol[0] = imptosol[Psi];
    Nsol = dev_sol[1] = imptosol[N];
    Hsol = dev_sol[2] = imptosol[H];

    /*set up the appropriate block interconnections*/
    blktype[Psol][Psol] = B_BLCK;
    blktype[Psol][Nsol] = B_DIAG;
    blktype[Psol][Hsol] = B_DIAG;

    blktype[Nsol][Psol] = B_DIAG;
    blktype[Nsol][Nsol] = B_DIAG;
    blktype[Nsol][Hsol] = B_DIAG;

    blktype[Hsol][Psol] = B_DIAG;
    blktype[Hsol][Nsol] = B_DIAG;
    blktype[Hsol][Hsol] = B_DIAG;

    dev_elim[0] = FALSE;
    dev_elim[1] = TRUE;
    dev_elim[2] = TRUE;

    /*build the appropriate pointers*/
    for(i = 0; i < dev_nsol; i++) {
	for(j = 0; j < dev_nsol; j++) {
	    si = dev_sol[i];
	    sj = dev_sol[j];
	    facblk[si][sj] = TRUE;
	    switch( blktype[si][sj] ) {
	    case B_NONE :
		bia[si][sj] = NULL;
		birow[si][sj] = NULL;
		bjrow[si][sj] = NULL;
		baoff[si][sj] = 0;
		break;
	    case B_DIAG :
		bia[si][sj] = ia_d;
		birow[si][sj] = irow_d;
		bjrow[si][sj] = jrow_d;
		baoff[si][sj] = aoff_d;
		break;
	    case B_TRI :
	    case B_BLCK :
		bia[si][sj] = ia;
		birow[si][sj] = irow;
		bjrow[si][sj] = jrow;
		baoff[si][sj] = aoff;
		break;
	    }
	}
    }
}







/************************************************************************
 *									*
 *	ckt_scram( reorder) - This routine renumbers the circuit info	*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
ckt_scram( reorder)
    int *reorder;
{
    int i, j;

    for(i = 0; i < nn; i++) nd2cont[i] = -1;


    /*change all the triangle node pointers in existence*/
    for(i = 0; i < n_con; i++) {
	for(j = 0; j < contacts[i].np; j++) {
	    contacts[i].ndc[j] = reorder[ contacts[i].ndc[j] ];
	    nd2cont[ contacts[i].ndc[j] ] = i;
	}
    }

    /*and that's all there is to it. */
    return(0);
}


/************************************************************************
 *									*
 *	ckt_ia_bld( ) Get the circuit connections set up.		*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
ckt_ia_bld( )
{
    register int i, j, k, ij, c, p, row;
    short *taken;
    int num;

    /*first assign the current and voltage numbers to the contacts*/
    for(i = 0; i < n_con; i++) {
	contacts[i].volnod = 2 * i;
	contacts[i].curnod = 2 * i + 1;
    }

    /*get the device circuit block set*/
    ia_cn = salloc(int, 2 * n_con * (nn+1));
    taken = salloc(short, nn);

    /*build the list of circuit to voltage node connections*/
    ij = ia_cn[0] = nn+1;
    for(i = 0; i < n_con; i++)  {

	row = 2 * i;

	/*build the list of the neighbors to this node*/
	/*start with a zero list*/
	num = 0;

	/*two forks here - they connections of the nodes to circuit, and vice versa*/
	if ( (c = pt2cont[ nd[row]->pt ]) != -1 ) {
	    taken[num++] = contacts[c].volnod;
	    taken[num++] = contacts[c].curnod;
	}
	if ( (c = pt2cont[ nd[row+1]->pt ]) != -1 ) {
	    taken[num++] = contacts[c].volnod;
	    taken[num++] = contacts[c].curnod;
	}
	for(j = 0; j < contacts[i].np; j++) {
	    p = contacts[i].pt[j];
	    for(k = 0; k < pt[p]->nn; k++)
		taken[num++] = pt[p]->nd[k];
	}

	/*clean the list up*/
	clean_list( taken, &num );

	/*now write it into ia*/
	for(j = 0; j < num; j++) if ( taken[j] > row ) ia_cn[ij++] = taken[j];
	ia_cn[row+1] = ij;
	row = row + 1;
	for(j = 0; j < num; j++) if ( taken[j] > row ) ia_cn[ij++] = taken[j];
	ia_cn[row+1] = ij;
    }
    /*the rest*/
    for(i = row+1; i < nn; i++) ia_cn[i+1] = ia_cn[i];
    ao_cn = ia_cn[nn] - ia_cn[0];

    /*circuit block*/
    ia_cc = salloc(int, nn+1 + 2 * n_con);
    ij = ia_cc[0] = nn+1;
    for(i = 0; i < n_con; i++) {
	row = 2 * i;
	ia_cc[ij++] = row + 1;
	ia_cc[row+1] = ij;
	ia_cc[row+2] = ij;
    }
    for(i = 2*n_con; i < nn; i++) ia_cc[i+1] = ia_cc[i];
    ao_cc = ia_cc[nn] - ia_cc[0];
    free( ia_cn );
    free( taken );
}



