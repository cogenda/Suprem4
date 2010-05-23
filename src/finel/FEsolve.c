static char sccsid[]="FEsolve.c 3.6 9/9/88 23:46:05";
/*----------------------------------------------------------------------
 *
 * FEsolve - problem independent FE solver. Maybe.
 *
 * Copyright c 1985 The board of trustees of the Leland Stanford
 *                  Junior University. All rights reserved.
 * This subroutine may not be used outside of the SUPREM4 computer
 * program without the prior written consent of Stanford University.
 *
 * Original: CSR Dec 1986
 *---------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "constant.h"
#include "global.h"
#undef EXTERN
#define EXTERN
#include "FEgeom.h"
#include "FEmath.h"


#define neq (ia[0]-1)
static int FEverbose;
extern double FEupdnorm();

#ifdef DEBUG
#ifdef CONVEX
int *FDia;
double *FDa;
int FDaoff;
int *FDil;
int FDloff;
double *FDrhs;
#endif
int *pv_ext;
#endif



#define VECTOR2 for (i = 0, k=0; i < FEnn; i++) for (j = 0; j < FEdf; j++, k++)
#define VECTOR  for (k = 0; k < FEnn*FEdf; k++)


/*-----------------FEsolve----------------------------------------------
 *----------------------------------------------------------------------*/
FEsolve( int verbose)
{
    int *ia, *il, aoff, loff;	/* Matrix map */
    double *a, *l, *rhs;	/* Matrix elements, right hand side */
    int *pv= 0;			/* Inverse ordering */
    double *solo, *deln, *delo, *derr;
    double elambda(); /* Lambda step, initial value, errnorm */
    int i, j, k;		/* Node, dof, eqn indices */
    int ContinMeth=2;		/* Which continuation method I decided on. */
    FEverbose = verbose;

    /*
     * Figure out how big a problem we have.
     */
#ifndef bobpack
    FEsymb ( &ia, &il, &aoff, &loff, &pv);
#else
    FEsymb1( &ia, &il, &aoff, &loff, &pv);
#endif
    a = salloc( double, ia[neq] + aoff); a[0] = 0;
    l = salloc( double, il[neq] + loff);
    rhs=salloc( double, neq);


#ifdef DEBUG
#ifdef CONVEX
    FDia = ia;
    FDa = a;
    FDaoff = aoff;
    FDil = il;
    FDloff = loff;
    FDrhs = rhs;
#endif
    pv_ext = pv;
#endif

    /* The first thing is to stuff the fixity conditions in the solution */
    lambda = 0;
    VECTOR2
	if( FEnd[ i]->fixity[ j] == 1)
	    FEnd[ i]->sol[ j] = FEnd[ i]->bc[ j];

    /* Solve the initial linear problem */
    noderiv = 0;     a[0] = 0;
    (void)FEnewton( ia, aoff, a, il, loff, l, rhs);
    if (FEnonloop == 1) goto done;

    /*
     * The continuation loop
     */
    solo = salloc( double, FEdf*FEnn);
    deln = salloc( double, FEdf*FEnn);
    delo = salloc( double, FEdf*FEnn);
    derr = salloc( double, FEdf*FEnn);
    VECTOR deln[k] = delo[k] = 0;

    if (ContinMeth==0)
	FEmilne( ia, aoff, a, il, loff, l, rhs, solo, deln, delo, derr);
    else if( ContinMeth==1)
	FEfdiff( ia, aoff, a, il, loff, l, rhs, solo, deln, delo, derr);
    else if( ContinMeth==2){
	FEstep(  ia, aoff, a, il, loff, l, rhs, solo, deln, delo, derr, 0.25, 0.25);
	FEstep(  ia, aoff, a, il, loff, l, rhs, solo, deln, delo, derr, 0.25, 0.5);
	FEstep(  ia, aoff, a, il, loff, l, rhs, solo, deln, delo, derr, 0.5, 1.0);
    }

    free(deln); free(delo); free(solo); free(derr);

 done:

    /* Save nodal stresses for the plotting */
    ave_stress();

    /* Set the node ordering back. Helps keep me sane */
    if (FEorder & O_RESETORDER)
	if (pv) { FEscramble( pv); free(pv); }

    free(ia);if(il!=ia)free(il);free(a);free(l);free(rhs);
}

/*-----------------CONTINUATION-----------------------------------------
 * Three different varieties of continuation.
 *----------------------------------------------------------------------*/


/*-----------------FEfdiff----------------------------------------------
 * 1) A forward difference sort of continuation.
 *----------------------------------------------------------------------*/
