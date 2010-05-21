h14699
s 00000/00000/00040
d D 2.1 93/02/03 15:10:33 suprem45 2 1
c Initial IV.GS release
e
s 00040/00000/00000
d D 1.1 93/02/03 15:08:31 suprem45 1 0
c date and time created 93/02/03 15:08:31 by suprem45
e
u
U
f e 0
t
T
I 1
/* date: 5 jan 87 (mel)
 *
 * "qpgauss" :	return area under a gaussian curve
 *
 * written: Mark E. Law (jan 87)
 *
 * imp_qpgauss.c	5.1	7/3/91	12:08:41
 */

#include <math.h>
#define  abs(V) ((V) < 0.? -(V): (V))
#include "global.h"
#include "constant.h"
#include "implant.h"

int qpgauss(dx,  cvals)
double	 dx ;
struct imp_info	 *cvals;	/* Pearson-IV constants, area, max Z*/
{

    double	 x;
    double  	 val = 1, v0;
    double	 qp = 0.0 ;

    /* --- start ---- */

    /*integrate from zero down to the bottom of the profile*/
    v0 = imp_vert(0.0, cvals);
    for (x = dx; (val > 1e-36) || (x < cvals->vert[GUS_RP]); x += dx) {
	 val = imp_vert(x , cvals);
	 qp += val + v0;
	 v0 = val;
    }

    /* The area under the Pearson. Also convert units since conc is
     *	atoms/cm^2  and dx is in microns.
     */
    cvals->area = 0.5 * qp * dx * 1e-4 ;	/* The total area */
    cvals->maxz = x;
}
E 1
