/* date: 26 jun 85 (mje)
 *
 * "setprs" : Get Pearson-IV implantation constants.
 *	We hash the previous several requests.
 *
 * notes:
 *	o  Fix it so that the data array is not copied to the destination,
 *	   but we simple return a pointer to the static data in the hash
 *	   table (jun 30, mje)
 *
 * written: Michael Eldredge (jun 85)
 *
 * imp_setprs.c	5.1	7/3/91	12:08:43
 */

#include <math.h>
#include "constant.h"
#include "global.h"
#include "impurity.h"
#include "material.h"

#include "implant.h"

/* MAP Suprem-IV numbers to those of Suprem-III (which is how they are
 * stored in the implant range data file.
 */
#define S3_BF2  5

/* "setprs" : get Pearson constants for given ion/matterial/energy */
int
setprs(ion, mat, energy, cvals)
	int	 ion ;			/* implanting ion */
	int	 mat ;			/* matterial type */
	double	 energy ;		/* implantation energy */
	struct imp_info	 *cvals ;	/* implant information */
	{

	double	 rang, sigp, gam1, kurt, later ;
	int	 ierr ;

	/* ---- start of setprs ---- */
	/*map the ion number into the data table numbers*/
	switch( ion ) {
	case B:
	case P:
	case As:
	case Sb:
	case iBe:
	case iMg:
	case iSe:
	case iSi:
	case iSn:
	case iGe:
	case iZn:
	case iC:
	case iG:
		break;
	case BF2:
		ion = S3_BF2;
		break;
	case I:
	case Cs:
		ion = -1;
		break;
	default:
		return -1 ;
	}

	/*map the material numbers*/
	switch(mat) {
	case Si:
	case SiO2:
	case SiNi:
	case Poly:
	case Al:
	case GaAs:
		break;
	case OxNi:
		mat = SiNi;
		break;
	case PhRs:
		mat = SiNi;
		break;
	default:
		return -1;
	}

	/* Try the file, if we haven't been overridden */
	if ( override ) {
	    rang = Rp;
	    sigp = delRp;
	    gam1 = Rgam;
	    kurt = Rkurt;
	}
	else {
	    if ((ierr=fgtmom(ion,mat,energy,&rang,&sigp,&gam1,&kurt,&later))<0)
		return(ierr) ;		/* Error from range file */
	}

	/*We found the data, but before we leave, we munge it up a bit */

	/*Now use the range statistics to calulate the appropriate constants*/
	if ( imp_model == PEARS ) {
	    gtprs(rang, sigp, gam1, kurt, cvals) ;
	    cvals->vert[PRS_RP] = rang ;
	    cvals->lat[LATSIG] = later ;
	    cvals->lat[LATAREA] = sqrt(2.0 * PI) * later;

	    /* And use the Pearson constants to get the Area and Max depth */
	    cvals->maxz = rang + 1e3 * sigp;    /*initialize to huge depth*/
	    qpprs( PRS_DX, cvals->maxz, cvals ) ;
	}
	else if ( imp_model == GAUSS ) {
	    cvals->vert[GUS_RP]    = rang ;
	    cvals->lat[LATSIG] = later ;
	    cvals->lat[LATAREA] = sqrt(2.0 * PI) * later;
	    cvals->vert[GUS_SIG]   = sigp;
	    cvals->maxz = rang + 1e5 * sigp;    /*initialize to huge depth*/

	    /*figure out the max depth and the area*/
	    qpgauss(PRS_DX, cvals);
	}


	return(0) ;
	}

