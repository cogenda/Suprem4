/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   coeffox.c                Version 5.1     */
/*   Last Modification : 7/3/91  10:52:26 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "material.h"
#include "diffuse.h"

/************************************************************************
 *									*
 * oxcoeff.c - coefficient reader 					*
 *									*
 ************************************************************************/


/* -- Piece included from Oxidant.c for storage of segregation coeffs -- */

/*
 * Oxidant diffusion and reaction coefficients in SiO2 are derived
 * from the Deal-Grove coeffs, whose constants are stored in material.h
 * These numbers are mostly to allow people to diffuse oxidant in
 * say thin nitrides.
 */

/*definitions for impurity constants as a function of material*/
/*most useful when M==SiO2 and S={O2,H2O}*/
#define N1(S,M)	impur[S].constant[M][0][0] /*oxidant atoms/cc material*/
#define HN(S,M) impur[S].constant[M][1][0] /*Henry's law coeff*/

#define D0(S,M) impur[S].constant[M][2][0] /*pre ex constant of diffusivity*/
#define DE(S,M) impur[S].constant[M][3][0] /*energy constant of diffusivity*/

/*coefficients which are boundary - related. */
#define Seg0(S,M1,M2) 	impur[S].seg[0][M1][M2]
#define SegE(S,M1,M2) 	impur[S].seg[1][M1][M2]
#define Trn0(S,M1,M2) 	impur[S].seg[2][M1][M2]
#define TrnE(S,M1,M2) 	impur[S].seg[3][M1][M2]

/*-----------------COEFFOX----------------------------------------------
 * Read the oxide coefficients.
 * Original: CSR 4/86
 *----------------------------------------------------------------------*/

