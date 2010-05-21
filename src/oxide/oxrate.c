static char sccsid[]="$Header oxrate.c rev 5.1 7/3/91 10:52:38";
/*----------------------------------------------------------------------
 *
 * oxrate - computing and manipulating oxide growth coefficients.
 *
 * Copyright c 1985 The board of trustees of the Leland Stanford
 *                  Junior University. All rights reserved.
 * This subroutine may not be used outside of the SUPREM4 computer
 * program without the prior written consent of Stanford University.
 *
 * Original: CSR Sun Jun  2 22:09:03 MDT 1985
 *---------------------------------------------------------------------*/
#include <math.h>	/* for Arrhenius laws */
#include <global.h>
#include <constant.h>
#include <geom.h>
#include <material.h>
#include <impurity.h>

/* Cache the values of the thin oxide coeffs on the three common orientations */
static float trateS[3], tlenS[3];

/*-----------------DEAL_GROVE-------------------------------------------
 * Compute the Deal-Grove coefficients from the given data.
 *----------------------------------------------------------------------*/
deal_grove (temp, ornt, oxytyp, arate, brate)
    float temp; 		/* temperature */
    int   ornt;			/* substrate orientation */
    int oxytyp;			/* dry/wet oxidation */
    float *arate, *brate; 	/* A and B coefficients */
{
    int il, ip, io;
    float lcl, pcl;		/* HCL terms */
    float kT = kb*temp;
    static float boverai, bi, ti; /* intrinsic values of B/A, B, t */
    float bovera, b;		/* computed values of B/A, B, t*/

    /* Figure low/high temperature range */
    if( temp < lbreak[ oxytyp ]) il = 0; else il=1;
    if( temp < pbreak[ oxytyp ]) ip =  0; else ip=1;

    /* Get the intrinsic rates */
    boverai = lrate0[ oxytyp ][ il ] * exp (-lratee[ oxytyp ][ il ]/kT);
    bi =      prate0[ oxytyp ][ ip ] * exp (-pratee[ oxytyp ][ ip ]/kT);

    /* Adjust for orientation */
    boverai *= oridep[ ornt];
    
    /* Adjust for pressure */
    bovera = boverai * pow( pressure, lpdep[ oxytyp]);
    b      = bi      * pow( pressure, ppdep[ oxytyp]);
    
    /* Adjust for chlorine */
    if( oxytyp == 0) {		/* dry*/
	cldep (temp, &lcl, &pcl);
	bovera *= lcl;
	b *= pcl;
    }

    /* Copy to passed parameters */
    if (boverai == 0) {		/* surely something wrong */
	*arate = 0;
	*brate = 0;
	return;
    }
    *brate = b;
    if (bovera == 0) *arate = 0;
    else             *arate = b/bovera;

    /* Just afore ye' go: */
    /* Store the three thin-oxide datasets for known orientations */
    if( oxytyp == 0) /* dry */
	for( io = Or100; io <= Or111; io++) {
	    ti = thox0 [ oxytyp ][ io] * exp (-thoxe [ oxytyp ][ io ]/kT);
	    trateS[ io ] = ti * pow( pressure, ppdep[ oxytyp ]);
	    tlenS [ io ] = thoxl[ oxytyp ][ io ];
	}

    return;
}


/*-----------------ThinOxideCorr----------------------------------------
 * This routine calculates the thin oxide correction to dxdt.
 * The oxidant diffusion is calculated using the standard B, B/A, then
 * this is added to the dx/dt.  It's not a diffusive term, in other words.
 * It exists in Ovel but not in Orr.
 *----------------------------------------------------------------------*/
float ThinOxideCorr( thick, normal)
    double thick;		/* thickness-dependent model (Massoud) */
    double normal[2];		/* The local normal vector */

