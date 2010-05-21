/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *************************************************************************/
/*   circuit.c                Version 5.2     */
/*   Last Modification : 7/3/91 15:44:15  */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "device.h"
#include "impurity.h"
#include "matrix.h"
#include "diffuse.h"






/************************************************************************
 *									*
 *	circuit_setup() - This routine calculates the circuit values.	*
 *									*
 *  Original:	MEL	7/85						*
 *									*
 ************************************************************************/
circuit_setup(ss, temp, nsol, solve, coeff, val, nu1, nu2, nu3 )
int ss;
float temp;
int nsol;
int *solve;
double *coeff;
double **val;
double **nu1, *nu2, *nu3;
{
    register int i, ve;
    int Cl = imptosol[CKT];

    /*step through currents through contacts*/
#ifdef FOO
    for(c = 0; c < n_con; c++) {
	ve = contacts[c].volnod;
	ce = contacts[c].curnod;

	if ( ce < ve ) {
	    for( l = ia_cc[ce]; ia_cc[l] != ve; l++ );
	    vcl = l;
	    cvl = l + ao_cc;
	}
	else {
	    for( l = ia_cc[ve]; ia_cc[l] != ce; l++ );
	    vcl = l + ao_cc;
	    cvl = l;
	}


	newb[Cl][ve] += val[Cl][ce];
	a[Cl][Cl][vcl] -= 1.0;
    }
#endif

    /*step through the coltage supplies*/
    for( i = 0; i < n_vss; i++ ) {
	ve = vss[i].con;
	newb[Cl][ve] = vss[i].val - val[Cl][ve];
	a[Cl][Cl][ve] = 1.0;
    }
}

