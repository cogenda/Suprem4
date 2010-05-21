/*----------------------------------------------------------------------
**  Copyright 1986 by
**  The Board of Trustees of the Leland Stanford Junior University
**  All rights reserved.
**
**  This routine may not be used without the prior written consent of
**  the Board of Trustees of the Leland Stanford University.
**----------------------------------------------------------------------
**/


/* date: 10 feb 87 (mje)
 * "gplot.h" : definitions for calling 'gplot' routines.
 * written:  Michael Eldredge (nov 84)
 * modified: MJE (may 86) added REVCO call.
 * modified: MJE (dec 86) 
 *	+ added PDEV, PFIL stuff and defs for setpfil(), etc..
 *	+ moved gtoa & atog to NOT_SAVED.
 * modified: MJE (feb 87) added greset().
 */

/* the following few have historical numbers (from 'tplot' days) */
#define  G_CLEAR  1
#define  G_DRAW   2
#define  G_MOVE   3
/*#define  G_POST	  19	/* DON'T USE THIS!! USE G_GTOA INSTEAD */

#define  G_SCALE  11
#define  G_TRANS  13
#define  G_ROTATE 15
#define  G_ANGLE  17

/* stay out of the 20s for historical reasons, some old tplot stuff used them */
#define  G_CLIPL  31
#define  G_CLIPH  32
#define  G_LINE   33
#define  G_PEN    34
#define  G_USR1   35
#define  G_USR2   36
#define  G_DMODE  37
#define  G_AREA   38
#define  G_FILS   39
#define  G_RESET  42

#define  G_MARK   98
#define  G_PEND   99

/* These commands (>= G_NOT_SAVED) never get saved but are useful */
#define  G_NOT_SAVED 100
#define  G_NULL   100	/* may want this less than G_NOT_SAVED */
#define  G_ASAVE  101
#define  G_PSIZE  102	/* gpgets: return plot size */
#define	 G_CLOC   103	/* cursor location */
#define  G_DABLE  104	/* Disable some h.w. function */
#define  G_REVCO  105	/* return the current gplot rev code. */
#define	 G_PDEV	  106	/* set the plot device - gpmisc() */
#define  G_PFIL	  107	/* set the output file - gpmisc() */
#define  G_ATOG	  108	/* -- context switch: back to graphics */
#define  G_GTOA   109	/* -- context switch: back to alpha */
#define  G_XMAT	  110	/* for gpmisc get/set xform matrix */

/* Sub-commands : */
/*   For setting clipping, reset to original values, clip on physical or 
 *    on logical axis
 */
#define  G_ONLOG   0
#define  G_ONPHS   1
#define  G_DOBREAK 10	/* do a  break of lines when clipped.. */
#define  G_NOBREAK 11	/* don't break lines when clipped, connect them */

/* Begin, stop, toggle on & off of some functions (like auto-save) */
#define  G_BEGIN 0
#define  G_ON    1
#define  G_OFF   2
#define  G_STOP  3

#define	 G_SET	 1
#define  G_GET	 2
#define  G_MUL	 3
#define  G_CHECK 21		/* gpmisc(), see if it is defined */

/* drawing mode sub-commands */
#define G_MSET   0
#define G_MCLR   1
#define G_MCMP   2

/* gpopen() mode values. -- incase anyone needs them */
#define	GPO_RD	0x01		/* read only */
#define GPO_WR	0x02		/* write only */
#define GPO_RW  (GPO_RD|GPO_WR)	/* read and write */
#define GPO_AP	0x04		/* append mode */
#define GPO_HD	0x08		/* check header record */

/* some useful macros to make use of some functions a little simpler */
#define setpdev(S)	gpmisc(G_PDEV , G_SET , (int*)0, (float*)0, S)
#define setpfil(S)	gpmisc(G_PFIL , G_SET , (int*)0, (float*)0, S)
#define setsfil(S)	gpmisc(G_ASAVE, G_SET , (int*)0, (float*)0, S)
#define gpgeti(C,IV,FV)	gpmisc(C      , G_GET , IV     , FV	  , "")
#define gpseti(C,IV,FV)	gpmisc(C      , G_SET , IV     , FV	  , "")


#define gclear()       gplot2(G_CLEAR, 0, 0.,0.)
#define gpend()        gplot2(G_PEND , 0, 0.,0.)
#define gplot(X,Y,P)   gplot2(P, 0, X, Y)
#define gmove(X,Y)     gplot2(G_MOVE , 0, X , Y)
#define gdraw(X,Y)     gplot2(G_DRAW , 0, X , Y)
#define gscale(X,Y)    gplot2(G_SCALE, 0, X,Y)
#define gtrans(X,Y)    gplot2(G_TRANS, 0, X,Y)
#define grotat(X,Y,A) (gplot2(G_ROTATE,0,X,Y),gplot2(G_ANGLE,0,A,0.))

#define gnline(N)     gplot2(G_LINE , N ,0.,0.)	/* line style */
#define gnpen(N)      gplot2(G_PEN  , N ,0.,0.)	/* pen size */
#define gnfill(N)     gplot2(G_FILS , N ,0.,0.)	/* fill pattern */
#define garea(X,Y,H)  gplot2(G_AREA , H ,X ,Y )
#define gclipl(X,Y,H) gplot2(G_CLIPL, H, X ,Y )
#define gcliph(X,Y,H) gplot2(G_CLIPH, H, X ,Y )

#define greset(C)     gplot2(G_RESET, C, 0., 0.) /*reset something to default*/

/* All users should make use of ggtoa() calls (e.g. before prompting)
 *  but need not call gatog() since this transition is handled automatically
 *  by gplot.
 */
#define gatog()	      gplot2(G_ATOG , 0 ,0.,0.)
#define ggtoa()	      gplot2(G_GTOA , 0 ,0.,0.)
#define gpost()	      gplot2(G_GTOA , 0 ,0.,0.)

