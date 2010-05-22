/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   FEconvert.c                Version 5.1     */
/*   Last Modification : 7/3/91  10:52:20 */


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "constant.h"
#include "global.h"
#include "geom.h"
#include "FEgeom.h"
#include "material.h"
#include "impurity.h"
#include "diffuse.h"		/* for noni */


#define X 0
#define Y 1
#define D 2
#define XX 0
#define YY 1
#define XY 2
#define NEL 7			/* number of nodes in triox */
				/* don't change to 6 unless !SilStress */
/************************************************************************
 *									*
 *	FEconvert() - generate temporary data structures for FE method	*
 *	Original: CSR Sep 86						*
 *									*
 ************************************************************************/

/* Speed malloc by globbing the calls */
static  FEelt_typ *FEeltbase;
static  FEnd_typ  *FEndbase;
static  int aFEne, aFEnn;
static  double *impbase;

/* The finite element table: */
extern int write_problem(), read_problem();
extern int triox_stiff(), triox_bc(), triox_nodal_stress();
extern int FE_oxbulk(), sup4_ecoeff();
extern int tri6_stiff(), tri6_Snodal_stress();
extern int tri7_stiff(), tri7_nodal_stress();
extern int oxload();

#define TRIOX	0
#define ELAST6  1
#define ELAST7  2
#define DUMMY   3
#define SILOX   4

