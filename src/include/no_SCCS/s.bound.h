h38348
s 00000/00000/00046
d D 2.1 93/02/03 15:13:14 suprem45 2 1
c Initial IV.GS release
e
s 00046/00000/00000
d D 1.1 93/02/03 15:12:03 suprem45 1 0
c date and time created 93/02/03 15:12:03 by suprem45
e
u
U
f e 0
t
T
I 1
/*	bound.h		Version 5.1	*/
/*	Last Modification:  7/3/91 08:54:55 */


/*	bound.h		Version 1.2		*/
/*	Last Modification:	12/11/89 13:44:42		*/

/************************************************************************
 *									*
 *   Original : RYSH         Stanford University        April, 1989	*
 *									*
 *     Copyright c 1989 The board of trustees of the Leland Stanford 	*
 *                      Junior University. All rights reserved.		*
 *     This subroutine may not be used outside of the SUPREM4 computer	*
 *     program without the prior written consent of Stanford University.*
 *									*
 *									*
 *     This file declares the global min and max as well as the real    *
 *	min and max in a series of frames for imagetool			*
 *									*
 *									*
 ************************************************************************/

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

/* Global min and max specified by user if more than one frame exists */
EXTERN float gmin;
EXTERN float gmax;

/* Real min and max calculated by this program */
EXTERN float min_x;
EXTERN float max_x;
EXTERN float min_y;
EXTERN float max_y;

/* defines */
#define SUNMAXX 1150
#define SUNMAXY 900
#define MACMAXX 720
#define MACMAXY 520
#define ERROR -1

E 1
