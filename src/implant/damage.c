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
/*   damage.c                Version 5.1     */
/*   Last Modification : 7/3/91 12:08:33 */

#include <stdio.h>
#include <math.h>


#include "sysdep.h"
#include "constant.h"
#include "global.h"	
#include "geom.h"
#include "material.h"	
#include "implant.h"
#include "impurity.h"
#ifdef ardent 
#define ERFC(A) ((A>0)?(erfc(A)):(1.0+erf(A)))
#else
#define ERFC(A) erfc(A)
#endif



/*
 *	Data supplied from Hobler and Selberherr
 */
#define DB 0
#define DC 1
#define DD 2
#define DP 3
#define DE 4
double a1d[4][5] = {
      {  1.21,  -0.0161,  7850,    0.0852, 47.7 },
      {  0.174,  0.00213,  188,    0.571,  44.2 },
      {  0.0833, 0.00051,  -14.5,  0.529,   7.34 },
      {  0.808, -0.0435,    -6.54, 0.660,   9.10 }
      };
double a2d[4][5] = {
      {  0.166, -0.00118,  219,    0.643,  77.9 },
      { -0.131,  0.00148,    8.97, 1.040,  56.9 },
      { -2.40,   0.153,      4.75, 0.971,   5.21 },
      { -2.680,  0.176,      4.48, 0.913,   5.01 }
      };
double a3d[4][5] = {
      { -0.395,  0.00251,   92.58, 0.272,  51.2 },
      { -0.372,  0.00710,  414,    0.217,  27.2 },
      { -0.931,  0.00448,    3.14, 0.985,   6.89 },
      { -1.930,  0.147,      3.06, 0.917,   4.34 }
      };
double nvd[4][5] = {
      { -0.341,  0.00270,  130,    0.307,  38.6 },
      { -0.169,  0.00083,  418,    0.364,  74.6 },
      { -0.0489,-0.00033,   52.9,  0.765,  28.2 },
      { -0.0560, 0.00020,   58.8,  0.768,  46.8 }
      };


/************************************************************************
 *									*
 *	setdam( imp, energy, damstat ) - Calculate the damage stats	*
 *  for the given ion and energy.					*
 *									*
 *  Original:	MEL	7/87						*
 *									*
 ************************************************************************/
