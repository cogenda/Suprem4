/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   FEgeom.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:54:52 */

/************************************************************************
 *									*
 *	FEgeom.h - Definitions of FE geometry				*
 *									*
 ************************************************************************/
#define MAXDIM 3		/* 3d is max! */
#define MAXDOF 3		/* no more than 3 dof/node */
#define MAXSC 6			/* number of stress components */
#define MAXNEL 20		/* 20 nodes/element for parabolic cube */
#define MAXGP  8		/* 8 integration points in the cube */

struct FEnd_str {
    double cord[MAXDIM];	/* x,y,z coordinates */
    double sol[MAXDOF];		/* Solution values */
    int fixity[MAXDOF];		/* BC of x,y velocity */
    double bc[MAXDOF];		/* Some BC info */
    double sig[MAXSC];		/* Stress and strain */
    double eps[MAXSC];
    double garbage[20];		/* Normal vector, reaction rates, blah... */
    int valence;
    char *user;			/* Anything the user wants to put here */
    };
typedef struct FEnd_str FEnd_typ;

struct FEelt_str {
    int nd[MAXNEL];		/* The global node numbers */
    int face[6];		/* B.C. info for each face (or nbr) */
    int desc;			/* What element this is */
    int mat;			/* Material code of element */
    };
typedef struct FEelt_str FEelt_typ;

/* Descriptor for finite elements */
struct FEdesc_str {
    int nel;			/* Number of nodes per element */
    int gaussp;			/* Number of gauss points */
    int (*stiff)();
    int (*coeff)();
    int (*bound)();
    int (*nodal_stress)();
    };
typedef struct FEdesc_str FEdesc_typ;

struct FEmat_str {
    double coeff[20];		/* Routines use it as they like */
};
typedef struct FEmat_str FEmat_typ;


EXTERN FEnd_typ  **FEnd;
EXTERN FEelt_typ **FEelt;
EXTERN int FEne, FEnn, FEdf, FEdm, FEnsc;
EXTERN FEdesc_typ FEdesc[20];

#define M_ELASTIC 0
#define M_VISCOUS 1
#define M_PLASTIC 2
