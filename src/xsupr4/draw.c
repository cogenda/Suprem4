
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "GraphP.h"
#define nlog10(x)      (x == 0.0 ? 0.0 : log10(x) + 1e-15)
#define ZERO_THRES 1.0e-5

/* Default line styles */
static char *defStyle[] = {
    "1", "10", "11110000", "010111", "1110",
    "1111111100000000", "11001111", "0011000111"
};

/* Default color names */
static char *defColors[] = {
    "red", "SpringGreen", "blue", "yellow",
    "cyan", "sienna", "orange", "coral"
};


static int rdFindMax(GraphWin *wi);
void DrawTitle(char *w, GraphWin * wi);
int DrawGridAndAxis(char *w, GraphWin * wi);
int DrawData(char *w, GraphWin *wi);
void DrawLabel(char *gw, GraphWin * wi);
int DrawLegend(char *gw, GraphWin * wi);
void DrawNumber(char *w, GraphWin * wi, double val, int x, int y, int just, int logflag);


/*
 * This routine draws the title of the graph centered in
 * the window.  It is spaced down from the top by an amount
 * specified by the constant PADDING.  The font must be
 * fixed width.  The routine returns the height of the
 * title in pixels.
 */
void DrawTitle(char *w, GraphWin * wi)
{
    wi->dev_info.xg_text(w, wi->dev_info.area_w/2,
			 wi->dev_info.axis_pad,
			 wi->Title, T_TOP, T_TITLE);
}



#define SCREENX(ws, userX) \
    (((int) (((userX) - ws->UsrOrgX)/ws->XUnitsPerPixel + 0.5)) + ws->XOrgX)
#define SCREENY(ws, userY) \
    (ws->XOppY - ((int) (((userY) - ws->UsrOrgY)/ws->YUnitsPerPixel + 0.5)))


/*
 * This routine draws grid line labels in engineering notation,
 * the grid lines themselves,  and unit labels on the axes.
 */
