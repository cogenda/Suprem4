h17845
s 00000/00000/00028
d D 2.1 93/02/03 15:13:24 suprem45 2 1
c Initial IV.GS release
e
s 00028/00000/00000
d D 1.1 93/02/03 15:12:08 suprem45 1 0
c date and time created 93/02/03 15:12:08 by suprem45
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
/*   key.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:55:12 */

#define TRUE 1
#define FALSE 0

extern struct par_str **current;	/*current pointer into table*/

extern struct par_str **pop();		/*routine to handle pops to stack*/
extern struct par_str **push();
extern struct bool_exp *node();

extern int depth;
extern int cardnum;
extern char depthstr[20];

extern struct par_str *cards[NUMPAR];
E 1
