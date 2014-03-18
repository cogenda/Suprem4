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
/*   grid_loop.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:20:52 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "impurity.h"
#include "regrid.h"
#include "matrix.h"		/* For the old, new areas */
#define assert(x) if (!(x)) panic("assertion failed")

double fmin(x,y) double x,y; {return (x<=y)? x : y;}
double fmax(x,y) double x,y; {return (x>=y)? x : y;}

#define PC(x) if (err = (x)) panic(err)

/* Addiction to convenience fodder */
#define tpt(T,V)   (nd[ tri[ (T)]->nd[ (V)]]->pt)
#define ploc(P,D)  (pt[ (P)]->cord[ (D)])
#define ploco(P,D) (pt[ (P)]->cordo[ (D)])
#define bedge(T,E) (tri[ (T)]->nb[ (E)] < 0 || \
		    tri[ tri[ (T)]->nb[ (E)]]->regnum != tri[ (T)]->regnum)
#define X 0
#define Y 1
#define cross(A,B) ((A)[X] * (B)[Y] - (A)[Y] * (B)[X])
#define dot(A,B) ((A)[X] * (B)[X] + (A)[Y] * (B)[Y])
#define vcos(A,B) (dot(A,B)/sqrt(dot(A,A)*dot(B,B)+1e-10))

#define point(n)    nd[n]->pt


/*-----------------DetectLoop-------------------------------------------
 * Requires: tri[ie]->nb
 * Provides: tri[ie]->nb, nd[in]->tri
 *----------------------------------------------------------------------*/
