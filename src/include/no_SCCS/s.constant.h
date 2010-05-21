h12917
s 00001/00001/00055
d D 2.1 93/02/03 15:13:16 suprem45 2 1
c Initial IV.GS release
e
s 00056/00000/00000
d D 1.1 93/02/03 15:12:04 suprem45 1 0
c date and time created 93/02/03 15:12:04 by suprem45
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
/*   constant.h                Version 5.1     */
/*   Last Modification : 7/3/91  %U */

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

/*some useful constants for boolean expressions*/
#define TRUE 1
#define FALSE 0

/*some global generally used sizes of arrays and structures*/

/*the maximum number of different materials that may be present*/
#define MAXMAT 10

/*the maximum number of regions > MAXMAT*/
#define MAXREG 100

/*define the number of impurities*/
D 2
#define MAXIMP 30
E 2
I 2
#define MAXIMP 50
E 2

/*a useful boltzmann constant value*/
#define KB  8.61738e-5
#define kb  8.61738e-5

#ifndef MAXFLOAT
#define MAXFLOAT        1.0e38		/* Maximum floating point on Vax. */
#endif
#define LARGE		1.0e19		/* Large but can be manipulated.  */
#define EPS             1.0e-6		/* Typical roundoff for unity.    */
#define PREC            1.0e-12		/* Precision around unity.	  */
#define MAYBE           -1
#define PI              3.1415926535897932
#define LOG2            0.6931471805599453

#define NIL		(char *) 0	/* avoid conflict with sdtio */

/*define the maximum dimensionality*/
#define MAXDIM 3		/*three d is max....*/

/*define the maximum number of vertices/edges of the elements*/
#define MAXVRT 3
#define MAXSID 3
E 1
