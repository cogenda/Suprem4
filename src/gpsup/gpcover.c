
#include <stdio.h>
#include <math.h>
#include "gplot.h"

/*flags for a log axis..*/
int xl, yl;

/*label flags*/
char *xlab, *ylab, *tlab;

/*next point is a move*/
int mv;

float xscl, yscl;
float xmax, xmin, ymax, ymin;
int axis_done;
int cur_line;
int line_type;

xgPlotInit()
{
    axis_done = 0;
    cur_line = 1;
    line_type = 1;
}

check_x()
{
}


void xgSetBounds(lowX, upX, lowY, upY)
double lowX, lowY;		/* Lower left corner  */
double upX, upY;		/* Upper right corner */
{
    xmin = lowX;
    xmax = upX;
    ymin = lowY;
    ymax = upY;
}


void xgLogAxis(xlog, ylog)
int xlog, ylog;
{
    xl = xlog;
    yl = ylog;
}

#define STRDUP(str)       (char *)(strcpy(malloc((unsigned) (strlen(str)+1)), (str)))

void xgAxisLabels(xl, yl, ti)
char *xl, *yl, *ti;
{
    if ( xlab != NULL ) {free(xlab); xlab = NULL;}
    if ( ylab != NULL ) {free(ylab); ylab = NULL;}
    if ( tlab != NULL ) {free(tlab); tlab = NULL;}

    if (xl != NULL) xlab = STRDUP(xl);
    if (yl != NULL) ylab = STRDUP(yl);
    if (ti != NULL) tlab = STRDUP(ti);
}

int xgNewSet()
{
    if (!axis_done) {
	int gp_iv[4]; float gp_fv[4];
	float pxmax, pymax, tmp;

	gpgeti(G_PSIZE, gp_iv, gp_fv);
	pxmax = gp_fv[0]; pymax = gp_fv[1];

	gplot2(G_RESET, G_TRANS, 0.0, 0.0);
	gplot2(G_RESET, G_SCALE, 0.0, 0.0);
	gplot2(G_RESET, G_ROTATE, 0.0, 0.0);

	if (yscl < 0) {
	    tmp = ymax;
	    ymax = -ymin;
	    ymin = -tmp;
	}

	axis_2d(xmin, xmax, ymin, ymax, xscl, yscl, pxmax, pymax,
		(yscl < 0.0), xlab, ylab, yl);

	axis_done = 1;
    }
    gnline(cur_line);
    cur_line++;
    mv = 1;

    /* set clipping boundaries */
    gplot2( G_CLIPL, G_ONLOG, xmin, ymin );
    gplot2( G_CLIPH, G_ONLOG, xmax, ymax );
}

int xgNewSetLT(line_spec)
int line_spec;
{
    if (!axis_done) {
	int gp_iv[4]; float gp_fv[4];
	float pxmax, pymax, tmp;

	gpgeti(G_PSIZE, gp_iv, gp_fv);
	pxmax = gp_fv[0]; pymax = gp_fv[1];

	gplot2(G_RESET, G_TRANS, 0.0, 0.0);
	gplot2(G_RESET, G_SCALE, 0.0, 0.0);
	gplot2(G_RESET, G_ROTATE, 0.0, 0.0);

	if (yscl < 0) {
	    tmp = ymax;
	    ymax = -ymin;
	    ymin = -tmp;
	}

	axis_2d(xmin, xmax, ymin, ymax, xscl, yscl, pxmax, pymax,
		(yscl < 0.0), xlab, ylab, yl);

	axis_done = 1;
    }
    line_type=line_spec;
    gnline(line_type);
    cur_line++;
    mv = 1;

    /* set clipping boundaries */
    gplot2( G_CLIPL, G_ONLOG, xmin, (yscl>0) ? ymin : -ymin );
    gplot2( G_CLIPH, G_ONLOG, xmax, (yscl>0) ? ymax : -ymax );
}


