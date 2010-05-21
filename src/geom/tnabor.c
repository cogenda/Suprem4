static char sccsid[]="tnabor.c Version 5.1  Last modified 7/3/91 08:34:07";
/*------------------TNABOR----------------------------------------------
 *
 * Determine whether two triangles are neighbours.
 * If so, return ke = side of ie on which in abuts,
 *               kn = side of in on which ie abuts.
 *
 * Copyright c 1985 The board of trustees of the Leland Stanford
 *                  Junior University. All rights reserved.
 * This subroutine may not be used outside of the SUPREM4 computer
 * program without the prior written consent of Stanford University.
 *
 * Original: CSR Nov 83
 *---------------------------------------------------------------------*/
#include <global.h>
#include <constant.h>
#include <geom.h>
#include <regrid.h>

int tnabor (ie, in, ke, kn)
    int ie, in, *ke, *kn;
{
    register int pe0, pe1, pe2, pn0, pn1, pn2;
    int ic, a[2][2];

    switch( mode ) {
    case TWOD :
	pe0 = nd[ tri[ ie]->nd[0]]->pt;
	pe1 = nd[ tri[ ie]->nd[1]]->pt;
	pe2 = nd[ tri[ ie]->nd[2]]->pt;
	pn0 = nd[ tri[ in]->nd[0]]->pt;
	pn1 = nd[ tri[ in]->nd[1]]->pt;
	pn2 = nd[ tri[ in]->nd[2]]->pt;

	/* Unroll the loop for speed */
	ic = 0;
	if(pe0==pn0) {a[ic][0] = 0; a[ic][1] = 0; ic++;}
	if(pe0==pn1) {a[ic][0] = 0; a[ic][1] = 1; ic++;}
	if(pe0==pn2) {a[ic][0] = 0; a[ic][1] = 2; ic++;}
	if(pe1==pn0) {a[ic][0] = 1; a[ic][1] = 0; ic++;}
	if(pe1==pn1) {a[ic][0] = 1; a[ic][1] = 1; ic++;}
	if(pe1==pn2) {a[ic][0] = 1; a[ic][1] = 2; ic++;}
	if(pe2==pn0) {a[ic][0] = 2; a[ic][1] = 0; ic++;}
	if(pe2==pn1) {a[ic][0] = 2; a[ic][1] = 1; ic++;}
	if(pe2==pn2) {a[ic][0] = 2; a[ic][1] = 2; ic++;}
	
	if (ic < 2)  
	    return(FALSE);
	else if (ic > 2)
	    panic ("bad call to tnabor");	/*NOTREACHED*/
	else {
	    *ke = 3 - a[0][0] - a[1][0];
	    *kn = 3 - a[0][1] - a[1][1];
	    return(TRUE);
	}
	break;

    case ONED :
	pe0 = nd[ tri[ ie]->nd[0]]->pt;
	pe1 = nd[ tri[ ie]->nd[1]]->pt;
	pn0 = nd[ tri[ in]->nd[0]]->pt;
	pn1 = nd[ tri[ in]->nd[1]]->pt;

	ic = FALSE;
	if ( pe0 == pn0 ) { *ke = 1; *kn = 1; ic = TRUE; }
	if ( pe0 == pn1 ) { *ke = 1; *kn = 0; ic = TRUE; }
	if ( pe1 == pn0 ) { *ke = 0; *kn = 1; ic = TRUE; }
	if ( pe1 == pn1 ) { *ke = 0; *kn = 0; ic = TRUE; }

	return( ic );
	break;
    
    }
    return(0);
}
