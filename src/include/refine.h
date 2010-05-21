/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *     Copyright c 1991 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *									 *
 *************************************************************************/
/*   refine.h                Version 5.1     */
/*   Last Modification : 7/3/91 08:55:19 */

/************************************************************************
 *									*
 *	refine.h - This include holds the globals for the refinement	*
 *  code.								*
 *									*
 ************************************************************************/


#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif


struct line {
    double x, y;	/*the location of the point*/
    int p;		/*the point index*/
    int map;		/*the match on the other line*/
    };

#define XC(A) pt[A]->cord[0]
#define YC(A) pt[A]->cord[1]

/*the condition of points in the etch*/
#define OUT 0
#define IN 1
#define ON 2
