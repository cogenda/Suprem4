/*************************************************************************
 *									 *
 *     Copyright c 1989 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *************************************************************************/
/*   contact.c                Version 5.2     */
/*   Last Modification : 7/3/91 15:44:16  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "constant.h"
#include "geom.h"
#include "material.h"
#include "device.h"


/************************************************************************
 *									*
 *	contact() - This routine defines contacts and their information.*
 *									*
 *  Original:	MEL	11/88						*
 *									*
 ************************************************************************/
contact( par, param )
char *par;
int param;
{
    char *str;
    int num = -1;

    if( InvalidMeshCheck()) return( -1);

    /*if we are asked to generate contacts*/
    if ( is_specified(param, "generate") ) gen_contact();

    /*get the number of the contact the rest applies to*/
    if (is_specified(param, "number")) {

	/*get the number*/
	num = get_float( param, "number" );

	/*is this a valid number*/
	if ( (num < 0) || (num >= n_con) ) return( -1 );

	/*let's get the contact parameters stored away*/
	if (is_specified(param, "name")) {
	    str = get_string( param, "name" );
	    contacts[num].name = salloc( char, strlen(str)+1 );
	    strcpy( contacts[num].name, str );
	}
    }
    return(0);
}

#define NB(i,j) (tri[i]->nb[j])


/************************************************************************
 *									*
 *	contact() - This routine defines contacts and their information.*
 *									*
 *  Original:	MEL	11/88						*
 *									*
 ************************************************************************/
gen_contact()
{
    register int i, j, k, p;
    register int n1, n2, nt, n;
    int *chk[3], len;
    double minx, miny, maxx, maxy;
    double mat, matlist[MAXMAT];
    char *matnam[MAXMAT];

    matnam[0] = "gas";
    matnam[1] = "oxide";
    matnam[2] = "nitride";
    matnam[3] = "silicon";
    matnam[4] = "poly";
    matnam[5] = "oxynitride";
    matnam[6] = "aluminum";
    matnam[7] = "photoresist";

    /*malloc some space for the check array*/
    chk[0] = salloc( int, (ne+1) );
    chk[1] = salloc( int, (ne+1) );
    chk[2] = salloc( int, (ne+1) );

    /*clear the current number of contacts*/
    n_con = 0;

    /*no point has been checked*/
    for(i = 0; i < ne; i++) chk[0][i] = chk[1][i] = chk[2][i] = FALSE;
    for(i = 0; i < nn; i++) nd2cont[i] = -1;
    for(i = 0; i < np; i++) pt2cont[i] = -1;

    /*check all edges until we get lucky*/
    for(i = 0; i < ne; i++) {

	/*check each side*/
	for(j = 0; j < 3; j++) {

	    n1 = tri[i]->nd[(j+1)%3];
	    n2 = tri[i]->nd[(j+2)%3];

	    /*if we've already checked this side*/
	    if ( chk[j][i] ) continue;

	    /*set checked*/
	    chk[j][i] = TRUE;

	    /*if the side looks contactish*/
	    if ( IS_CONT(i,j) ) {

		/*initialize contact*/
		contacts[n_con].name = NULL;
		len = 20;
		contacts[n_con].pt = salloc( int, len+1 );
		contacts[n_con].ndc = salloc( int, len+1 );

		/*figure out the contacting material number*/
		if ( tri[i]->nb[j] < 0 )
		    mat = GAS;
		else
		    mat = mat_reg( reg_tri(NB(i,j)));

		/*add the two points we know about*/
		p = nd[n1]->pt;
		contacts[n_con].pt[0] = p;
		pt2cont[p] = n_con;
		for( nt = -1, k = 0; k < pt[p]->nn; k++ ) {
		    n = pt[p]->nd[k];
		    if ( nd[n]->mater == mat ) nt = n;
		}
		if ( nt == -1 ) panic("consistency error in contact generation");
		contacts[n_con].ndc[0] = nt;
		nd2cont[nt] = n_con;

		p = nd[n2]->pt;
		contacts[n_con].pt[1] = p;
		pt2cont[p] = n_con;
		for( nt = -1, k = 0; k < pt[p]->nn; k++ ) {
		    n = pt[p]->nd[k];
		    if ( nd[n]->mater == mat ) nt = n;
		}
		if ( nt == -1 ) panic("consistency error in contact generation");
		contacts[n_con].ndc[1] = nt;
		nd2cont[nt] = n_con;

		contacts[n_con].np = 2;

		/*recursively get the neighbor contact points*/
		find_con( n1, n_con, chk, &len );
		find_con( n2, n_con, chk, &len );

		/*increment the number of contacts*/
		n_con++;
	    }
	}
    }

    /*free the check array*/
    free( chk[0] );
    free( chk[1] );
    free( chk[2] );

    /*prepare a report about the contacts in question*/
    for(i = 0; i < n_con; i++) {

	for(j = 0; j < MAXMAT; j++) matlist[j] = 0;

	/*find the min and max dimensions, prepare material checklist*/
	p = contacts[i].pt[0];
	minx = maxx = pt[p]->cord[0];
	miny = maxy = pt[p]->cord[1];
	for(j = 1; j < contacts[i].np; j++) {
	    p = contacts[i].pt[j];
	    if ( pt[p]->cord[0] < minx ) minx = pt[p]->cord[0];
	    if ( pt[p]->cord[0] > maxx ) maxx = pt[p]->cord[0];
	    if ( pt[p]->cord[1] < miny ) miny = pt[p]->cord[1];
	    if ( pt[p]->cord[1] > maxy ) maxy = pt[p]->cord[1];

	    for(k = 0; k < pt[p]->nn; k++)
		matlist[ nd[ pt[p]->nd[k] ]->mater ] = 1;

	    matlist[ nd[ contacts[i].ndc[j] ]->mater ] = 2;
	}

	/*print out report*/
	printf("contact number %d:\n", i);
	printf("    contacted materials ");
	for(j = 0; j < MAXMAT; j++) if (matlist[j] == 1) printf("%s ",matnam[j]);
	printf("\n");
	printf("    contact  material ");
	for(j = 0; j < MAXMAT; j++) if (matlist[j] == 2) printf("%s ",matnam[j]);
	printf("\n    and is bounded by:\n");
	printf("    x :\t%e\t%e\n", minx, maxx);
	printf("    y :\t%e\t%e\n", miny, maxy);
    }
}


