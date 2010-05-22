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

#ifndef GlobalH
/* Already defined in include/global.h - DClark 19Oct98 */
#ifdef STATIC_ALLOCATION_TIME
  #ifdef EXTERN
  #undef EXTERN
  #endif
  #define EXTERN
#else
  #define EXTERN extern
#endif /* STATIC_ALLOCATION_TIME */
#endif /* GlobalH */


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

