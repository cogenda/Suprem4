

/*	check_active.c		Version 5.1		*/
/*	Last Modification:	7/3/91 08:30:44		*/

/*      rewritten Nov 2, 1992 by S. E. Hansen		*/


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
    int i, imp, impa;

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
    }
}
