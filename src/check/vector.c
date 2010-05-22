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
/*   vector.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:12:35 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
extern double erf(), erfc();
#include <ctype.h>
#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include "expr.h"
#include "material.h"
#include "impurity.h"
#include "diffuse.h"
#ifdef DEVICE
#include "device.h"
#endif



/************************************************************************
 *									*
 *	get_solval( val, type ) - This routine sets up a vector array	*
 *  based on the value contained in the type parameter.			*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
char *get_solval( val, type )
float *val;
int type;
{
    int index, imp, j;
    register int i;
    double *n, *e, *v, d1, d2;
    int psol, isol;
    double Vt, ltemp;

    ltemp = last_temp != 0.0 ? last_temp : 1173.0 ;
    Vt = ltemp * kb;

    /*two main cases here, the first is an impurity*/
    switch( type ) {
    case As   :
    case B    :
    case Ga   :
    case Sb   :
    case P    :
    case XVEL :
    case YVEL :
    case T    :
    case Au   :
    case Sxx  :
    case Syy  :
    case Sxy  :
    case Cs   :
    case GRN  :
    case iBe  :
    case iMg  :
    case iSe  :
    case iSi  :
    case iSn  :
    case iGe  :
    case iZn  :
    case iC   :
    case iG   :
	index  = imptosol[ type ];
	if ( index == -1 ) return( "The value is not available for plotting\n" );
	for(i = 0; i < nn; i++) val[ i ] = sol_nd(i, index);
	return( NULL );
        break;

    case OXY  :
	index = imptosol[ (imptosol[O2] != -1)? O2 : H2O ];
	if ( index == -1 ) return( "The value is not available for plotting\n" );
	for(i = 0; i < nn; i++) val[ i ] = sol_nd(i, index);
	return( NULL );
        break;

    case I    :
    case V    :
    case Psi  :
	init_pseudo( ltemp );
	comp_mat( ltemp );
	index  = imptosol[ type ];
	if ( index == -1 ) return( "The value is not available for plotting\n" );
	for(i = 0; i < nn; i++) val[ i ] = sol_nd(i, index);
	return( NULL );
        break;

    case N    :
    case H    :
	index = imptosol[ type ];
	if ( index == -1 ) {
	    init_pseudo( ltemp );
	    comp_mat( ltemp );
	    if ( type != N ) Vt = -Vt;
	    index = imptosol[Psi];
	    for(i=0; i<nn; i++) val[i] = exp(sol_nd(i,index) / Vt) * Ni(mat_nd(i));
	}
	else
	    for(i=0; i<nn; i++) val[i] = sol_nd(i,index);
	break;

    case CIS  :
	init_pseudo( ltemp );
	comp_mat( ltemp );
	index = imptosol[ Psi ];
	comp_intparam( ltemp );
	n = salloc( double, (nn+1) );
	e = salloc( double, (nn+1) );
	v = salloc( double, (nn+1) );
	for(i=0; i<nn; i++) n[i] = exp(sol_nd(i,index) / Vt);
	get_Cstar( I, nn, ltemp, n, v, e );
	for(i=0; i<nn; i++) val[i] = v[i];
	sfree( v );
	sfree( e );
	sfree( n );
	break;

    case CVS  :
	init_pseudo( ltemp );
	comp_mat( ltemp );
	index = imptosol[ Psi ];
	comp_vacparam( ltemp );
	n = salloc( double, (nn+1) );
	e = salloc( double, (nn+1) );
	v = salloc( double, (nn+1) );
	for(i=0; i<nn; i++) n[i] = exp(sol_nd(i,index) / Vt);
	get_Cstar( V, nn, ltemp, n, v, e );
	for(i=0; i<nn; i++) val[i] = v[i];
	sfree( v );
	sfree( e );
	sfree( n );
	break;

    case DOP :
	n = salloc( double, (nn) );
	for(i = 0; i < nn; i++) n[i] = 0.0;

	/*sum the signed value of the active concentration*/
	for ( j = 0; j < n_imp; j++ ) {
	    imp = soltoimp[j];
	    switch (imp) {
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
		    for(i = 0; i < nn; i++) n[i] -= sol_nd(i, j);
		} else {
		    for(i = 0; i < nn; i++) n[i] += sol_nd(i, j);
		}
		break;

	    default    :
		break;
	    }
	}
	gaas_act(ltemp, n);
	for(i=0; i<nn; i++) val[i] = n[i];
	sfree( n );
	break;

    case X    :
	for( i = 0; i < nn; i++ ) val[i] = cordinate(pt_nd(i),0);
	break;

    case Y    :
	for( i = 0; i < nn; i++ ) val[i] = cordinate(pt_nd(i),1);
	break;