int DrawGridAndAxis(char *w, GraphWin * wi)
{
    int startX;
    int Yspot, Xspot;
    double Xincr, Yincr, Xstart, Ystart, Yindex, Xindex;
    XSegment segs[2];
    double initGrid(), stepGrid();
    int tickFlag = FALSE;
    int logXFlag = wi->logx;
    int logYFlag = wi->logy;
    char *XUnitText = wi->XUnitText;
    char *YUnitText = wi->YUnitText;
    XSegment bb[4];

    /*
     * With the powers computed,  we can draw the axis labels.
     */
    Yspot = wi->dev_info.bdr_pad * 2 + wi->dev_info.title_height;
    wi->dev_info.xg_text(w,
			 wi->dev_info.bdr_pad, Yspot, YUnitText,
			 T_UPPERLEFT, T_AXIS);

    startX = wi->dev_info.area_w - wi->dev_info.bdr_pad;
    wi->dev_info.xg_text(w,
			 startX, wi->XOppY, XUnitText, T_RIGHT, T_AXIS);

    /*
     * First,  the grid line labels
     */
    Yincr = (wi->dev_info.axis_pad + wi->dev_info.axis_height) * wi->YUnitsPerPixel;
    Ystart = initGrid(wi->UsrOrgY, Yincr, logYFlag);
    for (Yindex = Ystart;  Yindex < wi->UsrOppY;  Yindex = stepGrid()) {
	Yspot = SCREENY(wi, Yindex);
	/* Write the axis label */
	if ( !logYFlag && ( fabs(Yindex/Yincr) < 1.0e-3 ) )
	    DrawNumber(w, wi, 0.0, wi->dev_info.bdr_pad +
			     (8 * wi->dev_info.axis_width),
			     Yspot, T_RIGHT, logYFlag);
	else
	    if (wi->xg_yscl < 0.0)
		DrawNumber(w, wi, -Yindex, wi->dev_info.bdr_pad +
			     (8 * wi->dev_info.axis_width),
			     Yspot, T_RIGHT, logYFlag);
	    else
		DrawNumber(w, wi, Yindex, wi->dev_info.bdr_pad +
			     (8 * wi->dev_info.axis_width),
			     Yspot, T_RIGHT, logYFlag);
    }

    Xincr = (wi->dev_info.axis_pad + (wi->dev_info.axis_width * 7)) * wi->XUnitsPerPixel;
    Xstart = initGrid(wi->UsrOrgX, Xincr, logXFlag);
    for (Xindex = Xstart;  Xindex < wi->UsrOppX;  Xindex = stepGrid()) {
	Xspot = SCREENX(wi, Xindex);
	/* Write the axis label */
	if ( !logXFlag && ( fabs(Xindex/Xincr) < ZERO_THRES ) )
	    DrawNumber(w, wi, 0.0, Xspot,
			     wi->dev_info.area_h - wi->dev_info.bdr_pad,
			     T_BOTTOM, logXFlag);
	else
	    if (wi->xg_xscl < 0.0)
		DrawNumber(w, wi, -Xindex, Xspot,
			     wi->dev_info.area_h - wi->dev_info.bdr_pad,
			     T_BOTTOM, logXFlag);
	    else
		DrawNumber(w, wi, Xindex, Xspot,
			     wi->dev_info.area_h - wi->dev_info.bdr_pad,
			     T_BOTTOM, logXFlag);
    }

    /*
     * Now,  the grid lines or tick marks
     */
    Yincr = (wi->dev_info.axis_pad + wi->dev_info.axis_height) * wi->YUnitsPerPixel;
    Ystart = initGrid(wi->UsrOrgY, Yincr, logYFlag);
    for (Yindex = Ystart;  Yindex < wi->UsrOppY;  Yindex = stepGrid()) {
	Yspot = SCREENY(wi, Yindex);
	/* Draw the grid line or tick mark */
	if (tickFlag) {
	    segs[0].x1 = wi->XOrgX;
	    segs[0].x2 = wi->XOrgX + wi->dev_info.tick_len;
	    segs[1].x1 = wi->XOppX - wi->dev_info.tick_len;
	    segs[1].x2 = wi->XOppX;
	    segs[0].y1 = segs[0].y2 = segs[1].y1 = segs[1].y2 = Yspot;
	} else {
	    segs[0].x1 = wi->XOrgX;  segs[0].x2 = wi->XOppX;
	    segs[0].y1 = segs[0].y2 = Yspot;
	}
	if ((fabs(Yindex) < ZERO_THRES) && !logYFlag) {
	    wi->dev_info.xg_seg(w, 1, segs, 0, L_ZERO, 0, 0);
	    if (tickFlag) {
		wi->dev_info.xg_seg(w, 1, &(segs[1]), 0, L_ZERO, 0, 0);
	    }
	} else {
	    wi->dev_info.xg_seg(w, 1, segs, 0, L_AXIS, 0, 0);
	    if (tickFlag) {
		wi->dev_info.xg_seg(w, 1, &(segs[1]), 0, L_AXIS, 0, 0);
	    }
	}
    }

    Xincr = (wi->dev_info.axis_pad + (wi->dev_info.axis_width * 7)) * wi->XUnitsPerPixel;
    Xstart = initGrid(wi->UsrOrgX, Xincr, logXFlag);
    for (Xindex = Xstart;  Xindex < wi->UsrOppX;  Xindex = stepGrid()) {
	Xspot = SCREENX(wi, Xindex);
	/* Draw the grid line or tick marks */
	if (tickFlag) {
	    segs[0].x1 = segs[0].x2 = segs[1].x1 = segs[1].x2 = Xspot;
	    segs[0].y1 = wi->XOrgY;
	    segs[0].y2 = wi->XOrgY + wi->dev_info.tick_len;
	    segs[1].y1 = wi->XOppY - wi->dev_info.tick_len;
	    segs[1].y2 = wi->XOppY;
	} else {
	    segs[0].x1 = segs[0].x2 = Xspot;
	    segs[0].y1 = wi->XOrgY; segs[0].y2 = wi->XOppY;
	}
	if ((fabs(Xindex) < ZERO_THRES) && !logXFlag) {
	    wi->dev_info.xg_seg(w, 1, segs, 0, L_ZERO, 0, 0);
	    if (tickFlag) {
		wi->dev_info.xg_seg(w, 1, &(segs[1]), 0, L_ZERO, 0, 0);
	    }
	} else {
	    wi->dev_info.xg_seg(w, 1, segs, 0, L_AXIS, 0, 0);
	    if (tickFlag) {
		wi->dev_info.xg_seg(w, 1, &(segs[1]), 0, L_AXIS, 0, 0);
	    }
	}
    }

    /* Draw bounding box */
    bb[0].x1 = bb[0].x2 = bb[1].x1 = bb[3].x2 = wi->XOrgX;
    bb[0].y1 = bb[2].y2 = bb[3].y1 = bb[3].y2 = wi->XOrgY;
    bb[1].x2 = bb[2].x1 = bb[2].x2 = bb[3].x1 = wi->XOppX;
    bb[0].y2 = bb[1].y1 = bb[1].y2 = bb[2].y1 = wi->XOppY;
    wi->dev_info.xg_seg(w, 4, bb, 0, L_AXIS, 0, 0);
}


