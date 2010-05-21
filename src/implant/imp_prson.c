/* date: 23 apr 85 (mje)
 *
 * "prson" : Calculate the value of a Pierson type-IV distribution.
 *
 * calling sequence:
 *	y = prson(x, a, b0, b2) ;
 *
 * where:
 *	INPUT:
 *	x	- (double) distance from centre of distribution.
 *	a	- (double)
 *	b0	- (double)
 *	b2	- (double)
 *
 *	OUTPUT:
 *	y	- (double) The values of the pierson distribution at X.
 *
 * notes:
 *
 *	Calculates the value of a Pierson Type-IV distribution described 
 *	by:
 *
 *       d f(x)      (x-a) * f(x)        0 < b1*b1/4*b0*b2 < 1
 *       ------ = ------------------
 *         dx     b0 + b1*x + b2*x*x            b1=a
 *
 *
 * written: A. G. Gonzalez (22 jul 77)
 * mod	  : Stephen E. Hansen (20 jan 82)
 *		Translate to RATFOR and addapt to SUPREM-III.
 * mod    : Michael Eldredge  (jul 83)
 *		UNIX conversion to Ratfor using FORTRAN-77.
 * mod    : Michael Eldredge  (apr 85)
 *		Covert to C. Addapt to SUPREM-IV.
 *
 * imp_prson.c	5.1	7/3/91	12:08:38
 */

/* system intinsics : get decls for sqrt(), log(), atan2() */
#include <math.h>
#define	 abs(A)	((A) >= 0.0? (A) : -(A))
#define	 min2(A,B)	((A) >= (B)? (B) : (A))
#define	 max2(A,B)	((A) >= (B)? (A) : (B))

#include "global.h"
#include "constant.h"
#include "implant.h"

double
prson(x, pc)
	double	 x ;
	struct imp_info	 *pc;		/* Pearson-IV constants */
	{

	double	 b1, disc, f1, f2, f3, f4, f5 ;
	double	 val ;				/* return value */
	double	 a0 = pc->vert[PRS_A0] ;
	double	 b0 = pc->vert[PRS_B0] ;
	double	 b2 = pc->vert[PRS_B2] ;
	double y1, y2, ymn, ymx;

	/* ---- start of prson ---- */
	b1 = a0 ;
	disc = (b1 * b1) - (4.0 * b0 * b2) ;
	f1 = log( abs(b0 + b1 * x + b2 * x * x) ) / (2.0 * b2) ;
	f2 = b1 / b2 + 2.0 * a0 ;
	f3 = 2.0 * b2 * x + b1 ;
	f4 = 0.0 ;
	f5 = 0.0 ;


	/* Pearson IV type solutions are here! */
	if (disc < 0.0) {
		f4 = sqrt(-disc) ;
		f5 = atan2(f3, f4) ;
		val = f1 - f2 * f5 / f4 ;
		val = exp(val - pc->vert[PRS_PEAK]) ;
		}

	/* Pearson V and VI have limited range */
	else {
	    f4 = sqrt(disc) ;
	    y1 = -0.5 * (a0 - f4) / b2;
	    y2 = -0.5 * (a0 + f4) / b2;
	    ymn = min2(y1,y2);
	    ymx = max2(y1,y2);

	    if ( (a0 < ymn) && (x > ymn) ) 
		val = 0.0;
	    else if ( (a0 > ymn) && (a0 < ymx) && ((x < ymn) || (x > ymx)) ) 
		val = 0.0;
	    else if ( (a0 > ymx) && (x < ymx) ) 
		val = 0.0;
	    else {
		/* Pearson V type solutions */
		if (disc  ==  0.0) {
		    val = f1 +f2 / f3 ;
		    val = exp(val - pc->vert[PRS_PEAK]) ;
		}
		/* Pearson VI type solutions */
		else {			/* if (disc > 0.) */
		    f5 = log( abs((f3 - f4) / (f3 + f4)) ) ;
		    val = f1 - 0.5 * f2 * f5 / f4 ;
		    val = exp(val - pc->vert[PRS_PEAK]) ;
		}
	    }
	}

	return(val) ;
	}
