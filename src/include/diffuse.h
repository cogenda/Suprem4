/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   diffuse.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:55:02 */

/************************************************************************
 *									*
 *	diffuse.h - This file contains data needed for the matrix setup	*
 *  and solve of the diffusion problem.					*
 *									*
 ************************************************************************/

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

/*set up a definition for the norm function*/
EXTERN double norm();

/*add a couple of solution vectors for the diffusion timestep algorithm*/
EXTERN double total;		/*the total time so far*/
EXTERN double *old[MAXIMP];	/*solution at last time step*/
EXTERN double *mid[MAXIMP];	/*solution at the middle of the time step leap*/
EXTERN double *new[MAXIMP];	/*solution at latest time step*/
EXTERN double *rhs[MAXIMP];	/*answer vectors for trapazoidal rule*/
EXTERN double *newb[MAXIMP];	/*answer vectors at current time point*/
EXTERN double *oldb[MAXIMP];	/*answer vectors at last time point*/

/*active concentrations and derivitives*/
EXTERN double *newa[MAXIMP];		/*active portion of each impurity*/
EXTERN double *mida[MAXIMP];		/*active portion of each impurity*/
EXTERN double *olda[MAXIMP];		/*active portion of each impurity*/
EXTERN double *dact[MAXIMP][MAXIMP];	/*partial of net w/r each imp*/
EXTERN double *equil[MAXIMP];		/*equilibrium concentrations if appropriate*/
EXTERN double *dequ[MAXIMP];		/*partial of equilibrium concentrations if appropriate*/
EXTERN double *net;		/*net concentration at each point*/
EXTERN double *dnet[MAXIMP];		/*derivitive of the net concentration at each point*/
EXTERN double *noni;		/*n over ni at each point*/

/*truncation errors as a function of impurity*/
EXTERN double LTE[MAXIMP];
EXTERN double ABE[MAXIMP];


/*the type of timestep flag*/
#define TR  1
#define BDF 2
#define SS  3

struct call_str {
    int type;
    double old_del, new_del;
    float temp;
    double **old, **mid, **new;
    double **olda, **mida, **newa;
    double *oco, *mco, *nco;
    int *sol;
    int nsol;
    int *elim;
    int *il;
    double *l;
    int loff;
    };

EXTERN struct call_str cs;