FEfdiff( ia, aoff, a, il, loff, l, rhs, solo, deln, delo, derr)
    int *ia, aoff, *il, loff;
    double *a, *l, *rhs;
    double *solo, *deln, *delo, *derr;
{
    double nm, odl, dl, lambda0, ctol=0.1, dp, d1, d2;
    int i, j, k;

    nm = 0;
    odl = 0;
    dl = 0.25;			/* Try being aggressive */
    do {
	lambda0 = lambda;
	VECTOR2 solo[ k] = FEnd[i]->sol[j];

	/*
	 * Solve at lambda + dl: if we can't, cut back lambda and retry.
	 */
	lambda += dl;
	if (FEverbose)
	    printf("Continuation step to lambda = %g step %g\n", lambda, dl);

#ifdef projection
	/* Try projection if the update isn't too big */
	if (dl < 1.5*odl)
	    VECTOR2 if (FEnd[i]->fixity[j] != 1)
		FEnd[i]->sol[j] = solo[k] + dl/odl* delo[k];
#endif

	/* Force recomputation of Jacobian at each lambda step */
	noderiv=0;
	if (FEnewton( ia, aoff, a, il, loff, l, rhs)<=0) {
	    printf("Continued too far, backing off.\n");
	    lambda = lambda0;
	    dl /= 4;
	    VECTOR2 FEnd[i]->sol[j] = solo[ k];
	    continue;
	}
	if (lambda > 0.999) break;

	/*
	 * At first lambda step, have no idea of error, try same again
	 */
	dp = 0; d1 = 0; d2 = 0;
	if (odl == 0) {
	    odl = dl;
	    dl  = dl;
	    VECTOR delo[ k] = FEnd[ i]->sol[ j] - solo[ k];
	}
	/*
	 * Otherwise see how linear the answer was
	 */
	else {
	    VECTOR2 deln[ k] = FEnd[ i]->sol[ j] - solo[ k];
	    VECTOR  derr[ k] = deln[ k] - dl/odl * delo[ k];
	    nm = FEupdnorm( derr);
	    dp = 0; d1 = 0; d2 = 0;
	    VECTOR dp += deln[k]*delo[k];
	    VECTOR d1 += deln[k]*deln[k];
	    VECTOR d2 += delo[k]*delo[k];
	    odl = dl;
	    VECTOR delo[ k] = deln[ k];
	    dl *= sqrt( ctol/nm);
	}
	if (FEverbose)
	    printf("Continuation upd: nm %g increase would be %g (dot %.5f)\n", nm, dl/odl, (d1*d2==0)?0 : dp*dp/(d1*d2));
	/* Never mind accuracy, we just want stability */
	if (dl > 4*odl) dl = 4*odl;
	if (dl < odl) dl = odl;
	if (lambda + dl > 1) dl = 1-lambda;
    } while (lambda < 0.999);
}


/*-----------------FEstep-----------------------------------------------
 * 2) a heuristic stepping algorithm
 *----------------------------------------------------------------------*/

FEstep( ia, aoff, a, il, loff, l, rhs, solo, deln, delo, derr, lami, lamf)
    int *ia, aoff, *il, loff;
    double *a, *l, *rhs;
    double *solo, *deln, *delo, *derr;
    double lami, lamf;
{
    double dl, lambda0;
    int i, j, k, nstep=0;
    static double min_dl = 1e-3;

    dl = lami; if (lambda + dl > lamf) dl = lamf - lambda;
    while(1) {
	lambda0 = lambda;
	VECTOR2 solo[ k] = FEnd[i]->sol[j];

	/*
	 * Solve at lambda + dl: if we can't, cut back lambda and retry.
	 */
	lambda += dl;
	if (FEverbose)
	    printf("Continuation step #%d to lambda = %g step %g\n", nstep, lambda, dl);

	/* Force recomputation of Jacobian at each lambda step */
	noderiv=0;
	if (FEnewton( ia, aoff, a, il, loff, l, rhs)<=0) {
	    printf("Continued too far, backing off.\n");
	    lambda = lambda0;
	    dl /= 2; nstep--;
	    VECTOR2 FEnd[i]->sol[j] = solo[ k];
	    if( dl < min_dl) {
		fprintf( stderr, "Newton convergence failed. Giving up...\n");
		lambda=1;
		break;
	    }
	    continue;
	}
	if (lambda > 0.9999*lamf) break;

	/* Next time, try twice as hard. */
	dl *= 2; nstep++;

	/* But don't go over the top */
	if (lambda + dl > lamf) dl = lamf-lambda;
    }
}

/*-----------------FEmilne----------------------------------------------
 * 3) Milne's device for step estimation.
 *----------------------------------------------------------------------*/
