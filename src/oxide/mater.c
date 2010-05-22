/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   mater.c                Version 5.1     */
/*   Last Modification : 7/3/91  10:52:32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"

#define chosen(A) ((is_specified( param, (A)))&&(get_bool( param, (A))))
#   define fetch(N,V,S,A)\
    if (is_specified (param, N)) V = S*get_float (param, N) + A

/************************************************************************
 *									*
 *	c_mater() - This routine reads in the parameters for materials	*
 *  and computes the parameters necessary for the diffusion steps.	*
 *									*
 *  Original:	MEL	6/87						*
 *									*
 ************************************************************************/
c_mater( par, param )
char *par;
int param;
{
    int mat = -1;
    int wd = -1;
    int type = -1;

    /*figure out which material was picked*/
    mat = ChosenMater( par, param, 0);
    if ( mat == -1 ) {
	fprintf(stderr, "A material must be selected on the material card\n");
	return(-1);
    }
    if ( mat == SiO2 ) {
	if( is_specified( param, "wet") && get_bool( param, "wet")) wd=1;
	if( is_specified( param, "dry") && get_bool( param, "dry")) wd=0;
	if( ((is_specified( param, "visc.0")) || (is_specified( param, "visc.E")))
	   && (wd == -1)) {
	    fprintf( stderr, "Wet/dry must be specified with oxide viscosity\n");
	    return(-1);
	}
    }

    /*get the parameters for the material in question*/
    fetch("Ni.0", Ni0(mat), 1.0, 0.0);
    fetch("Ni.E", NiE(mat), 1.0, 0.0);
    fetch("Ni.Pow", NiP(mat), 1.0, 0.0);
    fetch("eps", Eps(mat), 8.854e-14, 0.0);

    /* store wet oxide in special position off end */
    /* wetox should really be a different material */
    fetch( "visc.0", mu0[ (wd<1)?mat:MAXMAT], 1.0, 0);
    fetch( "visc.E", mue[ (wd<1)?mat:MAXMAT], 1.0, 0);
    fetch( "visc.x", nux[ mat], 1.0, 0);
    fetch( "Young.m", E[ mat], 1.0, 0);
    fetch( "Poiss.r", pr[ mat], 1.0, 0);


    fetch("intrin.sig", IntStress[mat], 1.0, 0.0);
    if ( is_specified( param, "lcte") ) {
	 char *s = get_string( param, "lcte");
	 if (atherm[ mat] != 0) free( atherm[ mat]);
	 atherm[ mat] = (char *) malloc( 1+strlen(s));
	 strcpy( atherm[ mat], s);
    }

    /*get the parameters for the activation model in GaAs*/
    if( is_specified( param, "n.type") && get_bool( param, "n.type")) type=0;
    if( is_specified( param, "p.type") && get_bool( param, "p.type")) type=1;
    if( (is_specified( param, "act.a") || is_specified( param, "act.b")) && type < 0 ) {
	fprintf(stderr, "n.type or p.type must be specified with act.a\n");
	return(-1);
    }
    if( is_specified( param, "act.a") ) {
	char *s = get_string( param, "act.a");
	if (act_a[mat][type] != NULL) free(act_a[mat][type]);
	act_a[mat][type] = (char *)malloc(strlen(s)+1);
	(void)strcpy(act_a[mat][type], s);
    }
    if( is_specified( param, "act.b") ) {
	char *s = get_string( param, "act.b");
	if (act_b[mat][type] != NULL) free(act_b[mat][type]);
	act_b[mat][type] = (char *)malloc(strlen(s)+1);
	(void)strcpy(act_b[mat][type], s);
    }
    return(0);
}


/*-----------------ChosenMater------------------------------------------
 * Return the material number of whatever material is selected.
 * Original CSR 7/87
 *----------------------------------------------------------------------*/
ChosenMater( par, param, other)
     char *par;
     int param;
     int other;			/* 1 for /mater, 0 for mater */
{
    int i, offset = other? 0 : 1;
    for (i = 0; i < MAXMAT; i++)
	if (MatNames[i] && *MatNames[i] &&
	    get_bool( param, MatNames[i]+offset)) return(i);
    return(-1);
}

ChosenBC( par, param, other)
     char *par; int param; int other;
{
    int i, offset = other? 0:1;
    for (i = 0; i <3; i++)
	if (get_bool( param, BCNames[i]+offset)) return( i + BC_OFFSET );
    return( -1 + BC_OFFSET );
}



/************************************************************************
 *									*
 *	comp_mat() - This routine computes the temperature dependent	*
 *  material parameters used in the diffusion solver.			*
 *									*
 *  Original:	MEL	6/87						*
 *									*
 ************************************************************************/

comp_mat( temp )
float temp;
{
    register int i;

    for(i = 0; i < MAXMAT; i++)
	Ni(i) = Ni0(i) * exp( log(temp) * NiP(i) ) * exp( -NiE(i) / (kb * temp) );
    Ni( GAS ) = 1.0;

    set_crystal();
}


/*-----------------SET_CRYSTAL------------------------------------------
 * Take the substrate orientation and calcualte the transformation matrix.
 * Original CSR 7/87
 *----------------------------------------------------------------------*/
