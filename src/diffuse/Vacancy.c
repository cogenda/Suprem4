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
/*   Vacancy.c                Version 5.1     */
/*   Last Modification : 7/3/91 10:49:51 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "impurity.h"
#include "defect.h"
#include "material.h"
#include "diffuse.h"
#include "matrix.h"

extern float form_eval();

/************************************************************************
 *									*
 *	This file contains definitions of the Vacancy routines. It 	*
 *  includes the routine to calculate diffusion coefficients, the one	*
 *  to calculate any coupling terms between species, and the one to	*
 *  to calculate the boundary condition terms for the Vacancies.	*
 *  Provided the user does not want to change the parameters or access	*
 *  other data, they should be able to modify these extensively without *
 *  any harm to the convergence or solution techniques.			*
 *  									*
 *  Several data base access routines are defined in the file species.c	*
 *  to make it easier for the user to implement routines and get at	*
 *  other data.								*
 *									*
 ************************************************************************/


/************************************************************************
 *									*
 *	Vdiff_coeff(temp, noni, inter, vacan) - This routine calculates	*
 *  the vacancy diffusivity given the temperature, n/ni, and		*
 *  local defect concentrations.					*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
double Vdiff_coeff(temp, new, equil, noni, idf, vdf, iprt, vprt)
float temp;		/*temperature*/
double **new;		/*the current concentration set*/
double **equil;		/*the equilibrium concentration set*/
double *noni;		/*the scaled electron concentration*/
double *idf;		/*the interstitial diffusivity*/
double *vdf;		/*the vacancy diffusivity*/
double *iprt;		/*the partial of the interstitial diffusivity*/
double *vprt;		/*the partial of the vacancy diffusivity*/
{
    register int i;
    int mat;
    double diff;

    for(i = 0; i < nn; i++) {
	mat = nd[i]->mater;

	/*calculate the total diffusivity*/
	diff = Di(V,mat);

	idf[i] = 0.0;
	vdf[i] = diff;

	iprt[i] = 0.0;
	vprt[i] = 0.0;
    }
}






/************************************************************************
 *									*
 *	Vcoupling( ) - This routine calculates the recombination of	*
 *  vacancies with the other species.					*
 *									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/
Vcoupling( temp, area, new, equil, dequ, rhs )
float temp; 		/*the temperature*/
double *area;		/*the area of each node*/
double **new;		/*all the concentrations*/
double **equil;		/*the equilibrium concentrations*/
double **dequ;		/*the nodal areas*/
double **rhs;		/*the current right hand side*/
{
    register int is = imptosol[I];
    register int vs = imptosol[V];
    register int ps = imptosol[Psi];
    register int i, mat;
    double cpl, t1, t2;

    /*if we are solving for vacancies*/
    if ( (is != -1)  ) {

	/*for each node, compute the recombination*/
	for(i = 0; i < nn; i++) {

	    /*get the material*/
	    mat = nd[i]->mater;

	    /*compute the coupling terms*/
	    cpl = Kr(V,mat) * area[i];

	    /*calculate left hand side terms*/
	    a[vs][is][i] += cpl * new[vs][i];
	    a[vs][vs][i] += cpl * new[is][i];
	    a[vs][ps][i] -= cpl * (equil[is][i]*dequ[vs][i] + equil[vs][i]*dequ[is][i]);

	    /*calculate right hand side terms at new point*/
	    cpl *= equil[is][i] * equil[vs][i];
	    t1 = new[is][i] / equil[is][i];
	    t2 = new[vs][i] / equil[vs][i];
	    rhs[vs][i] -= cpl * (t1 * t2 - 1.0);
	}
    }
}






