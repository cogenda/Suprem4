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
/*   plot.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:38:59 */

#include <stdio.h>
#include "global.h"
#include "constant.h"
#include "skel.h"
#include "dbaccess.h"
#include "plot.h"

#define XC(A)	cordinate( A, 0 )
#define YC(A)	cordinate( A, 1 )

void node_pl(int i)
{
    char str[256];

    sprintf(str, "%d", i);
    xgLabel(str, XC(pt_nd(i)), YC(pt_nd(i)));
    if ( pl_debug ) {xgUpdate(FALSE); check_x();}
}

void edge_pl(int i)
{
    xgNewGroup();
    xgPoint( XC( pt_edg(i,0) ), YC(pt_edg(i,0)) );
    xgPoint( XC( pt_edg(i,1) ), YC(pt_edg(i,1)) );
    if ( pl_debug ) {xgUpdate(FALSE); check_x();}
}

void reg_pl (int i)
{
    struct LLedge *f, *b;
    int temp;

    temp = pl_debug;
    pl_debug = FALSE;
    WALK (sreg[i]->bnd, f, b) {
	edge_pl (f->edge);
	if (temp) {
	    node_pl( nd_edg(f->edge,0) );
	    node_pl( nd_edg(f->edge,1) );
	}
    }
    pl_debug = temp;
    if ( pl_debug ) {xgUpdate(FALSE); check_x();}
}

void surf_pl(int color)
{
    int i;

    for(i = 0; i < ned; i++) {
	if (is_surf(pt_edg(i,0)) && is_surf(pt_edg(i,1)) ) edge_pl(i);
    }
}

void xgdebug()
{
    pl_debug=1;
    xgNewSet();
    xgSetName("debug");
}
