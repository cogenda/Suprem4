h04550
s 00003/00002/00045
d D 2.1 93/02/03 15:29:33 suprem45 2 1
c Initial IV.GS release
e
s 00047/00000/00000
d D 1.1 93/02/03 15:28:46 suprem45 1 0
c date and time created 93/02/03 15:28:46 by suprem45
e
u
U
f e 0
t
T
I 1
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
/*   material.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:38:56 */

#include <stdio.h>
#include <global.h>
#include <constant.h>
#include <dbaccess.h>

/*define a macro to get the material code for the material*/
#define XCORD(A, B) (pt[ nd[ tri[A]->nd[B] ]->pt ]->cord[0])
#define YCORD(A, B) (pt[ nd[ tri[A]->nd[B] ]->pt ]->cord[1])

/************************************************************************
 *									*
 *	material() - This routine sorts through all the triangles and   *
 *  draws any sides that are material boundaries.			*
 *									*
 ************************************************************************/
D 2
material()
E 2
I 2
material(line_bound)
    int line_bound;
E 2
{
    register int ir;	/*current triangle*/
    register int sr;


D 2
    xgNewSet();
E 2
I 2
    xgNewSetLT(line_bound);
E 2
    xgSetName("Materials");
    for(ir = 0; ir < nreg; ir++) {
	sr = skel_reg(ir);
	reg_pl(sr);
	free_skel(sr);
    }
}
E 1
