/*----------------------------------------------------------------------
 *
 * subroutines to write files for pisces.
 *
 * Copyright c 1987 The board of trustees of the Leland Stanford
 *                  Junior University. All rights reserved.
 * This subroutine may not be used outside of the SUPREM4 computer
 * program without the prior written consent of Stanford University.
 *
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 * Original: CSR Sun Jun  7 20:47:20 1987
 *									 *
 *************************************************************************/
/*   pi_meshio.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:30:50  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include "material.h"
#include "impurity.h"
#include "sysdep.h"
#include "diffuse.h"
#include "regrid.h"
#include "expr.h"
#include "plot.h"

static char *erret;
#undef EC
#undef X
#undef Y
#define EC(x) if(erret=(x)) {fprintf( stderr, "Pisces mesh i/o error: %s\n", erret); return(-1);}
#define X 0
#define Y 1

static int corrmat[MAXMAT] =
/* 0Gas   1Ox 2Nit 3Si 4Poly    5Oxni     6none     7Al 8Photo*/
{-32767, -1,    -2,  1, -32767, -32767, -32767, -32767, -32767};
#define RECOG(mater) (corrmat[ mater ] != -32767)


/*-----------------pi_write---------------------------------------------
 * write a pisces file
   name   Where to write it
   show   Whether to plot the electrode setup.
 *----------------------------------------------------------------------*/
int pi_write( char *name, int show)
{
    int i;
    float *Pr1, *Ptconc;
    int Pnp, Pne, Pnb, Pnelect, *Pnbc, *Pietype, *Ppt, Pnmat, Preg[10];
    char *pi_elect(), *pi_dop(), *pi_mat();
    FILE *lu;

    if ( mode != TWOD )
	fprintf(stderr,"pi_write:can only export two dimensional structures\n");

    /* If the file is stuck it ain't worth a <beep> */
    if ((lu = fopen(name, "w")) == NULL) {
	fprintf(stderr, "pi_write: cannot open %s\n", name);
	return(-1);
    }

    /* Electrodes, doping, materials Pisces doesn't recognize */
    EC( pi_elect( &Pnb, &Pnelect, &Pnbc, &Pietype));
    EC( pi_dop ( &Pr1, &Ptconc));
    EC( pi_mat( &Pnp, &Pne, &Pnmat, &Ppt, Preg));

    /* Ready to roll. The minus sign is all the warning pisces gets
       that this is a suprem4 file. */
    fprintf( lu, "%d %d %d\n", Pnp, Pne, Pnb);
    fprintf( lu, "%d %d\n", Pnelect, -Pnmat);

    /* Points */
    for( i = 0; i < np; i++)
        if (Ppt[ i] >= 0)
            fprintf( lu, "%16e %16e %16e %16e\n",
		    cordinate(i,0), cordinate(i,1), Pr1[i], Ptconc[i]);

    /* Triangles */
    for( i = 0; i < ne; i++) {
	if ( num_vert(i) != 3 ) {
	    fprintf(stderr, "pi_write:can only export triangles\n");
	    return(-1);
	}
        if (RECOG(mat_tri(i)))
	    fprintf( lu, "%d %d %d %d\n", Preg[ reg_tri(i) ] + 1,
	           Ppt[ pt_nd(vert_tri(i,0)) ] + 1,
	           Ppt[ pt_nd(vert_tri(i,1)) ] + 1,
	           Ppt[ pt_nd(vert_tri(i,2)) ] + 1);
    }

    /* Electrodes */
    for( i = 0; i < Pnb; i++)
	fprintf( lu, "%d %d\n", Ppt[ Pnbc[i]] + 1, Pietype[i]);

    /* Regions */
    for (i = 0; i < nreg; i++)
	if (RECOG(mat_reg(i))) fprintf( lu, "%d ", corrmat[ mat_reg(i)]);
    fprintf( lu, "\n");

    fclose(lu);

    /* Tell the electrode numbers */
    ShowElect( show, Pnelect, Pnb, Pnbc, Pietype);

    free(Pr1); free(Ptconc); free(Pnbc); free(Pietype); free(Ppt);
    return(0);
}


/*-----------------PI_ELECT---------------------------------------------
 * make up electrode numbers the way pisces wants them (list vector)
 *----------------------------------------------------------------------*/
#define NPE 9			/* 9 electrodes in pisces */

