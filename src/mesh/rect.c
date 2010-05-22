static char sccsid[]="$Header: rect.c rev 5.1 7/3/91 08:30:53";
/*----------------------------------------------------------------------
 *
 * rect.c - Some quick hacks to allow us to use rectangular meshes.
 *
 * Copyright c 1985 The board of trustees of the Leland Stanford
 *                  Junior University. All rights reserved.
 * This subroutine may not be used outside of the SUPREM4 computer
 * program without the prior written consent of Stanford University.
 *
 *									 *
 *     Copyright c 1990 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 * Original: CSR Nov85 (borrowed from 3d poisson solver)
 *----------------------------------------------------------------------*/

#include <stdio.h>	/* For malloc & stderr */
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sysdep.h>

#include <constant.h>
#include <dbaccess.h>
#include <material.h>
#include <global.h>	/* For min() */

int nxy[MAXDIM];
#define NX (nxy[0])
#define NY (nxy[1])
#define NZ (nxy[2])

#define MAXBNDS 2*MAXDIM+1
#define TYPLOC 2*MAXDIM

unsigned maxuser;
unsigned maxplanes;
float *uloc[MAXDIM];			/* User locations */
float *uspa[MAXDIM];			/* User spacing */
char  **utag[MAXDIM];			/* User labels */
float *rloc [MAXDIM];			/* Final locations. */
int   *umap[MAXDIM];			/* Where user nodes maps to */

int   und[MAXDIM];				/* User number in each dim */
int   ureg[MAXMAT][MAXBNDS];			/* User regions */
int   uedge[MAXMAT][MAXBNDS];			/* User regions */
int   umattyp[MAXMAT];				/* Groan */
int   nur;					/* Number of user regions */
int   nuj;					/* Number of user edges */
double mr = 1.5;				/* The min ratio parameter*/
int   rect_err;

char XY[]="xyz";

/*-----------------RECT_BOOT--------------------------------------------
 * Initialize rectangular mesh data.
 *----------------------------------------------------------------------*/
rect_boot()
{
    static int before = 0;
    int i, j;

    for(i = 0; i < MAXDIM; i++) { nxy[i] = 1; und[i] = 0; }

    nuj = nur = 0;
    rect_err = 0;

    if ( !before ) {
	maxuser = 50;
	maxplanes = 300;
	for(i = 0; i < MAXDIM; i++) {
	    uloc[i] = salloc(float, maxuser);
	    uspa[i] = salloc(float, maxuser);
	    utag[i] = scalloc(char *, maxuser);
	    rloc[i] = salloc(float, maxplanes);
	    umap[i] = salloc(int, maxuser);
	}
    }

    /* Clear out the tags array */
    for (i = 0; i < MAXDIM; i++)
	for (j = 0; j < maxuser; j++) {
	    if (before && utag[ i][ j]) free( utag[ i][ j]);
	    utag[ i][ j] = 0;
	}
    before++;

    /* Just in case anyone's watching...*/
    for (i = 0; i < MAXMAT; i++) umattyp[i] = -1;
}


/*-----------------RLINE------------------------------------------------
 * Reads user mesh planes.
 *
 * Return value: none
 * Diagnostics: complains if there are too many input nodes
 *----------------------------------------------------------------------*/