FEmilne( ia, aoff, a, il, loff, l, rhs, solo, deln, delm, derr)
    int *ia, aoff, *il, loff;
    double *a, *l, *rhs;
    double *solo, *deln, *delm, *derr;
{
    double odl, dl, lambda0, ttt, nm, ctol=0.1;
    int i,j,k;

    odl = dl = 1.0;		/* let's be aggressive */
    do {
	lambda0 = lambda;
	VECTOR2 solo[ k] = FEnd[i]->sol[j];

	/*
	 * Solve at lambda + dl: if we can't, cut back lambda and retry.
	 */
	lambda += dl;
	VECTOR2 FEnd[ i]->sol[ j] += dl/odl * deln[ k];

	if (FEverbose)
	    printf("Continuation step to lambda = %g step %g\n", lambda, dl);

	noderiv = 0;
	if (FEnewton( ia, aoff, a, il, loff, l, rhs)<=0) {
	    printf("Continued too far! Backing off\n");
	    lambda = lambda0;
	    dl /= 2;
	    VECTOR2 FEnd[i]->sol[j] = solo[ k];
	    continue;
	}
	ttt = FEupdnorm( deln);
	VECTOR2 deln[ k] = FEnd[ i]->sol[ j] - solo[ k];
	if (ttt != 0)
	    printf("update vector : guess vector %g %g\n", FEupdnorm( deln)/ttt,
		   dl/odl);

	if (lambda > 0.999) break;

	/*
	 * Solve at lambda + dl/2, using interpolated initial guess.
	 */
	lambda = lambda0 + dl/2;
	VECTOR2 FEnd[ i]->sol[ j] = 0.5*(FEnd[ i]->sol[ j] + solo[ k]);
	if (FEverbose)
	    printf("Continuation halfstep to lambda = %g\n", lambda);

	if (FEnewton( ia, aoff, a, il, loff, l, rhs)<=0) {
	    printf("Continued too far! Backing off\n");
	    lambda = lambda0;
	    dl /= 4;
	    VECTOR2 FEnd[i]->sol[j] = solo[k];
	    continue;
	}
	VECTOR2 delm[ k] = FEnd[ i]->sol[ j] - solo[ k];

	/* Have now solved lambda0 + dl! */
	lambda = lambda0 + dl;
	VECTOR2 FEnd[ i]->sol[ j] = solo[ k] + deln[ k];

	/*
	 * Compare and calculate update.
	 */
	VECTOR derr[ k] = deln[ k]  - 2*delm[ k];
	nm = FEupdnorm( derr);
	odl = dl;
	dl *= sqrt( ctol/nm);
	if (FEverbose)
	    printf("Continuation upd: nm %g increase %g\n", nm, dl/odl);
	if (lambda + dl > 1) dl = 1-lambda;

    } while (lambda < 0.999);
}



/*-----------------FEnewton---------------------------------------------
 * Newton solver with Jacobian recyling, auto damping.
 *----------------------------------------------------------------------*/
FEnewton( ia, aoff, a, il, loff, l, rhs)
    int *ia, aoff, *il, loff;
    double *a, *l, *rhs;
{
    int loop, converge;
    int i, j, k, TmpND;
    double upd, FErhsnorm(), norm, onorm, *sol0;
    double *updv;		/* May keep update vector to scale with tk */
    double *nupd, progress;
    float ProjIter;
    static float DtoND = 3.5;	/* Full time to partial time - machine dep!!! */
    static float CutCrit=1.0e-2; /* How far to keep cutting tk - 0.5 means give up */
    static float HiNr=0.3;	/* When to stop factoring */
    static int mGrow=0;		/* How often we'll let the Newton increase */
    int        nGrow;		/* number of consecutive Newton increases */
    static int tkDroppedThisLoop;
    double tk=1;


    sol0 = salloc( double, neq);
    updv = salloc( double, neq);
    nupd = salloc( double, neq);

    /* If we come in with noderiv, need to get a rhs */
    if (noderiv)
	{ FEassmb( ia, aoff, a, rhs);}

    /* The Newton loop. */
    nGrow=0;
    for( loop = 0, converge = 0; !converge && loop< FEnonloop; loop += noderiv?1:2) {

	/* Get a new matrix? */
	if (!noderiv) {
	    FEassmb( ia, aoff, a, rhs);
	    numfac( neq, (int *) 0, 1, ia, aoff, a, il, loff, l);
	}

	/* Get an update, keep old solution */
	onorm = FErhsnorm( a, rhs);
	numbac( neq, il, loff, l, rhs);

	VECTOR2 sol0[ k] = FEnd[i]->sol[j];
	VECTOR updv[k] = rhs[k];
	upd = FEupdnorm( updv);

	/* Use upd, not cutback upd, to decide on convergence */
	/* Don't break because haven't added that last little update yet */
	converge = upd < FErel;

	tkDroppedThisLoop=0;
	while( tk > CutCrit ) {

	    /* Add the update */
	    VECTOR2
		if (FEnd[i]->fixity[j] != 1)
		    FEnd[i]->sol[j] = sol0[k] + tk*updv[ k];

	    if (converge) {
		norm = 1e-38;
	    } else {

		/* See what that does to the RHS */
		TmpND = noderiv; noderiv = 1;
		FEassmb( ia, aoff, a, rhs);
		noderiv = TmpND;

		/* Calculate new norm of rhs */
		norm = FErhsnorm( a, rhs);
	    }

	    if (FEverbose)
		printf("Newton loop %2d%c cut %8.2g upd %10.4g orhs %10.4g rhs %10.4g\n",
		       loop, noderiv?'*':' ', tk, upd, onorm, norm);

	    /* Try rescuing a NR step */
	    if (norm >= onorm && noderiv) break;

	    /* Update the relaxation factor? */
	    if( norm > onorm) {
		tk /= 4;
		tkDroppedThisLoop++;
	    }
	    else {
		if (!tkDroppedThisLoop)
		    tk = (tk < 0.5)? 2*tk : 1;
		break;
	    }
	}

	/* Random logic to decide whether to give up, refactor the Jacobian,
	   or keep going */
	progress = norm/onorm;
	if( progress >=1) {
	    if( noderiv) {
		VECTOR2 FEnd[ i]->sol[ j] = sol0[k];
		noderiv=0;
	    }
	    else {
		if( ++nGrow >= mGrow) {
		    converge = -1;
		    break;
		}
		else {		/*else give it one more try*/
		    if( tk < CutCrit) tk = 1.01*CutCrit;
		}
	    }
	} else {
	    nGrow = 0;
	    if (progress > HiNr)
		noderiv = 0;
	    else {
		ProjIter = log( FErel/upd)/log (progress);
		if (ProjIter < DtoND) noderiv = 1;
	    }
	}
    }

    free( sol0);
    free( updv);
    free( nupd);
    return( converge);
}

