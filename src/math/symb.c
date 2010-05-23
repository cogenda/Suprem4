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
/*   symb.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:44:21 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "diffuse.h"
#include "impurity.h"
#include "material.h"
#include "matrix.h"
#define MAXDOF 3		/* too painful to do right */
#include "FEmath.h"		/* for FErel */
#include "poly.h"		/* for FErel */

#define TURNEDON( A, B ) (is_specified(A,B) && get_bool(A,B))

/************************************************************************
 *									*
 *	This file implements the symbolic card.  The card accepts a 	*
 *  parameter for calculating minimum fill.  It also accepts parameters	*
 *  specifying the type of linear solver to use.			*
 *									*
 ************************************************************************/



/************************************************************************
 *									*
 *	symb( par, param ) - this card handles config parameters 	*
 *  for the system.							*
 *									*
 *	Original :	Mark E. Law	Oct, 1984			*
 *									*
 ************************************************************************/
method( par, param )
char *par;
int param;
{
    char *s;
    float tmpf;
    int imp = -1;

    /*get the impurity type for all this stuff*/
    if ( TURNEDON( param, "vacancies" ) )  	imp = V;
    if ( TURNEDON( param, "interstitial" ) )	imp = I;
    if ( TURNEDON( param, "arsenic" ) )		imp = As;
    if ( TURNEDON( param, "phosphorus" ) )  	imp = P;
    if ( TURNEDON( param, "antimony" ) ) 	imp = Sb;
    if ( TURNEDON( param, "boron" ) )  		imp = B;
    if ( TURNEDON( param, "oxidant" ) )  	imp = O2;
    if ( TURNEDON( param, "velocity" ) )  	imp = XVEL;
    if ( TURNEDON( param, "traps" ) )  		imp = T;
    if ( TURNEDON( param, "gold" ) )  		imp = Au;
    if ( TURNEDON( param, "psi" ) ) 		imp = Psi;
    if ( TURNEDON( param, "cesium" ) ) 		imp = Cs;
    if ( TURNEDON( param, "electron" ) ) 	imp = N;
    if ( TURNEDON( param, "hole" ) ) 		imp = H;
    if ( TURNEDON( param, "circuit" ) ) 	imp = CKT;
    if ( TURNEDON( param, "gallium" ) ) 	imp = Ga;
    if ( TURNEDON( param, "beryllium" ) ) 	imp = iBe;
    if ( TURNEDON( param, "magnesium" ) ) 	imp = iMg;
    if ( TURNEDON( param, "selenium" ) ) 	imp = iSe;
    if ( TURNEDON( param, "isilicon" ) ) 	imp = iSi;
    if ( TURNEDON( param, "tin" ) ) 		imp = iSn;
    if ( TURNEDON( param, "germanium" ) ) 	imp = iGe;
    if ( TURNEDON( param, "zinc" ) ) 		imp = iZn;
    if ( TURNEDON( param, "carbon" ) ) 		imp = iC;
    if ( TURNEDON( param, "generic" ) ) 	imp = iG;

    /*the impurity dependent quantities*/
    if ( imp != -1 ) {

	/*the error bounds*/
	if (is_specified(param, "rel.error")) LTE[imp] = get_float(param, "rel.error");
	if (is_specified(param, "abs.error")) ABE[imp] = get_float(param, "abs.error");

    }

    /*should we perform a minimum fill?*/
    if ( is_specified( param, "min.fill" ) )
	methdata.minfill  = get_bool( param, "min.fill");
    if ( is_specified( param, "min.freq" ) )
	methdata.minfreq  = get_float( param, "min.freq");

    /*how to solve the blocks?*/
    if ( TURNEDON( param, "guass")) methdata.block = SOR;
    if ( TURNEDON( param, "cg"))    methdata.block = ICCG;


    /*how often do we factor the matrix in time solutions???*/
    if ( TURNEDON( param, "time"))  methdata.factor = RF_TIM;
    if ( TURNEDON( param, "err"))   methdata.factor = RF_ERR;
    if ( TURNEDON( param, "newton"))   methdata.factor = RF_ALL;

    /*what should we use as the preconditioner?*/
    if ( TURNEDON( param, "diag"))  methdata.fill = DIAG;
    if ( TURNEDON( param, "knot"))   methdata.fill = NONE;
    if ( TURNEDON( param, "full.fac"))   methdata.fill = FULL;

    /*how many back vectors for cg methods*/
    if ( is_specified( param, "back" ) )methdata.back = get_int( param, "back" );

    /*the size of the initial time step*/
    if ( is_specified( param, "init.time" ) )
	methdata.init_time = get_float( param, "init.time" );

    /*the type of timestep to use*/
    if ( TURNEDON( param, "trbdf"))   methdata.timemeth = TRBDF;

    if (s = get_string( param, "formula")) {
	strcpy( methdata.dtform, s );
	methdata.timemeth = FORM;
    }

    /*fetch the model of the defects*/
    if ( TURNEDON( param, "fermi"))  {
	impur[I].flags = PSEUDO | DIFFUSING | MOBILE | STEADY | LOCKSTEP;
	impur[V].flags = PSEUDO | DIFFUSING | MOBILE | STEADY | LOCKSTEP;
	methdata.defmodel = FERMI;
    }
    if ( TURNEDON( param, "two.d"))  {
	impur[I].flags = DIFFUSING | MOBILE;
	impur[V].flags = DIFFUSING | MOBILE;
	methdata.defmodel = TWODIM;
    }
    if ( TURNEDON( param, "steady")) {
	impur[I].flags = DIFFUSING | MOBILE | STEADY | LOCKSTEP;
	impur[V].flags = DIFFUSING | MOBILE | STEADY | LOCKSTEP;
	methdata.defmodel = STEADY;
    }
    if ( TURNEDON( param, "full.cpl")) {
	impur[I].flags = DIFFUSING | MOBILE;
	impur[V].flags = DIFFUSING | MOBILE;
	methdata.defmodel = FULLCPL;
    }

    /*do we need to solve poisson*/
    impur[Psi].flags = PSEUDO | DIFFUSING | MOBILE | STEADY | LOCKSTEP;

    /*get the oxide model setup*/
    if ( mode != ONED ) {
	if( TURNEDON( param, "erfc"))     oxide_model = O_ANALYTIC;
	if( TURNEDON( param, "erf1"))     oxide_model = O_ERF1;
	if( TURNEDON( param, "erf2"))     oxide_model = O_ERF2;
	if( TURNEDON( param, "erfg"))     oxide_model = O_ERFG;
	if( TURNEDON( param, "vertical")) oxide_model = O_VERTICAL;
	if( TURNEDON( param, "compress")) oxide_model = O_ELASTIC;
	if( TURNEDON( param, "viscous")) oxide_model = O_VISCOUS;
    }

    if( is_specified( param, "grid.oxide")){
	oxide_grid = 1e-4*get_float( param, "grid.oxide");
	if( oxide_grid < 10e-8 && oxide_grid != 0) {
	    fprintf( stderr, "grid.oxide (%g) below 10A is unsafe, resetting to 0.1u\n",oxide_grid);
	    oxide_grid = 0.1e-4;
	}
    } else {
	/* Assume zero for analytic. */
	if (oxide_model == O_ANALYTIC || oxide_model == O_ERF1 ||
	    oxide_model == O_ERF2     || oxide_model == O_ERFG)
	    oxide_grid = 0.0;
	else
	    /* Get default otherwise */
	    if( oxide_grid == 0)
		oxide_grid = 1e-4*get_float(param, "grid.oxide");
    }

    /*get the grain grid parameter*/
    if( is_specified( param, "grid.grain")){
        grain_grid = 1e-4*get_float( param, "grid.grain");
        if( grain_grid < 10e-8 && grain_grid != 0) {
            fprintf( stderr, "grid.oxide (%g) below 10A is unsafe, resetting to 0.1u\n",grain_grid);
            grain_grid = 0.01e-4;
        }
    } else {
            if( grain_grid == 0)
                grain_grid = 4.0e-6;
    }

    if( is_specified( param, "redo.oxide"))
	oxide_redo = 0.01*get_float( param, "redo.oxide");

    if( is_specified( param, "oxide.rel"))
	FErel = get_float( param, "oxide.rel");

    if( is_specified( param, "oxide.gdt"))
	oxide_gdt = get_float( param, "oxide.gdt");

    if( is_specified( param, "grain.gdt"))
        grain_gdt =  get_float( param, "grain.gdt");

    if( is_specified( param, "oxide.early"))
	oxide_Etol = get_float( param, "oxide.early");

    if( is_specified( param, "oxide.late"))
	oxide_Ltol = get_float( param, "oxide.late");

    if( oxide_Ltol <= 0 || oxide_Etol <= 0 || oxide_Etol > oxide_Ltol) {
	fprintf( stderr, "Bad values for oxide.early (%g) and oxide.late (%g), resetetting\n", oxide_Etol, oxide_Ltol);
	oxide_Etol = 0.5;
	oxide_Ltol = 0.9;
    }

    if( is_specified( param, "gloop.emax")) {
	tmpf = get_float( param, "gloop.emax");
	if( tmpf < 90.1)
	    fprintf( stderr, "gloop.emax must be more than 90 degrees!\n");
	else
	    gloop_emax = cos( tmpf/180*PI);
    }
    if( is_specified( param, "gloop.emin")) {
	tmpf = get_float( param, "gloop.emin");
	if( tmpf < 90.1)
	    fprintf( stderr, "gloop.emin must be more than 90 degrees!\n");
	else
	    gloop_emin = cos( tmpf/180*PI);
    }
    if( is_specified( param, "gloop.imax")) {
	tmpf = get_float( param, "gloop.imax");
	if( tmpf < 90.1)
	    fprintf( stderr, "gloop.imax must be more than 90 degrees!\n");
	else
	    gloop_imin = cos( tmpf/180*PI);
    }

    /*skip silicon in stress calculation?*/
    if (is_specified( param, "skip.sil")) {
	SilStress = !get_bool( param, "skip.sil");
    }

    /*get the iteration limit*/
    if ( is_specified( param, "blk.itlim" ) )
	methdata.blkitlim = get_int( param, "blk.itlim" );

    /*for the pairs - change them up*/
    if ( imp == O2 ) {
	LTE[H2O] = LTE[O2];
	ABE[H2O] = ABE[O2];
    }
    if ( imp == XVEL ) {
	LTE[YVEL] = LTE[XVEL];
	ABE[YVEL] = ABE[XVEL];
    }
}