rline (par, param)
    char *par;
    int param;
{
    int i, j, dim, nu;
    double lloc, spa;
    char *s;

    /* Dimension: parser better guarantee one of these guys is set */
    if (get_bool (param, "x.direction")) dim = 0;
    if (get_bool (param, "y.direction")) dim = 1;
    if (get_bool (param, "z.direction")) dim = 2;

    /* Store the number so far in this direction in a local */
    nu = und[ dim];

    /* Where the line is */
    lloc = 1e-4 * get_float (param, "location");
    if (nu > 0 && lloc <= uloc[dim][nu-1]) {
	fprintf( stderr, "Error: %c node %d is in non-increasing order\n",
				XY[ dim], nu);
	return( -1);
    }

    /*
     * What spacing to use: user spacings are preserved by being negative
     * in all comparisons with smaller spacings
     */
    if (is_specified( param, "spacing"))
	spa =  -1e-4 * get_float (param, "spacing");
    else
	spa = MAXFLOAT;

    /* Line handle */
    s = get_string( param, "tag");

    /* Store away for later use. */
    if( nu >= maxuser) {
	maxuser += 50;
	for(i = 0; i < MAXDIM; i++) {
	    uloc[i] = sralloc(float, maxuser, uloc[i]);
	    uspa[i] = sralloc(float, maxuser, uspa[i]);
	    utag[i] = sralloc(char *, maxuser, utag[i]);
	    umap[i] = sralloc(int, maxuser, umap[i]);
	    for(j = nu; j < maxuser; j++) utag[i][j] = NULL;
	}
    }
    uloc[dim][nu] = lloc;
    uspa[dim][nu] =  spa;
    utag[dim][nu] = (s)
	? (char *)strcpy( (char *) malloc( 1+strlen(s)), s)
	: NULL;

    und[ dim]++;
    return(0);
}

/*-----------------SMOOTH-----------------------------------------------
 * Smooth out neighboring values of h.
 *----------------------------------------------------------------------*/
smooth (dim)
    int dim;
{
    int in, jn, *done, jmin, jmax;
    double hmin, lej, hM;

    /* Set defaults: */
    for (in = 0; in < und[dim]-1; in++)
    {
	/* Get the length of the interval */
	lej = fabs(uloc[dim][in+1] - uloc[dim][in]);

	/* Use it if shorter than the existing spacing (initially MAXFLOAT) */
	if (lej < uspa[dim][in]) uspa[dim][in] = lej;
	if (lej < uspa[dim][in+1]) uspa[dim][in+1] = lej;
    }

    /* User specified values were negative, so the above missed them */
    /* Now make 'em positive */
    for (in = 0; in < und[dim]; in++)
	if (uspa[dim][in] < 0) uspa[dim][in] = -uspa[dim][in];

    /* Search points in order of increasing h, and round down big h */
    done = salloc(int, maxuser);
    for (in = 0; in < und[dim]; in++)
	done[in] = 0;

    for (in = 0; in < und[dim]; in++) {

	/* Set jmin to index of minimum remaining h. */
	for (hmin = MAXFLOAT, jmin=0, jn = 0; jn < und[dim]; jn++) {
	    if (done[jn]) continue;
	    if (uspa[dim][jn] < hmin) {
		hmin = uspa[dim][jn];
		jmin = jn;
	    }
	}

	for (jmax = jmin - 1; jmax < jmin + 2; jmax += 2) {
	    if (jmax < 0 || jmax >= und[dim]) continue;
	    lej = fabs (uloc[dim][jmax] - uloc[dim][jmin]);
	    hM  = (hmin + (mr-1)*lej)/mr;
	    uspa [dim][jmin] = min (hM, uspa[dim][jmin]);
	}
	done[jmin]=TRUE;
    }

    free(done);
    return(0);
}


/*-----------------ADDINT-----------------------------------------------
 * Add planes between the user specified ones.
 *----------------------------------------------------------------------*/
addint (dim)
    int dim;
{
    int nn, is, j, i;
    double r, f, x, dx;


    umap [dim][0] = 0;
    rloc  [dim][0] = uloc[dim][0];
    nxy [dim] = 1;

    for (is = 0; is < und[dim]-1; is++)
    {
	x = uloc[dim][is];
	dx = uloc[dim][is+1] - x;
	dvpram (uspa[dim][is], uspa[dim][is+1], fabs(dx),  &nn, &r, &f);

	for (j = 0; j < nn; j++)
	{
	    x += f*dx;
	    rloc [dim][nxy[dim]] = x;
	    if (++nxy [dim] >= maxplanes) {
		maxplanes += 300;
		for(i = 0; i < MAXDIM; i++) {
		    rloc[i] = sralloc(float, maxplanes, rloc[i]);
		}
	    }
	    f *= r;
	}

	rloc [dim][nxy [dim]] = uloc[dim][is+1];
	if (++nxy [dim] >= maxplanes) {
	    maxplanes += 300;
	    for(i = 0; i < MAXDIM; i++) {
		rloc[i] = sralloc(float, maxplanes, rloc[i]);
	    }
	}
	umap [dim][is+1] = nxy [dim]-1;
    }
    return( 0);
}