/*-----------------FEnorm-----------------------------------------------
 *----------------------------------------------------------------------*/
double FErhsnorm( a, rhs)
    double *a, *rhs;
{
    int i, j, k;
    double RhsNorm[MAXDOF], norm, size;

    for (j = 0; j < FEdf; j++) RhsNorm[j] = 0;

    VECTOR2 if (FEnd[ i]->fixity[j] != 1 && FEnd[i]->fixity[j] != 4) {
	size = rhs[k]/(FEabe[j]*a[k]);
	RhsNorm[j] += size*size;
    }

    for (norm = 0, j = 0; j < FEdf; j++) norm += RhsNorm[j];
    return( sqrt( norm));
}

double FEupdnorm( delta)
    double *delta;
{
    int i, j, k;
    double norm = 0, sol, upd;
    float profile=1e20;

    VECTOR2 if (FEnd[ i]->fixity[j] != 1) {
	    sol = FEnd[ i]->sol[j]; if (sol < 0) sol = -sol;
	    upd = delta[ k] / (FEabe[j]+sol); if (upd < 0) upd = -upd;
	    if( upd > profile)
		printf("%d %d %f\n",i,j,upd);
	    if( upd > norm)
		norm = upd;
	}
    return( norm);
}



/*-----------------FEsymb-----------------------------------------------
 * Generate the matrix maps.
 * Notes:
 * build_ia() already does 90% off the work of generate() - should
 * do it and eliminate generate() call.
 *----------------------------------------------------------------------*/
#define ND 32
#define GMD 64
static int *full_ia;		/* Used by FEconnect as well as FEsymb */

FEsymb( Pia, Pil, Paoff, Ploff, Ppv)
    int **Pia, **Pil, *Paoff, *Ploff, **Ppv;
{
    int *ia, *il, aoff, loff;	/* Matrix map */
    int *reorder;		/* Ordering at each step */
    int *accord, *accordn, *pv;	/* Accumulated ordering, temp copy, temp iv */
    int i, *lvls, lsize=0, nonzero, FEconnect();
    char talk[60];


    accord = salloc( int, FEnn);
    accordn = salloc( int, FEnn);
    for( i = 0; i < FEnn; i++) accord[i] = i;

    /* Choose a node ordering */
    if (FEorder & O_RCM) {
	build_ia ( &full_ia);
	reorder = salloc( int, FEnn);
	lvls = salloc( int, FEnn);
	tomcal(full_ia, reorder, lvls);
	free(lvls);
	FEscramble( reorder);
	for( i = 0; i < FEnn; i++) accordn[ i] = reorder[ accord[ i]];
	for( i = 0; i < FEnn; i++) accord[ i] = accordn[ i];
	free( reorder);
	free( full_ia);
    }
    if (FEorder & O_MF) {
	build_ia( &full_ia);
	reorder = salloc( int, FEnn);
	lsize = min_ia_fill( full_ia, reorder, FEverbose);
	FEscramble( reorder);
	for( i = 0; i < FEnn; i++) accordn[ i] = reorder[ accord[ i]];
	for( i = 0; i < FEnn; i++) accord[ i] = accordn[ i];
	free( reorder);
	free( full_ia);
    }
    /* Invert so we can later unscramble the nodes */
    pv = salloc( int, FEnn);
    for( i = 0; i < FEnn; i++)
	pv[ accord[i]] = i;
    free( accord);
    free( accordn);


    /* Generate Bank-Smith final form of ia */
    build_ia (&full_ia);
    nonzero = full_ia[FEnn] - full_ia[0];
    aoff = FEdf*FEnn + 1 + FEdf*FEdf*nonzero/2;
    ia = salloc( int, aoff);
    assert( generate( FEnn, FEdf, 0, FEconnect, &ia, &aoff) >= 0);
    free(full_ia);

    /* Compute symbolic factorization. Just guess at size of il */
    if (lsize != 0) loff = FEdf*FEdf*FEnn + 1 + FEdf*FEdf*lsize;
    else            loff = 80*FEnn*FEdf*FEdf;
    il = salloc( int, loff);
    assert(symfac( neq, (int *) 0, 1, ia, aoff, &il, &loff) >= 0);
    if (FEsymm) {
	aoff = 0;
	loff = 0;
    }
    sprintf(talk,"Viscous symfac %.3e", (float) FlopCount(il));


    /* Etc, etc */
    *Pia = ia; *Paoff = aoff; *Pil = il; *Ploff = loff;
    *Ppv = pv;
}