set_crystal()
{
    float ortho, norm; int i, j; double swap;
#   define X 0
#   define Y 1
#   define Z 2
#   define dot3(A,B) ((A)[X]*(B)[X] + (A)[Y]*(B)[Y] +(A)[Z]*(B)[Z])
#   define cross3(A,B,C) (C)[X] = (A)[Y]*(B)[Z] - (A)[Z]*(B)[Y];\
			 (C)[Y] = (A)[Z]*(B)[X] - (A)[X]*(B)[Z];\
			 (C)[Z] = (A)[X]*(B)[Y] - (A)[Y]*(B)[X];

    /* The substrate to crystal xform matrix */
    /* First get the vertical direction */
    switch( sub_ornt) {
      case Or100: xcrystal[1][X] = 1; xcrystal[1][Y] = 0; xcrystal[1][Z] = 0;
	      break;
      case Or110: xcrystal[1][X] = 0; xcrystal[1][Y] = 1; xcrystal[1][Z] = 1;
	      break;
      case Or111: xcrystal[1][X] = 1; xcrystal[1][Y] = 1; xcrystal[1][Z] = 1;
	      break;
      default: fprintf( stderr, "Warning: unknown substrate orientation, using <100>\n");
	      xcrystal[1][X] = 1; xcrystal[1][Y] = 0; xcrystal[1][Z] = 0;
	      break;
	  }

    /* Here it is assumed that mask edges are on <011_>, normally the case*/
    xcrystal[0][X] = 0; xcrystal[0][Y] = 1; xcrystal[0][Z] = -1;

    /* Check orthognality (currently redundant) */
    ortho = dot3( xcrystal[0], xcrystal[1]);
    if (ortho < -1e-6 || ortho > 1e-6) {
	fprintf( stderr, "Warning: orientation is not orthognal to <011>!\n");
	xcrystal[0][X] = 1; xcrystal[0][Y] = 0; xcrystal[0][Z] = 0;
    }

    /* Compute the third direction */
    cross3( xcrystal[0], xcrystal[1], xcrystal[2]);

    /* Normalize the lot */
    for (i = 0; i < 3; i++) {
	norm = sqrt( dot3( xcrystal[ i], xcrystal[ i]));
	for (j = 0; j < 3; j++)
           xcrystal[ i][j] /= norm;
    }

    /* And finally, transpose */
    for (i = 0; i < 3; i++)
        for (j = 0; j < i; j++) {
	    swap = xcrystal[ i][ j];
	    xcrystal[ i][ j] = xcrystal[ j][ i];
	    xcrystal[ j][ i] = swap;
	}
    return;
}

/*-----------------SupToXtal--------------------------------------------
 * Convert an XY vector to crystal coordinates
 *----------------------------------------------------------------------*/
SupToXtal( normal, vec3)
     double *normal, *vec3;
{
    double vec2[2], nlen;

    if ( mode == TWOD ) {
	/* Use the normal if given unless told otherwise */
	if (normal && ornt_dep && (nlen = hypot(normal[0], normal[1]))>1e-30) {
	    vec2[0] = normal[0] / nlen;
	    vec2[1] = normal[1] / nlen;
	}
	else {
	    /* Use vertical */
	    vec2[0] = 0;
	    vec2[1] = -1;
	}
	vec3[X] = xcrystal[X][X]*vec2[X] + xcrystal[X][Y]*vec2[Y];
	vec3[Y] = xcrystal[Y][X]*vec2[X] + xcrystal[Y][Y]*vec2[Y];
	vec3[Z] = xcrystal[Z][X]*vec2[X] + xcrystal[Z][Y]*vec2[Y];
    }
    else if ( mode == ONED ) {
	vec2[0] = 0;
	vec2[1] = -1;
	vec3[X] = xcrystal[X][X]*vec2[X] + xcrystal[X][Y]*vec2[Y];
	vec3[Y] = xcrystal[Y][X]*vec2[X] + xcrystal[Y][Y]*vec2[Y];
	vec3[Z] = xcrystal[Z][X]*vec2[X] + xcrystal[Z][Y]*vec2[Y];
    }
}



/*-----------------DoOriDep---------------------------------------------
 * Calculate orientation dependence of some variable.
 * Algorithm is to calculate a function of 3-d orientation, then
 * use that value to interpolate between the same function of the three
 * known orientations.
 * Original: CSR 8/87
 *----------------------------------------------------------------------*/
float DoOriDep( dir, valOri)
    double *dir;		/* Crystal direction */
    float valOri[3];		/* value in three known directions */
{
				/* Known values: */
    				/*100,   110,      111*/
    static float fval[ Nornt] = {1.0, 0.70710678, 0.57735027};
    int i;
    float xsq=dir[0]*dir[0], ysq=dir[1]*dir[1], zsq=dir[2]*dir[2],
          len = xsq+ysq+zsq, func;

    if (len == 0) {
	fprintf( stderr, "DoOriDep: bad direction supplied\n");
	return(1.0);
    }

    /* Normalize */
    xsq /= len; ysq /= len; zsq /= len;

    /* Calculate a symmetric function */
    func = sqrt( xsq*xsq + ysq*ysq + zsq*zsq );

    /* Interpolate between known values */
    i = -1;
    if (func >= fval[0]) return( valOri[0]);
    if (func <= fval[0] && func >= fval[1]) i = 0;
    if (func <= fval[1] && func >= fval[2]) i = 1;
    if (func <= fval[2]) return( valOri[2]);
    if (i<0) panic("bad interpolation");
    return( valOri[i] +
	   (valOri[i+1]-valOri[i]) * (func-fval[i]) / (fval[i+1]-fval[i]));
}