/*-----------------DVPRAM-----------------------------------------------
 * Spacing parameters for dividing an edge.
 *----------------------------------------------------------------------*/
dvpram (hl, hr, el, nnew, ratio, first)
    int *nnew;
    double hr, hl, *ratio, *first, el;
{
    double r,rn,hl2,hr2;

    if (hl==0 || hr==0) {
	nnew = 0;
	return;
	}

    /* Check for easy cases. */
    *nnew  = 0;
    *ratio = 1.0;
    *first = 1.0;
    if (hl > el) hl = el;
    if (hr > el) hr = el;

    /* Spacing is just length of edge. */
    if (fabs(el-hl) <= EPS*el && fabs(el-hr) <= EPS*el) return;

    /* Spacing is same at both ends. */
    if (fabs(hr-hl) <= EPS*el) {
	*nnew  = (int) ((el / hl)-0.49999);
	*first = 1.0 / (*nnew+1);
	return;
	}

    /* Ideally, the spacing increases by a fixed ratio, starting at
     * hleft, and increasing to hright, so that
     *     elength = hl + r*hl + r**2*hl + ... + r**n*hl, and
     *                                           \------->is hr.
     * Then elength = (r**(n+1)-1)/(r-1)*hl = (r*hr-1)/(r-1)*hl
     * which can be solved as r = (el-hl/el-hr), and
     * plug back in hr/hl = r**n to calculate n.
     * However, n may not be an integer - round and adjust first step
     * accordingly.
     */

    hr2 = hl/mr + (1-1/mr)*el;
    hl2 = hr/mr + (1-1/mr)*el;
    hr = min(hr,hr2);
    hl = min(hl,hl2);

    r = (el-hl)/(el-hr);
    rn = hr/hl;
    *nnew = (int) ((log(rn)/log(r))+0.5);
    /*
    if (Debug)
       printf("rn=%g,r=%g,fnew=%g,nnew=%d\n",r,rn,((log(rn)/log(r))+0.5),*nnew);
    */

    if (*nnew == 0) return;

    *ratio = exp(log(rn)/ *nnew);
    if (*ratio < 1/mr) *ratio = 1/mr;
    if (*ratio > mr  ) *ratio = mr;


    *first = (*ratio-1) / (exp ((*nnew+1) * log(*ratio)) - 1);
    return;
}

/*-----------------SQUARES----------------------------------------------
 * Generate triangle numbers for rectangular mesh
 *----------------------------------------------------------------------*/