void xgSetName(name)
char *name;			/* New set name */
{
}

void xgSetValue(val)
double val;
{
}


void xgSetMark(mark)
int mark;
{
}


void xgNewGroup()
/*
 * Set up for reading new group of points within a dataset.
 */
{
    mv = 1;
}

#define INITSIZE 4
void xgPoint(xval, yval)
double xval, yval;		/* New point         */
{
    if (mv)
	gplot2(G_MOVE, 0, xscl*xval, yscl*yval );
    else
	gplot2(G_DRAW, 0,  xscl*xval, yscl*yval );
    mv = 0;
}


void xgLabel(lab, x, y)
char *lab;
double x, y;
{
    float hsize, wsize;

    x *= xscl;
    y *= yscl;

    hsize = 0.025 * (ymax - ymin);
    wsize = 0.025 * (xmax - xmin);

    symbl2( x, y, lab, strlen(lab), hsize, wsize, 0.0, 0.0, 0.5 );
}


void xgSetScale(x,y)
double x, y;
{
    xscl = x;
    yscl = y;
}

void xgClear()
{
    gpend();
    gclear();
    axis_done = 0;
    cur_line = 1;
    line_type = 1;
}


void xgUpdate( reset_axis )
int reset_axis;
{
    ggtoa();
    if (reset_axis) gpend();
}

/************************************************************************
 *									*
 *	axis_2d(xmin, xmax, ymin, ymax, xscale, yscale, pxmax, pymax,   *
 *              no_fill, flip, xlab, ylab) - 				*
 *  This routine draws a two dimensional axis for the two d plots.  It 	*
 *  labels and adds tic marks.						*
 *									*
 *	Original:	MEL	Nov, 1984				*
 *									*
 ************************************************************************/
axis_2d(xmin, xmax, ymin, ymax, xscale, yscale, pxmax, pymax, 
	flip, xlab, ylab, ylog)
