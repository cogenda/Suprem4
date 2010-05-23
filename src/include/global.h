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

extern void panic();
EXTERN char *MallocResult;
#define salloc(O,N)  ((O *)malloc((unsigned)((N)*sizeof(O))))
#define scalloc(O,N) ((O *)calloc((unsigned)(N),(unsigned)(sizeof(O))))
#define sralloc(O,N,P) ((O *)realloc((void *)(P), (unsigned)((N)*sizeof(O))))

#define sfree(P) free((char *)P)

/***************************************************
 *                                                 *
 *       Standard procedures and defines.          *
 *                                                 *
 ***************************************************/
extern int    mod();

extern int get_bool();
extern int get_int();
extern float get_float();
extern char *get_string();
#define Fetch( V, S ) if (is_specified(param,S)) V=get_float(param,S)
#define Listed( S ) ( get_bool(param,S) && is_specified(param,S) )

/***************************************************
 *                                                 *
 *              Operating Mode			   *
 *                                                 *
 ***************************************************/
#define ONED 1
#define TWOD 2
EXTERN int mode;

