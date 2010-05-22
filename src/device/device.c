/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *************************************************************************/
/*   device.c                Version 5.2     */
/*   Last Modification : 7/3/91 15:44:22  */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "matrix.h"
#include "impurity.h"
#include "diffuse.h"
#include "device.h"

/*the list of device functions*/
extern time_val();
extern Nboundary(), Ncoupling();
extern double Nmobil();
extern Hboundary(), Hcoupling();
extern double Hmobil();
extern Psiboundary();
extern poisson_block(), electron_block(), hole_block(), circuit_setup();


/************************************************************************
 *									*
 *	device_init( par, param ) - initialize the device info.		*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
device_init( )
{
    int mat, mat2, i;

    n_con = 0;
    n_ckt = 0;
    dopcalcdone = FALSE;

    /*Electrons*/
    impur[N].diff_coeff = Nmobil;;
    impur[N].coupling = Ncoupling;
    impur[N].boundary = Nboundary;
    impur[N].algebc = NULL;
    impur[N].active = NULL;
    impur[N].time_val = time_val;
    impur[N].block_set = electron_block;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
		impur[N].constant[mat][i][0] = 0.0;
		impur[N].constant[mat][i][1] = 0.0;
		}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[N].seg[SEG0][mat][mat2] = 1.0;
	    impur[N].seg[SEGE][mat][mat2] = 0.0;
	    impur[N].seg[TRN0][mat][mat2] = 0.0;
	    impur[N].seg[TRNE][mat][mat2] = 0.0;
	}
    }
    CLEAR_FLAGS(N, ALL_FLAGS);
    SET_FLAGS(N, PSEUDO);

    /*interstitials*/
    impur[H].diff_coeff = Hmobil;
    impur[H].coupling = Hcoupling;
    impur[H].boundary = Hboundary;
    impur[H].algebc = NULL;
    impur[H].active = NULL;
    impur[H].time_val = time_val;
    impur[H].block_set = hole_block;
    /*constants*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
		impur[H].constant[mat][i][0] = 0.0;
		impur[H].constant[mat][i][1] = 0.0;
		}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[H].seg[SEG0][mat][mat2] = 1.0;
	    impur[H].seg[SEGE][mat][mat2] = 0.0;
	    impur[H].seg[TRN0][mat][mat2] = 0.0;
	    impur[H].seg[TRNE][mat][mat2] = 0.0;
	}
    }
    CLEAR_FLAGS(H, ALL_FLAGS);
    SET_FLAGS(H, PSEUDO);

    /*potential*/
    CLEAR_FLAGS(Psi, ALL_FLAGS);
    SET_FLAGS(N, (MOBILE | DIFFUSING | STEADY | LOCKSTEP) );
    impur[Psi].diff_coeff = NULL;
    impur[Psi].boundary = Psiboundary;
    impur[Psi].algebc = NULL;
    impur[Psi].coupling = NULL;
    impur[Psi].active = NULL;
    impur[Psi].time_val = NULL;
    impur[Psi].block_set = poisson_block;
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
		impur[Psi].constant[mat][i][0] = 0.0;
		impur[Psi].constant[mat][i][1] = 0.0;
		}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[Psi].seg[0][mat][mat2] = 0.0;
	    impur[Psi].seg[1][mat][mat2] = 0.0;
	    impur[Psi].seg[2][mat][mat2] = 0.0;
	    impur[Psi].seg[3][mat][mat2] = 0.0;
	}
    }

    /*circuit terms*/
    CLEAR_FLAGS(CKT, ALL_FLAGS);
    impur[CKT].diff_coeff = NULL;
    impur[CKT].boundary = NULL;
    impur[CKT].algebc = NULL;
    impur[CKT].coupling = NULL;
    impur[CKT].active = NULL;
    impur[CKT].time_val = NULL;
    impur[CKT].block_set = circuit_setup;
    /*these are all useless...*/
    for(mat = 0; mat < MAXMAT; mat++) {
	for(i = 0; i < 25; i++) {
		impur[Psi].constant[mat][i][0] = 0.0;
		impur[Psi].constant[mat][i][1] = 0.0;
		}
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {
	    impur[Psi].seg[0][mat][mat2] = 0.0;
	    impur[Psi].seg[1][mat][mat2] = 0.0;
	    impur[Psi].seg[2][mat][mat2] = 0.0;
	    impur[Psi].seg[3][mat][mat2] = 0.0;
	}
    }
}



/************************************************************************
 *									*
 *	device( par, param ) - device solves the device equations.	*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
device( par, param )
char *par;
int param;
{
    double *area;
    int tfl, tfm;
    char *movie;
    double temp = 293.0;


    if( InvalidMeshCheck()) return( -1);

    area = salloc(double, nn);
    nd2cont = salloc(int, nn);
    nd2cont = salloc(int, nn);
    Ec = salloc(double, nn);
    Ev = salloc(double, nn);

    /*change the solution flags from the default configuration*/
    tfl = GET_FLAGS(Psi);
    CLEAR_FLAGS(Psi, tfl);
    SET_FLAGS(Psi, (MOBILE | DIFFUSING | STEADY | LOCKSTEP) );
    tfm = methdata.factor;
    methdata.factor = RF_ALL;

    /*do we solve for electrons?*/
