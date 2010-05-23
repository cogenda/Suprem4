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
/*   oxgrow.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:52:33 */
#include <stdio.h>
#include <sys/times.h>

#include <global.h>
#include <constant.h>
#include <geom.h>
#include <material.h>
#include <impurity.h>
#include <matrix.h>

/*-----------------OXGROW-----------------------------------------------
 * Controller for oxide growth.
 * Remember: All nodes must be added before
 * computing velocities, otherwise new nodes won't have any.
 *----------------------------------------------------------------------*/
oxgrow (temp, ornt, oxhow, dt )
    float temp;		/* Processing temperature */
    int ornt;		/* Orientation of substrate */
    int oxhow;		/* Dry or wet oxidation */
    double *dt;		/* Time increment */
{
    extern double total;	/* Total diffusion time */
    struct tms before, after;

    times(&before);

    /* Compute the velocity vector using whatever oxide growth model */
    oxide_vel (temp, ornt, oxhow, *dt);

    /* Check for boundary loops */
    /* Before grid_add to clean up boundary before decorating with new nodes*/
    if(DetectLoop())
	UpdateSymbolic++;

    /* Now drop in any new nodes. Velocities, concentrations as old ones */
    if (grid_add(*dt))
	UpdateSymbolic++;

    /* Make a pointwise velocity list: do this AFTER np has been bumped */
    point_vel( );

    /*cut the time step back */
    back_pedal( dt );

    /* Mark nodes for later removal */
    ChooseKillNodes( *dt);

    /* some barfola statistics */
    times(&after);
    print_time("Time for oxidation step", &before, &after);

    return;
}

/*-----------------POINT_VEL--------------------------------------------
 * Turns node velocities into point velocities
 *----------------------------------------------------------------------*/

point_vel()
{
    int ip, nx, ix;
    int arr[3];

    arr[0] = imptosol[XVEL];
    arr[1] = imptosol[YVEL];

    /* Move the points */
    for (ip = 0; ip < np; ip++) {

	/*
	 * Well, displacement is a function of node and there are
	 * several nodes at a point, so we have to make a decision.
	 * For now we let the point move with its 0th node and
	 * hope all the other nodes at that point wanted to go to the
	 * same place. Except: when there is a silicon node there,
	 * we make sure to go with it. This means the silicon/oxide
	 * boundary moves into silicon, as wanted.
	 */
	if ( (nx = node_mat( nd_pt(ip,0), Si ) ) == -1 )
	    if ( (nx = node_mat( nd_pt(ip,0), Poly ) ) == -1 )
		    nx = nd_pt(ip, 0);

	for(ix = 0; ix < mode; ix++)
	    pt[ip]->vel[ix] = nd[nx]->sol[arr[ix]];
    }
}

/*------------------MOVE_POINT------------------------------------------
 * Turn point velocities into honest-to-god point displacements.
 *----------------------------------------------------------------------*/
move_point( dt )
     double dt;
{
    register int ip, ix;

    for (ip = 0; ip < np; ip++) {
	for( ix = 0; ix < mode; ix++ )
	    pt[ ip]->cord[ ix] += pt[ip]->vel[ix] * dt;
    }
}

/*-----------------TOT_AREAS--------------------------------------------
 * Integrate the total area in each material (volume check)
 *----------------------------------------------------------------------*/
tot_areas( oar, nar )
double *oar, *nar;
{
    double newa[MAXMAT], olda[MAXMAT], delta;
    int i;

    for( i = 0; i < MAXMAT; i++) newa[i] = olda[i] = 0;

    for( i = 0; i < nn; i++) {
	newa[ nd[i]->mater ] += 1e8*nar[i];
	olda[ nd[i]->mater ] += 1e8*oar[i];
    }

    if ( newa[Si] != olda[Si] )
    printf("Silicon consumed %10.5f oxide grown %10.5f (%5.3f)\n", olda[Si] - newa[Si],
	   newa[SiO2] - olda[SiO2], (newa[SiO2] - olda[SiO2])/(olda[Si] - newa[Si]));

    for (i = 0; i < MAXMAT; i++) {
	if (olda[i] != 0){
	    delta = (newa[i] - olda[i])/olda[i];
	    if (delta > 1e-6 || delta < -1e-6)
		printf("change in %s area %8.5f%%\n", MatNames[i]+1, delta*100);
	}
    }
    return;
}


/*-----------------ClockTri---------------------------------------------
 * Searches the grid for clockwise triangles and (optionally) fixes them.
 * Returns total number of clockwise triangles, negative if some are zero.
 * Original: CSR 08/87.
 *----------------------------------------------------------------------*/
#define X 0
#define Y 1
ClockTri( FixEm)
    int FixEm;
{
    int ie, *n, swap, nclock=0, nzero=0;
    float *c0, *c1, *c2, area, area_tri();

    if ( mode == ONED ) return( 0 );

    for (ie = 0; ie < ne; ie++) {
	n = tri[ie]->nd;
	c0 = pt[ nd[ *(n++)]->pt]->cord;
	c1 = pt[ nd[ *(n++)]->pt]->cord;
	c2 = pt[ nd[ *(n++)]->pt]->cord;
	area = area_tri( c0[X], c0[Y], c1[X], c1[Y], c2[X], c2[Y]);
	if( area == 0) nzero++;
	if( area < 0 ) nclock++;
	if (FixEm && area < 0) {
		n = tri[ie]->nd;
		swap = n[1]; n[1] = n[2]; n[2] = swap;
		n = tri[ie]->nb;
		swap = n[1]; n[1] = n[2]; n[2] = swap;
	    }
    }
    return( (nzero+nclock) * (nzero? -1 : 1));
}

