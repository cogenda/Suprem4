/* date: 26 jun 85 (mje)
 *
 * "gtprs" : Return Pierson-IV parameters given moments.
 *
 * calling sequence:
 *	gtprs(Rp, sigp, gam1, &a0, &b0, &b2)
 *
 * where:
 *	INPUT:
 *	Rp	- (double) Projected range.
 *	sigp	- (double) S.D.
 *	gam1	- (double) Third moment.
 *	kurt	- (double) Fourth moment.
 *
 *	OUTPUT:
 *	const	- (double []) Pearson-IV constants.
 *
 * mod      : Michael Eldredge (apr 85)
 *	Converted to C and modified for Suprem-IV.
 *
 * imp_gtprs.c	5.1	7/3/91	12:08:36
 */

#include <stdio.h>
/* Need to know the offset into the constants array */
#include "global.h"
#include "constant.h"
#include "implant.h"

gtprs(rp, sigp, gam1, kurt, cvals)
	double	 rp, sigp, gam1, kurt ;
	struct imp_info	 *cvals;		/* Pearson-IV constants */
	{

	double	 ca, gam1sq;

	/* ---- start of gtprs ---- */

	 /* The 4th moment is in the tables, just get it.  */


	/* now the Pearson coefficients.... */
	gam1sq = gam1 * gam1;
	ca = 10. * kurt - 12. * gam1sq - 18. ;		/* cap A.  */

	cvals->vert[PRS_A0] = -gam1 * sigp * (kurt + 3.) / ca ;
	cvals->vert[PRS_B0] = -sigp * sigp * (4. * kurt - 3. * gam1sq) / ca ;
	cvals->vert[PRS_B2] = -(2. * kurt - 3. * gam1sq - 6.)  / ca ;

	}