coeffox (par, param)
    char *par;
    int param;
{
    int ornt=(-1);	/* the orientation for which the coefficient applies */
    int spes;		/* which oxidizing species we mean */
    int mat, mat2;	/* what material the coefficient applies to. */
    int dw;		/* index: 0 for O2, 1 for H2O */
    int tmp;
    float oxyconc;
    char *s;

#   define given(a) is_specified( param, a)
#   define fetch(N,V,S,A)\
    if (is_specified (param, N)) V = S*get_float (param, N) + A
#   define fetchd(N,V,S,A,p1,p1n,p2,p2n)\
    if (is_specified( param, N)) {\
        if (p1 >= 0 && p2 >= 0)  V = S*get_float (param, N) + A;\
        else fprintf( stderr, "When specifying %s you must also specify %s%s%s\n", \
		     N, p1n, (*p2n)?" and ":"", p2n);}
#   define chosen(A) is_specified(param, A) && get_bool(param, A)

    /*
     * 1d stuff which comes first cos it doesn't depend on anything
     */
    fetch ("initial", tinit, 1e-4, 0);
    fetch ("spread", spread, 1.0, 0);
    fetch ("mask.edge", xorg, 1e-4, 0);

    fetch( "nit.thick", tinitN, 1e-4, 0);
    fetch( "erf.q",    erf1_q, 1e-4, 0);
    fetch( "erf.delta", erf1_delta, 1e-4, 0);
    if( is_specified( param, "erf.lbb")) {
	s = get_string( param, "erf.lbb");
	if (erf_Lbb) free( erf_Lbb);
	erf_Lbb = (char *) malloc( 1 + strlen(s));
	strcpy( erf_Lbb, s);
    }
    if( is_specified( param, "erf.h")) {
	s = get_string( param, "erf.h");
	if (erf_H) free( erf_H);
	erf_H = (char *) malloc( 1 + strlen(s));
	strcpy( erf_H, s);
    }

    /*
     * The fermi level model which depends not on orienation or dry/wet
     */
    if( given("baf.dep"))
	BAf_dep = get_bool( param, "baf.dep");
    if( given("baf.ebk")) {
	BAf_ebk = get_float( param, "baf.ebk");
	BAf_ebkr = sqrt( BAf_ebk);
    }
    fetch( "baf.pe", BAf_Vp, 1, 0);
    fetch( "baf.ppe", BAf_Vpp, 1, 0);
    fetch( "baf.ne", BAf_Vn, 1, 0);
    fetch( "baf.nne", BAf_Vnn, 1, 0);
    fetch( "baf.k0", BAf_Kco0, 1, 0);
    fetch( "baf.ke", BAf_Kcoe, 1, 0);

    /*
     * The chlorine dependent model
     */
    fetch( "hcl.pc", ClPct, 1, 0);

    if( given("hclT")) {
	nclt= -1; cldt=0;
	get_array( param, "hclT", &cldt, &nclt);
	if( !mono_array( cldt, nclt, "hclT"))
	    nclt = 0;
	for( tmp = 0; tmp < nclt; tmp++)
	    cldt[ tmp] += 273;
     }
    if( given("hclP")) {
	nclp= -1; cldp=0;
	get_array( param, "hclP", &cldp, &nclp);
	if( !mono_array( cldp, nclp, "hclP"))
	    nclp = 0;
    }
    if( given("hcl.par")) {
	tmp=nclt*nclp;
	get_array( param, "hcl.par", &pcldat, &tmp);
    }
    if( given("hcl.lin")) {
	tmp=nclt*nclp;
	get_array( param, "hcl.lin", &lcldat, &tmp);
    }

    /*
     * Other parameters: what material?
     */
    mat = ChosenMater( par, param, 0);
    mat2 = ChosenMater( par, param, 1);

    /* Volume expansion coefficient */
    if (given( "alpha")) {
	if (mat >= 0 && mat2 >= 0) {
	    fetch("alpha", alpha[ mat][mat2], 1, 0);
	    alpha[ mat2][mat] = 1/ alpha[ mat][ mat2];
	}
	else fprintf( stderr, "When specifying alpha you must also specify materials 1 and 2\n");
    }

    /* find out what orientation */
    if (given("orient"))
	switch (ornt = get_int (param, "orient")) {
	  case 100: ornt = Or100; break;
	  case 110: ornt = Or110; break;
	  case 111: ornt = Or111; break;
	  default: fprintf( stderr, "Unknown orientation %d\n", ornt);
		   ornt = -1; break;
    }

    /* Orientation dependence? */
    fetchd( "ori.fac", oridep[ ornt], 1.0, 0, ornt, "orientation", 1, "");
    if (given("ori.dep"))    ornt_dep = get_bool( param, "ori.dep");

    /* Stress dependence */
    if (given("stress.dep")) stress_dep = get_bool( param, "stress.dep");
    fetch( "Vc", Vc, 1e-24, 0);
    fetch( "Vr", Vr, 1e-24, 0);
    fetch( "Vd", Vd, 1e-24, 0);
    fetch( "Vt", Vt, 1e-24, 0);
    fetch( "Dlim", Dlim, 1, 0);

    /* Surface tension term */
    fetch( "gamma", STgamma, 1, 0);

    /* The number of oxygen atoms in one cm3 of oxide */
    if (is_specified (param, "theta")) {
	oxyconc = get_float(param, "theta");

	/* convert to the number of oxidant molecules to give one cm3 etc */
	N1( O2, mat)  =   oxyconc;
	N1( H2O, mat) = 2*oxyconc;
    }

    /* What kind of oxidation? */
    spes = -1;
    if (get_bool (param, "dry")) {dw = 0; spes = O2;}	/* maybe :-) */
    if (get_bool (param, "wet")) {dw = 1; spes = H2O;}

    /* ------------------------------------------------------------ *
     * The basic B and B/A definitions
     * ------------------------------------------------------------ */

    /* temp. breaks between the lower and higher models for B/A and B*/
    fetchd ("l.break", lbreak[ dw], 1.0, 273, spes, "wet/dry", 1, "");
    fetchd ("p.break", pbreak[ dw], 1.0, 273, spes, "wet/dry", 1, "");

    /* B/A prefactor and energy in the lower and higher regimes */
    fetchd ("lin.l.0", lrate0[ dw][ 0], 1e-4/60, 0, spes, "wet/dry", 1, "");
    fetchd ("lin.l.e", lratee[ dw][ 0], 1.0,     0, spes, "wet/dry", 1, "");
    fetchd ("lin.h.0", lrate0[ dw][ 1], 1e-4/60, 0, spes, "wet/dry", 1, "");
    fetchd ("lin.h.e", lratee[ dw][ 1], 1.0,     0, spes, "wet/dry", 1, "");

    /* B prefactor and energy in the lower and higher regimes */
    fetchd ("par.l.0", prate0[ dw][ 0], 1e-8/60, 0, spes, "wet/dry", 1, "");
    fetchd ("par.l.e", pratee[ dw][ 0], 1.0,     0, spes, "wet/dry", 1, "");
    fetchd ("par.h.0", prate0[ dw][ 1], 1e-8/60, 0, spes, "wet/dry", 1, "");
    fetchd ("par.h.e", pratee[ dw][ 1], 1.0,     0, spes, "wet/dry", 1, "");

    /* Some funny numbers for Massoud's fast initial growth model */
    fetchd ("thinox.0", thox0[ dw][ ornt], 1e-4/60, 0, spes, "wet/dry", ornt, "orientation");
    fetchd ("thinox.e", thoxe[ dw][ ornt], 1.0, 0, spes, "wet/dry", ornt, "orientation");
    fetchd ("thinox.l", thoxl[ dw][ ornt], 1e-4, 0, spes, "wet/dry", ornt, "orientation");

    /* The Henry's law constant */
    fetchd ("henry.coeff", HN (spes, mat), 1.0, 0, spes, "wet/dry", mat, "material");

    /* Pressure dependence of B/A and B */
    fetchd ("l.pdep", lpdep[ dw], 1.0, 0, spes, "wet/dry", 1, "");
    fetchd ("p.pdep", ppdep[ dw], 1.0, 0, spes, "wet/dry", 1, "");

    /* And for the war-hardened physicists out there, here's a way */
    /* to specify diffusion and reaction coefficients for yourselves!*/
    fetchd( "diff.0", D0 (spes, mat), 1.0, 0, spes, "wet/dry", mat, "material");
    fetchd( "diff.e", DE (spes, mat), 1.0, 0, spes, "wet/dry", mat, "material");

    /* fetch any segregation data */
    if (given ("seg.0")) {
	if (spes >= 0 && mat >= 0 && mat2 >= 0) {
	    fetch( "seg.0", Seg0( spes, mat, mat2), 1, 0);
	    Seg0( spes, mat2, mat) = 1/Seg0( spes, mat, mat2);
	}
	else fprintf( stderr, "When specifying seg.0 you must also specify wet/dry and materials 1 and 2\n");
    }
    if (given ("seg.E")) {
	if (spes >= 0 && mat >= 0 && mat2 >= 0) {
	    fetch( "seg.E", SegE( spes, mat, mat2), 1, 0);
	    SegE( spes, mat2, mat) = -SegE( spes, mat, mat2);
	}
	else fprintf( stderr, "When specifying seg.E you must also specify wet/dry and materials 1 and 2\n");
    }
    fetchd( "trn.0", Trn0( spes, mat2, mat) = Trn0( spes, mat, mat2), 1, 0,
	   spes, "wet/dry", mat2*mat, "materials 1 and 2");
    fetchd( "trn.E", TrnE( spes, mat2, mat) = TrnE( spes, mat, mat2), 1, 0,
	   spes, "wet/dry", mat2*mat, "materials 1 and 2");

    return(0);
}