FEconvert( element, dof)
     int element;
     int dof;
{
    int ie, in, j, jl, jr, nbj, jnbj, *nl, is;
    double *dp, *dpl, *dpr;
    double rad, rad2, myr;
    static int CylindricalSymmetry = 0;
    int n_user = 2;		/* how much extra baggage */

    /*
     * 7node triangles need FEnn > np; this formula works if no domain holes.
     * Use slight overestimate to handle dummy elements.
     */
    FEdf = dof;
    FEdm = 2;
    FEnsc = 3;
    aFEne = 2*nn;
    aFEnn = 2*aFEne + 2*nn -1;

    /*Allocate and initialize the triangle storage*/
    FEelt = scalloc( FEelt_typ *, aFEne );
    FEeltbase = scalloc( FEelt_typ, aFEne);
    for (ie = 0; ie < aFEne; ie++)
	FEelt[ ie] = FEeltbase + ie;

    /* Similarly for the nodes */
    FEndbase = scalloc( struct FEnd_str, aFEnn);
    FEnd = scalloc( struct FEnd_str *, aFEnn );
    for (in = 0; in < aFEnn; in++)
	FEnd[ in] = FEndbase + in;

    /* Some more storage for the impurities */
    impbase = scalloc( double, n_user*aFEnn);
    for (in = 0; in < aFEnn; in++)
	FEnd[ in]->user = (char *) (impbase + n_user*in);

    /* Current values */
    FEne = ne;
    FEnn = np;

    /* Nodal coordinates and solutions*/
    for (in = 0; in < FEnn; in++) {
	FEnd[ in]->cord[ 0] = pt[ in]->cord[0];
	FEnd[ in]->cord[ 1] = pt[ in]->cord[1];

	stash_noniC( in, (double *)FEnd[ in]->user);
    }

    /* Triangle materials, corner nodes, nbrs */
    for (ie = 0; ie < FEne; ie++) {
	FEelt[ ie]->mat = mat_reg( reg_tri( ie));
	FEelt[ ie]->desc = element;
	for (j = 0; j < 3; j++)	{
	    FEelt[ ie]->nd[ j  ] = nd[ tri[ ie]->nd[ j]]->pt;
	    FEelt[ ie]->nd[ j+3] = -1;
	    FEelt[ ie]->face[ j] = tri[ ie]->nb[ j];
	}
    }

    /*generate extra nodes*/
    for (ie = 0; ie < FEne; ie++) {
	nl = FEelt[ ie]->nd;

	/*midside nodes*/
	for (j = 0; j < 3; j++) {

	    /* Nothing to do if another triangle already fixed us up */
	    if (nl[ 3+j] != -1) continue;

	    /* OK, grab the next available node */
	    nl[ 3+j] = FEnn;

	    /* Average the guys coordinates */
	    jl = nl[ (j+1)%3];
	    jr = nl[ (j+2)%3];
	    FEnd[ FEnn]->cord[0] = 0.5*(FEnd[jl]->cord[0] + FEnd[jr]->cord[0]);
	    FEnd[ FEnn]->cord[1] = 0.5*(FEnd[jl]->cord[1] + FEnd[jr]->cord[1]);

	    if (CylindricalSymmetry) {
		nbj = FEelt[ie]->face[j];
		if ((nbj < 0 && nbj > BC_OFFSET) ||
		    (nbj >= 0 && FEelt[nbj]->mat != FEelt[ie]->mat))
		{
		    rad = hypot( FEnd[jl]->cord[0], FEnd[jl]->cord[1]);
		    rad2 = hypot( FEnd[jr]->cord[0], FEnd[jr]->cord[1]);
		    if (rad < 0.999*rad2 || rad2 < 0.999*rad) panic("huh?");
		    myr = hypot( FEnd[FEnn]->cord[0], FEnd[ FEnn]->cord[1]);
		    FEnd[ FEnn]->cord[0] *= rad/myr;
		    FEnd[ FEnn]->cord[1] *= rad/myr;
		}
	    }


	    /* and solutions */
	    dp = (double *) FEnd[ FEnn]->user;
	    dpl = (double *) FEnd[ jl]->user;
	    dpr = (double *) FEnd[ jr]->user;
	    for (is = 0; is < n_user; is++)
		dp[ is] = 0.5*(dpl[is] + dpr[is]);

	    FEnn++;
	    assert( FEnn < aFEnn);

	    /* ALSO STORE NODE NUMBER IN ADJACENT TRIANGLE! */
	    nbj = FEelt[ ie]->face[ j];
	    if (nbj < 0) continue;

	    for (jnbj = 0; jnbj < 3; jnbj++)
		if (FEelt[ nbj]->face[ jnbj] == ie) break;
	    assert(jnbj < 3);

	    /*fix the corresponding node in the new grid*/
	    FEelt[ nbj]->nd[ 3+jnbj] = FEelt[ ie]->nd[ 3+j];
	}

	/*bubble node*/
	if( FEdesc[ element].nel < 7) continue;

	FEelt[ ie]->nd[ 6] = FEnn;
	FEnd[ FEnn]->cord[0] = (FEnd[nl[0]]->cord[0] + FEnd[nl[1]]->cord[0] + FEnd[nl[2]]->cord[0])/3;
	FEnd[ FEnn]->cord[1] = (FEnd[nl[0]]->cord[1] + FEnd[nl[1]]->cord[1] + FEnd[nl[2]]->cord[1])/3;
	FEnn++;
    }

    valencies();

    return(0);
}

/*-----------------DUMMY_SIL--------------------------------------------
 * Dummy up the silicon elements during oxidation,
 * or the oxide elements in subsequent postprocessing.
 *----------------------------------------------------------------------*/
dummy_sil( doSil)
    int doSil;			/* tru => simulate silicon */
{
    int ie, i, j;
    FEelt_typ *ae; FEnd_typ *an;

    for (ie = 0; ie < FEne; ie++) {
	ae = FEelt[ ie];

	/* Make it an elastic element if it's silicon postprocessing */
	/* Might as well go for 7 noded since storage is already allocated */
	if( doSil && ae->mat == Si)
	    ae->desc = SILOX;

	/* Skip if non-dummy */
	if ( (doSil && ae->mat == Si) || (!doSil && ae->mat != Si)) continue;

	ae->desc = DUMMY;
	for (i = 0; i < FEdesc[ DUMMY].nel; i++) {
	    an = FEnd[ ae->nd[ i]];
	    for (j = 0; j < FEdf; j++) {
		an->fixity[j] = 1;
		an->bc    [j] = 0;
	    }
	}
    }
}





/************************************************************************
 *									*
 *	FE2S4()	- Store computed displacements in Suprem-4		*
 *									*
 ************************************************************************/