/************************************************************************
 *									*
 *	Vboundary - This routine handles the computation of vacancy	*
 *  boundary conditions at node i and solutions j and k.  The time step	*
 *  is delta and the temperature is temp.  Vel is the velocity of the	*
 *  boundary.								*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
Vboundary( bval )
struct bound_str *bval;
{
    register int Sisol;
    int sol;
    double tmp;
    double vloc;
    double vpow, tpow;
    float vmax, Ovel(), Oss();
    double normal[2];
    int Osp = ((imptosol[H2O] == -1)?(O2):(H2O));
    double K;
    int mat0 = bval->mat[0];
    int mat1 = bval->mat[1];
    int row;
    int imp = V;
    int ps = imptosol[Psi];

    normal[0] = 0.0; normal[1] = -1.0;

    /*the surface condition is a mix of injection and recombination*/
    sol = imptosol[ imp ];

    /*make sure the mat1 stuff is silicon - ish */
    if ( (mat1 == Si) || (mat1 == GaAs)
			|| ( (mat0 != Si) && (mat1 == Poly) ) ) {
	Sisol = 1;
	row = bval->loc[1][1];
    }
    else {
	Sisol = 0;
	row = bval->loc[0][0];
    }

    /*compute the magnitude of the local velocity*/
    vloc = sqrt( bval->vel[0] * bval->vel[0] + bval->vel[1] * bval->vel[1] );

    /*compute the initial velocity of growth*/
    if ( imptosol[XVEL] != -1 )
	vmax = Ovel( bval->temp, Osp, Oss( Osp, SiO2 ), normal, 1.0 );
    else
	vmax = 1.0;

    /*do the surface recombination terms*/
    if ( model(V,mat0, mat1) & RECOM ) {
	/*get the velocity power*/
	if ( vloc != 0.0 )
	    vpow = pow( vloc / vmax, Kpow(V,mat0,mat1) );
	else
	    vpow = 0.0;

	/*compute the surface recombination velocity*/
	K = Ksurf(V,mat0,mat1) * ( Krat(V,mat0,mat1) * vpow + 1.0 );

	/*calculate the surface recombination term*/
	tmp = bval->cpl * K;
	left_side(row, sol, tmp);
	tmp = bval->cpl * K * dequ[sol][row];
	a[sol][ps][row] -= tmp;

	/*right hand side term at new time step*/
	tmp = bval->cpl * K * ( bval->conc[Sisol] - bval->eq[Sisol] );
	right_side(row, sol, bval->rhs, -tmp);
    }

    /*growth model for injection*/
    if ( model(V,mat0, mat1) & GROWTH ) {
	/*get the velocity power*/
	if ( vloc != 0.0 )
	    vpow = pow( vloc / vmax, Gpow(V,mat0,mat1) );
	else
	    vpow = 0.0;

	/*compute the injection term*/
	tmp = - bval->dela[Sisol] * vmole(V,mat0,mat1) * theta(V,mat0,mat1) * vpow;
	right_side(row, sol, bval->rhs, tmp);
    }

    /*time dependent model for injection*/
    if ( model(V,mat0, mat1) & TIME ) {
	if ( injstr(V,mat0,mat1) != NULL )
	    tpow = form_eval( injstr(V,mat0,mat1), total+bval->delta, bval->cord );
	else
	    tpow = A(V,mat0,mat1) *
		   pow( t0(V,mat0,mat1)+total+bval->delta, Tpow(V,mat0,mat1));
	tmp = bval->cpl * tpow;
	right_side(row, sol, bval->rhs, tmp);
    }
}



/************************************************************************
 *									*
 *	time_val( ) - This routine computes the time derivative of the	*
 *  specified impurity.							*
 *									*
 *  Original:	MEL	8/88 (rewrite of two older routines)		*
 *									*
 ************************************************************************/
Vtime_val( vs, rhsnm, cs )
int vs;
double *rhsnm;
struct call_str *cs;
{
    register int i, j, mat, s, si;
    int dop[MAXIMP], ndop;
    int ip = which(V);
    int as = imptosol[As];
    int ps = imptosol[Psi];
    double cpl;
    double gam, igam;
    double t2, t3;
    double n, tmp, dtot, tot, gprt[MAXCHG];

    if ( IS_PSEUDO(V) ) {
	for(i = 0; i < nn; i++) rhs[vs][i] = newb[vs][i];
	return;
    }


    /*compute the number of dopants*/
    for( ndop = j = 0; j < n_imp; j++) {
	switch( soltoimp[j] ) {
	case As :
	case B  :
	case Sb :
	case P  :
	    dop[ndop++] = j;
	}
    }

