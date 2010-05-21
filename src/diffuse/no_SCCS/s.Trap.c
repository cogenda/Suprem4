h03932
s 00004/00006/00070
d D 2.1 93/02/03 14:46:49 suprem45 2 1
c Initial IV.GS release
e
s 00076/00000/00000
d D 1.1 93/02/03 14:41:18 suprem45 1 0
c date and time created 93/02/03 14:41:18 by suprem45
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
 *************************************************************************/
/*   Trap.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:49:49 */


#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "matrix.h"
#include "material.h"
#include "diffuse.h"

#include "defect.h"

/************************************************************************
 *									*
 *	This file contains definitions of the Interstitial Trap 	*
 *  routines.  It includes a routine to calculate any coupling terms	*
 *  between traps and interstitials.  It also contains a routine to	*
 *  return the total number of traps.					*
 *  									*
 *  Several data base access routines are defined in the file species.c	*
 *  to make it easier for the user to implement routines and get at	*
 *  other data.								*
 *									*
 ************************************************************************/




/************************************************************************
 *									*
 *	trap() - This routine sets up the parameters of the bulk traps	*
 *  for defects.							*
 *									*
 *  Original:	MEL	11/85						*
 *									*
 ************************************************************************/
trap(par, param)
char *par;
int param;
{
    int mat;

D 2
#   define fetch( V, S ) if (is_specified(param,S)) V=get_float(param, S)
#   define listed( S ) ( get_bool(param,S) && is_specified(param,S) )

E 2
    /*get the material number specified*/
    if ( get_bool( param, "silicon" ) ) mat = Si;
    if ( get_bool( param, "oxide" ) )   mat = SiO2;
    if ( get_bool( param, "oxynitride" ) )      mat = OxNi;
    if ( get_bool( param, "poly" ) )    mat = Poly;
    if ( get_bool( param, "nitride" ) ) mat = SiNi;
    if ( get_bool( param, "gas" ) )     mat = GAS;
    if ( get_bool( param, "aluminum" ) )        mat = Al;
    if ( get_bool( param, "photoresist" ) )     mat = PhRs;
I 2
    if ( get_bool( param, "gaas" ) )     mat = GaAs;
E 2

D 2
    fetch( Tfrac0(mat), "frac.0" );
    fetch( TfracE(mat), "frac.E" );
    fetch( Ttot(mat), "total" );
E 2
I 2
    Fetch( Tfrac0(mat), "frac.0" );
    Fetch( TfracE(mat), "frac.E" );
    Fetch( Ttot(mat), "total" );
E 2

    if ( is_specified(param,"enable") ) trap_on = get_bool(param,"enable");

}
	

E 1
