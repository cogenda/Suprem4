/* date: 26 jun 85 (mje)
 *
 * "qp_prs" :	return area under a pierson-IV curve.
 *
 * written: Michael Eldredge (jan 85)
 *
 * imp_qpprs.c	5.1	7/3/91	12:08:42
 */

#include <math.h>
#define  abs(V) ((V) < 0.? -(V): (V))
#include "global.h"
#include "constant.h"
#include "implant.h"

#define	 Bool short
#define  T 1
#define  F 0

double	 prson() ;

qpprs(dx,  maxd, cvals)
	double	 dx ;
	double maxd;
	struct imp_info	 *cvals;	/* Pearson-IV constants, area, max Z*/
	{

	double	 x , ppeak ;
	double	 v, v0 = 0.0 ;
	double	 qp = 0.0 ;
	int	 deking = F ;

	/* --- start ---- */

	cvals->vert[PRS_PEAK] = 0.0;
	ppeak = log( prson(cvals->vert[PRS_A0], cvals) ) ;	/* value at centre */
	cvals->vert[PRS_PEAK] = ppeak ;

	/*integrate from zero down to the bottom of the profile*/
	v0 = imp_vert(0.0, cvals);
	for (x = dx ; x < maxd ; x += dx) {
		v = imp_vert(x , cvals);

		if (v > v0 && deking) break ;
		if (v < v0) deking = T ;
		if (v == 0.0 && deking) break;
		qp += (v + v0) ;
		v0 = v ;

		}

	/* The area under the Pearson. Also convert units since conc is
	 *	atoms/cm^2  and dx is in microns.
	 */
	cvals->area = 0.5 * qp * dx * 1e-4 ;	/* The total area */
	cvals->maxz = x;
	}