DetectLoop()
{
    int ie, j, nbj, tje, toje, tj, tjj, didSomething, ncc, nc, nc2, LoseMat;
    int ier, iem, ttje, ttoje, ttj, ttjj, tttje, tttoje, tttj, tttjj;
    int fix_conn=0, pass, cando;
    int v[3], nb[3], ntri, inl;
    int pc, pcc, pc2, pcf, pcb;
    float db[MAXDIM], d1[MAXDIM], d2[MAXDIM], df[MAXDIM];
    float dnb[MAXDIM], dnf[MAXDIM], dcos, dsin;
    char *alloc_tri();

    if ( mode == ONED ) return( 0 );

    /* This may take several passes */
    for( pass = 0, didSomething = 1; didSomething; pass++) {
	didSomething = 0;
	if (pass > 10) panic("DetectLoop infinite looped!");

    /* For each interface edge */
    ie = 0;
    while( done_tri(ie) ) {

	    /*for each triangle face*/
	    for(j = 0; j < num_face(ie); j++) {

		nbj = neigh_fc(ie, j);
		if( nbj >= 0 && reg_tri(nbj) == reg_tri(ie)) continue;

		ier = reg_tri(ie);
		iem = mat_reg(ier);

		/* Get the coordinates of the boundary nodes */
		ncc = nd_face_ele(ie, j, 0);
		nc  = nd_face_ele(ie, j, 1);
		pcc = pt_nd(ncc);
		pc  = pt_nd(ncc);

		/* Get the next clockwise one */
		trotate( ie, (j+1)%3, 0, &tje, &toje, &tj, &tjj);
		nc2 = vert_tri( toje, tjj);
		pc2 = pt_nd(nc2);

		/* Calculate the cos between PccPc and PcPc2 */
		d1[X] = cordinate(pc,0) - cordinate(pcc,0);
		d1[Y] = cordinate(pc,1) - cordinate(pcc,1);
		d2[X] = cordinate(pc2,0) - cordinate(pc,0);
		d2[Y] = cordinate(pc2,1) - cordinate(pc,1);
		dcos = vcos(d1,d2);
                dsin = cross(d1,d2) / sqrt( fabs(dot(d1,d1)*dot(d2,d2))+1e-10 );

		/* Inside corner or outside corner? */
		if( dsin > 0) {

		    /* Does the boundary turn back sharply? (Intrusion!)*/
		    if (dcos > gloop_imin) continue;

		    /* Fix by swallowing the intrusion area into this region */
		    /* Is that area currently vaccuum? */
		    if (nbj < 0) {

			/* On both sides? */
			if (tje >= 0) {
			    fprintf( stderr, "Intrusion with intervening layer not yet done\n");
			    continue;
			}
			if (tje != nbj)
			    fprintf( stderr, "Warning: intrusion with inhomogeneous BC\n");
			didSomething = 1;
			fix_conn++;
			if( verbose >= V_BARF)
			    printf("GridLoop: adding %s triangle\n", MatNames[iem]+1);

			/* A new triangle */
			v[0] = nc;
			v[1] = vert_tri( toje, tjj);
			v[2] = ncc;
			nb[0] = nbj;
			nb[1] = ie;
			nb[2] = toje;
			ntri = mk_ele(3, v, 3, nb, reg_tri(ie), FALSE);

			/* Update neighbor connections */
			set_neigh_t(ie,j,ntri);
			set_neigh_t(toje,3-tj-tjj,ntri);

			/* Take care of any gas node that might be there. */
			if ( (inl = node_mat(nd_pt(pc,0),GAS)) != -1 )
			    rem_1nd( inl );

		    }

		    /* Intrusion currently belongs to a different material */
		    else {
			/* hope it is just one triangle */
			if( tje != nbj) {
			    fprintf( stderr, "Multiple triangle intrusion case to be completed\n");
			    continue;
			}
			didSomething = 1;
			fix_conn++;
			if( verbose >= V_BARF)
			    printf("GridLoop: taking  %s triangle from %s\n", MatNames[iem]+1, MatNames[mat_tri( nbj)]+1);

			/* Subvert that triangle's material */
			LoseMat = mat_reg( reg_tri(nbj));
			reg_tri(nbj) = reg_tri(ie);

			/* Rename its nodes */
			vert_tri(tje,0) = node_mat( vert_tri(tje,0), iem);
			vert_tri(tje,1) = node_mat( vert_tri(tje,1), iem);
			vert_tri(tje,2) = node_mat( vert_tri(tje,2), iem);

			/* The corner node is no longer on a material interface */
			if ( (inl = node_mat(nd_pt(pc,0),LoseMat)) != -1 )
			    rem_1nd( inl );

		    }
		}

		/* Outside corner */
		else {

		    /* Does the boundary turn more than 90 degrees? */
		    if (dcos > gloop_emin) continue;


		    /* Definitely a problem if more than 170 */
		    if (dcos < gloop_emax) {
			if ( ie != toje) {
			    fprintf( stderr, "Can't do multiple triangle extrusion yet\n");
			    continue;
			}
		    }

		    /* Otherwise take next and previous edges into consideration */
		    else {

			if( ie != toje) continue;

			/* Look forward */
			trotate( toje, tjj, 0, &ttje, &ttoje, &ttj, &ttjj);

			pcf = pt_nd(vert_tri(ttoje, ttjj));
			df[X] = cordinate(pcf,0) - cordinate(pc2,0);
			df[Y] = cordinate(pcf,1) - cordinate(pc2,1);

			/* Calculate a vector at 45 to d2 */
			dnf[X] = d2[X] - d2[Y];
			dnf[Y] = d2[Y] + d2[X];

			/* Look back */
			trotate( ie, (j+2)%3, 1, &tttje, &tttoje, &tttj, &tttjj);

			pcb = pt_nd(vert_tri( tttoje, tttjj));
			db[X] = cordinate(pcc,0) - cordinate(pcb,0);
			db[Y] = cordinate(pcc,1) - cordinate(pcb,1);

			/* Calculate a vector at 45 to d1 */
			dnb[X] = d1[X] + d1[Y];
			dnb[Y] = d1[Y] - d1[X];

			/* Try to distinguish between a 90 corner in the boundary */
			/* and a 90 jag in the boundary */
			cando = 0;
			if (dot( df, dnf) > 0 && cross( dnf, df) > 0 && dot( db, dnb) > 0)
			    cando = 1;
			if (dot( db, dnb) > 0 && cross( db, dnb) > 0 && dot( df, dnf) > 0)
			    cando = 1;
			if (!cando) continue;
		    }

		    /* If it's wholly internal, give it away */
		    if ( nbj >= 0 && tje >= 0) {

			/* Might be better to make an arbitrary decision */
			if( reg_tri( nbj) != reg_tri( tje)) {
			    fprintf( stderr, "Can't decide whether to give %s triangle to %s or %s\n", MatNames[iem]+1, MatNames[ mat_tri(nbj)]+1, MatNames[ mat_tri( tje)]+1);
			    continue;
			}

			didSomething = 1;
			fix_conn++;
			if( verbose >= V_BARF)
			    printf("GridLoop: giving %s triangle to %s\n", MatNames[iem]+1, MatNames[ mat_tri(nbj)]+1);

			/* Subvert the material */
			reg_tri( ie) = reg_tri( nbj);

			/* Rename its nodes */
			vert_tri(ie,0) = node_mat( vert_tri(ie,0), mat_tri(ie));
			vert_tri(ie,1) = node_mat( vert_tri(ie,1), mat_tri(ie));
			vert_tri(ie,2) = node_mat( vert_tri(ie,2), mat_tri(ie));

			/* The corner node loses its node of this material */
			if ( (inl = node_mat(nd_pt(pc,0),iem)) != -1 )
			    rem_1nd( inl );
		    }

		    /* Otherwise give it to the vacuum */
		    else {

			didSomething = 1;
			fix_conn++;
			if( verbose >= V_BARF)
			    printf("GridLoop: losing %s triangle\n", MatNames[ iem]+1);

			/* Kill the node at the point that is sticking out */
			if ( (inl = node_mat(nd_pt(pc,0),iem)) != -1 )
			    rem_1nd( inl );

			/* Kill this triangle */
			rem_ele(ie, TRUE);

		    }
		}

	    } /*next edge*/
	    next_tri(ie);
	}
    }

    if (fix_conn) bd_connect("after loop removal");
    return(fix_conn);
}