#ifdef bobpack

/*-----------------FEsymb1----------------------------------------------
 * The all new symbolic stuff for bobpack.
 *----------------------------------------------------------------------*/
FEsymb1( Pia, Pil, Paoff, Ploff, Ppv)
    int **Pia, **Pil, *Paoff, *Ploff, **Ppv;
{
    int *ia, *il, aoff, loff;	/* The matrix maps to be built */
    int *reorder;		/* dummy ordering */
    int i, nonzero;


    /* Call bobpack with clean arguments */
    build_ia( &full_ia);
    bob_symb( full_ia, FEdf, &ia, &aoff, &il, &loff);

    /* Dummy up the permutation vector */
    reorder = salloc( int, FEnn);
    for( i = 0; i < FEnn; i++)
	reorder[ i] = i;

    /* Point the calling arguments */
    *Pia = ia;
    *Pil = il;
    *Paoff = aoff;
    *Ploff = loff;
    *Ppv = reorder;

}
#endif

/*-----------------BUILD_IA---------------------------------------------
 * This routine generates a list of connections of each node.
 * The result is returned in sparse matrix format without further ado.
 *----------------------------------------------------------------------*/
build_ia(pia)
    int **pia;			/* The list to generate */
{
    struct IntP {int val; struct IntP *next;}
        *LinkPool, **NodeLink, *tmp, *l, *m, *bl;
    int lused, swap, ie, j, jn, k, kn, *ia, in, nlink, nel;
    FEdesc_typ *desc;


    /* Each triangle contributes FEnel*FEnel links.*/
    for (nlink = 0, ie=0; ie < FEne; ie++) {
	desc = &FEdesc[ FEelt[ ie]->desc];
	if (desc->coeff != 0)
	    nlink += desc->nel * desc->nel;
	else
	    nlink += desc->nel;
    }
    LinkPool = salloc(struct IntP, nlink);
    lused = 0;

    NodeLink = salloc(struct IntP *, FEnn);
    for (in = 0; in < FEnn; in++)
	NodeLink[in] = 0;

    /* Generate links */
    for (ie = 0; ie < FEne; ie++) {
	desc = &FEdesc[ FEelt[ ie]->desc];
	nel = desc->nel;
	if (desc->coeff != 0) {
	    for (j = 0; j < nel; j++) {
		jn = FEelt[ie]->nd[ j];
		for (k = 0; k < nel; k++) {
		    kn = FEelt[ ie]->nd[ k];
		    tmp = NodeLink[ jn];
		    NodeLink[ jn] = &LinkPool[lused++];
		    NodeLink[ jn]->val = kn;
		    NodeLink[ jn]->next = tmp;
		}
	    }
	}
	else {
	    for (j = 0; j < nel; j++) {
		jn = FEelt[ie]->nd[ j];
		tmp = NodeLink[ jn];
		NodeLink[ jn] = &LinkPool[lused++];
		NodeLink[ jn]->val = jn;
		NodeLink[ jn]->next = tmp;
	    }
	}
    }
    assert(lused == nlink);

    /* Clean up the lists: */
    for (lused = 0, in = 0; in < FEnn; in++) {

	/* Bubble sort - ok for short lists */
	for (l = NodeLink[ in]; l != 0; l = l->next) {
	    for (m = l->next; m != 0; m = m->next) {
		if (l->val > m->val) {
		    swap = l->val;
		    l->val = m->val;
		    m->val = swap;
		}
	    }
	}

	/* Now remove redundant elements */
	for (bl = NodeLink[ in], lused++; l = bl->next; )
	    if (l->val == bl->val)
		bl->next = l->next;
	    else
		{bl = l; lused++;}
    }

    /* Create the ia array */
    (*pia) = ia = salloc( int, FEnn + 1 + lused);
    ia[ 0] = FEnn+1;
    for (in = 0; in < FEnn; in++)
	for (ia[in+1] = ia[ in], l = NodeLink[ in]; l != 0; l = l->next)
	    ia[ ia[in+1]++] = l->val;

    free(LinkPool);
    free(NodeLink);
}