float xmin, xmax;	/*limits for the x axis*/
float ymin, ymax;	/*limits for the y axis*/
float xscale, yscale;   /*scaling to be applied to the labels*/
float pxmax, pymax;	/*physical screen size*/
int flip;		/*flip the y axis upside down*/
char *xlab, *ylab;	/*axis labels*/
int ylog;		/*draw y axis with a log scale*/
{
    int minor_tic;		/*number of tic marks*/
    float dist;		/*distance between tic marks*/
    float sxmin, sxmax;	/*screen location of start and stop of axis*/
    float symin, symax; /*screen location of start and stop of axis*/
    float lx, ly;	/*length of x and y axis*/
    float hgt;
    float xst, yst;	/*start of axis*/
    float len;		/*length of axis*/
    float axang;	/*axis angle*/
    float Fval, Eval;	/*axis value at start, end*/
    float Flab, Llab;	/*first, last labeled value*/
    float ltic; 	/*value of labeled ticks*/
    int utic;		/*number of tics between*/
    float labht, tlang;	/*tic label height, angle*/
    int tloc; 		/*tic label location flag*/
    float tang;		/*tic labels angle to draw at*/
    float tht, tdp;	/*height and depth of tics*/
    float axtht;	/*axis title height*/
    int lform;		/*location of title*/
    char *vform;	/*the tic label format string*/

    sxmin = 0.20 * pxmax;
    symin = 0.08 * pymax;
    sxmax = 0.97 * pxmax;
    symax = 0.95 * pymax;
    lx = sxmax - sxmin;
    ly = symax - symin;

    hgt = 0.02 * ly;

    Fval = xmin;
    Eval = xmax;
    axtcs2(Fval, Eval, 0, 0, &Flab, &Llab, &dist, &minor_tic);

    /* Let's start with some identification */
    /* Yaxis logic: hgt=0.02 ly, chars=0.025 + offset 0.5*char, margin=0.05 */
    if ( tlab != NULL )
    symb2(sxmin + lx/2,	/* Half way along the x axis */
	   symax,		/* Top of the y axis */
	   tlab,
	   strlen(tlab),	/* Is symb2 stupid or what? */
	   1.25*hgt,		/* Char size */
	   0.0,			/* No rotation */
	   -0.5,		/* hornt: (x,y) is bottom of string */
	   0.5);		/* wornt: (x,y) is center of string */

    /*do it*/
    gnline(1);
    axplt2(sxmin,	/*start of axis in physical coordinates, x*/
	   symin,	/*start of axis in physical coordinates, y*/
	   lx,		/*length of the axis in plot units*/
	   0.0,		/*angle that the axis is drawn at*/
	   Fval,	/*axis value at start location*/
	   Eval,	/*axis value at end location*/
	   Flab,	/*first labeled value on the axis*/
	   Llab,	/*last labeled value on the axis*/
	   dist,	/*distance between major tic marks*/
	   minor_tic,	/*number of minor tics between labels*/
	   hgt,		/*height of the labels on the axis*/
	   0.0,		/*angle for the labels*/
	   0,		/*flag - labels on bottom of axis*/
	   90.0,	/*angle at which tics are to be drawn*/
	   hgt,		/*height of major tic marks*/
	   hgt*0.5,	/*depth of major tic marks*/
	   xlab,	/*title of the axis*/
	   hgt,		/*title height*/
	   -1,		/*location of the title*/
	   "4.2f");	/*format for the labels*/
    

    /*do the y axis*/
    if ( ! flip ) {
	xst = sxmin;
	yst = symin;
	axang = 90.0;
	tlang = 270.0;
	tloc = 1;
	tang = 270.0;
	lform = 1;
    }
    else {
	xst = sxmin;
	yst = symax;
	axang = 270.0;
	tlang = 90.0;
	tloc = 0;
	tang = 270.0;
	lform = -2;
    }
    hgt = 0.02 * lx;

    if ( !ylog ) {
	Fval = ymin;
	Eval = ymax;
    } else {
	Fval = pow( 10.0, ymin * yscale);
	Eval = pow( 10.0, ymax * yscale);
    }
    axtcs2(Fval, Eval, 0, ylog, &Flab, &Llab, &ltic, &utic);


    /*do it*/
    axplt2(xst,			/*start of axis in physical coordinates*/
	   yst,			/*end of axis in physical coordinates*/
	   ly,	 		/*length of the axis in plot units*/
	   axang,		/*angle that the axis is drawn at*/
	   Fval,		/*axis value at start location*/
	   Eval,		/*axis value at end location*/
	   Flab,		/*first labeled value on the axis*/
	   Llab,		/*last labeled value on the axis*/
	   ltic,		/*distance between major tic marks*/
	   utic,		/*number of minor tics between labels*/
	   hgt,			/*height of the labels on the axis*/
	   tlang,		/*angle for the labels*/
	   tloc,		/*flag - labels on bottom of axis*/
	   tang,		/*angle at which tics are to be drawn*/
	   hgt,			/*height of major tic marks*/
	   hgt*0.5,		/*depth of major tic marks*/
	   ylab,		/*title of the axis*/
	   hgt,			/*title height*/
	   lform,		/*location of the title*/
	   "5.2g");		/*format for the labels*/

    gtrans(sxmin, symin);
    if ( ! flip) {
	gscale( lx / (xmax - xmin), ly / (ymax - ymin));
	gtrans(-xmin, -ymin);
    }
    else {
	gscale( lx / (xmax - xmin), ly / (ymax - ymin));
	gtrans(-xmin, ymax);
    }
}


#undef gmove
#undef gdraw

gmove(x,y) 
float x, y; 
{
    gplot2(G_MOVE,0,x,y);
}
gdraw(x,y) 
float x, y; 
{
    gplot2(G_DRAW,0,x,y);
}