#ifdef DEBUG
drawtri( ie)
    int ie;
{
    int ip  = point( vertex( ie, 0)), ip1 = point( vertex( ie, 1)),
        ip2 = point( vertex( ie, 2));
    char cmdbuf[256];
    float xm, xM, ym, yM;

    xm = fmin( fmin(xcord(ip), xcord(ip1)), xcord(ip2));
    xM = fmax( fmax(xcord(ip), xcord(ip1)), xcord(ip2));
    ym = fmin( fmin(ycord(ip), ycord(ip1)), ycord(ip2));
    yM = fmax( fmax(ycord(ip), ycord(ip1)), ycord(ip2));

    sprintf( cmdbuf, "plot.2 boundary grid x.min=%f y.min=%f x.max=%f y.max=%f; label x=%f y=%f label=%s\n",
	    1e4*(xm - 3*(xM-xm)), 1e4*(ym - 3*(yM-ym)),
	    1e4*(xM + 3*(xM-xm)), 1e4*(yM + 3*(yM-ym)),
	    0.333e4*(xcord(ip)+xcord(ip1)+xcord(ip2)),
	    0.333e4*(ycord(ip)+ycord(ip1)+ycord(ip2)),
	    "c");
    do_str(cmdbuf);
}

drawpt( ip, lab)
    int ip;
    char *lab;
{
    char cmdbuf[256];
    sprintf( cmdbuf, "label x=%f y=%f label=%s", xcord(ip)*1e4, ycord(ip)*1e4, lab);
    do_str(cmdbuf);
}
#endif