#ifdef FULLDEV
    if ( get_bool(param, "electron") )
	SET_FLAGS(N, (DIFFUSING | MOBILE) );
    else {
	SET_FLAGS(N, PSEUDO);
	qfn = get_float( param, "qfn" );
    }

    /*do we solve for holes*/
    if ( get_bool(param, "holes") )
	SET_FLAGS(H, (DIFFUSING | MOBILE) );
    else {
	SET_FLAGS(H, PSEUDO);
	qfp = get_float( param, "qfp" );
    }
#else
    SET_FLAGS(N, PSEUDO);
    qfn = 0.0;
    SET_FLAGS(H, PSEUDO);
    qfp = 0.0;
#endif

    /*how wide is the device we are solving?*/
    dev_width = get_float(param, "width") * 1.0e-4;
    devVt = 8.62e-5 * temp;
    devVti = 1.0 / devVt;

    /*set up the circuit - currently dead easy*/
    n_ckt = n_con;

    /*compute material parameters at this temperature*/
    comp_mat( temp );
    /*fix the ni value at room temp*/
    Ni(Si) = Ni(Poly) = 1.07e10;
    Ncon(Si) = Ncon(Poly) = 2.86e19;
    Econ(Si) = Econ(Poly) = 0.5 * 1.1242;
    Nval(Si) = Nval(Poly) = 3.1e19;
    Eval(Si) = Eval(Poly) = - 0.5 * 1.1242;

    /*add the impurities*/
    if ( imptosol[Psi] == -1 ) add_impurity( Psi, 0.0, -1 );
    if ( imptosol[N] == -1 )   add_impurity( N, 1.0e5, -1 );
    if ( imptosol[H] == -1 )   add_impurity( H, 1.0e5, -1 );

    /*prepare the symbolic entries*/
    dev_prep( area );

#ifdef FULLDEV
    if ( imptosol[CKT] == -1 ) add_impurity( CKT, 0.0, -1 );

    if ( get_bool(param, "init") ) init_dev( temp );

    /*read the movie string*/
#else
    movie = get_string( param, "movie" );
    init_dev( temp );
#endif

    /*the time dependent writes*/
    dev_solve( area, movie );

#ifdef FULLDEV
    /*sum up the currents*/
    for(i = 0; i < n_con; i++) {
	cur = 0.0;
	for(j = 0; j < contacts[i].np; j++)
	    cur += nd[ contacts[i].ndc[j] ]->sol[ imptosol[Psi] ];
	printf("contact number %d - coulombs %e\n", i, cur);
	cur = 0.0;
	for(j = 0; j < contacts[i].np; j++)
	    cur += nd[ contacts[i].ndc[j] ]->sol[ imptosol[N] ];
	printf("contact number %d - amps %e\n", i, cur);
    }
#endif
    CLEAR_FLAGS(Psi, ALL_FLAGS);
    SET_FLAGS(Psi, tfl);
    methdata.factor = tfm;

    free(area);
    free(nd2cont);
    free(pt2cont);
    free(Ec);
    free(Ev);
    return(0);
}


/************************************************************************
 *									*
 *	init_dev( ) simple routine to calculate charge neutral starting	*
 *  conditions.								*
 *									*
 *  Original:	MEL	2/89						*
 *									*
 ************************************************************************/
init_dev(temp)
double temp;
{
    register int i, j, imp;
    register int Ps;
    double Vt = kb * temp;
    double ni, c, t1;
    int Ns = imptosol[N];
    int Hs = imptosol[H];

    /*solution locations*/
    Ps = imptosol[Psi];

    /*compute the active concentration*/
    for(i = 0; i < nn; i++) net[i] = 1.0;
    for ( j = 0; j < n_imp; j++ ) {
	switch ( (imp = soltoimp[j]) ) {
	case Asa   :
	case Ba    :
	case Sba   :
	case Pa    :
	case iBea  :
	case iMga  :
	case iSea  :
	case iSia  :
	case iSna  :
	case iGea  :
	case iZna  :
	case iCa   :
	case iGa   :
	    if (IS_ACCEPTOR(imp)) {
		for(i = 0; i < nn; i++) net[i] -= sol_nd(i, j);
	    } else {
		for(i = 0; i < nn; i++) net[i] += sol_nd(i, j);
	    }
	    break;

	default    :
	    break;
	}
    }

    /*solve each semiconductor node for charge netrality*/
    if ( temp < 600.0 ) {
	for(i = 0; i < nn; i++) {
	    if ( IS_SEMI( nd[i]->mater ) ) {
		ni = Ni(nd[i]->mater);
		c = net[i] / ni;
		nd[i]->sol[Ps] = Vt * log(fabs(c)) * c / fabs(c);
	    }
	    else {
		nd[i]->sol[Ps] = 0.0;
	    }
	}
    }
    else {
	for(i = 0; i < nn; i++) {
	    if ( IS_SEMI( nd[i]->mater ) ) {
		ni = 2.0 * Ni(nd[i]->mater);
		c = net[i] / ni;
		t1 = sqrt( c * c + 1.0 );
		nd[i]->sol[Ps] = Vt * log( c + t1 );
	    }
	    else {
		nd[i]->sol[Ps] = 0.0;
	    }
	}
    }

    if ( Ns != -1 )
	for(i = 0; i < nn; i++)
	    set_sol_nd(i, Ns, Ni(mat_nd(i)) * exp( sol_nd(i,Ps) / Vt ));
    if ( Hs != -1 )
	for(i = 0; i < nn; i++)
	    set_sol_nd(i, Hs, Ni(mat_nd(i)) * exp( -sol_nd(i,Ps) / Vt ));
}