/************************************************************************
 *									*
 *	do_symb() - This routine computes the symbolic setup and 	*
 *  allocates array space for the current number of impurities.		*
 *									*
 *  Original:	MEL	7/85						*
 *									*
 ************************************************************************/
do_symb(nsol, sol, il, l, loff)
int nsol;	/*number of solution variables being worked with*/
int *sol;	/*the locations*/
int **il;	/*the il index array for this set of variables*/
double **l;	/*the l array for this set of variables*/
int *loff;	/*the offset*/
{
    register int si, i, j;
    struct tms before, after; 	/*arrays for timing of the algorithms*/
    int byte, cnt;

    /*set up each block*/
    for(byte = 0, i = 0; i < nsol; i++) {
	si = sol[i];

	/*the workspace array*/
	if (wrkspc[si] != NULL) {free( wrkspc[si] ); wrkspc[si] = NULL; }
	wrkspc[si] = (double *)calloc( nn, sizeof( double ) );

	/*if we are using the cg block method allocate space for its arrays*/
	if ( methdata.block == ICCG ) {
	    cnt = 0;

	    for(j = 0; j < MAXBACK; j++) {
		if (p[j][si] != NULL) {free( p[j][si] ); p[j][si] = NULL;}
		if (ap[j][si] != NULL) {free( ap[j][si] ); ap[j][si] = NULL;}
		p[j][si]   = (double *)calloc( nn, sizeof( double ) );
		ap[j][si]  = (double *)calloc( nn, sizeof( double ) );
		cnt += 2;
	    }

	    if (r[si] != NULL) {free( r[si] ); r[si] = NULL;}
	    if (qinvr[si] != NULL) {free( qinvr[si] ); qinvr[si] = NULL;}
	    if (aqinvr[si] != NULL) {free( aqinvr[si] ); aqinvr[si] = NULL;}
	    r[si]      = (double *)calloc( nn, sizeof( double ) );
	    qinvr[si]  = (double *)calloc( nn, sizeof( double ) );
	    aqinvr[si] = (double *)calloc( nn, sizeof( double ) );
	    cnt += 3;

	    byte += cnt * sizeof( double ) * nn;
	}
    }

    /*do a symbolic factorization*/
    times(&before);

    /*guess a length*/
    if ( nsol > 0 ) {
	if ( *il ) free( *il );
	*loff = 20 * nn * nsol;
	*il = (int *)malloc( *loff * sizeof(int) );

	/*factor the the thing symbolically*/
	(void)symfac(nn, sol, nsol, ia, aoff, il, loff);

	if ( verbose >= V_BARF )
	    printf("il length %d\n", il[0][nsol*nn] + *loff );

	if ( *l ) free( *l );
	*l = (double *)malloc( (il[0][nsol*nn] + *loff + 1) * sizeof(double) );

	times(&after);

	print_time("symbolic factorization", &before, &after);
    }

    if (verbose >= V_CHAT)
	printf("symbolic allocation %d bytes\n", byte);
}



 /************************************************************************
 *									*
 *	scramble( reorder) - This routine renumbers the nodes        	*
 *									*
 *  Original:	CSR	09/86						*
 *									*
 ************************************************************************/
