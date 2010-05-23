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
material(line_bound)
    int line_bound;
{
    register int ir;	/*current triangle*/
    register int sr;


    xgNewSet();
    xgSetName("Materials");
    for(ir = 0; ir < nreg; ir++) {
	sr = skel_reg(ir);
	reg_pl(sr);
	free_skel(sr);
    }
}
