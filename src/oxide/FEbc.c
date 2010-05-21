/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   FEbc.c                Version 5.1     */
/*   Last Modification : 7/3/91  10:52:18 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "constant.h"
#include "geom.h"
#include "FEgeom.h"
#include "material.h"
#define hypot(x,y) sqrt((x)*(x)+(y)*(y))

#define X 0
#define Y 1
#define D 2
#define XX 0
#define YY 1
#define XY 2

/************************************************************************
 *									*
 *	FEbc()	- boundary conditions					*
 *									*
 ************************************************************************/

/*-----------------FEoxbc-----------------------------------------------
 * Boundary conditions for viscous flow solution.
 *----------------------------------------------------------------------*/
FEoxbc()
{
    clear_bc();

    /* Mark silicon nodes as special */
    dummy_sil(0);
    sil_face();

    /* Gas node concentrations */
    gas_fixities();

    /* Reflecting BCs: last to overwrite sil_face's bc==4 info*/
    refl_disp();

    /* Ready to SOLVE! */

}

/*-----------------FEsilbc----------------------------------------------
 * Boundary conditions for silicon stress postcalculation.
 *----------------------------------------------------------------------*/
FEsilbc()
{
    clear_bc();

    /* Solve silicon and only silicon */
    dummy_sil(1);

    /* The load terms are calculated by oxload */
    /* Need to free up those nodes */
    SilOxLoad();

    /* Reflecting BCs last as usual */
    refl_disp();

    /* Go away and solve it */
}



/*-----------------FEbc-------------------------------------------------
 * Boundary conditions for thermal stress calculations.
 *----------------------------------------------------------------------*/
FEbc( temp, par, param)
    float temp;
    char *par;
    int param;
{
    clear_bc();
    FEdirichlet( par, param);
    refl_disp();
}

/*-----------------REFL_DISP------------------------------------------*
 * Set normal velocities to zero at reflecting boundaries
 *--------------------------------------------------------------------*/
refl_disp()
{
    int ie, *glob, j, nbj, nface, *fnp, *fnode, dirs[2], dir, nel;
    double *orig, *other, diff;
    FEelt_typ *me; FEnd_typ *ina;
    static int tface[3][3] = {{1,2,3},{2,0,4},{0,1,5}},
               rface[4][3] = {{0,1,4},{2,3,5},{0,2,6},{1,3,7}};

    /* For each reflecting face */
    for (ie = 0; ie < FEne; ie++) {
	me = FEelt[ ie];
	glob = me->nd;
	nel = FEdesc[ me->desc].nel;
	nface = nel/2;

	for (j = 0; j < nface; j++) {
	    nbj = me->face[ j];
	    if (nbj > BC_OFFSET+1) continue;

	    /* Figure out what coordinate is constant along this face */
	    fnode = (nface==3)? tface[j] : rface[j];
	    fnp = fnode;
	    orig = FEnd[ glob[ *fnp]]-> cord;
	    dirs[ X] = dirs[ Y] = 1;

	    /* By comparing each of the nodes coordinates to the first one */
	    for ( fnp++ ; fnp < fnode + 3; fnp++) {
		other = FEnd[ glob[ *fnp]]->cord;
		diff = other[ X] - orig[ X];
		if( diff < -1e-8 || diff > 1e-8) dirs[ X] = 0;
		diff = other[ Y] - orig[ Y];
		if( diff < -1e-8 || diff > 1e-8) dirs[ Y] = 0;
	    }
	    if( dirs[ X] + dirs[ Y] != 1 && nbj < BC_OFFSET+1) {
		fprintf(stderr, "Warning: this neumann face is not parallel to one coordinate axis!");
		fprintf(stderr, "(%g , %g) <--> (%g , %g)\n",
		    orig[0], orig[1],
		    FEnd[ glob[ *(fnode+2)]]->cord[0],
		    FEnd[ glob[ *(fnode+2)]]->cord[1]);
		continue;
	    }

	    /* Ok, now for each node on the face, set its dir velocity to 0*/
	    for( fnp = fnode; fnp < fnode+3; fnp++) {
		for (dir = X; dir <= Y; dir++)
		    if (dirs[ dir]) {
			ina = FEnd[ me->nd[ *fnp]];
			ina->fixity[ dir] = 1;
			ina->bc[     dir] = 0.0;
		    }
	    }
	}
    }
}

/*-----------------FEdirichlet------------------------------------------
 * I want to do standard problems as test cases.
 *----------------------------------------------------------------------*/