/*-----------------FEconnect--------------------------------------------
 * This because min_fill wants a subroutine returning the list of
 * connections of each node. Remind me to teach generate about sparse
 * matrix format some day...
 *----------------------------------------------------------------------*/

FEconnect(me, taken, num)
     int me;
     short *taken;
     int *num;
{
    int i, avail;
    avail = *num;

    *num = full_ia[ me+1] - full_ia[ me];
    if (*num >= avail) return(-1);

    for (i = 0; i < *num; i++)
	taken[i] = full_ia[ full_ia[ me] + i];

    return(0);
}

/************************************************************************
 *									*
 *	FEscramble( reorder) - This routine renumbers the nodes        	*
 *									*
 *  Original:	CSR	09/86						*
 *									*
 ************************************************************************/
FEscramble( reorder)
    int *reorder;
{
    FEnd_typ **NewNd;
    int ie, j, in, nel;

    NewNd = salloc( FEnd_typ *, FEnn);

    /*Rewire all the triangle->node pointers in existence*/
    for (ie=0; ie < FEne; ie++) {
	nel = FEdesc[ FEelt[ ie]->desc].nel;
	for (j=0; j < nel; j++) {
	    FEelt[ie]->nd[ j] = reorder[ FEelt[ ie]->nd[ j]];
    }}

    /*Rewire all the nodes*/
    for (in=0; in < FEnn; in++) {
	NewNd[ reorder[ in]] = FEnd[ in];
    }

    for (in=0; in < FEnn; in++)
	FEnd[ in] = NewNd[ in];

    /*outta here*/
    free( NewNd);
}



/*-----------------FlopCount--------------------------------------------
 * How many factorization flops?
 *----------------------------------------------------------------------*/
FlopCount(il)
    int *il;
{
    int i, count=0, vlen;
    for (i = 0; i < il[0]-2; i++) {
	vlen = il[i+1]-il[i];
	count += vlen*vlen + vlen;
    }
    return( count);
}


#ifdef goldenoldie
/*-----------------FEupdate---------------------------------------------
 *----------------------------------------------------------------------*/
FEupdate( delta, loop)
    double *delta;
    int loop;
{
    int i, j;
    double ddelta, upd, sol, metric;

    /* Do the update */
    ddelta = 0;
    for (i = 0; i < FEnn; i++){
	for (j = 0; j < FEdf; j++){

	    /*Fixity conditions are not solved. delta value is a flux*/
	    if (FEnd[ i]->fixity[ j] == 1) continue;

	    /* Update the solution, scaled by s */
	    upd = delta[ FEdf*i + j];
	    FEnd[i]->sol[j] = sol =  sol0[ FEdf*i + j] + s*upd;

	    /* Measure relative size */
	    if (sol < FEabe[j] && sol > -FEabe[j]) metric = FEabe[j];
	    else metric = sol;
	    upd /= metric;

	    ddelta += upd*upd;
	}
    }
    ddelta = sqrt(ddelta);

    if( FEverbose)
	printf("Newton loop %2d  dx %9.2e\n", loop, ddelta);

    return (ddelta < FErel);
}

/*-----------------XchSol-----------------------------------------------
 *----------------------------------------------------------------------*/
XchSol( v, which)
    double *v;
    int which;
{
#define loop	for (i = 0; i < FEnn; i++) for (j = 0; j < FEdf; j++)
    int i, j;

    switch( which) {
    case SAVE:
	loop v[ i*FEdf + j] = FEnd[i]->sol[j];
	break;
    case RESTORE:
	loop FEnd[i]->sol[j]= v[ i*FEdf + j];
	break;
    case UPD:
	loop if (FEnd[ i]->fixity[ j] != 1)
	    FEnd[i]->sol[j] += v[ i*FEdf + j];
	break;
    case DELV:
	loop
    default:
	panic("notreached");
    }
    return(0);
}

/*-----------------MXVL-------------------------------------------------
 * Sparse matrix with levels multiply (currently unused)
 *----------------------------------------------------------------------*/
mxvl(nv, ia, aoff, a, v, av, lvl)
     int nv;
     int *ia;
     int aoff;
     double *a;
     double *v;
     double *av;
     int *lvl;
{
    int i, j;

    for (i=0; i < nv; i++) av[i] = a[i]*v[i];

    for(i=0; i < nv; i++) {
	for (j = ia[i]; j < ia[i+1]; j++) {
	    if(lvl[ia[j]] != lvl[i]) continue;
	    av[i] += a[j+aoff] * v[ia[j]];
	    av[ia[j]] += a[j] *v[i];
	}
    }
}

#endif /*goldenoldie*/


#ifdef DoPreBubble

