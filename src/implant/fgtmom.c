
/* "fgtmom" : Return impatation range statistics for given element and matterial
 *
 * date: 15 jan 85 (mje)
 *
 * written: Michael Eldredge (jan 85)
 *
 * fgtmom.c	5.1	7/3/91	12:08:34
 */

#include <stdio.h>
#include <stdlib.h>
#include "sysdep.h"

#define  OP_READ  "r"

#define CMT_CHAR '#'

#define Bool	int
#define T	1
#define F	0

#define E_NOERR		0
#define E_GTMOM_OPEN   -1
#define E_GTMOM_ESMALL -2
#define E_GTMOM_EBIG   -3
#define E_GTMOM_NONE   -4	/* couldn't find the elem/mattr pair */

int
fgtmom(elem, mattr, energy, rp, sigp, gam1, kurt, later)
	int	elem, mattr ;
	double	energy ;
	double	*rp, *sigp, *gam1, *kurt, *later ;
	{

	/* Locals.... */
	Bool	already = F;	/* Not already at the right mattr & elem */
	Bool	found =F ;	/* Haven't found energy yet */
	char	line[132] ;
	int	ierr = E_NOERR ;
	FILE	*fp ;
	char *file;

	int	kmatt, kelem ;
	double	ax, rx, sx, cx , kx, lx ;
	double	a0, r0, s0, c0 , k0, l0 ;
	double	fact1, fact2 ;


	/* ---- start gtmom ---- */
	if ( file = (char *)getenv( "IMPFILE" ) )
	    fp = fopen( file, OP_READ );
	else
	    fp = fopen(IMP_FILE, OP_READ) ;
	if (fp == NULL)	return(E_GTMOM_OPEN) ;

	/* Loop through the energy values until we find the closest energy
	 *  to that specified or an error occurs.
	 */

	while (! found && fgets(line, 132, fp) ) {
		if (line[0] == CMT_CHAR) continue ;	/* bag comment line */

		sscanf(line , "%d %d %lf %lf %lf %lf %lf %lf" ,
			    &kmatt, &kelem, &ax, &rx, &sx, &cx, &kx, &lx);

  		/* If this the material and element we want. */
		if (kmatt == mattr  &&  kelem == elem) {

			/* If this is the first time we've found this
			 *  combination of material and impurity types.
			 */
			if (!already) {
				/* energy too small for table */
				if (energy < ax)  {
					ierr = E_GTMOM_ESMALL ;
					goto done ;
					}
				already = T ;
				}

			/* Found energy or overshot */
			found = (energy <= ax) ;

			/* If we havn't found the right data yet , save the
			 *  last (less than expected) values. If we don't
			 *   find an exact energy match then we'll interpolate.
			 */
			if (!found) {
				a0=ax ;
				r0=rx ;
				s0=sx ;
				c0=cx ;
				k0=kx ;
				l0=lx ;
				}

			}/*of if matterial matches */

		/* If this is not the material and element we want, but we
		 *  found them before, then the energy specified is too large.
		 */
		else if (already) {
			ierr = E_GTMOM_EBIG ;
			goto done ;
			}

		}/* of while reading */

	/* If we ended because of an EOF, then we never found the pair */
	if (!found) {			/* still no found */
		ierr = E_GTMOM_NONE ;
		goto done ;
		}


	/* Now let's interpolate between the points if necessary and get
	 *  the needed values.
	 */
	if (ax == energy) fact2 = 1.0 ;			      /* Exact match. */
	else              fact2 = (energy - a0) / (ax - a0);  /* Interpolate. */
	fact1 = 1.0 - fact2 ;

	*rp   = r0 * fact1 + rx * fact2 ;	/* Range. */
	*sigp = s0 * fact1 + sx * fact2 ;	/* Standard deviation. */
	*gam1 = c0 * fact1 + cx * fact2 ;	/* Third moment. */
	*kurt = k0 * fact1 + kx * fact2 ;	/* Fourth moment. */
	*later = l0 * fact1 + lx * fact2 ;	/* Lateral standard deviation*/

#ifdef DEBUG
	printf("gtmom: %g Kev -> Rp = %g, sigp = %g, gam1 = %g, kurt = %g\n" ,
			energy , *rp, *sigp, *gam1, *kurt);
#endif

	/* All finished, close up the file an be gone */
done:
	fclose(fp) ;
	return (ierr) ;
	}
