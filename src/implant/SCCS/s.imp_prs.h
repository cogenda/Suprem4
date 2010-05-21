h60777
s 00024/00000/00000
d D 1.1 93/02/03 15:08:34 suprem45 1 0
c date and time created 93/02/03 15:08:34 by suprem45
e
u
U
f e 0
t
T
I 1
/* date: 26 jun 85 (mje)
 *
 * "imp_prs.h" : Defines for pearson implantation.
 *
 * written: Michael Eldredge (jun 85)
 *
 *	imp_prs.h	4.1	8/18/89	16:29:15
 */

/* A good dx to use in integrating charge, etc. */
#define  PRS_DX 5.0e-4

/* Data array offsets */
#define	 PRS_A0    0
#define  PRS_B0    1
#define  PRS_B2    2
#define  PRS_MAXZ  3		/* Max allowable input to Pearson func */
#define  PRS_AREA  4		/* Area for this Pearson curve */
#define	 PRS_RP	   5		/* Projected range */
#define	 PRS_PEAK  6		/* Value at the centre */
#define  PRS_SIGLAT  7		/* The lateral standard deviation*/
#define  PRS_SIZE    8		/* Number of above things */

#define	 PRS_HASHSZ 6		/* Number of layers is good */
E 1