FEdirichlet( par, param)
    char *par;
    int param;
{
    int ie, j, i, k, v, c;
    FEelt_typ *ae; FEnd_typ *an;
    float string_to_real();
    char *s, *ubc[10], *vbc[10], name[256], *get_string();

    for (i = 3; i < 10; i++) {
	ubc[i] = vbc[i] = 0;
	sprintf( name, "ubc%d",i);
	if( s = get_string( param, name)) {
	    ubc[i] = (char *)malloc( 1+ strlen( s));
	    strcpy( ubc[i], s);
	}
	sprintf( name, "vbc%d",i);
	if( s = get_string( param, name)) {
	    vbc[i] = (char *)malloc( 1+ strlen( s));
	    strcpy( vbc[i], s);
	}
    }

    /*
     * Run over boundary codes.
     * This requires more passes over the triangles but allows the user
     * to specify which ones override.
     */
    for( c = 3; c< 10; c++) {
	if( !ubc[c] && !vbc[c]) continue;

	for (ie = 0; ie < FEne; ie++) {
	    ae = FEelt[ ie];
	    for( j = 0; j < 3; j++) {
		/*
		 * Interpret boundary code info
		 */
		if (ae->face[j] == BC_OFFSET+c) {
		    face_ache(ae, j);

		    /*
		     * For each node on the face
		     */
		    for( k = 1; k <= 3; k++) {
			v = (k<3)?(j+k)%3:j+k;
			an = FEnd[ ae->nd[ v]];

			fmacro( "x", an->cord[X], "%e"); fmacro( "y", an->cord[Y], "%e");

			/* Normal velocity? */
			if( ubc[c] && (ubc[c][0] == 'N')) {
			    float speed = string_to_real( ubc[c]+1,-1.0);
			    an->fixity[X] = 1;
			    an->fixity[Y] = 1;
			    an->bc[X] = speed * an->garbage[X];
			    an->bc[Y] = speed * an->garbage[Y];
			}
			/* Specified velocity */
			else {
			    if( ubc[c]) {
				an->fixity[X] = 1;
				an->bc[X] = string_to_real( ubc[c], -1.0);
			    }
			    if( vbc[c]) {
				an->fixity[Y] = 1;
				an->bc[Y] = string_to_real( vbc[c], -1.0);
			    }
			}
		    }
		}
	    }
	}
    }
    umacro("x");
    umacro("y");
    for( i = 3; i < 10; i++) {
	if( ubc[i]) free( ubc[i]);
	if( vbc[i]) free( vbc[i]);
    }
}

face_ache( ae, j)
    FEelt_typ *ae;
    int j;
{
    int ip1, ip2, ip; double vlen;
    int in;

    /* Get local normal at either end and average for center*/
    ip1 = ae->nd[ (j+1)%3];
    in = node_mat( pt[ip1]->nd[0], ae->mat);
    if( dlocal_normal( in, -2, FEnd[ ip1]->garbage+X)<0) {
	fprintf( stderr, "Bad normal at (%g,%g)\n",pt[ip1]->cord[0], pt[ip1]->cord[1]);
	};
    ip2 = ae->nd[ (j+2)%3];
    in = node_mat( pt[ip2]->nd[0], ae->mat);
    if( dlocal_normal( in, -2, &(FEnd[ ip2]->garbage[X]))<0) {
	fprintf( stderr, "Bad normal at (%g,%g)\n",pt[ip1]->cord[0], pt[ip1]->cord[1]);
	}
    ip = ae->nd[ j+3];
    FEnd[ ip]->garbage[X] = 0.5*(FEnd[ip1]->garbage[X] + FEnd[ip2]->garbage[X]);
    FEnd[ ip]->garbage[Y] = 0.5*(FEnd[ip1]->garbage[Y] + FEnd[ip2]->garbage[Y]);
    vlen = hypot( FEnd[ip]->garbage[X], FEnd[ip]->garbage[Y]);
    FEnd[ ip]->garbage[X] /= vlen;
    FEnd[ ip]->garbage[Y] /= vlen;
}




/*-----------------GAS_FIXITIES-----------------------------------------
 * Put the gas concentration into the boundary nodes.
 * Wonder how long this lasts, until we need a stress dependence here too?
 *----------------------------------------------------------------------*/
gas_fixities()
{
    static int tface[3][3] = {{1,2,3},{2,0,4},{0,1,5}};
    int ie, j, *k, *kk; FEnd_typ *an; FEelt_typ *ae; float Oss();

    for (ie = 0; ie < FEne; ie++) {
	ae = FEelt[ ie];
	for (j = 0; j < 3; j++) {
	    if (ae->face[j] == BC_OFFSET+2) {

		/* At each gas boundary, mark the nodes */
		for (k = tface[j], kk=k+3; k < kk; k++) {
		    an = FEnd[ ae->nd[ *k]];
		    an->fixity[ D] = 1;
		    an->bc[ D] = Oss( gas_type, SiO2);
		}
	    }
	}
    }
}

/*-----------------SIL_FACE---------------------------------------------
 * Mark silicon nodes as being fixed (sort of) and calculate interface
 * normals.
 *----------------------------------------------------------------------*/