char *pi_elect( nb, nelect, nbc, ietype)
     int *nb, *nelect, **nbc, **ietype;
{
    int Pnb, Pnelect, Pnelect0, *tmp, i, j, k, *Pnbc, *Pietype, bcode;
    int r_to_elec[MAXMAT];	/* Electrode code for each region  */

    /* Make space - freed above */
    Pnbc = scalloc(int, 2*np);	*nbc = Pnbc;
    Pietype = scalloc(int, 2*np);*ietype = Pietype;
    tmp = scalloc(int, 2*np);

    /* Figure out how many electrodes there are, total */
    Pnelect = 0;
    for (i = 0; i < num_reg; i++)
	if (mat_reg(i) == Al || mat_reg(i) == Poly)
	    r_to_elec[ i] = ++Pnelect;
	else
	    r_to_elec[ i] = -1;
    if (Pnelect+1 > NPE)
	return("Too many electrodes found");

    /* Now list points which face metal, poly or backside */
    Pnelect0 = Pnelect;

    /* Each recognized triangle */
    for (i = 0; i < ne; i++) {
	if (!RECOG(mat_tri(i))) continue;

	/* Each of it's faces */
	for (j = 0; j < num_face(i); j++) {
	    bcode = 0;
	    if ( is_face_bck(i,j) )
		bcode = Pnelect = Pnelect0+1;
	    /* Facing metal or poly? */
	    else if (neigh_t(i,j) >= 0) {
		int nr = neigh_t(i,j);
		if (mat_tri(nr) == Al || mat_tri(nr) == Poly)
		    bcode = r_to_elec[tri[nr]->regnum];
	    }

	    /* Nope */
	    if (bcode <= 0) continue;

	    /* Either way: remember nodes on face */
	    for(k = 0; k < num_nd_fc(i,j); k++)
		tmp[ pt_nd(nd_face_ele(i,j,k)) ] = bcode;
	}
    }

    Pnb = 0;
    for (i = 0; i < np; i++) {
	if (bcode = tmp[i]) {
	    Pnbc[ Pnb] =  i;
	    Pietype[ Pnb] = bcode;
	    Pnb++;
	}
    }
    free(tmp);
    *nb = Pnb;
    *nelect = Pnelect;
    return(0);
}


/*-----------------ShowElect--------------------------------------------
 * @ needs to know how the pisces electrodes were numbered
 *----------------------------------------------------------------------*/
int ShowElect(show, Pnelect, Pnb, Pnbc, Pietype)
     int show, Pnelect, Pnb, *Pnbc, *Pietype;
{
    double xlo[NPE], xhi[NPE], ylo[NPE], yhi[NPE];
    int i, ie; float *ac, txmin, txmax, tymin, tymax, xslop, yslop;
    static char CommandBuf[ BUFSIZ];

    /* Initialize bounds */
    for (ie = 1; ie <= Pnelect; ie++) {
	xlo[ie] = MAXFLOAT; ylo[ie] = MAXFLOAT;
	xhi[ie] = -MAXFLOAT; yhi[ie] = -MAXFLOAT;
    }

    /* Draw a picture to make things easy */
    if (show) {
	dev_lmts(&txmin, &txmax, &tymin, &tymax);
	txmin *= 1e4; txmax *= 1e4; tymin *= 1e4; tymax *= 1e4;
	xslop = 0.1*(txmax-txmin);
	yslop = 0.1*(tymax-tymin);
	sprintf( CommandBuf, "plot.2 bound fill x.mi=%f x.ma=%f y.mi=%f y.ma=%f",
		txmin - xslop, txmax + xslop, tymin - yslop, tymax + yslop);
	do_str( CommandBuf);
    }

    /* Each electrode point */
    for (i = 0; i < Pnb; i++) {
        ac = cord_arr( Pnbc[ i] );
	ie = Pietype[ i];

	if (show) {
	    sprintf( CommandBuf, "label x=%e y=%e label=%d\n", ac[X]*1e4,ac[Y]*1e4,ie);
	    do_str(CommandBuf);
	}

	/* Does it stretch its electrode box? */
	if (ac[X] < xlo[ ie]) xlo[ ie] = ac[X];
	if (ac[X] > xhi[ ie]) xhi[ ie] = ac[X];
	if (ac[Y] < ylo[ ie]) ylo[ ie] = ac[Y];
	if (ac[Y] > yhi[ ie]) yhi[ ie] = ac[Y];
    }

    /* Report answers */
    for (ie = 1; ie <= Pnelect; ie++)
	printf( "Electrode %d: xmin %8.3f xmax %8.3f ymin %8.3f ymax %8.3f\n",
	       ie, 1e4*xlo[ ie], 1e4*xhi[ ie], 1e4*ylo[ ie], 1e4*yhi[ ie]);
    return 0;
}


/*-----------------PI_DOP-----------------------------------------------
 *----------------------------------------------------------------------*/
char *pi_dop( r1, tconc)
     float **r1, **tconc;
{
    int i, k;
    float *Pr1 = scalloc( float, np), *Ptconc = scalloc( float, np);
    float *tmp = scalloc( float, nn);
    char SelectBuf[100];
    *r1 = Pr1;
    *tconc = Ptconc;

    if (last_temp == 0)
	return("please specify a temperature");

    /* Calculate net and total, letting select do the work*/
    do_str("select z=doping");
    for (i = 0; i < nn; i++) tmp[i] = z[i];

    strcpy( SelectBuf, "select z=( ");
    if (imptosol[As] != -1) strcat( SelectBuf, "active(arsenic) + ");
    if (imptosol[B ] != -1) strcat( SelectBuf, "active(boron) + ");
    if (imptosol[Sb] != -1) strcat( SelectBuf, "active(antimony) + ");
    if (imptosol[P ] != -1) strcat( SelectBuf, "active(phosphorus) + ");
    strcat( SelectBuf, "0 )");
    do_str(SelectBuf);

    /* Copy the silicon value if there is one */
    for (i = 0; i < np; i++) {
	if ((k = node_mat(nd_pt(i,0), Si)) < 0) continue;
	Ptconc[ i] = z[ k];
	Pr1[ i] = tmp[ k];
	/* Pisces hates 0 concs */
	if (Ptconc[ i] == 0) Ptconc[ i] = 1.0;
	if (Pr1   [ i] == 0) Pr1   [ i] = 1.0;
    }

    free( tmp);
    return(0);
}

