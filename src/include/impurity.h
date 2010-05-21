/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *									 *
 *************************************************************************/
/*   impurity.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:55:09 */

/************************************************************************
 *									*
 *	This file contains the definitions and constants for the 	*
 *  impurity data.  This also includes the defect data.  It depends on	*
 *  constant.h being included first.					*
 *									*
 ************************************************************************/

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

/*typedef a pointer to a function, (it works, trust me)*/
typedef int (* PTR_FNC)();
typedef double (* PTR_DBLFNC)();

/*define constants for each impurity*/
#define V  0		/*Vacancies*/
#define I  1		/*Interstitials*/
#define As 2		/*Arsenic*/
#define P  3		/*Phosphorus*/
#define Sb 4		/*antimony*/
#define B  5		/*Boron*/
#define N 6		/*electron concentration*/
#define H 7		/*hole concentration*/
#define XVEL 8		/*X velocity*/
#define YVEL 9		/*Y velocity*/
#define O2 10		/*Dry O2*/
#define H2O 11		/*Wet O2*/
#define T  12		/*Interstitial Traps*/
#define Au 13		/*Gold*/
#define Psi 14		/*Potential*/
#define Sxx 15		/* Components of stress - not solution variables*/
#define Syy 16		/* but too expensive to recompute for plotting */
#define Sxy 17
#define Cs  18		/* Cesium for oxide charges */
#define DELA 19		/*change in interface area - not solution variable*/
#define Asa 20		/*Arsenic active concentration*/
#define Pa  21		/*Phosphorus active concentration*/
#define Sba 22		/*antimony active concentration*/
#define Ba  23		/*Boron active concentration*/
#define GRN 24		/*Polysilicon grain size*/
#define Ga 25		/*p-type tracer to help model miyake*/
#ifdef DEVICE
#define CKT 26		/*this is never, ever stored in nodes*/
#endif
#define iBe  31		/*Beryllium impurity*/
#define iBea 32		/*Beryllium active concentration*/
#define iMg  33		/*Magnesium impurity*/
#define iMga 34		/*Magnesium active concentration*/
#define iSe  35		/*Selenium impurity*/
#define iSea 36		/*Selenium active concentration*/
#define iSi  37		/*Silicon impurity*/
#define iSia 38		/*Silcon active concentration*/
#define iSn  39		/*Tin impurity*/
#define iSna 40		/*Tin active concentration*/
#define	iGe  41		/*Germanium impurity*/
#define	iGea 42		/*Germanium active concentration*/
#define	iZn  43		/*Zinc impurity*/
#define	iZna 44		/*Zinc active concentration*/
#define	iC   45		/*Carbon impurity*/
#define	iCa  46		/*Carbon active concentration*/
#define	iG   47		/*Generic impurity*/
#define	iGa  48		/*Generic active concentration*/

extern double charge();

/*set up an array of pointer from solution numbers to impurity number*/
EXTERN int soltoimp[MAXIMP];

/*set up an array of impurity numbers to solution indices*/
EXTERN int imptosol[MAXIMP];

/*declare an structure for each impurity of all its needed data*/
struct imp_str {
    double constant[MAXMAT][25][2];	/*diffusion and generation constants*/
    double seg[8][MAXMAT][MAXMAT];	/*the segregation coefficients*/
    int flags;				/*steady state, diffusing, or mobile*/
    PTR_DBLFNC diff_coeff;		/*routine which calculates diff coeffs*/
    PTR_FNC coupling;			/*set up impurity-impurity coupling*/
    PTR_FNC boundary;			/*set up boundary conditions*/
    PTR_FNC algebc;			/*set up boundary conditions*/
    PTR_FNC active;			/*active concentration calculator*/
    PTR_FNC time_val;			/*time term calculation function*/
    PTR_FNC block_set;			/*block jacobian terms*/
};

