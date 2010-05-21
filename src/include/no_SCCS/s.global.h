h62291
s 00003/00001/00072
d D 2.1 93/02/03 15:13:21 suprem45 2 1
c Initial IV.GS release
e
s 00073/00000/00000
d D 1.1 93/02/03 15:12:07 suprem45 1 0
c date and time created 93/02/03 15:12:07 by suprem45
e
u
U
f e 0
t
T
I 1
/*************************************************************************
 *									 *
 *   Original : MEL         Stanford University        Sept, 1984	 *
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   global.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:55:06 */

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

#define TRUE 1
#define FALSE 0


/***************************************************
 *                                                 *
 *            Standard variables.                  *
 *                                                 *
 ***************************************************/
#define V_QUIET  10
#define V_NORMAL 100
#define V_CHAT   1000
#define V_BARF   10000
EXTERN int verbose;

/***************************************************
 *                                                 *
 *            Standard macros.                     *
 *                                                 *
 ***************************************************/
#define FOR(i,l,u) for(i = (l); i <= (u); i++)
#define max(a,b) (((a) > (b))?(a):(b))
#define min(a,b) (((a) < (b))?(a):(b))
#define strequ(a,b) (!strcmp(a,b))

extern panic();
EXTERN char *MallocResult;
#define salloc(O,N) (O *) ((MallocResult =  (char *)malloc((unsigned)((N)*sizeof(O)))) ? MallocResult : (char *)panic("Out of memory"))
#define scalloc(O,N) (O *) ((MallocResult =  (char *)calloc((unsigned)(N),(unsigned)(sizeof(O)))) ? MallocResult : (char *)panic("Out of memory"))
#define sralloc(O,N,P) (O *) ((MallocResult = (char *)realloc((char *)P, (unsigned)((N)*sizeof(O)))) ? MallocResult : (char *)panic("Out of memory"))

#define sfree(P) free((char *)P)

/***************************************************
 *                                                 *
D 2
 *            Standard procedures.                 *
E 2
I 2
 *       Standard procedures and defines.          *
E 2
 *                                                 *
 ***************************************************/
extern int    mod();

extern get_bool();
extern get_int();
extern float get_float();
extern char *get_string();
I 2
#define Fetch( V, S ) if (is_specified(param,S)) V=get_float(param,S)
#define Listed( S ) ( get_bool(param,S) && is_specified(param,S) )
E 2

/***************************************************
 *                                                 *
 *              Operating Mode			   *
 *                                                 *
 ***************************************************/
#define ONED 1
#define TWOD 2
EXTERN int mode;

E 1