static double gridBase, gridStep, gridJuke[101];
static int gridNJuke, gridCurJuke;

#define ADD_GRID(val)	(gridJuke[gridNJuke++] = log10(val))

double initGrid(double low, double step, int logFlag)
{
    double ratio, x;
    double RoundUp(), stepGrid();

    gridNJuke = gridCurJuke = 0;
    gridJuke[gridNJuke++] = 0.0;

    if (logFlag) {
	ratio = pow(10.0, step);
	gridBase = floor(low);
	gridStep = ceil(step);
	if (ratio <= 3.0) {
	    if (ratio > 2.0) {
		ADD_GRID(3.0);
	    } else if (ratio > 1.333) {
		ADD_GRID(2.0);	ADD_GRID(5.0);
	    } else if (ratio > 1.25) {
		ADD_GRID(1.5);	ADD_GRID(2.0);	ADD_GRID(3.0);
		ADD_GRID(5.0);	ADD_GRID(7.0);
	    } else {
		for (x = 1.0; x < 10.0 && (x+.5)/(x+.4) >= ratio; x += .5) {
		    ADD_GRID(x + .1);	ADD_GRID(x + .2);
		    ADD_GRID(x + .3);	ADD_GRID(x + .4);
		    ADD_GRID(x + .5);
		}
		if (floor(x) != x) ADD_GRID(x += .5);
		for ( ; x < 10.0 && (x+1.0)/(x+.5) >= ratio; x += 1.0) {
		    ADD_GRID(x + .5);	ADD_GRID(x + 1.0);
		}
		for ( ; x < 10.0 && (x+1.0)/x >= ratio; x += 1.0) {
		    ADD_GRID(x + 1.0);
		}
		if (x == 7.0) {
		    gridNJuke--;
		    x = 6.0;
		}
		if (x < 7.0) {
		    ADD_GRID(x + 2.0);
		}
		if (x == 10.0) gridNJuke--;
	    }
	    x = low - gridBase;
	    for (gridCurJuke = -1; x >= gridJuke[gridCurJuke+1]; gridCurJuke++){
	    }
	}
    } else {
	gridStep = RoundUp(step);
	gridBase = floor(low / gridStep) * gridStep;
    }
    return(stepGrid());
}

double stepGrid()
{
    if (++gridCurJuke >= gridNJuke) {
	gridCurJuke = 0;
	gridBase += gridStep;
    }
    return(gridBase + gridJuke[gridCurJuke]);
}


/*
 * This routine rounds up the given positive number such that
 * it is some power of ten times either 1, 2, or 5.  It is
 * used to find increments for grid lines.
 */