    /*precompute the time coupling terms*/
    if ( cs->type == TR ) {
	cpl = 2.0 / cs->new_del;
    }
    else if ( cs->type == BDF ) {
	/* wieghting terms */
	gam = cs->old_del / ( cs->new_del + cs->old_del );
	igam = 1.0 - gam;
	cpl = (2.0 - gam) / cs->new_del;
	t2 = (1.0 / gam) / cs->new_del;
	t3 = (igam * igam / gam) / cs->new_del;
    }


    /*since we can't get it vectorized anyway, one big outer loop*/
    for(i = 0; i < nn; i++) {
	cs->newa[vs][i] = cs->new[vs][i];
	dtot = 1.0;

	mat = nd[i]->mater;
	n = noni[i];

	if ( methdata.defmodel == FULLCPL ) {
	    /*compute the total partition value*/
	    tot = Dfrac[ip][mat][CN]
		    + ((Dfrac[ip][mat][CTM] * n + Dfrac[ip][mat][CDM]) * n
			    + Dfrac[ip][mat][CM]) * n
		    + ((Dfrac[ip][mat][CTP] / n + Dfrac[ip][mat][CDP]) / n
			    + Dfrac[ip][mat][CP]) / n ;

	    /*compute the partition functions*/
	    gprt[CN] = Dfrac[ip][mat][CN] / tot;
	    gprt[CM] = n * Dfrac[ip][mat][CM] / tot;
	    gprt[CDM] = n * n * Dfrac[ip][mat][CDM] / tot;
	    gprt[CTM] = n * n * n * Dfrac[ip][mat][CTM] / tot;
	    gprt[CP] = Dfrac[ip][mat][CP] / (tot * n);
	    gprt[CDP] = Dfrac[ip][mat][CDP] / (tot * n * n);
	    gprt[CTP] = Dfrac[ip][mat][CTP] / (tot * n * n * n);

	    /*for each current impurity, compute the number of pairs*/
	    for(j = 0; j < ndop; j++)  {
		s = dop[j];
		si = soltoimp[s];

		/*compute the equilibrium function times charge breakdown*/
		tmp = gprt[CN] * Kcpl(V,mat,si,CN)
			+ gprt[CP] * Kcpl(V,mat,si,CP)
			+ gprt[CDP] * Kcpl(V,mat,si,CDP)
			+ gprt[CTP] * Kcpl(V,mat,si,CTP)
			+ gprt[CM] * Kcpl(V,mat,si,CM)
			+ gprt[CDM] * Kcpl(V,mat,si,CDM)
			+ gprt[CTM] * Kcpl(V,mat,si,CTM);

		/*derivitive w/r to the substitutional value*/
		a[vs][s][i]  += cpl * cs->nco[i] * tmp * cs->new[vs][i] * dact[s][s][i];
		dtot += tmp * cs->newa[s][i];
		cs->newa[vs][i] += tmp * cs->new[vs][i] * cs->newa[s][i];
	    }
	}

	/*compute the appropriate term*/
	if ( cs->type == TR ) {
	    /*trapazoidal rule setup*/
	    a[vs][vs][i] += cpl * cs->nco[i] * dtot;

	    /*right hand side sum of flux terms*/
	    rhs[vs][i] = newb[vs][i] + oldb[vs][i];

	    /*the time terms*/
	    rhs[vs][i] -= cpl * ( cs->nco[i] * cs->newa[vs][i]
			      -   cs->oco[i] * cs->olda[vs][i] );

	}
	else if ( cs->type == BDF ) {
	    a[vs][vs][i] += cpl * cs->nco[i] * dtot;

	    /*right hand side terms*/
	    rhs[vs][i] = newb[vs][i];
	    rhs[vs][i] -=   cs->nco[i] * cpl * cs->newa[vs][i]
			  - cs->mco[i] * t2  * cs->mida[vs][i]
			  + cs->oco[i] * t3  * cs->olda[vs][i];

	}

	/*do it for Arsenic clusters as well*/
	if ( FALSE ) { /*(methdata.defmodel == FULLCPL) && (as != -1) ) {*/
	    if ( cs->type == TR ) {
		/*trapazoidal rule setup*/
		a[vs][as][i] += cpl * cs->nco[i] * (1.0 - dact[as][as][i]);
		a[vs][vs][i] -= cpl * cs->nco[i] * dact[as][vs][i];
		a[vs][ps][i] -= cpl * cs->nco[i] * dact[as][ps][i];

		/*the time terms*/
		rhs[vs][i] -= cpl * ( cs->nco[i] * (cs->new[as][i] - cs->newa[as][i])
				  -   cs->oco[i] * (cs->old[as][i] - cs->olda[as][i]) );

	    }
	    else if ( cs->type == BDF ) {
		a[vs][as][i] += cpl * cs->nco[i] * (1.0 - dact[as][as][i]);
		a[vs][vs][i] -= cpl * cs->nco[i] * dact[as][vs][i];
		a[vs][ps][i] -= cpl * cs->nco[i] * dact[as][ps][i];

		/*right hand side terms*/
		rhs[vs][i] -=   cs->nco[i] * cpl * (cs->new[as][i] - cs->newa[as][i])
			      - cs->mco[i] * t2  * (cs->mid[as][i] - cs->mida[as][i])
			      + cs->oco[i] * t3  * (cs->old[as][i] - cs->olda[as][i]);

	    }
	}

	/*clear out the defect rows - for non silicon and poly*/
	if ( (nd[i]->mater != Si) && (nd[i]->mater != Poly)
					&& (nd[i]->mater != GaAs) ) {
	    clear_row( i, vs );
	    rhs[vs][i] = 0.0;
	    a[vs][vs][i] = 1.0;
	    cs->new[vs][i] = equil[vs][i];
	}

	rhsnm[vs] += rhs[vs][i] * rhs[vs][i];
	if ( a[vs][vs][i] == 0.0 ) a[vs][vs][i] = 1.0;
    }
}



