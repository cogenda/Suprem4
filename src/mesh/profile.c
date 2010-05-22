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
/*   profile.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:30:52 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <global.h>
#include <constant.h>
#include <dbaccess.h>
#include <impurity.h>	/* For B, P, Sb, As */
#include <material.h>
#include <defect.h>
#include <expr.h>




/************************************************************************
 *									*
 *	initialize( par, param ) - This routine allows the user to read *
 *  in a file of doping data and apply it to the whole device.		*
 *									*
 *  Original :	MEL	Sept, 1988					*
 *									*
 ************************************************************************/
profile( par, param )
char *par;
int param;
{
    register int i;
    char *infile;
    float *val;
    int imp, sol;
    double offset;

    /*get the type of mesh to be read, and the filename*/
    infile = get_string( param, "infile" );
    if (is_specified(param,"boron")     &&get_bool(param, "boron"))     imp=B;
    if (is_specified(param,"antimony")  &&get_bool(param, "antimony"))  imp=Sb;
    if (is_specified(param,"arsenic")   &&get_bool(param, "arsenic"))   imp=As;
    if (is_specified(param,"phosphorus")&&get_bool(param,"phosphorus")) imp=P;
    if (is_specified(param,"beryllium") &&get_bool(param,"beryllium"))	imp=iBe;
    if (is_specified(param,"magnesium") &&get_bool(param,"magnesium"))	imp=iMg;
    if (is_specified(param,"selenium")  &&get_bool(param,"selenium"))	imp=iSe;
    if (is_specified(param,"isilicon")  &&get_bool(param,"isilicon"))	imp=iSi;
    if (is_specified(param,"tin")       &&get_bool(param,"tin"))	imp=iSn;
    if (is_specified(param,"germanium") &&get_bool(param,"germanium"))	imp=iGe;
    if (is_specified(param,"zinc")      &&get_bool(param,"zinc"))	imp=iZn;
    if (is_specified(param,"carbon")    &&get_bool(param,"carbon"))	imp=iC;
    if (is_specified(param,"generic")   &&get_bool(param,"generic"))	imp=iG;
    if (is_specified(param,"interstitial")&&get_bool(param,"interstitial")) imp=I;
    if (is_specified(param,"vacancy")&&get_bool(param,"vacancy")) imp=V;

    /* create the pointers for the new impurity */
    if ( imp== -1 ) {
	fprintf(stderr, "Must specify impurity to read in!\n");
	return(-1);
    }
    if ( is_specified(param, "offset") )
	    offset = get_float(param, "offset");
    else
	offset = 0.0;

    val = salloc(float, nn);
    if (read_data( val, infile, 2 ,offset ) == -1) return(-1);

    if ( (imp == I) || (imp == V) ) {
	if ( last_temp != 0.0 )
	    init_pseudo( last_temp );
	else
	    init_pseudo( 1173.0 );
	get_defaults( NULL );
	damage_read = TRUE;
    }
    else {
	add_impurity( imp, 1.0e5, -1);
    }

    sol = imptosol[imp];

    /*for all mesh points, add in the new doping values*/
    for(i = 0; i < nn; i++) {
	/*don't add defects to non si materials*/
	switch(imp) {
	case I :
	case V :
	    if ( (mat_nd(i) == Si) || (mat_nd(i) == Poly)
					|| (mat_nd(i) == GaAs) )
		set_sol_nd(i,sol, val[i]+sol_nd(i,sol));
	    break;
	default :
	    set_sol_nd(i,sol, val[i]+sol_nd(i,sol));
	}
    }

    free(val);
    return(0);
}




/************************************************************************
 *									*
 *	read_data( arr, file, col ) Read and interpolate data into 	*
 *  passed array.							*
 *									*
 *  Original :	HYP	9/90						*
 *									*
 ************************************************************************/
read_data( val, infile, col, offset  )
float *val, offset;
char *infile;
int col;
{
    register int i, j, cnt;
    FILE *lu;
    char iline[BUFSIZ];
    double x, y, *data;

    /*make sure we can open and read the data file we have*/
    if (( lu = fopen( infile, "r" )) == NULL) {
	fprintf(stderr, "profile: can not open %s\n", infile);
	return(-1);
    }

    /*read all the data in*/
    data = salloc(double, 2*3000);
    for(cnt = 0;  fgets( iline, BUFSIZ, lu) != NULL;  ) {

	if ( sscanf( iline, "%le %le", &x, &y ) == 2 ) {
	    data[2*cnt+0] = (x - offset) * 1.0e-4;
	    data[2*cnt+1] = log(y);
	    cnt++;
	}
    }

    if ( cnt < 2 ) {
	fprintf(stderr, "profile: %s contains less than two points\n", infile);
	free(data);
	return(-1);
    }

    /*for all mesh points, add in the new doping values*/
    for(i = 0; i < nn; i++) {

	x = depth_cord( pt_nd(i) );

	/*don't process unless we are in the bounds of the doping*/
	if ( (x >= data[2*0+0]) && (x <= data[2*(cnt-1)+0]) ) {

	    for(j = 0; (j < cnt-1) && (x > data[2*(j+1)+0]); j++);

	    if ( j < cnt-1 ) {

		y = (x - data[2*j+0]) / (data[2*(j+1)+0] - data[2*j+0]);
		y = (data[2*(j+1)+1] - data[2*j+1]) * y + data[2*j+1];

		val[i] = exp(y);
	    }
	}
	else {
	    val[i] = 0.0;
	}
    }

    free(data);
    fclose( lu );
    return(0);
}