double RoundUp(double val)
{
    int exponent, idx;

    exponent = (int) floor(nlog10(val));
    if (exponent < 0) {
	for (idx = exponent;  idx < 0; idx++) {
	    val *= 10.0;
	}
    } else {
	for (idx = 0;  idx < exponent; idx++) {
	    val /= 10.0;
	}
    }
    if (val > 5.0) val = 10.0;
    else if (val > 2.0) val = 5.0;
    else if (val > 1.0) val = 2.0;
    else val = 1.0;
    if (exponent < 0) {
	for (idx = exponent;  idx < 0;  idx++) {
	    val /= 10.0;
	}
    } else {
	for (idx = 0;  idx < exponent;  idx++) {
	    val *= 10.0;
	}
    }
    return val;
}

#define LEFT_CODE	0x01
#define RIGHT_CODE	0x02
#define BOTTOM_CODE	0x04
#define TOP_CODE	0x08

/* Clipping algorithm from Neumann and Sproull by Cohen and Sutherland */
#define C_CODE(xval, yval, rtn) \
rtn = 0; \
if ((xval) < wi->UsrOrgX) rtn = LEFT_CODE; \
else if ((xval) > wi->UsrOppX) rtn = RIGHT_CODE; \
if ((yval) < wi->UsrOrgY) rtn |= BOTTOM_CODE; \
else if ((yval) > wi->UsrOppY) rtn |= TOP_CODE

/*
 * This routine draws the data sets themselves using the macros
 * for translating coordinates.
 */
int DrawData(char *w, GraphWin *wi)
{
    DataSet *temp;
    double sx1, sy1, sx2, sy2, tx, ty;
    int idx, subindex;
    int code1, code2, cd, mark_inside;
    int X_idx;
    XSegment *ptr;
    PointList *thisList;
    int markFlag;
    int noLines = FALSE;
    int lineWidth = 0;

    /*make sure Xsegs is big enough*/
    if ((X_idx = rdFindMax(wi)) > wi->max_segs) {
	wi->max_segs += X_idx + 100;
	wi->Xsegs = (XSegment *) realloc(wi->Xsegs,
		 (unsigned) (wi->max_segs * sizeof(XSegment)));
    }

    for(idx = 0, temp = wi->Data; temp != NULL; temp = temp->next, idx++) {
	thisList = temp->list;
	markFlag = temp->marker;
	while (thisList) {
	    X_idx = 0;
	    for (subindex = 0;  subindex < thisList->numPoints-1;  subindex++) {
		/* Put segment in (sx1,sy1) (sx2,sy2) */
		sx1 = thisList->xvec[subindex];
		sy1 = thisList->yvec[subindex];
		sx2 = thisList->xvec[subindex+1];
		sy2 = thisList->yvec[subindex+1];
		/* Now clip to current window boundary */
		C_CODE(sx1, sy1, code1);
		C_CODE(sx2, sy2, code2);
		mark_inside = (code1 == 0);
		while (code1 || code2) {
		    if (code1 & code2) break;
		    cd = (code1 ? code1 : code2);
		    if (cd & LEFT_CODE) {	/* Crosses left edge */
			ty = sy1 + (sy2 - sy1) * (wi->UsrOrgX - sx1) / (sx2 - sx1);
			tx = wi->UsrOrgX;
		    } else if (cd & RIGHT_CODE) { /* Crosses right edge */
			ty = sy1 + (sy2 - sy1) * (wi->UsrOppX - sx1) / (sx2 - sx1);
			tx = wi->UsrOppX;
		    } else if (cd & BOTTOM_CODE) { /* Crosses bottom edge */
			tx = sx1 + (sx2 - sx1) * (wi->UsrOrgY - sy1) / (sy2 - sy1);
			ty = wi->UsrOrgY;
		    } else if (cd & TOP_CODE) { /* Crosses top edge */
			tx = sx1 + (sx2 - sx1) * (wi->UsrOppY - sy1) / (sy2 - sy1);
			ty = wi->UsrOppY;
		    }
		    if (cd == code1) {
			sx1 = tx;  sy1 = ty;
			C_CODE(sx1, sy1, code1);
		    } else {
			sx2 = tx;  sy2 = ty;
			C_CODE(sx2, sy2, code2);
		    }
		}
		if (!code1 && !code2) {
		    /* Add segment to list */
		    wi->Xsegs[X_idx].x1 = SCREENX(wi, sx1);
		    wi->Xsegs[X_idx].y1 = SCREENY(wi, sy1);
		    wi->Xsegs[X_idx].x2 = SCREENX(wi, sx2);
		    wi->Xsegs[X_idx].y2 = SCREENY(wi, sy2);
		    X_idx++;
		}

		/* Draw markers if requested and they are in drawing region */
		if (markFlag && mark_inside) {
		    /* Distinctive markers */
		    wi->dev_info.xg_dot(w, wi->Xsegs[X_idx-1].x1,
					wi->Xsegs[X_idx-1].y1, P_MARK, idx,
					idx);
		}
	    }
	    /* Handle last marker */
	    if (markFlag && (thisList->numPoints > 0)) {
		C_CODE(thisList->xvec[thisList->numPoints-1],
		       thisList->yvec[thisList->numPoints-1],
		       mark_inside);
		if (mark_inside == 0) {
		    /* Distinctive markers */
		    wi->dev_info.xg_dot(w, wi->Xsegs[X_idx-1].x2,
					wi->Xsegs[X_idx-1].y2, P_MARK, idx,
					    idx);
		}
	    }

	    /* Draw segments */
	    if (thisList->numPoints > 0 && (!noLines) && (X_idx > 0)) {
		ptr = wi->Xsegs;
		while (X_idx > wi->max_segs) {
		    wi->dev_info.xg_seg(w, wi->max_segs, ptr,
					lineWidth, L_VAR,
					idx, idx);
		    ptr += wi->max_segs;
		    X_idx -= wi->max_segs;
		}
		wi->dev_info.xg_seg(w,
				    X_idx, ptr,
				    lineWidth, L_VAR,
				    idx, idx);
	    }
	    /* Next subset */
	    thisList = thisList->next;
	}
    }
}