/*-----------------get_array--------------------------------------------
 * Read an array of floats from a string.
 *----------------------------------------------------------------------*/
get_array( param, name, fla, nfla)
    int param;			/* The mysterious param factor. */
    char *name;			/* The name of the parameter. */
    float **fla;			/* The array of floats. */
    int *nfla;			/* Input: how many required. Output: number used */
{
    int before = *nfla;
    parse_array( get_string( param, name), fla, nfla);
    if( before == -1 || *nfla == before) return;
    else {
	fprintf( stderr, "Wrong number of floats in parameter %s:\n", name);
	fprintf( stderr, "Number of parameters specified was %d, expected %d\n", *nfla, before);
	if( *fla != NULL) {
	    free( *fla); *fla = NULL;}
    }
}

/*-----------------parse_array------------------------------------------
 * Parse a string into an array of floats.
 *----------------------------------------------------------------------*/
parse_array( input, Pfla, nfla)
    char *input;
    float **Pfla;
    int *nfla;
{
    static char sep[] = "\t\r\n ,/:;_|";
    char *walk, *strtok();
    extern double atof();
    int mfla=0; float *fla=0;

    *nfla = 0;

    /* Walk along the list, parsing. */
    for(walk = strtok( input, sep); walk; walk = strtok( NULL, sep)) {

	/* find some storage for the value */
	if( *nfla >= mfla) {
	    if( !fla) {
		mfla = 128; fla = salloc( float, mfla);
	    } else {
		mfla *= 2; fla = sralloc( float, mfla, fla);
	    }
	}

	/* store it */
	fla[ (*nfla)++] = atof( walk);
    }

    *Pfla = fla;
}



mono_array( fla, nfla, name)
    float *fla;
    int nfla;
    char *name;
{
    int i;
    for (i = 0; i < nfla-1; i++)
	if( fla[i] >= fla[i+1]) {
	    if( name) fprintf( stderr, "Array %s is not in ascending order\n", name);
	    return(0);
	}
    return(1);
}

