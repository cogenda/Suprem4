/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   material.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:55:13 */

/************************************************************************
 *									*
 *	material.h - This file contains definitions of the material	*
 *  data.
 *									*
 ************************************************************************/

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

/* a variable to specify the type of gas in the ambient, and solid solubility*/
EXTERN int gas_type;		/*-1 for noble gas, impurity number else*/
EXTERN float gas_conc;		/*gas conc of impurity, -1 for solid sol*/
#define GAS_ARGON 1
#define GAS_DRYO2 10
#define GAS_WETO2 11
#define GAS_Sb 4
#define GAS_As 2
#define GAS_B  5
#define GAS_P  3
#define GAS_Be 31
#define GAS_Mg 33
#define GAS_Se 35
#define GAS_Si 37
#define GAS_Sn 39
#define GAS_Ge 41
#define GAS_Zn 43
#define GAS_C  45
#define GAS_G  47
#define oxidizing (gas_type == GAS_DRYO2 || gas_type == GAS_WETO2)

EXTERN char *MatNames[MAXMAT]
#ifdef STATIC_ALLOCATION_TIME
    = {"/gas", "/oxide", "/nitride", "/silicon", "/poly", "/oxynitride",
       "/aluminum", "/photoresist" , "/gaas"}
#endif
;       

EXTERN char *BCNames[3]
#ifdef STATIC_ALLOCATION_TIME
    = {"/reflect", "/backside", "/exposed"}
#endif
;
    
/*define numbers for the different types of insulators*/
#define SiO2	1
#define SiNi	2	/*nitride*/
#define OxNi    5	/*oxynitride*/
#define PhRs	7	/*photoresist*/

/*top surface material number - gas contact*/
#define GAS	0

/*define numbers for different types of semiconductors*/
#define Si	3
#define Poly	4
#define GaAs	8

/*let's not leave metals out*/
#define Al	6

/*don't compute field terms in these*/
#define INSULATE(A)	(((A) != Si) && ((A) != Poly) && ((A) != GaAs))

EXTERN double mat_const[MAXMAT][25]; 		/*material constant values*/
EXTERN char *act_a[MAXMAT][2];
EXTERN char *act_b[MAXMAT][2];

#define mattyp(A)	reg[A]->mater

EXTERN int sub_ornt;		/* the substrate orientation */
EXTERN double xcrystal[3][3];	/* transformation matrix from x,y to crstal cords */

/* ----------Oxidation parameters---------- */
#define Noxides 2		/* Support 2 oxide types, wet and dry */
#define Nornt 3			/* Support 3 orientations <100> <011> <111> */
#define Nrange 2		/* doubly activated process */
#define Or100 0
#define Or110 1
#define Or111 2

EXTERN int
    nclt, nclp;				/* how many chlorine pressure/temp entries*/

EXTERN float
    lbreak[Noxides],			/* linear break points */
    pbreak[Noxides],			/* para break points */
    
    lrate0[Noxides][Nrange],		/* linear rate prefactor */
    lratee[Noxides][Nrange],		/* linear rate activation energy */
    
    prate0[Noxides][Nrange],		/* parabolic rate prefactor */
    pratee[Noxides][Nrange],		/* parabolic rate activation energy */
    
    thox0[Noxides][Nornt],		/* thin oxide prefactor */
    thoxe[Noxides][Nornt],		/* thin oxide activation energy */
    thoxl[Noxides][Nornt],		/* thin oxide char. length */
    
    oridep[Nornt],			/* B/A relative to 111  */
    
    lpdep[Noxides],			/* linear rate pressure dependence */
    ppdep[Noxides],			/* parabolic rate p. dep. */
    pressure,				/* oxidation pressure */
    
    ClPct,				/* percent of chlorine */
    *cldt,				/* chlorine temperature columns */
    *cldp,				/* chlorine percentage rows */
    *lcldat,				/* matrix entries for b/a */
    *pcldat,				/* matrix entries for b */
					/* Fermi level dependence: */
    BAf_ebk,				/* exp (dEg/dT / k) */
    BAf_ebkr,				/* sqrt of " */
    BAf_Vp,				/* V+ energy level */
    BAf_Vpp,				/* V++ energy level */
    BAf_Vn,				/* V- energy level */
    BAf_Vnn,				/* V-- energy level */
    BAf_Kco0,				/* prefactor of coefficient */
    BAf_Kcoe,				/* exponential of coefficient */
    
    t1d,				/* one dimensional thickness */
    tinit,				/* what we started with */
    spread,				/* width of lateral distribution */
    xorg,				/* point around which to spread. */

    alpha[MAXMAT][MAXMAT];		/* alpha[SiO2][Si] = 2.2 */

/* Rates which can be computed once per diffusion, i.e. depend only
   on temperature, oxidizing species, pressure and chlorine %.
   Orientation effects, fermi level effects and thin oxide regime
   must be done nodewise because they involve local values */
EXTERN float
    B_DealGrove, A_DealGrove;

/* Another gaggle of parameters for the erfc model */
EXTERN float
    tinitN,			/* Initial nitride thickness */
    erf1_q,
    erf1_delta;
EXTERN char
    *erf_Lbb,
    *erf_H;

/* The oxidation model */
EXTERN int stress_dep;		/* Use stress dependency? */
EXTERN double Vc, Vr, Vd, Vt;	/* Viscosity, b/a, b reduction volumes */
EXTERN double Dlim;		/* Upper bound on diffusivity enhancement. */
EXTERN double STgamma;		/* Absolute value of surf tension coeff */
EXTERN int ornt_dep;		/* whether to use local orientation */
EXTERN int BAf_dep;		/* whether to use fermi level dependence */
EXTERN int SilStress;		/* whether to calculate silicon stress */

/* Should be somewhere else maybe */
EXTERN float oxide_grid;	/* Spacing between oxide layers */
EXTERN float oxide_redo;	/* Change tolerated before new solution */
EXTERN float oxide_gdt;		/* max ratio of grid motion to oxide_grid */
EXTERN float oxide_Etol;	/* earliest tolerance for choosekillnodes */
EXTERN float oxide_Ltol;	/* latest tolerance for choosekillnodes */
EXTERN float oxide_obfix;	/* how bad an obtuse can we tolerate? */
EXTERN float gloop_emax, gloop_emin, gloop_imin;
EXTERN int   norm_style;	/* use fudged normal? */

/* ----------Mechanical properties of the materials---------- */

/* viscosities get an extra entry for wet oxide
  (which should be a different material really) */
EXTERN double
    mu0[MAXMAT+1], mue[MAXMAT+1], E[MAXMAT], pr[MAXMAT], nux[MAXMAT];
EXTERN char *
    atherm[MAXMAT];		/* formula for the thermal expansion coeff */
EXTERN double
    IntStress[MAXMAT];		/* intrinsic stresses */

/*some useful definitions of the material constants*/
#define Ni(A)		mat_const[(A)][0]	/*intrinsic conc*/
#define Eps(A)		mat_const[(A)][1]	/*relative permittivity*/
#define Ni0(A)		mat_const[(A)][2]	/*pre exp of Ni*/
#define NiP(A)		mat_const[(A)][3]	/*power of T in Ni*/
#define NiE(A)		mat_const[(A)][4]	/*activation of Ni*/
#define Ncon(A)		mat_const[(A)][5]	/*conduction band density of states*/
#define Nval(A)		mat_const[(A)][6]	/*valence band density of states*/
#define Econ(A)		mat_const[(A)][7]	/*conduction band energy*/
#define Eval(A)		mat_const[(A)][8]	/*valence band energy*/