/* Code can get executed by viscous or elastic threads (ick) */
#define SavAll 0
#define SavNoSil 1
#define SavOnlySil 2
FE2s4all() {FE2s4( SavAll);}
FE2s4ox() {FE2s4( SavNoSil);}
FE2s4sil() {FE2s4( SavOnlySil);}

FE2s4( SavWhat)
    int SavWhat;
{
    int ip, in, i, j;
    static int imps[]={XVEL, YVEL, Sxx, Syy, Sxy};
    extern double cutback;
    FEnd_typ *an; nd_typ *ja; float mult;
    pt_typ *ap;

    /* Initialize solution storage */
    for (i = 0; i < 5; i++) add_impurity( imps[i], 0.0, -1);
    for (in = 0; in < nn; in++)
	for (i = 0; i < 5; i++) nd[ in]->sol[ imptosol[ imps[i]]] = 0;


    /* Walk over the points */
    for (ip = 0; ip < np; ip++) {
	an = FEnd[ ip];
	ap = pt[ ip];

	/* for all the nodes at this point... */
	for (j = 0; j < ap->nn; j++) {
	    ja = nd[ ap->nd[ j]];

	    /* Update velocities of all nodes associated with this point*/
	    /* Oxidation problems: silicon nodes go opposite ways */
	    if (ja->mater == Si && SavWhat == SavNoSil)
		mult = -1 / (alpha[SiO2][Si] - 1);
	    else
		mult = 1;

		ja->sol[ imptosol[XVEL]] = mult * an->sol[ X];
		ja->sol[ imptosol[YVEL]] = mult * an->sol[ Y];

	    /* Update concentration: hide negative from s4, otherwise */
	    /* time step algorithm will infinite loop */
	    /* Skip in elastic case */
	    if( imptosol[ gas_type] != -1)
		ja->sol[ imptosol[ gas_type]] = max( an->sol[ D], 1.0);

	    /* Copy stresses */
	    if (SavWhat == SavAll|| (SavWhat == SavOnlySil && ja->mater == Si) ||
				    (SavWhat == SavNoSil   && ja->mater != Si)) {
		ja->sol[ imptosol[ Sxx]] = an->sig[XX];
		ja->sol[ imptosol[ Sxy]] = an->sig[XY];
		ja->sol[ imptosol[ Syy]] = an->sig[YY];
	    }
	}
    }
}

FEfree()
{
    free( FEnd );
    free( FEelt );
    free( FEeltbase);
    free( FEndbase);
    free( impbase);
}

/*-----------------VALENCIES--------------------------------------------
 * Compute nodal valencies
 *----------------------------------------------------------------------*/
valencies()
{
    int i, ie, nel, *j, *jj; FEelt_typ *ae;

    for (i = 0; i < FEnn; i++) FEnd[ i]->valence = 0;

    for (ie = 0; ie < FEne; ie++) {
	ae = FEelt[ ie];
	nel = FEdesc[ ae->desc].nel;
	/* FIX ME */
	if (ae->mat == 3) continue;

	for (j = ae->nd, jj = j+nel; j < jj; j++)
	    FEnd[ *j]->valence++;
    }
}


extern float proc_temp;

/*-----------------STASH_NONI-------------------------------------------
 * Stores suprem4's value of n/ni and current oxidant conc
 * in the finite element structure so it can be passed into Oxidant.c
 *----------------------------------------------------------------------*/
stash_noniC( in, where)
    int in;
    double where[];
{
    int Ps = imptosol[Psi];
    int is, io, iconc = imptosol[ gas_type];
    double Kt = KB * proc_temp;

    if( (is = node_mat( pt[in]->nd[0], Si)) >= 0)
	where[0] = exp( nd[ is]->sol[Ps] / Kt );
    else if( (is = node_mat( pt[in]->nd[0], Poly)) >= 0)
	where[0] = exp( nd[ is]->sol[Ps] / Kt );
    else
	where[0] = 1.0;

    if( (io = node_mat( pt[in]->nd[0], SiO2)) >= 0)
	where[1] = nd[ io]->sol[ iconc];
}

