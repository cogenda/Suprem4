/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *									 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   device.h                Version 5.1     */
/*   Last Modification : 7/3/91  10:38:29 */

/************************************************************************
 *									*
 *	This file contains definitions of useful things for the device	*
 *  solver.  								*
 *									*
 ************************************************************************/

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

#define MAXCKT 3000	/*the maximum number of circuit nodes*/

#define IS_SEMI(A) ( ((A) == Poly) || ((A) == Si) || ((A) == GaAs) )
#define EX BC_OFFSET+2
#define BC BC_OFFSET+1
#define NBR(I,J) (tri[(I)]->nb[(J)])

/*a contact is a semiconductor material touching an Exposed or backside or anything
  touching aluminum*/
#define IS_CONT(A,B) ( (IS_SEMI(mat_reg(reg_tri((A)))) && \
		       ( (NBR(A,B) == EX) || (NBR(A,B) == BC) ) ) || \
		       ( (NBR(A,B) > 0) && (mat_reg(reg_tri(NBR(A,B))) == Al ) ) )

/*Contact information*/
struct contact_str {
    char *name;		/*name of the contact*/
    int np;		/*the number of points*/
    int *pt;		/*list of the points involved*/
    int *ndc;		/*list of the contact material nodes involved*/
    int curnod;		/*circuit node for current out this contact*/
    int volnod;		/*circuit node for voltage at this boundary*/
    double conres;	/*the contact resistance in ohm/cm2*/
    };

/*ten is the most external contacts allowed*/
EXTERN struct contact_str contacts[10];
EXTERN int n_con;				/*number of contacts*/
EXTERN int *nd2cont;
EXTERN int *pt2cont;

/*the total net dopant*/
EXTERN int dopcalcdone;

/*the device width*/
EXTERN double dev_width;
EXTERN double qfn, qfp;		/*fixed quasis for sub set solutions*/
EXTERN double devVt, devVti;		

EXTERN int n_ckt;		/*the number of circuit nodes*/

/*various branch relationships*/
struct vss_str {		/*voltage supply*/
    int con; 			/*the contact it is attached to*/
    double val;			/*the voltage value of v(con)*/
    };
EXTERN int n_vss;
EXTERN struct vss_str vss[MAXCKT];

EXTERN int *dev_il;		/*column index pointers for steady state solve*/
EXTERN int dev_loff;		/*upper triangular offset for steady state solve*/
EXTERN double *dev_l;		/*the lower triangular values for steady state*/
EXTERN int dev_sol[MAXIMP];	/*solution numbers of those being worked on*/
EXTERN int dev_nsol;		/*number of those being worked on*/
EXTERN int dev_elim[MAXIMP];		/*eliminate stuff*/

/*some circuit connectivity pieces*/
EXTERN int *ia_cc, ao_cc;
EXTERN int *ia_cn, ao_cn;

/*band edges as a function of doping and material*/
EXTERN double *Ec, *Ev;

/*quasi fermi functions*/
extern double Phi_n(), Phi_p();