/*
 * This routine draws the title of the graph centered in
 * the window.  It is spaced down from the top by an amount
 * specified by the constant PADDING.  The font must be
 * fixed width.  The routine returns the height of the
 * title in pixels.
 */
void DrawLabel(char *gw, GraphWin * wi)
{
    LabelList *spot;
    int code1;

    for(spot = wi->Label; spot; spot = spot->next) {
	C_CODE(spot->x, spot->y, code1);
	if (!code1)
	    wi->dev_info.xg_text(gw,
				 SCREENX(wi, spot->x),
				 SCREENY(wi, spot->y),
				 spot->label, spot->just, spot->style);
    }
}


/*
 * This draws a legend of the data sets displayed.  Only those that
 * will fit are drawn.
 */
int DrawLegend(char *gw, GraphWin * wi)
{
    DataSet *temp;
    int cnt;

    int idx, spot, lineLen, oneLen;
    XSegment leg_line;
    int markFlag;

    spot = wi->XOrgY;
    lineLen = 0;
    /* First pass draws the text */
    for(temp = wi->Data; temp != NULL; temp = temp->next) {
	if ((temp->list) && (spot + wi->dev_info.axis_height + 2 < wi->XOppY)) {
	    if (temp->setName) {
		/* Meets the criteria */
		oneLen = strlen(temp->setName);
		if (oneLen > lineLen) lineLen = oneLen;
		wi->dev_info.xg_text(gw,
				     wi->XOppX + wi->dev_info.bdr_pad,
				     spot+wi->dev_info.bdr_pad,
				     temp->setName,
				     T_UPPERLEFT, T_AXIS);
	    }
	    else {
		if (8 > lineLen) lineLen = oneLen;
		DrawNumber(gw, wi, temp->setValue,
			   wi->XOppX + wi->dev_info.bdr_pad,
			   spot+wi->dev_info.bdr_pad, T_UPPERLEFT, 0);
	    }
	    spot += 2 * wi->dev_info.axis_height;
        }
    }

    lineLen = lineLen * wi->dev_info.axis_width;
    leg_line.x1 = wi->XOppX + wi->dev_info.bdr_pad;
    leg_line.x2 = leg_line.x1 + lineLen;
    spot = wi->XOrgY;

    /* second pass draws the lines */
    for(cnt = 0, temp = wi->Data; temp != NULL; temp = temp->next, cnt++) {
	markFlag = temp->marker;
	if ((temp->list) && (spot + wi->dev_info.axis_height + 2 < wi->XOppY)) {
	    leg_line.y1 = leg_line.y2 = spot - wi->dev_info.legend_pad;
	    wi->dev_info.xg_seg(gw,
				  1, &leg_line, 1, L_VAR,
				  cnt, cnt);
	    if (markFlag) {
		wi->dev_info.xg_dot(gw,
				      leg_line.x1, leg_line.y1,
				      P_MARK, cnt, cnt);
	    }
	    spot += 2 * wi->dev_info.axis_height;
	}
    }
}