setdam( imp, energy, damstat )
int imp;
double energy;
double *damstat;
{
    int in;
    double a, b, c, d, p, e0, e;
    double a1, a2, a3, nv, z0, z1, c1, c2, wt;
    double sqpi = sqrt( PI / 2.0 );
    double sq2  = sqrt( 2.0 );

    /*convert the impurity number to the data table*/
    switch(imp) {
	case B:  in = 0;
		 break;
	case P:  in = 1;
		 break;
	case As: in = 2;
		 break;
	case Sb: in = 3;
		 break;
    }

    /*compute the a1 term*/
    b = a1d[in][DB];
    c = a1d[in][DC];
    d = a1d[in][DD];
    p = a1d[in][DP];
    e0 = a1d[in][DE];
    a = d * p * exp( log(e0) * (p - 1) ) - (3.0 * c * e0 + 2.0 * b) * e0;
    e = ((c * e0 + b) * e0 + a) * e0 - d * exp( log(e0) * p );
    if ( energy < e0 ) 
	a1 = ((c * energy + b) * energy + a) * energy;
    else 
	a1 = d * exp( log(energy) * p ) + e;
    /*convert from angstroms to microns*/
    a1 *= 1.0e-4;

    /*compute the a2 term*/
    b = a2d[in][DB];
    c = a2d[in][DC];
    d = a2d[in][DD];
    p = a2d[in][DP];
    e0 = a2d[in][DE];
    a = d * p * exp( log(e0) * (p - 1) ) - (3.0 * c * e0 + 2.0 * b) * e0;
    e = ((c * e0 + b) * e0 + a) * e0 - d * exp( log(e0) * p );
    if ( energy < e0 ) 
	a2 = ((c * energy + b) * energy + a) * energy;
    else 
	a2 = d * exp( log(energy) * p ) + e;
    /*convert from angstroms to microns*/
    a2 *= 1.0e-4;

    /*compute the a3 term*/
    b = a3d[in][DB];
    c = a3d[in][DC];
    d = a3d[in][DD];
    p = a3d[in][DP];
    e0 = a3d[in][DE];
    a = d * p * exp( log(e0) * (p - 1) ) - (3.0 * c * e0 + 2.0 * b) * e0;
    e = ((c * e0 + b) * e0 + a) * e0 - d * exp( log(e0) * p );
    if ( energy < e0 ) 
	a3 = ((c * energy + b) * energy + a) * energy;
    else 
	a3 = d * exp( log(energy) * p ) + e;
    /*convert from angstroms to microns*/
    a3 *= 1.0e-4;

    /*compute the number generated term*/
    b = nvd[in][DB];
    c = nvd[in][DC];
    d = nvd[in][DD];
    p = nvd[in][DP];
    e0 = nvd[in][DE];
    a = d * p * exp( log(e0) * (p - 1) ) - (3.0 * c * e0 + 2.0 * b) * e0;
    e = ((c * e0 + b) * e0 + a) * e0 - d * exp( log(e0) * p );
    if ( energy < e0 ) 
	nv = ((c * energy + b) * energy + a) * energy;
    else 
	nv = d * exp( log(energy) * p ) + e;

    /*compute the other terms*/
    z0 = a2 - a3 * a3 / a1;
    z1 = a2 - a3 * a3 / (2.0 * a1);

    /*c1,c1 are different dependent on light or heavy ions*/
    if ( ((imp == B) && (energy > 20.0)) || ((imp == P) && (energy > 55.0)) ) {
	c1 = a1*(exp(z0/a1) - 1) + a3*sqpi*exp(z1/a1)*ERFC((z0-a2)/(sq2*a3));
	c1 = 1.0 / c1;
	c2 = c1 * exp( z1/ a1 );
	wt = 0.0;
    }
    else if ( (imp == B) || (imp == P) ) {
	c2 = a3 * sqpi * ERFC( - a2 / (sq2 * a3) );
	c2 = 1.0 / c2;
	c1 = 0.0;
	wt = 2.0;
    }
    else if ( (imp == As) || (imp == Sb) ) {
	c2 = -a1*exp((z0-z1)/a1) + a3*sqpi*(2.0-ERFC(a2/(sq2*a3))-ERFC((z0-a2)/(sq2*a3)));
	c2 = 1.0 / c2;
	c1 = c2 * exp( -z1 / a1 );
	wt = 1.0;
    }
    /*convert from microns-1 to centi-1*/
    c1 *= 1.0e4;
    c2 *= 1.0e4;

    /*store the values away*/
    damstat[DAM_A1] = a1;
    damstat[DAM_A2] = a2;
    damstat[DAM_A3] = a3;
    damstat[DAM_NV] = nv;
    damstat[DAM_C1] = c1;
    damstat[DAM_C2] = c2;
    damstat[DAM_Z0] = z0;
    damstat[DAM_WT] = wt;
}




/************************************************************************
 *									*
 *	dam_vert( depth, damstat ) - Get the vertical component of the  *
 *  damage profile.							*
 *									*
 *  Original:	MEL	7/87						*
 *									*
 ************************************************************************/
double dam_vert( depth, damstat )
double depth;
double *damstat;
{
    double val;

    /*for light ions*/
    if ( damstat[DAM_WT] == 0.0 ) {
	
	/*figure which half of the distribution*/
	if ( depth < damstat[DAM_Z0] ) 
	    val = damstat[DAM_C1] * exp( depth / damstat[DAM_A1] );
	else {
	    val = (depth - damstat[DAM_A2]) / damstat[DAM_A3];
	    val = damstat[DAM_C2] * exp( - val * val / 2.0 );
	}

    }
    else if ( damstat[DAM_WT] == 1.0 ) {

	/*figure which half of the distribution*/
	if ( depth > damstat[DAM_Z0] ) 
	    val = damstat[DAM_C1] * exp( depth / damstat[DAM_A1] );
	else {
	    val = (depth - damstat[DAM_A2]) / damstat[DAM_A3];
	    val = damstat[DAM_C2] * exp( - val * val / 2.0 );
	}

    }
    else if ( damstat[DAM_WT] == 2.0 ) {
	val = (depth - damstat[DAM_A2]) / damstat[DAM_A3];
	val = damstat[DAM_C2] * exp( - val * val / 2.0 );
    }

    /*scale by the damage amounts*/
    val *= damstat[DAM_NV];

    return( val );
}



/************************************************************************
 *									*
 *	dam_lat( side, ldel, rdel ) - This routine calculates the 	*
 *  lateral distribution function for lateral standard deviation side	*
 *  and distance from the right and left sides of the slice. 		*
 *									*
 *  Original:	MEL	1/87						*
 *									*
 ************************************************************************/
double dam_lat(pos, left, right, data)
double pos, left, right;
struct imp_info *data;
{
    double val;
    double siglat;

    /*get the lateral standard dev and convert to centimeters*/
    siglat = data->dam[DAM_A3] * 1.0e-4;

    /*calculate the value */
    val = IMP_LAT( siglat, pos-left, pos-right );

    return( val );

}