/*-----------------Pi-mat-----------------------------------------------
 *----------------------------------------------------------------------*/
char *pi_mat( Pnp, Pne, Pnmat, Ppt, Preg)
     int *Pnp, *Pne, *Pnmat, **Ppt, *Preg;
{
    int i, j;

    /* Count the number of regions with reconized materials */
    *Pnmat = 0;
    for (i = 0; i < num_reg; i++)
	if (corrmat[ mat_reg(i)] != -32767)
	    Preg[ i] = (*Pnmat)++;

    /* Some points will be dropped. Find out the node numbers in the new grid */
    *Ppt = salloc(int, np);
    for (i = 0; i < np; i++) (*Ppt)[i] = -1;

    /* For each recognized triangle */
    for (i = 0; i < ne; i++)
	if (corrmat[ mat_tri(i) ] != -32767)
	    /* Mark it's nodes as permanent */
	    for (j = 0; j < num_vert(i); j++)
		(*Ppt)[ pt_nd(vert_tri(i,j)) ] = 1;


    /* Assign node and triangle numbers by counting up from 1 */
    for (*Pnp = 0, i = 0; i < np; i++)
	if ((*Ppt)[ i] > 0) (*Ppt)[ i] = (*Pnp)++;
    for (*Pne = 0, i=0; i < ne; i++)
	if (RECOG( mat_tri(i))) (*Pne)++;

    return(0);
}


/*-----------------REFLECT----------------------------------------------
 * Reflect a grid around its left or right edge.
 *----------------------------------------------------------------------*/
void reflect_grid( sign, xy)
     float sign;		/* -1 is left, +1 is right */
    int xy;			/* 0 is x, 1 is y */
{
    float extreme;
    char *alloc_tri(), *alloc_nd(), *alloc_pt();
    int i, neO, npO, j, ip;
    int an; int ap;
    float c[MAXDIM];
    int no_gas, maxn;
    int *nodmap, nb[3], vl[3];

    nodmap = salloc(int, 2*nn);

    /* First pass is to get the smallest/largest x cordinate */
    extreme = - sign * MAXFLOAT;
    for (i = 0; i < np; i++) {
	if (sign * cordinate(i, xy) > sign * extreme)
	    extreme = cordinate(i,xy);
    }

    /* OK, now loop over elements, creating new points and nodes as necc. */
    neO = ne;
    npO = np;

    /* Duplicate the points*/
    for( i = 0; i < npO; i++) {
	/* The interface points are stillborn */
	if (fabs( cordinate(i,xy) - extreme) < 1e-8) {
	    /*we may have to remove the gas node*/
	    no_gas = TRUE;
	    if (is_surf(i) || is_back(i)) {
		maxn = num_neigh_pt(i);
		for(j = 0; j < maxn; j++) {
		    ip = neigh_pt(i,j);
		    if ( ((is_surf(ip)) || (is_back(ip))) &&
			 (fabs( cordinate(ip,xy) - extreme) > 1e-8))
			 no_gas = FALSE;
		}
		/*all the neighbor surface points lie on the reflecting line*/
		if ( no_gas ) {
		    fing_nd( node_mat( nd_pt(i,0), GAS ) );
		    clr_surf(i);
		    clr_back(i);
		}
	    }
	    for( j = 0; j < num_nd(i); j++) {
		an = nd_pt(i,j);
		nodmap[an] = an;
	    }
	}
	else {
	    for(j = 0; j < mode; j++) c[j] = cordinate(i,j);
	    c[xy] = 2*extreme - cordinate(i,xy);

	    ap = mk_pt(mode, c);
	    if (is_surf(i)) set_surf(ap);
	    if (is_back(i)) set_back(ap);

	    /* make nodes */
	    for( j = 0; j < num_nd(i); j++) {
		an = mk_nd(ap, mat_nd(nd_pt(i,j)));
		nodmap[nd_pt(i,j)] = an;
		copy_sol(nd_pt(i,j), an);
	    }
	}
    }

    /* Duplicate the triangles. */
    nb[0] = nb[1] = nb[2] = -1024;
    for (i = 0; i < neO; i++) {
	for(j = 0; j < num_vert(i); j++) vl[j] = nodmap[ vert_tri(i,j) ];
	(void)mk_ele_nd(num_vert(j), vl, reg_tri(i) );
    }

    /* Get rid of the duplicate interface nodes */
    waste();

    /* Rebuild the connectivity */
    bd_connect("after reflecting the grid");

    free(nodmap);
}
