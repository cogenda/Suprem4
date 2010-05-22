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
/*   pearson.c                Version 5.1     */
/*   Last Modification : 7/3/91 12:08:51 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "material.h"
#include "implant.h"

/************************************************************************
 *									*
 *	pearson( ion, back, dose, energy) - pearson calculates a 	*
 *  a pearson four model of a implantation of dose with energy of 	*
 *  impurity ion.							*
 *									*
 *  Original:	MEL	1/85						*
 *  Mod #  1:   Michael Eldredge 4/86,  Convert to use Pearson routines *
 *									*
 ************************************************************************/
do_implant( ion, angle, dose, energy, damage, sol, is, vs )
int ion;	/*the impurity number*/
double angle;	/*the implant angle*/
double dose;	/*the dose of the implant*/
double energy;	/*the energy of the implant*/
int damage;	/*calculate the damage profile*/
int sol;	/*storage location of implant*/
int is;		/*storage location of I defects*/
int vs;		/*storage location of V defects*/
{
    register int i, j, k, r;		/* every routine needs indices */
    struct imp_info  Idat[MAXREG];	/*the appropriate set*/
    struct imp_info  *data ;		/* pointer to constants */
    struct surf_info *surf, *cur;
    double bias, dosofar;
    double	val;
    int rx, rm, mat, id, il;
    int *node_done;
    double q, evaluate();
    double depth, tp, bt;
    double l[MAXDIM];
    double maxlat = 0;

    /* for all used materials, get the implant data */
    for(r = 0; r < nreg; r++) {
	mat = mat_reg(r);
	if (setprs(ion, mat, energy, &(Idat[r])) < 0) {
	    fprintf(stderr, "setprs failed: ion %d, mat %d, energy %g\n",
			    ion, mat, energy) ;
	    fprintf(stderr, "Couldn't find ion and energy in tables\n");
	    return;
	}

	/* implant damage set up */
	if ( ((mat == Si) || (mat == Poly) || (mat == GaAs) ) && (damage) ) {
	    setdam(ion, energy, Idat[r].dam);
	}

	if ( Idat[r].lat[LATSIG] > maxlat ) maxlat = Idat[r].lat[LATSIG];

    }

    /*compute a node to region mapping*/
    node_done = salloc(int, nn);

    /*initialize the surface structure information*/
    make_surf( &surf, angle, 8*maxlat );

    /*using the mode information, figure out the depth dimension*/
    if ( mode == ONED ) {
	id = 0;
	il = 1;
    }
    else {
	id = 1;
	il = 0;
    }

    /* For each vertical slice, compute the contribution */
    for( cur = surf; cur != NULL; cur = cur->next ) {

	/*work out the material by material offsets*/
	bias = cur->top[0];		/*location of material top*/
	dosofar = 0.0;

	/*each slice adds a contribution to each node just once*/
	for(i = 0; i < nn; i++) node_done[i] = FALSE;

	for(rm = 0; rm < cur->nmat; rm++) {
	    r = cur->mat[rm];

	    /*if into silicon, and calculating damage get it set up*/
	    mat = mat_reg(r);
	    data = &(Idat[r]);

	    if ( dosofar > dose ) continue;

	    /* find the (new) equivalent depth */
	    if ( dosofar != 0.0 ) {
		depth = (cur->bot[rm] - cur->top[rm]) * 1.0e4;
		bias = cur->top[rm] -
		       zeqv(dosofar,dose,PRS_DX,depth,data)*1e-4;
	    }

	    tp = (cur->top[rm] - bias) * 1.0e4;
	    bt = (cur->bot[rm] - bias) * 1.0e4;
	    dosofar += qeqv(tp, bt, dose, PRS_DX, data );

	    /*for all the nodes*/
	    /*Here we assume ions make right turns in the substrate...
	      They go straight down in this material, but go laterally
	      in their own....*/
	    for(rx = 0; rx < nreg; rx++) {
		for(j = 0; j < num_nd_reg(rx); j++) {
		    i = nd_reg(rx, j);

		    if (node_done[i]) continue;
		    l[0] = pt[ nd[i]->pt ]->cord[id];
		    l[0] -= bias;

		    l[1] = pt[ nd[i]->pt ]->cord[il];
		    l[2] = pt[ nd[i]->pt ]->cord[2];

		    node_done[i] = TRUE;

		    /* Infinite width value - no lateral accounting yet... */
		    val = evaluate(l,cur->lft,cur->rht,data)*dose/data->area;
		    nd[i]->sol[sol] += val;
		} /*for all nodes...*/
	    } /*for all regions*/

#ifdef FOO
	    /*do a damage calculation*/
	    if ( ((mat == Si) || (mat == Poly) || (mat == GaAs) ) && damage ) {
		for(j = 0; j < num_nd_reg(r); j++) {
		    i = nd_reg(r, j);

		    l[0] = pt[ nd[i]->pt ]->cord[0];
		    l[1] = pt[ nd[i]->pt ]->cord[1];
		    l[2] = pt[ nd[i]->pt ]->cord[2];

		    if (l[id] < cur->top[rm]) continue;
		    if (l[id] > cur->bot[rm]) continue;

		    /*scale depth into microns*/
		    depth = (l[id] - bias) * 1.0e4;

		    /*calculate the damage done here*/
		    val = dam_vert(depth, data->dam) * dose;
		    for(k = 0; k < 2*(mode-1); k += 2) {
			val *= dam_lat(l[il],cur->edge[k],cur->edge[k+1],data);
		    }
		    nd[i]->sol[is] += val;
		    nd[i]->sol[vs] += val;
		}
	    }
#endif
	} /*foreach each region at this slice*/
    }/*of foreach vertical slice */

    free_surf( &surf );
    free(node_done);

    return;	/* all ok */
}