/*flag values and flag tests*/
#define STEADY		0x001	/*is this a time dependent variable*/
#define DIFFUSING	0x002	/*is this a spatial dependent variable*/
#define MOBILE		0x004	/*is this a variable with del * D del C terms*/
#define LOCKSTEP	0x008	/*does this have to be solved in lock step*/
#define PSIACT		0x010	/*does the active concentration depend on psi*/
#define PSEUDO		0x020	/*pseudo variable for diffusion?*/
#define	ACTIVE_ACCEPTOR	0x040	/*active acceptor*/
#define ACTIVE		0x080	/*active distribution*/
#define IMPLANTED_IMP	0x100	/*implanted impurity*/
#define ALL_FLAGS	0x1ff
#define GET_FLAGS(A)		( impur[A].flags )
#define	SET_FLAGS(A,B)		( impur[A].flags |= (B) )
#define	CLEAR_FLAGS(A,B)	( impur[A].flags &= ~(B) )
#define IS_DIFFUSE(A)		( (impur[A].flags & DIFFUSING) ? 1 : 0 )
#define IS_MOBILE(A)		( (impur[A].flags & MOBILE) ? 1 : 0 )
#define IS_STEADY(A)		( (impur[A].flags & STEADY) ? 1 : 0 )
#define IS_LOCKED(A)		( (impur[A].flags & LOCKSTEP) ? 1 : 0 )
#define IS_PSIACT(A)		( (impur[A].flags & PSIACT) ? 1 : 0 )
#define IS_PSEUDO(A)		( (impur[A].flags & PSEUDO) ? 1 : 0 )
#define IS_ACCEPTOR(A)		( (impur[A].flags & ACTIVE_ACCEPTOR) ? 1 : 0)
#define IS_ACTIVE(A)		( (impur[A].flags & ACTIVE) ? 1 : 0)
#define IS_IMPLANTED(A)		( (impur[A].flags & IMPLANTED_IMP) ? 1 : 0 )

/*defines for the different values of the seg coefficients*/
#define SEG0	0
#define SEGE	1
#define TRN0	2
#define TRNE	3

/*declare a space for each one of the impurities and its data*/
EXTERN struct imp_str impur[MAXIMP];

/*now a variable to count how many impurities we are actually solving*/
EXTERN int n_imp;

/*structure for the parameters to be passed to the boundary code routine*/
struct bound_str {
    int nx[2];			/*the node numbers involved*/
    double delta;		/*timestep*/
    float temp; 		/*temperature*/
    double vel[2]; 		/*boundary velocity*/
    double conc[2];		/*solution values*/
    double eq[2];		/*equilibrium values*/
    int mat[2];			/*material values*/
    int loc[2][2];		/*coupling locations*/
    double cpl;			/*coupling length*/
    double **rhs;		/*right hand side*/
    float cord[2];		/*location of the nodes*/
    double vmax;		/*maximum velocity*/
    double dela[2];		/*change in the area*/
} *bval;
EXTERN int bcnum;


/*what oxide model to use*/
#define O_ANALYTIC 0
#define O_VERTICAL 1
#define O_ELASTIC  2
#define O_VISCOUS  3
#define O_VISCOEL  4
#define O_ERF1     5
#define O_ERF2     6
#define O_ERFG     7
EXTERN int oxide_model;

/*the following are definitions used in the impurity routine so that future
  implementers of models can do it without memorizing the data structure.*/
extern double get_conc();

/*
 *	A - the row index
 *	B - the impurity index
 *      C - the vector to store it in
 *	D - the value to be put in
 */
#define left_side(A, B, D)  a[B][B][A] += D
#define right_side(A, B, C, D) C[B][A] += D


extern clear_row();
extern remove_time();
extern double get_area();

/*
 *	A - is the row
 *	B - imp1
 *	C - imp2
 *	D - diagonal value
 *	E - the coupling value
 */

#define add_couple(A, B, C, D, E)  a[B][B][A] += D; a[B][C][A] = E

/*define aids for use in the one/two dimensional decisions for defects*/
#define TWODIM   0x00	
#define FERMI    0x02	
#define SSTATE   0x04
#define FULLCPL  0x08