scramble( reorder)
    int *reorder;
{
    int t, i, p, e, r;
    struct nd_str **newnd;

    newnd = salloc( struct nd_str * , nn );

    /*change all the triangle node pointers in existence*/
    for(t = 0; t < ne; t++) {
	for(i = 0; i < nvrt; i++) {
	    tri[t]->nd[i] = reorder[ tri[t]->nd[i] ];
	    /*tri[t]->nd0[i] = reorder[ tri[t]->nd0[i] ];*/
	}
    }

    /*change all the edge node pointers in existence*/
    for(e = 0; e < ned; e++) {
	nd_edg(e,0) = reorder[ nd_edg(e,0) ];
	nd_edg(e,1) = reorder[ nd_edg(e,1) ];
    }

    /*change all the edge node pointers in existence*/
    for(r = 0; r < nreg; r++) {
	for(i = 0; i < num_nd_reg(r); i++) {
	    nd_reg(r,i) = reorder[ nd_reg(r,i) ];
	}
    }

    /*change all the point node pointers in existence*/
    for(p = 0; p < np; p++) {
	for(i = 0; i < pt[p]->nn; i++) {
	    pt[p]->nd[i] = reorder[ pt[p]->nd[i] ];
	}
    }

    /*generate the reverse of the reorder list, set up indices*/
    for(i = 0; i < nn; i++) newnd[ reorder[i] ] = nd[ i ];

    /*copy the new list over*/
    for(i = 0; i < nn; i++)
	nd[i] = newnd[i];

    /*and that's all there is to it. */
    free(newnd);
    return;
}