/*
 * This routine figures out how to draw the axis labels and grid lines.
 * Both linear and logarithmic axes are supported.  Axis labels are
 * drawn in engineering notation.  The power of the axes are labeled
 * in the normal axis labeling spots.  The routine also figures
 * out the necessary transformation information for the display
 * of the points (it touches XOrgX, XOrgY, UsrOrgX, UsrOrgY, and
 * UnitsPerPixel).
 */
int TransformCompute(GraphWin * wi)
{
    DataSet *temp;
    double bbCenX, bbCenY, bbHalfWidth, bbHalfHeight;
    int idx, maxName, leftWidth;

    /*
     * First,  we figure out the origin in the X window.  Above
     * the space we have the title and the Y axis unit label.
     * To the left of the space we have the Y axis grid labels.
     */

    wi->XOrgX = wi->dev_info.bdr_pad + (8 * wi->dev_info.axis_width)
      + wi->dev_info.bdr_pad;
    wi->XOrgY = wi->dev_info.bdr_pad + wi->dev_info.title_height
      + wi->dev_info.bdr_pad + wi->dev_info.axis_height
	+ wi->dev_info.axis_height/2 + wi->dev_info.bdr_pad;

    /*
     * Now we find the lower right corner.  Below the space we
     * have the X axis grid labels.  To the right of the space we
     * have the X axis unit label and the legend.  We assume the
     * worst case size for the unit label.
     */

    maxName = 0;
    for(temp = wi->Data; temp != NULL; temp = temp->next) {
	if (temp->list) {
	    int tempSize;
	    if ( temp->setName  )
		tempSize = strlen(temp->setName);
	    else
		tempSize = 8;
	    if (tempSize > maxName) maxName = tempSize;
	}
    }

    /* Worst case size of the X axis label: */
    leftWidth = (strlen(wi->XUnitText)) * wi->dev_info.axis_width;
    if ((maxName*wi->dev_info.axis_width)+wi->dev_info.bdr_pad > leftWidth)
      leftWidth = maxName * wi->dev_info.axis_width + wi->dev_info.bdr_pad;

    wi->XOppX = wi->dev_info.area_w - wi->dev_info.bdr_pad - leftWidth;
    wi->XOppY = wi->dev_info.area_h - wi->dev_info.bdr_pad
      - wi->dev_info.axis_height - wi->dev_info.bdr_pad;

    if ((wi->XOrgX >= wi->XOppX) || (wi->XOrgY >= wi->XOppY)) {
	fprintf(stderr,"drawing area too small\n");
	return 0;
    }

    /*
     * We now have a bounding box for the drawing region.
     * Figure out the units per pixel using the data set bounding box.
     */
    wi->XUnitsPerPixel = (wi->hiX - wi->loX)/((double) (wi->XOppX - wi->XOrgX));
    wi->YUnitsPerPixel = (wi->hiY - wi->loY)/((double) (wi->XOppY - wi->XOrgY));

    /*
     * Find origin in user coordinate space.  We keep the center of
     * the original bounding box in the same place.
     */
    bbCenX = (wi->loX + wi->hiX) / 2.0;
    bbCenY = (wi->loY + wi->hiY) / 2.0;
    bbHalfWidth = ((double) (wi->XOppX - wi->XOrgX))/2.0 * wi->XUnitsPerPixel;
    bbHalfHeight = ((double) (wi->XOppY - wi->XOrgY))/2.0 * wi->YUnitsPerPixel;
    wi->UsrOrgX = bbCenX - bbHalfWidth;
    wi->UsrOrgY = bbCenY - bbHalfHeight;
    wi->UsrOppX = bbCenX + bbHalfWidth;
    wi->UsrOppY = bbCenY + bbHalfHeight;

    /*
     * Everything is defined so we can now use the SCREENX and SCREENY
     * transformations.
     */
    return 1;
}



