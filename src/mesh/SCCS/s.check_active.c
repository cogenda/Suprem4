h27708
s 00024/00013/00033
d D 2.1 93/02/03 15:23:09 suprem45 2 1
c Initial IV.GS release
e
s 00046/00000/00000
d D 1.1 93/02/03 15:22:11 suprem45 1 0
c date and time created 93/02/03 15:22:11 by suprem45
e
u
U
f e 0
t
T
I 1


/*	check_active.c		Version 5.1		*/
/*	Last Modification:	7/3/91 08:30:44		*/

I 2
/*      rewritten Nov 2, 1992 by S. E. Hansen		*/
E 2

I 2

E 2
/*----------------------------------------------------------------------
 *
 * check_active -   checks mesh files to see if active impurities
 *			exist.  If not, create the solution
 *			variables.  This is an easy way to convert
 *			old SUPREM-IV structure files to new ones.
 *
 * Copyright c 1989 The board of trustees of the Leland Stanford
 *                  Junior University. All rights reserved.
 * This subroutine may not be used outside of the SUPREM4 computer
 * program without the prior written consent of Stanford University.
 *
 * Original: RYSH/GC Aug89 
 *---------------------------------------------------------------------*/
#include <stdio.h>
#include <ctype.h>
#include "global.h"
#include "constant.h"
#include "sysdep.h"
#include "impurity.h"


check_active()
{
D 2
    if ((imptosol[B] != -1) && (imptosol[Ba] == -1)) {
	add_impurity(Ba, 1.0, -1);
    }
E 2
I 2
    int i, imp, impa;
E 2

D 2
    if ((imptosol[As] != -1) && (imptosol[Asa] == -1)) {
	add_impurity(Asa, 1.0, -1);
    }

    if ((imptosol[P] != -1) && (imptosol[Pa] == -1)) {
	add_impurity(Pa, 1.0, -1);
    }

    if ((imptosol[Sb] != -1) && (imptosol[Sba] == -1)) {
	add_impurity(Sba, 1.0, -1);
E 2
I 2
    for (i = 0; i < n_imp; i++) {
	switch( (imp = soltoimp[i]) ) {
	case As  : impa = Asa;  break;
	case Sb  : impa = Sba;  break;
	case B   : impa = Ba;   break;
	case P   : impa = Pa;   break;
	case iBe : impa = iBea; break;
	case iMg : impa = iMga; break;
	case iSe : impa = iSea; break;
	case iSi : impa = iSia; break;
	case iSn : impa = iSna; break;
	case iGe : impa = iGea; break;
	case iZn : impa = iZna; break;
	case iC  : impa = iCa;  break;
	case iG  : impa = iGa;  break;
	default  : impa = 0;    break;
	}
	if (impa && (imptosol[imp] != -1) && (imptosol[impa] == -1)) {
	    add_impurity(impa, 1.0, -1);
	    SET_FLAGS(impa, ACTIVE);
	}
E 2
    }
}
E 1
