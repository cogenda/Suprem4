/*************************************************************************
 *									 *
 *     Copyright c 1990 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *									 *
 *************************************************************************/
/*   skel.h                Version 5.1     */
/*   Last Modification : 7/3/91 08:55:22 */

/* Data base for skeletons 
   based on the tri - triangle generator data base */
#ifndef SKELDB
#define SKELDB

#define BEFORE             1
#define AFTER              0
#define OFFSET_ELEC     -1024	/* Distinguish BC from triangles by offset*/

/***************************************************
 *                                                 *
 *            Macros                               *
 *                                                 *
 ***************************************************/
#define nF(E)    (E->iscc ? edg[E->edge]->nd[1] : edg[E->edge]->nd[0])
#define nB(E)    (E->iscc ? edg[E->edge]->nd[0] : edg[E->edge]->nd[1])
#define WALK(START,F,B) for (B=0, F=START;  F!=B; F=F->next, B=START)
#define ined(N,E) ((N)==edge[E]->n[0] || (N)==edge[E]->n[1])
#define ELEC(N)  (node[N]->elec != NO_ELEC)

/***************************************************
 *                                                 *
 *            Types.                               *
 *                                                 *
 ***************************************************/

struct slink_str    {
		int i;			/* objects being linked. */
		struct Slink *next;	
		};

struct  LLedge 	{ 
		int edge;		/* Index of edge */
		int iscc;               /* Is the edge c-c in the region*/
		struct LLedge *next;	/* Next edge in the linked list */
		struct LLedge *prev;	/* Last " */
		double ang;		/* Angle between this & prev edge. */
		struct LLedge *gt;	/* Edge with greater angle. */
		struct LLedge *lt;	/* Edge with lesser  angle. */
		};

/*skeleton region structure*/
struct  sreg_str  	{ 
		int len;		/* length of region. */
		int mat;		/* Material index of this region. */
		struct LLedge *bnd; 	/* Pointer to linked list for region. */
		struct LLedge *maxa;	/* Pointer to edge of max angle. */
		struct LLedge *mina;	/* Pointer to edge of min angle. */
		int reg;		/* the full region structure */
		};

#define mat_skl(A)	sreg[A]->mat
#define len_skl(A)	sreg[A]->len


/***************************************************
 *                                                 *
 *            Variables.                           *
 *                                                 *
 ***************************************************/


/****
   Use the following trick to declare the structure
   everywhere, but allocate storage only once.
 ****/

#ifndef STATIC_ALLOCATION_TIME
#define EXT extern
#else
#define EXT
#endif

/*number of, and skeleton region data*/
EXT int nsreg;			
EXT struct sreg_str *sreg[MAXREG];

EXT int debug1,debug2;

EXT double mr;				/* Maximum spacing ratio. */
EXT double mgeom;			/* Minimum triangle geometry. */

/***************************************************
 *                                                 *
 *            Functions.                           *
 *                                                 *
 ***************************************************/
struct LLedge * eindex ();


double dist();
double intang();
double l_edge();
int lil();

double dmin();
double dmax();
double dabs();

#endif