void DrawNumber(char *w, GraphWin * wi, double val, int x, int y, int just, int logflag)
{
    double mantval;
    int expval;
    int Xspot = x, Yspot = y;
    char power[120];
    char mant[120];

    if ( logflag ) val = pow(10.0, val);

    expval = (int )floor(nlog10(fabs(val)));

    if ( expval < 2 && expval > -2 ) {
	sprintf(mant, "%.2f", val);
	wi->dev_info.xg_text(w, Xspot, Yspot, mant, just, T_AXIS);
	return;
    }

    mantval = val / pow( 10.0, (double)expval );
    (void)sprintf(power, "%d", expval);
    if ( mantval == 1.0 ) {
	strcpy(mant, "10");
    }
    else {
	(void)sprintf(mant, "%.1f", mantval);
	(void)strcat(mant, "x10");
    }

    switch( just ) {
    case T_RIGHT :
	Xspot -= strlen(power) * wi->dev_info.axis_width;
	wi->dev_info.xg_text(w, Xspot, Yspot, mant, T_RIGHT, T_AXIS);
	Xspot += wi->dev_info.bdr_pad;
	wi->dev_info.xg_text(w, Xspot, Yspot, power, T_LOWERLEFT, T_AXIS);
	break;

    case T_BOTTOM :
	Xspot += 0.5*((strlen(mant)-0.5*strlen(power))*wi->dev_info.axis_width);
	wi->dev_info.xg_text(w, Xspot, Yspot, mant, T_LOWERRIGHT, T_AXIS);
	Yspot -= 0.33 * wi->dev_info.axis_height;
	Xspot += wi->dev_info.bdr_pad;
	wi->dev_info.xg_text(w, Xspot, Yspot, power, T_LOWERLEFT, T_AXIS);
	break;

    case T_UPPERLEFT :
	Yspot += 1.0 * wi->dev_info.axis_height;
	Xspot += strlen(mant)*wi->dev_info.axis_width;
	wi->dev_info.xg_text(w, Xspot, Yspot, mant, T_RIGHT, T_AXIS);
	wi->dev_info.xg_text(w, Xspot, Yspot, power, T_LOWERLEFT, T_AXIS);
    }
}




/*
 * Returns the maximum number of items in any one group of any
 * data set.
 */
static int rdFindMax(GraphWin * wi)
{
    DataSet *temp;
    int i;
    PointList *list;
    int max = -1;

    for(temp = wi->Data; temp != NULL; temp = temp->next) {
        for (list = temp->list;  list;  list = list->next) {
            if (list->numPoints > max) max = list->numPoints;
        }
    }
    return max;
}


void do_redraw(char *userdata, GraphWin * wi)
{
  if ( TransformCompute(wi) ) {
    DrawTitle(userdata, wi);
    DrawLegend(userdata, wi);
    DrawGridAndAxis(userdata, wi);
    DrawData(userdata, wi);
    DrawLabel(userdata, wi);
  }
}