{
    int io;
    float DoOriDep(), trate[3], v;
    double XtalDir[3];

    if( gas_type != GAS_DRYO2)
	return(0);

    /*for thick oxides, nothing happens avoid overflow*/
    if ( thick > 1.0e-4 ) return(0.0);
    
    /* Calculate potential rate in each of three known crystal directions */
    for( io = Or100; io <= Or111; io++)
	trate[ io] = trateS[ io] * exp( -thick / tlenS[ io]);

    /* Interpolate to crystal direction */
    SupToXtal( normal, XtalDir);
    v = DoOriDep( XtalDir, trate);
    return( v );
}


/*-----------------cldep------------------------------------------------
 * The chlorine dependence model - lookup table approach.
 *----------------------------------------------------------------------*/
cldep (temp, lcl, pcl)
    float temp;
    float *lcl, *pcl;
{
    extern float linear_int();
    int ip, it;
    float v0, v1, v2, v3, s, t;

    if( ClPct == 0 || nclt == 0 || nclp == 0 || cldt == 0 || cldp == 0 || lcldat == 0 || pcldat == 0) {
	*lcl = 1;
	*pcl = 1;
	return;
    }
    
    /* Interpolate in chlorine and temperature matrix */
    linlook( ClPct, cldp, nclp, &ip, &s);
    linlook( temp, cldt, nclt, &it, &t);

    /* do bilinear interpolation on linear, parabolic terms*/ 
    v0 = lcldat[ nclt*(ip    ) + it    ];
    v2 = lcldat[ nclt*(ip    ) + it + 1];
    v1 = lcldat[ nclt*(ip + 1) + it    ];
    v3 = lcldat[ nclt*(ip + 1) + it + 1];
    *lcl = v0 + s*(v1-v0) + t*(v2-v0) + s*t*(v0+v3-v1-v2);

    v0 = pcldat[ nclt*(ip    ) + it    ];
    v2 = pcldat[ nclt*(ip    ) + it + 1];
    v1 = pcldat[ nclt*(ip + 1) + it    ];
    v3 = pcldat[ nclt*(ip + 1) + it + 1];
    *pcl = v0 + s*(v1-v0) + t*(v2-v0) + s*t*(v0+v3-v1-v2);
}

/*-----------------linlook-----------------------------------------
 * Linear lookup in a table.
 * Returns lower index of largest interval below value(i) and position of entry
 * in the interval.
 * For example, for val=1/4 between tbl[i] and tbl[i+1], return i and 0.25.
 * For val < table[0], this means 0 and 0.0.
 * For val > table[n-1], this means n-2 and 1.0.
 *----------------------------------------------------------------------*/
linlook( val, fla, n, ixl, s)
    float val, *fla;
    int n;
    int *ixl;			/* returned index */
    float *s;			/* returned interval position */
{
    int i;

    if( val <= fla[0]) {
	*ixl = 0; *s = 0.0; return;
    }
    if( val >= fla[ n-1]) {
	*ixl = n-2; *s = 1.0; return;
    }
    for( i = 0; i < n-1; i++)
	if( fla[ i] <= val && val <= fla[ i+1]) {
	    *ixl = i;
	    *s = (val - fla[ i])/(fla[ i+1]-fla[ i]);
	    return;
	}
    /*NOTREACHED*/
    panic("reached unreachable code");
    return;
}


/*-----------------DO_DEAL_GROVE----------------------------------------
 * Wrapper routine for deal grove to do it as rarely as possible.
 * Always calls with 111, expecting orientation effects to be
 * computed locally.
 * CSR July 87
 *----------------------------------------------------------------------*/

do_deal_grove (temp, s)
     float temp;		/* temperature */
     int s;			/* species */
{
    static float o_temp = 0, o_pressure = 0, o_cl= -1;
    static int o_ornt = -1, o_s = -1;
    if (o_temp != temp || o_pressure != pressure || o_s != s || o_ornt != sub_ornt || o_cl != ClPct) {
	o_ornt = sub_ornt;
	o_temp = temp;
	o_s = s;
	o_pressure = pressure;
	o_cl = ClPct;
	deal_grove (temp, Or111, (s == H2O), &A_DealGrove, &B_DealGrove);
    }
}