squares(new_mr)
    float new_mr;  /* New value of mr */
{
    int ie, i, j, k, ir, ij, dim, ix[3];
    int test, vl[12];
    float c1[MAXDIM];

    test = FALSE;
    for(i = 0; i < mode; i++) test = test || und[i] < 2;
    /* Do we have enough user data? */
    if (rect_err || test ) {
	fprintf( stderr, "user mesh data not given or incomplete\n");
	rect_boot();
	return( -1);
    }

    if (new_mr > 1) mr = new_mr;

    /* OK, fill out the spaces between the user specified lines */
    for (dim = 0; dim < mode; dim++) {
	if (smooth (dim)) { rect_boot(); return(-1); }
	if (addint (dim)) { rect_boot(); return(-1); }
	if (verbose >= V_NORMAL) {
	    printf ("Lines in the %c direction: \n", XY[dim]);
	    printf ("Number	Location\n");
	    for (i = 0; i < nxy[dim]; i++)
		printf ("%6d	%8f\n", i, rloc[dim][i] * 1.0e4);
	}
    }

    /* Well everything seems good, blow away the old mesh and go at it! */
    dis_all();

    /* Change the region and edge user numbers to mapped numbers*/
    /*for all regions*/
    for(ir = 0; ir < nur; ir++)
	/*for all dimensions*/
	for(dim = 0; dim < mode; dim++)
	    /*for high and low...*/
	    for(k = 0; k < 2; k++)
		ureg[ir][ 2*dim + k ] = umap[dim][ ureg[ir][2*dim + k] ];


    /*for all edges*/
    for (ij = 0; ij < nuj; ij++)
	/*for all dimensions*/
	for(dim = 0; dim < mode; dim++)
	    /*for high and low...*/
	    for(k = 0; k < 2; k++)
		uedge[ij][ 2*dim + k ] = umap[dim][ uedge[ij][2*dim + k] ];


    for(ix[0] = 0; ix[0] < nxy[0]; ix[0]++) {
	for(ix[1] = 0; ix[1] < nxy[1]; ix[1]++) {
	    for(ix[2] = 0; ix[2] < nxy[2]; ix[2]++) {
		for(ij = 0; ij < mode; ij++) {
		    c1[ij] = rloc[ij][ix[ij]];
		}
		(void)mk_pt( mode, c1 );
	    }
	}
    }

    /*for all the specified boundaries, mark the points*/
    for (ij = 0; ij < nuj; ij++) {
	/*for all "edges" of the element*/
	for(ix[0] = uedge[ij][0]; ix[0] <= uedge[ij][1]; ix[0]++) {
	    for(ix[1] = uedge[ij][2]; ix[1] <= uedge[ij][3]; ix[1]++) {
		for(ix[2] = uedge[ij][4]; ix[2] <= uedge[ij][5]; ix[2]++) {
		    k = ix[2] + nxy[2]*ix[1] + nxy[1]*nxy[2]*ix[0];
		    if (uedge[ij][TYPLOC] == EXPOSED) set_surf(k);
		    if (uedge[ij][TYPLOC] == BACKEDG) set_back(k);
		}
	    }
	}
    }

    /*form triangles / lines / tetahedra*/
    switch ( mode ) {
    case TWOD :
	ie = -1;
	/*for each region*/
	for (ir = 0; ir < nur; ir++) {
	    for (i=ureg[ir][0]; i < ureg[ir][1]; i++)
		for (j=ureg[ir][2]; j < ureg[ir][3]; j++) {
		    vl[0] = j   + NY*i;
		    vl[1] = j+1 + NY*i;
		    vl[2] = j   + NY*(i+1);
		    ie = mk_ele_pt(3, vl, ureg[ir][TYPLOC]);
		    vl[0] = j+1 + NY*i;
		    vl[1] = j+1 + NY*(i+1);
		    vl[2] = j   + NY*(i+1);
		    ie = mk_ele_pt(3, vl, ureg[ir][TYPLOC]);
		}
	}

	break;

    case ONED :
	for(ir = 0; ir < nur; ir++) {
	    for(ie = ureg[ir][0]; ie < ureg[ir][1]; ie++) {
		vl[0] = ie;
		vl[1] = ie+1;
		(void)mk_ele_pt(2, vl, ureg[ir][TYPLOC]);
	    }
	}
        break;
    }

    for (i = 0; i < nur; i++) (void)mk_reg(umattyp[i]);

    /* Clear out user data so we can come back */
    rect_boot();
    return(0);
}

/*-----------------RREGION----------------------------------------------
 * Read user region parameters.
 *----------------------------------------------------------------------*/

#define chosen(A) is_specified( param, A) && get_bool( param, A)