/************************************************************************
 *									*
 *	vacancy( par, param ) - This routine gets the vacancy 		*
 *  diffusion parameters from the cards and sets them up. They 		*
 *  are then used by the rest of the program.				*
 *									*
 *  Original:	MEL	1/85						*
 *									*
 ************************************************************************/
vacancy( par, param )
char *par;
int param;
{
    int mat, mat2 = -1;
    int imp = -1;
    char *tmp;

    /*get the material number specified*/
    if ( get_bool( param, "silicon" ) )		mat = Si;
    if ( get_bool( param, "oxide" ) )		mat = SiO2;
    if ( get_bool( param, "oxynitride" ) )	mat = OxNi;
    if ( get_bool( param, "poly" ) )		mat = Poly;
    if ( get_bool( param, "nitride" ) )		mat = SiNi;
    if ( get_bool( param, "gas" ) )		mat = GAS;
    if ( get_bool( param, "aluminum" ) )	mat = Al;
    if ( get_bool( param, "photoresist" ) )	mat = PhRs;
    if ( get_bool( param, "gaas" ) )		mat = GaAs;

    /*get the bulk values, if specified*/
    Fetch( D0(V,mat), "D.0" );
    Fetch( DE(V,mat), "D.E" );
    Fetch( Kr0(V,mat), "Kr.0" );
    Fetch( KrE(V,mat), "Kr.E" );
    Fetch( Cstar0(V,mat), "Cstar.0" );
    Fetch( CstarE(V,mat), "Cstar.E" );
    Fetch( ktrap0(V,mat), "ktrap.0" );
    Fetch( ktrapE(V,mat), "ktrap.E" );

    /*get the interface material if given*/
    if ( Listed( "/silicon" ) )    mat2 = Si;
    if ( Listed( "/oxide" ) )      mat2 = SiO2;
    if ( Listed( "/oxynitride" ) ) mat2 = OxNi;
    if ( Listed( "/nitride" ) )    mat2 = SiNi;
    if ( Listed( "/poly" ) )       mat2 = Poly;
    if ( Listed( "/gas" ) )        mat2 = GAS;
    if ( Listed( "/gaas" ) )       mat2 = GaAs;

    /*if no second material listed, no interfce parameters can be*/
    if ( mat2 != -1 ) {

	/*work with the model specifiers*/
	if ( is_specified(param, "time.inj") ) {
	    if ( get_bool(param, "time.inj") )
		model(V,mat, mat2) = model(V,mat, mat2) | TIME;
	    else
		model(V,mat, mat2) = model(V,mat, mat2) & ~TIME;
	    model(V,mat2,mat) = model(V,mat,mat2);
	}
	if ( is_specified(param, "growth.inj") ) {
	    if ( get_bool(param, "growth.inj") )
		model(V,mat, mat2) = model(V,mat, mat2) | GROWTH;
	    else
		model(V,mat, mat2) = model(V,mat, mat2) & ~GROWTH;
	    model(V,mat2,mat) = model(V,mat,mat2);
	}
	if ( is_specified(param, "recomb") ) {
	    if ( get_bool(param, "recomb") )
		model(V,mat, mat2) = model(V,mat, mat2) | RECOM;
	    else
		model(V,mat, mat2) = model(V,mat, mat2) & ~RECOM;
	    model(V,mat2,mat) = model(V,mat,mat2);
	}

	/*work on the recombination parameters*/
	if ( is_specified( param, "Ksurf.0" ) ) {
	    Ksurf0(V,mat, mat2) = get_float( param, "Ksurf.0" );
	    Ksurf0(V,mat2, mat) = Ksurf0(V,mat, mat2);
	}
	if ( is_specified( param, "Ksurf.E" ) ) {
	    KsurfE(V,mat, mat2) = get_float( param, "Ksurf.E" );
	    KsurfE(V,mat2, mat) = KsurfE(V,mat, mat2);
	}
	if ( is_specified( param, "Krat.0" ) ) {
	    Krat0(V,mat, mat2) = get_float( param, "Krat.0" );
	    Krat0(V,mat2, mat) = Krat0(V,mat, mat2);
	}
	if ( is_specified( param, "Krat.E" ) ) {
	    KratE(V,mat, mat2) = get_float( param, "Krat.E" );
	    KratE(V,mat2, mat) = KratE(V,mat, mat2);
	}
	if ( is_specified( param, "Kpow.0" ) ) {
	    Kpow0(V,mat, mat2) = get_float( param, "Kpow.0" );
	    Kpow0(V,mat2, mat) = Kpow0(V,mat, mat2);
	}
	if ( is_specified( param, "Kpow.E" ) ) {
	    KpowE(V,mat, mat2) = get_float( param, "Kpow.E" );
	    KpowE(V,mat2, mat) = KpowE(V,mat, mat2);
	}

	/*time dependent injection parameters*/
	if ( is_specified( param, "A.0" ) ) {
	    A0(V,mat, mat2) = get_float( param, "A.0" );
	    A0(V,mat2, mat) = A0(V,mat, mat2);
	}
	if ( is_specified( param, "A.E" ) ) {
	    AE(V,mat, mat2) = get_float( param, "A.E" );
	    AE(V,mat2, mat) = AE(V,mat, mat2);
	}
	if ( is_specified( param, "t0.0" ) ) {
	    t00(V,mat, mat2) = get_float( param, "t0.0" );
	    t00(V,mat2, mat) = t00(V,mat, mat2);
	}
	if ( is_specified( param, "t0.E" ) ) {
	    t0E(V,mat, mat2) = get_float( param, "t0.E" );
	    t0E(V,mat2, mat) = t0E(V,mat, mat2);
	}
	if ( is_specified( param, "Tpow.0" ) ) {
	    Tpow0(V,mat, mat2) = get_float( param, "Tpow.0" );
	    Tpow0(V,mat2, mat) = Tpow0(V,mat, mat2);
	}
	if ( is_specified( param, "Tpow.E" ) ) {
	    TpowE(V,mat, mat2) = get_float( param, "Tpow.E" );
	    TpowE(V,mat2, mat) = TpowE(V,mat, mat2);
	}

	/*growth dependent injection parameters*/
	if ( is_specified( param, "vmole" ) ) {
	    vmole(V,mat, mat2) = get_float( param, "vmole" );
	    vmole(V,mat2, mat) = vmole(V,mat, mat2);
	}
	if ( is_specified( param, "theta.0" ) ) {
	    theta0(V,mat, mat2) = get_float( param, "theta.0" );
	    theta0(V,mat2, mat) = theta0(V,mat, mat2);
	}
	if ( is_specified( param, "theta.E" ) ) {
	    thetaE(V,mat, mat2) = get_float( param, "theta.E" );
	    thetaE(V,mat2, mat) = thetaE(V,mat, mat2);
	}
	if ( is_specified( param, "Gpow.0" ) ) {
	    Gpow0(V,mat, mat2) = get_float( param, "Gpow.0" );
	    Gpow0(V,mat2, mat) = Gpow0(V,mat, mat2);
	}
	if ( is_specified( param, "Gpow.E" ) ) {
	    GpowE(V,mat, mat2) = get_float( param, "Gpow.E" );
	    GpowE(V,mat2, mat) = GpowE(V,mat, mat2);
	}
	if ( is_specified( param, "inj.str" ) ) {
	    tmp = get_string( param, "inj.str" );
	    injstr(V,mat,mat2) = injstr(V,mat2,mat) = salloc(char, strlen(tmp)+1);
	    strcpy( injstr(V,mat,mat2), tmp );
	}

	/*segregation parameters*/
	if ( is_specified( param, "Seg.0" ) ) {
	    Seg0(V,mat, mat2) = get_float( param, "Seg.0" );
	    Seg0(V,mat2, mat) = 1.0 / Seg0(V,mat, mat2);
	}
	if ( is_specified( param, "Seg.E" ) ) {
	    SegE(V,mat, mat2) = get_float( param, "Seg.E" );
	    SegE(V,mat2, mat) = - SegE(V,mat, mat2);
	}
	if ( is_specified( param, "Trn.0" ) ) {
	    Trn0(V,mat, mat2) = get_float( param, "Trn.0" );
	    Trn0(V,mat2, mat) = Trn0(V,mat, mat2);
	}
	if ( is_specified( param, "Trn.E" ) ) {
	    TrnE(V,mat, mat2) = get_float( param, "Trn.E" );
	    TrnE(V,mat2, mat) = TrnE(V,mat, mat2);
	}

    }

    /*Coupling coefficients with the impurities*/
    if ( Listed( "boron" ) )      imp = B;
    if ( Listed( "antimony" ) )   imp = Sb;
    if ( Listed( "arsenic" ) )    imp = As;
    if ( Listed( "phosphorus" ) ) imp = P;
    if ( Listed( "beryllium" ) )  imp = iBe;
    if ( Listed( "magnesium" ) )  imp = iMg;
    if ( Listed( "selenium" ) )   imp = iSe;
    if ( Listed( "isilicon" ) )    imp = iSi;
    if ( Listed( "tin" ) )        imp = iSn;
    if ( Listed( "germanium" ) )  imp = iGe;
    if ( Listed( "zinc" ) )       imp = iZn;
    if ( Listed( "carbon" ) )     imp = iC;
    if ( Listed( "generic" ) )    imp = iG;

    /*if imp listed than we have coupling coefficients*/
    if ( (imp != -1) ) {
	Fetch( Kcpl(V,mat,imp,CN), "neu.0" );
	Fetch( Kcpl(V,mat,imp,CM), "neg.0" );
	Fetch( Kcpl(V,mat,imp,CDM), "dneg.0" );
	Fetch( Kcpl(V,mat,imp,CTM), "tneg.0" );
	Fetch( Kcpl(V,mat,imp,CP), "pos.0" );
	Fetch( Kcpl(V,mat,imp,CDP), "dpos.0" );
	Fetch( Kcpl(V,mat,imp,CTP), "tpos.0" );
    }
    /*else we have charge ratios*/
    else {
	/*get the charge ratios*/
	Fetch( Dfrac0[which(V)][mat][CN], "neu.0" );
	Fetch( Dfrac0[which(V)][mat][CM], "neg.0" );
	Fetch( Dfrac0[which(V)][mat][CDM], "dneg.0" );
	Fetch( Dfrac0[which(V)][mat][CTM], "tneg.0" );
	Fetch( Dfrac0[which(V)][mat][CP], "pos.0" );
	Fetch( Dfrac0[which(V)][mat][CDP], "dpos.0" );
	Fetch( Dfrac0[which(V)][mat][CTP], "tpos.0" );

	Fetch( DfracE[which(V)][mat][CN], "neu.E" );
	Fetch( DfracE[which(V)][mat][CM], "neg.E" );
	Fetch( DfracE[which(V)][mat][CDM], "dneg.E" );
	Fetch( DfracE[which(V)][mat][CTM], "tneg.E" );
	Fetch( DfracE[which(V)][mat][CP], "pos.E" );
	Fetch( DfracE[which(V)][mat][CDP], "dpos.E" );
	Fetch( DfracE[which(V)][mat][CTP], "tpos.E" );
    }
}



