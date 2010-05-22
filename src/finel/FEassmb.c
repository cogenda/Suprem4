/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   FEassmb.c                Version 5.1     */
/*   Last Modification : 7/3/91  15:42:55 */
#include "constant.h"
#define  WANT_STATIC
#include "FEgeom.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern int noderiv;

#define MELDOF MAXNEL*MAXDOF	/* degrees of freedom per element */

/************************************************************************
 *									*
 *	FEassmb() - Assemble the stiffness matrix			*
 *									*
 ************************************************************************/
FEassmb( ia, aoff, a, rhs)
    int *ia, aoff;
    double *a;
    double *rhs;
{
    static double xl[MELDOF], disp[MELDOF], bc[MELDOF];
    static int    eqns[MELDOF], fixity[MELDOF];
    double stiff[MELDOF*MELDOF], wrhs[MELDOF];
    int dummy, NotYet=1;
    FEdesc_typ *desc; FEelt_typ *pie;
    int ie, i, ndf, nel;

    /*clear out the matrix and rhs*/
    dzero( rhs, ia[0]-1);
    if (!noderiv) dzero( a, ia[ia[0]-1]+aoff);

    /*Once upfront to handle the noderiv case*/
    dzero( stiff, MELDOF*MELDOF);

    /*loop on elements*/
    for (ie = 0; ie < FEne; ie++) {
	pie = FEelt[ ie];
	desc = &FEdesc[ pie->desc];
	nel = desc->nel;
	ndf = nel*FEdf;

	/*clear out the arrays to be filled*/
	if (!noderiv) dzero( stiff, ndf*ndf);
	for (i = 0; i < ndf; i++) wrhs[ i] = 0;

	/*Change from global to local info*/
	localize( ie, xl, disp, eqns, fixity, bc);

	/* avoid dummy elements */
	dummy = (desc->stiff == 0);
	if (!dummy) {

	    /*compute element stiffness matrix*/
	    (*(desc->stiff)) (stiff, wrhs, xl, disp, desc->coeff, pie->mat);

	    /*do element boundary conditions*/
	    (*(desc->bound)) (stiff, wrhs, xl, disp, desc->coeff, pie->mat, fixity, bc, ie, NotYet);
	    NotYet = 0;
	}

	/*add to global array*/
	addstf( ia, aoff, a, rhs, nel, stiff, wrhs, eqns, fixity, dummy);
    }

    return;
}


/************************************************************************
 *									*
 *	addstf() - Add stiffness contribution to the matrix & rhs	*
 *									*
 ************************************************************************/
addstf( ia, aoff, a, rhs, nel, stiff, wrhs,  eqns, fixity, dummy)
     int *ia, aoff;
     double *a;
     double *rhs;
     int nel;
     double stiff[MELDOF*MELDOF];
     double wrhs[MELDOF];
     int eqns[MELDOF];
     int fixity[MELDOF];
     int dummy;
{
    int i, j, aloc, iaja(), eldof = FEdf*nel;
    int row, col;

    /* Put b into the RHS */
    for (i=0; i < eldof; i++)
	rhs[ eqns[ i]] += wrhs[i];

    if (noderiv) return;

    /* Diagonal elements */
    for (i=0; i < eldof; i++) {
	aloc = eqns[ i];
	if (fixity[i] != 1)
	    a[ aloc] += stiff[ eldof*i + i];
	else
	    a[ aloc] = 1.0;
    }
    if( dummy) return;

    /* Offdiagonals: walk through the lower triangle of stiff */
    for (i=0; i < eldof; i++) {
	if (fixity[i] == 1) continue;

	for (j=0; j < i; j++) {
	    if (fixity[j] == 1) continue;

	    if (eqns[j] < eqns[i]) { /*global order same as local order*/
		aloc = iaja( ia, aoff, eqns[i], eqns[j]);
		row = i;
		col = j;
	    }
	    else {		     /*global order reversed*/
		aloc = iaja( ia, aoff, eqns[j], eqns[i]);
		row = j;
		col = i;
	    }
	    assert(aloc >= 0);
	    a[ aloc] += stiff[ eldof*row + col];
	    if (aoff != 0) {
		a[ aloc + aoff] += stiff[ eldof*col + row];
	    }
	}
    }
}

#define salloc(S,N) (S *) malloc( (N)*sizeof(S))

ave_stress()
{
    static double xl[MELDOF], disp[MELDOF], bc[MELDOF];
    static int    eqns[MELDOF], fixity[MELDOF];
    double epsn[MAXSC*MAXNEL], sign[MAXSC*MAXNEL];
    double *count, xsj;
    FEdesc_typ *desc; FEelt_typ *pie;
    int ie, i, in, j, nel;

    /* Initialize */
    count = salloc(double, FEnn);
    for (in=0;in<FEnn;in++) count[in] = 0;
    for (in=0;in<FEnn;in++)
	for (j=0;j<FEnsc;j++) {
	    FEnd[ in]->sig[j] = 0;
	    FEnd[ in]->eps[j] = 0;
	}

    /*loop on elements*/
    for (ie = 0; ie < FEne; ie++) {
	pie = FEelt[ ie];
	desc = &FEdesc[ pie->desc];
	nel = desc->nel;
	if (desc->nodal_stress == 0) continue;

	localize( ie, xl, disp, eqns, fixity, bc);

	(*(desc->nodal_stress)) (xl, disp, epsn, sign, desc->coeff, pie->mat, &xsj);

	/*add to nodes*/
	for (i = 0; i < nel; i++)  {
	    in = FEelt[ ie]->nd[i];
	    for (j = 0; j < FEnsc; j++)  {
		FEnd[ in]->eps[ j] += epsn[FEnsc*i+j];
		FEnd[ in]->sig[ j] += sign[FEnsc*i+j];
	    }
	    count[in] += 1;
	}
    }

    /* Average contributions */
    for (in = 0; in < FEnn; in++) {
	if (count[ in] != 0)
	    for (j = 0; j < FEnsc; j++) {
		FEnd[ in]->sig[ j] /= count[in];
		FEnd[ in]->eps[ j] /= count[in];
	    }
    }
    free(count);

}




/*-----------------LOCALISE---------------------------------------------
 * Once of this is enough.
 *----------------------------------------------------------------------*/

localize( ie, xl, disp, eqns, fixity, bc)
     int ie;
     double *xl, *disp;
     int *eqns, *fixity;
     double *bc;
{
    register int j, kc, kf;
    int i, in, nel;
    register FEnd_typ *inp;

    nel = FEdesc[ FEelt[ ie]->desc].nel;
    kc = kf = 0;

    /*set up local arrays*/
    for (i = 0; i < nel; i++) {
	in = FEelt[ ie]->nd[i];
	inp = FEnd[ in];
	for (j = 0; j < FEdm; j++)
	    xl[ kc++]       = inp->cord[j];

	for (j = 0; j < FEdf; j++) {
	    eqns  [ kf]  = FEdf*in+ j;
	    fixity[ kf]  = inp->fixity[j];
	    disp  [ kf]  = inp->sol[j];
	    bc    [ kf]  = inp->bc[j];
	    kf++;
	}
    }
}

/* Do a formatted dump of the element stiffness matrix */
dump_stiff( stiff, rhs, nel, stride, offset)
     double *stiff, *rhs;
     int nel, stride, offset;
{
    int i, j;

    for (i = offset; i < FEdf*nel; i += stride) {
	for (j = offset; j < FEdf*nel; j+= stride)
	    printf("%12e ", stiff[ FEdf*nel*i + j]);
	printf("= %12e\n", rhs[i]);
    }
}