/************************************************************************
 *									*
 *	get_connect(ind, taken, num) - This routine returns a sorted	*
 *  list of the neighbor nodes at a single passed node.  The list is 	*
 *  returned in taken and the number of neighbors is returned in num.	*
 *									*
 ************************************************************************/

get_connect(ind, taken, num)
int ind;	/*the node index*/
short *taken;	/*the result matrix*/
int *num;	/*the number of neighbors found: initially max.*/
{
    int i;

    *num = 0;

    /*there is always coupling between the different solutions at a node*/
    *num = num_neigh_nd(ind);
    for(i = 0; i < *num; i++) taken[i] = neigh_nd(ind, i);

    return(0);
}

/************************************************************************
 *									*
 *	clean_list( taken, num) - This routine sorts and removes 	*
 *  duplicate entries from a list of nodes.				*
 *									*
 ************************************************************************/

clean_list(taken, num)
    short *taken;
    int *num;
{
    int i, j, swap, ndup;

    ndup = *num;

    /*sort the neighbor list (bubble sort since lists are short)*/
    for(i = 0; i < ndup; i++) {
	for(j = i + 1; j < ndup; j++) {
	    if ( taken[i] > taken[j] ) {
		swap = taken[i];
		taken[i] = taken[j];
		taken[j] = swap;
	    }
	}
    }

    /*eliminate duplicates*/
    *num=1;
    for(i = 1; i < ndup; i++)
	if (taken[i] != taken[*num -1])
	    taken[ (*num)++] = taken[ i];

}


double norm2( arr, n )
double *arr;
int n;
{
    double f;
    int i;

    f = 0.0;

    for(i = 0; i < n; i++)  {
	f += arr[i] * arr[i];
    }

    f = sqrt( f / n );
    return( f );
}