#ifdef DEVICE
    case QFN :
	psol = imptosol[Psi];
	isol = imptosol[N];
	for( i = 0; i < nn; i++ )
	    val[i] = Phi_n(sol_nd(i,isol), sol_nd(i,psol), Ec[i], mat_nd(i), &d1,&d2);
	break;

    case QFP :
	psol = imptosol[Psi];
	isol = imptosol[H];
	for( i = 0; i < nn; i++ )
	    val[i] = Phi_p(sol_nd(i,isol), sol_nd(i,psol), Ev[i], mat_nd(i), &d1,&d2);
	break;

    case ECON :
	index = imptosol[Psi];
	for( i = 0; i < nn; i++ ) val[i] = - sol_nd(i,index) + Ec[i];
	break;

    case EVAL :
	index = imptosol[Psi];
	for( i = 0; i < nn; i++ ) val[i] = - sol_nd(i,index) + Ev[i];
	break;
#endif

    default :
	for(i = 0; i < nn; i++) val[i] = 0.0;
	break;
    }
    return( NULL );
}



/************************************************************************
 *									*
 *	vfunc( val, type ) - This routine sets up a vector array	*
 *  based on the vector function specified.  val contains the input 	*
 *  array as well as the output.					*
 *									*
 *  Original:	MEL	8/85						*
 *									*
 ************************************************************************/
char *vfunc( val, type, expr )
float *val;
int type;
struct vec_str *expr;	/*argument of the vector function*/
{
    register int i, imp, s;
    double cmax, cabs;
    struct vec_str *rex, *lex;	/*argument of the vector function*/

    switch( type ) {

    case DO_ACTIVE:

	rex = expr->right;
	if ( rex->type != SOLVAL )
	    return("Active can only be taken of an impurity value");

	imp = rex->value.ival;

	/*relate a stored concentration to the active substitutional portion*/
	switch( imp ) {
	case As	: imp = Asa; break;
	case Sb : imp = Sba; break;
	case P  : imp = Pa ; break;
	case B  : imp = Ba ; break;
	case iBe: imp = iBea ; break;
	case iMg: imp = iMga ; break;
	case iSe: imp = iSea ; break;
	case iSi: imp = iSia ; break;
	case iSn: imp = iSna ; break;
	case iGe: imp = iGea ; break;
	case iZn: imp = iZna ; break;
	case iC : imp = iCa  ; break;
	case iG : imp = iGa  ; break;
	default : imp = -1;
	}
	if ( imp == -1 )
	    return("Active can only be taken of an impurity value");

	s = imptosol[imp];

	/*put in the solution variable*/
	for(i = 0; i < nn; i++) val[i] = sol_nd(i,s);

	break;

    case SCALE:
	/*find the max and scale by it*/
	for(i = 0, cmax = 0.0; i < nn; i++) {
	    cabs = val[i]; if (cabs < 0) cabs *= -1;
	    if ( cabs > cmax ) cmax = cabs;
	}
	if (cmax != 0)
	    for(i = 0; i < nn; i++) val[i] /= cmax;
	break;

    case GRADX:
    case GRADY:
	RoughGrad( type, val);
	break;

    case DATA :
	lex = expr->left;
	rex = expr->right;
	if ( lex == NULL ) {
	    if ( rex->type != STRING ) {
		fprintf(stderr, "error in data function, argument must be a filename\n");
	    }
	    else {
		if (read_data( val, rex->value.sval, 2 ,0 )==-1)
		    fprintf(stderr, "error in data function, empty file");
	    }
	}
	else {
	    if ( lex->type != STRING ) {
		fprintf(stderr, "error in data function, argument must be a filename\n");
	    }
	    else if ( rex->type != RCONST ) {
		fprintf(stderr, "second argument must be column number\n");
	    }
	    else {
		if (read_data(val,lex->value.sval,(int)rex->value.dval ,0) == -1)
		    fprintf(stderr, "error in data function, empty file");
	    }
	}
	break;

    default:
	fprintf(stderr, "Internal error: unknown vector function %d\n", type);
	break;
    }
    return( NULL );
}




