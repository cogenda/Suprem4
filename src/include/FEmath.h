/*----------------------------------------------------------------------
 *
 * Math definitions for the finite element code.
 *
 * Copyright c 1985 The board of trustees of the Leland Stanford
 *                  Junior University. All rights reserved.
 * This subroutine may not be used outside of the SUPREM4 computer
 * program without the prior written consent of Stanford University.
 *
 * Original: CSR
 *---------------------------------------------------------------------*/
#define M_LU 1
#define M_ICCG 2

#define O_NONE 1
#define O_RCM 2
#define O_MF 4
#define O_LEX 8
#define O_RESETORDER 16
#define O_ND 32
#define O_GMD 64

#define F_PART 2
#define F_FULL 4
#define F_DIAG 8
#define F_IDENTITY 16

EXTERN float FErel;		/* Relative error criterion */
EXTERN float FEabe[MAXDOF];	/* Abs error crit / FErel */
EXTERN int FEsymm;		/* Is the matrix symmetric? */
EXTERN int FElinmeth;		/* What linear method */
EXTERN int FEnonloop;		/* How many nonlinear loops */
EXTERN int FEorder, FEorder_u;	/* What ordering to use */
EXTERN int FEcheck;		/* Check iccg against direct? */

EXTERN int noderiv;		/* Skip jacobian while computing residual? */
EXTERN double lambda;		/* Continuation parameter */