/* An interesting idea that turned out not to make a difference. */

    /* Move bubble nodes to the front */
    if( bubbleUp( &reorder)) {
	FEscramble( reorder);
	for( i = 0; i < FEnn; i++) accordn[ i] = reorder[ accord[ i]];
	for( i = 0; i < FEnn; i++) accord[ i] = accordn[ i];
	free( reorder);
    }
/*-----------------bubbleUp---------------------------------------------
 * Move bubble nodes to the front to arrange for early elimination.
 *----------------------------------------------------------------------*/
bubbleUp( Preorder)
    int **Preorder;
{
    int *reorder;
    int iBub, iNorm;		/* #bubbles, #normal nodes */
    int ie, in;			/* triangle, node walker */

    reorder = salloc(int, FEnn);
    for( in = 0; in < FEnn; in++) reorder[ in] = -1;
    iBub = 0;

    /* First count the bubble nodes */
    for( ie = 0; ie < FEne; ie++) {
	if (FEdesc[ FEelt[ ie]->desc].nel == 7) {
	    in = FEelt[ ie]->nd[6];
	    reorder[ in] = iBub++;
	}
    }
    if( iBub == 0){
	free( reorder);
	return(0);
    }

    /* Then go back and pick up the normal ones, putting them after the bubbles */
    iNorm = iBub;
    for( in = 0; in < FEnn; in++)
	if( reorder[ in] < 0)
	    reorder[ in] = iNorm++;

    assert( iNorm == FEnn);
    *Preorder = reorder;
    return( iBub);
}
#endif


/*#################### D E B U G ########################################*/
#ifdef debug
/*----------------------------------------------------------------------
 * Print the a matrix map
 *----------------------------------------------------------------------*/
Dump_ij(ia)
     int *ia;
{
    int i, j;
    for (i = 0; i < ia[ 0]; i++)
	printf ("ia[ %d] =  %d\n", i, ia[i]);

    for (j = 0; j < ia[ 0]-1; j++) {
	printf("row %d of ja: ", j);
	for (i = ia[j]; i < ia[j+1]; i++)
	    printf("%d ", ia[i]);
	printf("\n");
    }
}

/*----------------------------------------------------------------------
 * Transfer FE displacements to plotting storage.
 *----------------------------------------------------------------------*/
int DispUpd()
{
    int *pvi, i;
    pvi = salloc( int, FEnn);
    for (i = 0; i < FEnn; i++)
	pvi[ pv_ext[ i]] = i;
    FEscramble( pv_ext);
    FE2s4( 0);
    FEscramble( pvi);
    free( pvi);
    return(0);
}

/*----------------------------------------------------------------------
 * Counts how many silicon nodes for sparsity calculations.
 *----------------------------------------------------------------------*/
int CountDummy()
{
    int *mark,i,j,c=0;
    mark = salloc( int, FEnn);
    for (i = 0;i<FEnn;i++) mark[i] = 0;
    for (i=0; i < FEne; i++)
	if( FEelt[i]->mat != 3)
	    for (j=0; j < FEdesc[FEelt[i]->desc].nel; j++)
		mark[FEelt[i]->nd[j]]++;
    for(i=0; i < FEnn; i++)
	if(!mark[i]) c++;
    free(mark);
    return(c);
}

/*----------------------------------------------------------------------
 * Condition number and iterative correction estimator.
 *----------------------------------------------------------------------*/