rregion(par, param)
    char *par;
    int param;
{
    int mater;

    /* There is a new region */
    nur++;
    /* Actually if you want to be able to specify regions by sums of blocks*/
    /* just read a number on this card, update as necc, and stuff the*/
    /* number into ureg[.][TYPLOC] below. That's the way this started, but users*/
    /* preferred not to know about region numbers. Same for edges below*/
    ureg [nur-1][TYPLOC] = nur-1;

    /* Get its bounds */
    if (read_bound( param, &ureg[ nur-1][ 0], 1)) return(-1);


    /* This is to make sure the user specified one */
    if (chosen( "gas")) {
	fprintf( stderr, "No material specified for region %d\n", nur);
	return(- ++rect_err);
    }

    mater = Si;
    if (chosen( "silicon")) mater = Si;
    if (chosen( "oxide")) mater = SiO2;
    if (chosen( "oxynitride")) mater = OxNi;
    if (chosen( "nitride")) mater = SiNi;
    if (chosen( "poly")) mater = Poly;
    if (chosen( "aluminum")) mater = Al;
    if (chosen( "photoresist")) mater = PhRs;
    if (chosen( "gaas")) mater = GaAs;
    umattyp[ nur-1] = mater;

    return( 0);
}

/*-----------------REDGE------------------------------------------------
 * Read user edge data.
 *----------------------------------------------------------------------*/
redge(par, param)
    char *par;
    int param;
{
    /* There is a new edge */
    nuj++;

    if (read_bound( param, &uedge[ nuj-1][ 0], 0)) return(-1);

    if (chosen ("exposed"))    uedge[ nuj-1][ TYPLOC] = BC_OFFSET + 2;
    if (chosen ("backside"))   uedge[ nuj-1][ TYPLOC] = BC_OFFSET + 1;
    if (chosen ("reflecting")) uedge[ nuj-1][ TYPLOC] = BC_OFFSET;

    /* For our own use */
    if (is_specified( param, "code"))
	uedge [ nuj-1][ TYPLOC] = BC_OFFSET + get_int (param, "code");

    return( 0);
}

/*-----------------READ_BOUND-------------------------------------------
 * Read and error-check the xlo-yhi bounds of a region/edge
 *----------------------------------------------------------------------*/
char *names[6] = {"xlo", "xhi", "ylo", "yhi", "zlo", "zhi"};
read_bound (param, where, what)
    int param;		/* Magic cookie for the parser */
    int where[MAXBNDS];	/* Where to stuff this good info */
    int what;		/* 1 for regions, 0 for edges */
{
    int i;

    /* tag_look does its own error handling */
    for(i = 0; i < mode; i++) {
	if ((where[2*i  ] = tag_look( param, names[2*i  ], i)) < 0 ||
	    (where[2*i+1] = tag_look( param, names[2*i+1], i)) < 0)
	    return(-1);
    }

    for(i = 0; i < mode; i++) {
	if (where[2*i+1] < where[2*i]+what ) {
	    fprintf( stderr, "non-increasing bounds for %s %d\n",
			    what ? "region" : "edge", what ? nur : nuj);
	    return(- ++rect_err);
	}
    }
    return(0);
}


/*-----------------TAG_LOOK---------------------------------------------
 * Look up a name in the user tags field.
 *----------------------------------------------------------------------*/
tag_look( param, name, dim)
    int param;		/* Magic cookie for the parser */
    char *name;		/* Name of desired parameter */
    int dim;		/* Which tag dimension to look up */
{
    int i;
    char *s;

    s = get_string( param, name);
    if (!s) {
	fprintf( stderr, "no value specified for %s\n", name);
	return(- ++rect_err);
    }

    for (i = 0; i < und[ dim]; i++)
	if ( utag[dim][i] )
	    if (!strcmp( s, utag[dim][i])) return( i);

    fprintf( stderr, "no such %c tag: %s\n", XY[dim], s);
    return(- ++rect_err);
}