/************************************************************************
 *									*
 *	find_con() - This routine finds contacts and adds them to the	*
 *  the list of contact nodes.						*
 *									*
 *  Original:	MEL	1/89						*
 *									*
 ************************************************************************/
find_con( n, nc, chk, len )
int n;		/*the node to check*/
int nc;		/*the contact number we're working with*/
int *chk[3];	/*the list of checked nodes*/
int *len;	/*the allocated length of nodes*/
{
    register int i, ie, j, p, k;
    register int n1, n2, nt, mat1;

    /*check all the triangles this node is connected to*/
    for(i = 0; i < num_tri_nd(n); i++) {
	ie = tri_nd(n, i);

	/*for each edge*/
	for(j = 0; j < 3; j++) {

	    if ( chk[j][ie] ) continue;

	    /*get the edge numbers*/
	    n1 = tri[ie]->nd[(j+1)%3];
	    n2 = tri[ie]->nd[(j+2)%3];

	    if ( (n1 != n) && (n2 != n) ) continue;

	    /*set checked*/
	    chk[j][ie] = TRUE;

	    /*if the side looks contactish*/
	    if ( IS_CONT(ie,j) ) {

		if ( contacts[n_con].np == (*len-1) ) {
		    *len += 20;
                    contacts[n_con].pt = scalloc( int, *len );
		    contacts[n_con].ndc = sralloc( int, *len, contacts[n_con].ndc );
		}

		/*figure out the contacting material number*/
		if ( tri[ie]->nb[j] < 0 )
		    mat1 = GAS;
		else
		    mat1 = mat_reg( reg_tri(NB(ie,j)));

		/*make n1 the new point*/
		if ( n1 == n ) n1 = n2;

		p = nd[n1]->pt;
		contacts[n_con].pt[contacts[n_con].np] = p;
		pt2cont[p] = n_con;
		for( nt = -1, k = 0; k < pt[p]->nn; k++ ) {
		    n = pt[p]->nd[k];
		    if ( nd[n]->mater == mat1 ) nt = n;
		}
		if ( nt == -1 ) panic("consistency error in contact generation");
		contacts[n_con].ndc[contacts[n_con].np] = nt;
		nd2cont[nt] = n_con;

		contacts[n_con].np += 1;

		find_con( n1, nc, chk, len );
	    }
	}
    }
}