#ifdef CONVEX			/* only have fast fullsolve on convex */
dump_A(ia,aoff, il, loff)
    int *ia, aoff, *il, loff;
{
    int lneq, i, jn, jv, jf, j, *ipvt;
    double *a, *a1, *rhs, *rhs1, *deltav, *drhs; double *l, *mega, *work, cond, diff;
    double *crhs, *arhs, mdiff, *arhs2;
    static double dtol = 1e-6;
    FILE *lu;

    if (FEnn*FEdf != ia[0]-1) {
	fprintf(stderr, "all confused\n"); fflush(stderr); return(-1);
    }
    lneq = ia[0]-1;

    a = salloc( double, ia[lneq] + aoff);
    rhs=salloc( double, lneq);
    FEassmb( ia, aoff, a, rhs);
    TICK("temp assembly");
#ifdef DumpIt
    if ((lu = fopen( "A.dump", "w")) == NULL)
        {perror("couldn't open A.dump\n"); fflush(stderr); return(-1);}

    if (fwrite( &lneq, sizeof(int), 1, lu)!=1) {
	perror("short write neq\n"); fflush(stderr); return(-1);}

    if (fwrite( &aoff, sizeof(int), 1, lu)!=1) {
	perror("short write aoff\n"); fflush(stderr); return(-1);}

    if (fwrite( ia, sizeof(int), ia[lneq]-1, lu)!=ia[lneq]-1) {
	perror("short write ia\n"); fflush(stderr); return(-1);}

    if (fwrite(  a, sizeof(double), ia[lneq]-1+aoff, lu) != ia[lneq]-1+aoff) {
	perror("short write a\n"); fflush(stderr); return(-1);}
    fclose(lu);
#else

    /* Try making a square matrix out of it */
    mega = salloc( double, lneq*lneq);
    dzero( mega, lneq*lneq);

    for (i = 0; i < lneq; i++) {
	mega[lneq*i+i] = a[i]   / a[i];
	for (j = ia[i]; j < ia[i+1]; j++) {
	    mega[lneq*i+ia[j]] = a[j]/a[ia[j]];
	    mega[lneq*ia[j]+i] = a[j+aoff]/a[i];
	}
    }

    TICK("mega set up");
    /* Factorize and estimate condition number */
    ipvt = salloc( int, lneq);
    work = salloc( double, lneq);
    for( i = 0; i < lneq; i++) ipvt[i] = 0;
    for( i = 0; i < lneq; i++) work[i] = 0;
    decomp_(&lneq,&lneq,mega,&cond,ipvt,work);
    printf("Estimated condition number %g\n", cond);
    TICK("full solve");

    /* Solve and compare to sparse solution */
    for (i = 0; i < lneq; i++) work[i] = rhs[i]/a[i];
    solve_(&lneq,&lneq,mega,work,ipvt);

    l = salloc( double, il[lneq] + loff);
    crhs=salloc( double, lneq);
    for( i = 0; i < lneq; i++) crhs[i] = rhs[i];
    numfac( lneq, (int *) 0, 1, ia, aoff, a, il, loff, l);
    numbac( lneq, il, loff, l, rhs);
    mdiff=0;
    for (i = 0; i < lneq; i++) {
	diff = fabs((rhs[i] - work[i])/(1e-60 + fabs(rhs[i])+fabs(work[i])));
	if( diff > mdiff)
	    mdiff = diff;
	if (diff > dtol)
	    printf( "%d %d %12.5g %12.5g\n", i/FEdf, i%FEdf, work[i], rhs[i]);
    }

    /* Iterative correction */
    arhs=salloc( double, lneq);
    mxv( neq, ia, aoff, 0.0, a, rhs, arhs);
    for( i = 0; i < lneq; i++)
	arhs[i] = crhs[i] - arhs[i];
    numbac( lneq, il, loff, l, arhs);
    arhs2=salloc( double, lneq);
    mxv( neq, ia, aoff, 0.0, a, work, arhs2);
    for( i = 0; i < lneq; i++)
	arhs2[i] = crhs[i] - arhs2[i];
    numbac( lneq, il, loff, l, arhs2);
    printf("diff %g update norms 0 %g 1 %g 1f %g\n", mdiff, FEupdnorm( rhs), FEupdnorm( arhs), FEupdnorm( arhs2));

    free( crhs);
    free( arhs);
    free( l);
    free( work); free( mega); free( ipvt);

#endif


    printf("got here\n"); fflush(stdout);

    free(a); free(work); free(ipvt); free( mega); free(rhs);
}

dump_A_convex()
{
    dump_A( FDia, FDaoff, FDil, FDloff);
}
#endif				/* convex */



#ifdef notdef

/*-----------------DoBandOrd--------------------------------------------
 * Quick hack to time band ordering for small problems.
 *----------------------------------------------------------------------*/
DoBandOrd(ia,a)
    int *ia;
    double *a;
{
    int *mins, *idiag;
    int i,j,row,col,off;
    double *newa, *tmp;


    mins = salloc( int, neq);
    idiag = salloc( int, neq);

    /* Find min in each column */
    for( col = 0; col < neq; col++) mins[col] = col;

    for( col = 0; col < neq; col++) {
	for (j=ia[col]; j < ia[col+1]; j++) {
	    row = ia[j];
	    if( col < mins[row])
		mins[row] = col;
	}
    }

    /* Make the "diag" array */
    idiag[0] = 0;
    for ( i = 1; i < neq; i++)
	idiag[i] = idiag[i-1] + 1 + i - mins[i];

    /* Allocate and fill up the new a matrix */
    newa = salloc( double, idiag[neq-1]+1);
    tmp = salloc( double, idiag[neq-1]+1);
    for (col = 0; col < neq; col++) {
	newa[ idiag[col]] = a[ col];
	for (j = ia[i]; j < ia[i+1]-1; j++) {
	    row = ia[j];
	    off = row - mins[col];
	    if (off < 0 || off >= idiag[i]-idiag[i-1]) {
		abort();
	    }
	    newa[ idiag[ col-1] + 1 +off] = a[j];
	}
    }

    /* Fix up for fortran */
    for (i = 0; i < neq; i++)
	idiag[i] ++;

    TICK("set up");
    vfactor_( newa, idiag, neq, tmp);
    TICK("band factorization");
}
#ifdef CONVEX
FDBandOrd()
{
    DoBandOrd(FDia,FDa);
}
#endif /* convex */
#endif /* notdef */
#endif /* debug */
