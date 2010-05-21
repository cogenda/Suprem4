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
/*   plot.h                Version 5.1     */
/*   Last Modification : 7/3/91 08:55:16 */

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

/*downward compatibility for a while*/
EXTERN char *label;
EXTERN char *title;
EXTERN int sel_log;
EXTERN int znn;
EXTERN float *z;

/*function for quick sort of floats*/
extern compar();

/*following stuff is all used in the three dimensional code*/
/*definitions of the corners we are looking at*/
#define RIGHT 0x01
#define LEFT  0x02
#define UPPER 0x04
#define LOWER 0x08

/*This structure is used to hold the data for 1d extracter*/
struct d_str {
    float x;
    float y;
    int mat;
};

/*type of one d plot to make*/
#define XSEC 1
#define YSEC 2
#define ZSEC 3
#define BND  4

/*plot mins and maxs*/
EXTERN float xmin, xmax;
EXTERN float ymin, ymax;

/*viewport mins and maxs*/
EXTERN float vxmin, vxmax, vymin, vymax;

/*is the y axis upside down??*/
EXTERN int yflip;

EXTERN int pl_debug;