/************************************************************************
 *									*
 *	constants( str, tok ) - This routine parses the string in str	*
 *  to see if it matches any of the constants defined for vector 	*
 *  expressions and real number parsing.				*
 *									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/
constants( str, tok )
char *str;
struct tok_str *tok;
{

    /*sort of a mass case statement*/
    if ( ! strcmp( str, "Kb" ) ) {
	tok->type = RCONST;		tok->value.dval = 8.62e-5;
	return(0);
    }
    return( -1 );
}


/************************************************************************
 *									*
 *	sol_values( s, tok ) - This routine checks the string in s for 	*
 *  a match with one of the solution values defined.			*
 *									*
 *  Original:	MEL	2/85						*
 *									*
 ************************************************************************/
sol_values( s, tok )
char *s;
struct tok_str *tok;
{

    if (substring( "arsenic", s)) {
	tok->type = SOLVAL;
	tok->value.ival = As;
	return(0);
    }
    else if (substring( "boron", s)) {
	tok->type = SOLVAL;
	tok->value.ival = B;
	return(0);
    }
    else if (substring( "gallium", s)) {
	tok->type = SOLVAL;
	tok->value.ival = Ga;
	return(0);
    }
    else if (substring( "antimony", s)) {
	tok->type = SOLVAL;
	tok->value.ival = Sb;
	return(0);
    }
    else if (substring( "phosphorus", s)) {
	tok->type = SOLVAL;
	tok->value.ival = P;
	return(0);
    }
    else if (substring("interstitial", s)) {
	tok->type = SOLVAL;
	tok->value.ival = I;
	return(0);
    }
    else if (substring( "vacancy", s)) {
	tok->type = SOLVAL;
	tok->value.ival = V;
	return(0);
    }
    else if (substring( "time", s)) {
	tok->type = SOLVAL;
	tok->value.ival = TIM;
	return(0);
    }
    else if (substring( "x", s)){
	tok->type = SOLVAL;
	tok->value.ival = X;
	return(0);
    }
    else if (substring( "y", s)){
	tok->type = SOLVAL;
	tok->value.ival = Y;
	return(0);
    }
    else if (substring( "oxygen", s)){
	tok->type = SOLVAL;
	tok->value.ival = OXY;
	return(0);
    }
    else if (substring( "trap", s)){
	tok->type = SOLVAL;
	tok->value.ival = T;
	return(0);
    }
    else if (substring( "gold", s)){
	tok->type = SOLVAL;
	tok->value.ival = Au;
	return(0);
    }
    else if (substring( "Sxx", s)){
	tok->type = SOLVAL;
	tok->value.ival = Sxx;
	return(0);
    }
    else if (substring( "Syy", s)){
	tok->type = SOLVAL;
	tok->value.ival = Syy;
	return(0);
    }
    else if (substring( "Sxy", s)){
	tok->type = SOLVAL;
	tok->value.ival = Sxy;
	return(0);
    }
    else if (substring( "x.veloc", s)|| (!strcmp( "v.x", s)) || (!strcmp( "xv", s)) || (!strcmp( "vx", s))) {
	tok->type = SOLVAL;
	tok->value.ival = XVEL;
	return(0);
    }
    else if (substring( "y.veloc", s)|| (!strcmp( "v.y", s)) || (!strcmp( "yv", s)) || (!strcmp( "vy", s))) {
	tok->type = SOLVAL;
	tok->value.ival = YVEL;
	return(0);
    }
    else if (substring( "psi", s)){
	tok->type = SOLVAL;
	tok->value.ival = Psi;
	return(0);
    }
    else if (substring( "doping", s)){
	tok->type = SOLVAL;
	tok->value.ival = DOP;
	return(0);
    }
    else if (substring( "holes", s)){
	tok->type = SOLVAL;
	tok->value.ival = H;
	return(0);
    }
    else if (substring( "electrons", s)){
	tok->type = SOLVAL;
	tok->value.ival = N;
	return(0);
    }
    else if (substring( "ci.star", s)){
	tok->type = SOLVAL;
	tok->value.ival = CIS;
	return(0);
    }
    else if (substring( "cv.star", s)){
	tok->type = SOLVAL;
	tok->value.ival = CVS;
	return(0);
    }
    else if (substring( "cesium", s)){
	tok->type = SOLVAL;
	tok->value.ival = Cs;
	return(0);
    }
    else if (substring( "grain", s)){
	tok->type = SOLVAL;
	tok->value.ival = GRN;
	return(0);
    }
#ifdef DEVICE
    else if (substring( "qfn", s)){
	tok->type = SOLVAL;
	tok->value.ival = QFN;
	return(0);
    }
    else if (substring( "qfp", s)){
	tok->type = SOLVAL;
	tok->value.ival = QFP;
	return(0);
    }
    else if (substring( "Ec", s)){
	tok->type = SOLVAL;
	tok->value.ival = ECON;
	return(0);
    }
    else if (substring( "Ev", s)){
	tok->type = SOLVAL;
	tok->value.ival = EVAL;
	return(0);
    }
#endif
    if (substring( "beryllium", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iBe;
	return(0);
    }
    if (substring( "magnesium", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iMg;
	return(0);
    }
    if (substring( "selenium", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iSe;
	return(0);
    }
    if (substring( "isilicon", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iSi;
	return(0);
    }
    if (substring( "tin", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iSn;
	return(0);
    }
    if (substring( "germanium", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iGe;
	return(0);
    }
    if (substring( "zinc", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iZn;
	return(0);
    }
    if (substring( "carbon", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iC;
	return(0);
    }
    if (substring( "generic", s)) {
	tok->type = SOLVAL;
	tok->value.ival = iG;
	return(0);
    }

    return( -1 );
}




/************************************************************************
 *									*
 *	vec_func( s, tok ) - this routine checks to see if the input is	*
 *  a vector function name.						*
 *									*
 *  Original:	MEL	2/86						*
 *									*
 ************************************************************************/
vec_func( s, tok )
char *s;
struct tok_str *tok;
{

    if (substring( "active", s)){
	tok->type = VFN;
	tok->value.ival = DO_ACTIVE;
	return(0);
    }
    else if (substring( "scale", s)){
	tok->type = VFN;
	tok->value.ival = SCALE;
	return(0);
    }
    else if (substring( "gradx", s)){
	tok->type = VFN;
	tok->value.ival = GRADX;
	return(0);
    }
    else if (substring( "grady", s)){
	tok->type = VFN;
	tok->value.ival = GRADY;
	return(0);
    }
    else if (substring( "data", s)){
	tok->type = VFN;
	tok->value.ival = DATA;
	return(0);
    }

    return( -1 );
}

/*-----------------RoughGrad--------------------------------------------
 * Take the gradient of a vector valued function in the most obvious
 * way. Not an accurate procedure!
 *----------------------------------------------------------------------*/
#define XC 0
#define YC 1

RoughGrad( which, vector)
    int which;
    float *vector;
{
    int i, in, nnb, nb, e;
    float *accum, wt, len, ehed;

    /* Initialize averaging */
    accum = salloc( float, nn);

    /* Loop on nodes computing gradients */
    for(in = 0; in < nn; in++) {

	accum[in] = 0.0;
	wt = 0.0;
	nnb = num_edge_nd(in);

	/*for all the neighbors*/
	for(i = 0; i < nnb; i++) {
	    e = edge_nd(in,i);
	    len = len_edg(e);
	    ehed = cpl_edg(e);
	    nb = (nd_edg(e,0)==in)?(nd_edg(e,1)):(nd_edg(e,0));

	    accum[in] += ehed * ((vector[in]-vector[nb]) / len ) *
			 (cordinate(pt_nd(in),which) - cordinate(pt_nd(nb),which)) / len;
	    wt += ehed;
	}
	if ( wt != 0.0 ) accum[in] /= wt;
    }

    /* Average nodes */
    for (i = 0; i < nn; i++) vector[ i] = accum[i];

    sfree( accum);
}