/************************************************************************
 *									*
 *	comp_vacparam() - This routine computes the temperature 	*
 *  dependent vacancy parameters.					*
 *									*
 *  Original:	MEL	3/88						*
 *									*
 ************************************************************************/
comp_vacparam( temp )
float temp;
{
    double Vt = kb * temp;
    register int mat1, mat2;
    register int wi = which(V);


    /*for all the materials*/
    for(mat1 = 0; mat1 < MAXMAT; mat1++) {

	/*bulk parameters*/
	Di(V,mat1) = D0(V,mat1) * exp( - DE(V,mat1) / Vt );
	Kr(V,mat1) = Kr0(V,mat1) * exp( - KrE(V,mat1) / Vt );
	Cstar(V,mat1) = Cstar0(V,mat1) * exp( - CstarE(V,mat1) / Vt );
	ktrap(V,mat1) = ktrap0(V,mat1) * exp( - ktrapE(V,mat1) / Vt );
	Dfrac[wi][mat1][CN] = Dfrac0[wi][mat1][CN]*exp(-DfracE[wi][mat1][CN]/Vt);
	Dfrac[wi][mat1][CM] = Dfrac0[wi][mat1][CM]*exp(-DfracE[wi][mat1][CM]/Vt);
	Dfrac[wi][mat1][CDM] = Dfrac0[wi][mat1][CDM]*exp(-DfracE[wi][mat1][CDM]/Vt);
	Dfrac[wi][mat1][CTM] = Dfrac0[wi][mat1][CTM]*exp(-DfracE[wi][mat1][CTM]/Vt);
	Dfrac[wi][mat1][CP] = Dfrac0[wi][mat1][CP]*exp(-DfracE[wi][mat1][CP]/Vt);
	Dfrac[wi][mat1][CDP] = Dfrac0[wi][mat1][CDP]*exp(-DfracE[wi][mat1][CDP]/Vt);
	Dfrac[wi][mat1][CTP] = Dfrac0[wi][mat1][CTP]*exp(-DfracE[wi][mat1][CTP]/Vt);

	/*interface terms*/
	for(mat2 = 0; mat2 < MAXMAT; mat2++) {

	    /*surface recombination*/
	    Ksurf(V,mat1,mat2) = Ksurf0(V,mat1,mat2) * exp( - KsurfE(V,mat1,mat2) / Vt );
	    Krat(V,mat1,mat2) = Krat0(V,mat1,mat2) * exp( - KratE(V,mat1,mat2) / Vt );
	    Kpow(V,mat1,mat2) = Kpow0(V,mat1,mat2) * exp( - KpowE(V,mat1,mat2) / Vt );

	    /*growth model for injection*/
	    theta(V,mat1,mat2) = theta0(V,mat1,mat2) * exp( - thetaE(V,mat1,mat2) / Vt );
	    Gpow(V,mat1,mat2) = Gpow0(V,mat1,mat2) * exp( - GpowE(V,mat1,mat2) / Vt );

	    /*time dependent injection model*/
	    A(V,mat1,mat2) = A0(V,mat1,mat2) * exp( - AE(V,mat1,mat2) / Vt );
	    t0(V,mat1,mat2) = t00(V,mat1,mat2) * exp( - t0E(V,mat1,mat2) / Vt );
	    Tpow(V,mat1,mat2) = Tpow0(V,mat1,mat2) * exp( - TpowE(V,mat1,mat2) / Vt );

	    /*segregation terms*/
	    Seg(V,mat1,mat2) = Seg0(V,mat1,mat2) * exp( - SegE(V,mat1,mat2) / Vt );
	    Trn(V,mat1,mat2) = Trn0(V,mat1,mat2) * exp( - TrnE(V,mat1,mat2) / Vt );
	}
    }
}