sil_face()
{
    int ie, ip, j, nbr;
    FEnd_typ *an1, *an2, *an3; FEelt_typ *ae;
    double vlen;

    /*Loop looking for silicon faces*/
    for (ie = 0; ie < FEne; ie++) {
	ae = FEelt[ ie];
	if (ae->mat != SiO2) continue;

	for (j = 0; j < 3; j++) {
	    nbr = ae->face[ j];
	    if (nbr < 0 || FEelt[ nbr]->mat != Si) continue;

	    /*Set the fixities to 4*/
	    /*rememember to overwrite dummy_sil's 1's for diffusivity*/
	    an1 = FEnd[ ae->nd[ (j+1)%3]];
	    an2 = FEnd[ ae->nd[ (j+2)%3]];
	    an3 = FEnd[ ae->nd[  j+3   ]];
	    an1->fixity[ X] = 4; an1->fixity[ Y] = 4;	an1->fixity[ D] = 0;
	    an2->fixity[ X] = 4; an2->fixity[ Y] = 4;	an2->fixity[ D] = 0;
	    an3->fixity[ X] = 4; an3->fixity[ Y] = 4;	an3->fixity[ D] = 0;
	}
    }

    /*
     * Compute local normal, outward from oxide to silicon
     * For vertices, use standard routine.
     * This REQUIRES that corresponding nodes have the same indices
     * in the FE mesh and the suprem4 mesh.
     */
    for( ip = 0; ip < np; ip++) {
	int iox;
	if( (node_mat( pt[ip]->nd[0], Si)) >= 0 &&
	    (iox = node_mat( pt[ip]->nd[0], SiO2)) >= 0) {
	    (void)dlocal_normal( iox, Si, FEnd[ ip]->garbage+X);
	}
    }

    /*
     * For midside nodes, try the perpendicular
     */
    for (ie = 0; ie < FEne; ie++) {
	ae = FEelt[ ie];
	if (ae->mat != SiO2) continue;

	for (j = 0; j < 3; j++) {
	    nbr = ae->face[ j];
	    if (nbr < 0 || FEelt[ nbr]->mat != Si) continue;

	    an1 = FEnd[ ae->nd[ (j+1)%3]];
	    an2 = FEnd[ ae->nd[ (j+2)%3]];
	    an3 = FEnd[ ae->nd[  j+3   ]];
#ifdef average
	    an3->garbage[X] = 0.5*(an1->garbage[X] + an2->garbage[X]);
	    an3->garbage[Y] = 0.5*(an1->garbage[Y] + an2->garbage[Y]);
#else
	    an3->garbage[X] = an2->cord[Y] - an1->cord[Y];
	    an3->garbage[Y] = an1->cord[X] - an2->cord[X];
#endif
	    vlen = hypot(an3->garbage[X], an3->garbage[Y]);
	    an3->garbage[X] /= vlen;
	    an3->garbage[Y] /= vlen;
	}
    }


}


/*-----------------SilOxLoad--------------------------------------------
 * Free up the nodes at the silicon oxide interface so that load
 * conditions can be applied to them in the silicon postprocessing phase.
 * Also store information to be later found by oxload.
 *----------------------------------------------------------------------*/
SilOxLoad()
{
    static int tface[3][3] = {{1,2,3},{2,0,4},{0,1,5}};
    int ie, j, *k, *kk; FEnd_typ *an; FEelt_typ *ae;

    /* Find interfaces */
    for (ie = 0; ie < FEne; ie++) {
	ae = FEelt[ ie];
	if( ae->mat != Si) continue;
	for (j = 0; j < 3; j++) {
	    if (ae->face[j] >= 0 && FEelt[ ae->face[j]]->mat == SiO2) {

		/* For each node on the face */
		for (k = tface[j], kk=k+3; k < kk; k++) {
		    an = FEnd[ ae->nd[ *k]];

		    /* Set the fixities free */
		    an->fixity[ X] = 0;
		    an->fixity[ Y] = 0;

		    /* Store the tractions for oxload */
		    an->garbage[ 2+X] = -( an->sig[ XX] * an->garbage[ X] + an->sig[ XY] * an->garbage[ Y]);
		    an->garbage[ 2+Y] = -( an->sig[ XY] * an->garbage[ X] + an->sig[ YY] * an->garbage[ Y]);
		}
	    }
	}
    }
}



/*-----------------CLEAR_BC---------------------------------------------
 * Wipe out the boundary conditions.
 *----------------------------------------------------------------------*/
clear_bc()
{
    int in, *k, *kk; FEnd_typ *an;
    double *j, *jj;

    for (in=0; in < FEnn; in++) {
	an = FEnd[ in];
	for (j = an->sol, jj=j+FEdf; j < jj; ) *j++ = 0;
	for (j = an->bc,  jj=j+FEdf; j < jj; ) *j++ = 0;
	for (k = an->fixity, kk=k+FEdf; k < kk; )  *k++ = 0;
    }

}





