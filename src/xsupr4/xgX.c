/*
 * Generic Output Driver for X
 * X version 11
 *
 * This is the primary output driver used by the new X graph
 * to display output to the X server.  It has been factored
 * out of the original xgraph to allow mulitple hardcopy
 * output devices to share xgraph's capabilities.  Note:
 * xgraph is still heavily X oriented.  This is not intended
 * for porting to other window systems.
 * xgX.c 1.2
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include "GraphP.h"

static unsigned char dashlist[MAXCOL][2] = {
    {1, 1},
    {2, 2},
    {3, 1},
    {7, 7},
    {4, 4},
    {4, 1},
    {3, 2},
    {3, 3}};

#define PADDING 	2
#define SPACE 		10
#define TICKLENGTH	5
#define MAXSEGS		1000

void text_X();
void seg_X();
void dot_X();


void segGC(w, l_fg, l_style, l_width, l_chars, l_len)
Widget w;
Pixel l_fg;                     /* Foreground color */
int l_style;                    /* Line style       */
int l_width;                    /* Line width       */
char *l_chars;                  /* Character spec   */
int l_len;                      /* Length of spec   */
/*
 * Sets the fields above in a global graphics context.  If the
 * graphics context does not exist, it is created.
 */
{
    GraphWidget gw = (GraphWidget) w;
    XGCValues gcvals;
    unsigned long gcmask;

    gcvals.foreground = l_fg;
    gcvals.line_style = l_style;
    gcvals.line_width = l_width;
    gcmask = GCForeground | GCLineStyle | GCLineWidth;

    if ( gw->graph.seg_gc == (GC) 0) {
        gw->graph.seg_gc = XCreateGC(XtDisplay(gw), XtWindow(gw), gcmask, &gcvals);
    } else {
        XChangeGC(XtDisplay(gw), gw->graph.seg_gc, gcmask, &gcvals);
    }
    if (l_len > 0) {
        XSetDashes(XtDisplay(gw), gw->graph.seg_gc, 0, l_chars, l_len);
    }
}


void set_X(Widget new)
/*
 * Sets some of the common parameters for the X output device.
 */
{
    GraphWidget gw = (GraphWidget) new;
    GraphWin *wi = gw->graph.wi;
    int i;

    wi->dev_info.area_w = gw->core.width;
    wi->dev_info.area_h = gw->core.height;
    wi->dev_info.bdr_pad = PADDING;
    wi->dev_info.axis_pad = SPACE;
    wi->dev_info.legend_pad = 0;
    wi->dev_info.tick_len = TICKLENGTH;

    wi->dev_info.axis_width = XTextWidth(gw->graph.labelFont, "8", 1);
    wi->dev_info.axis_height = gw->graph.labelFont->max_bounds.ascent +
		    gw->graph.labelFont->max_bounds.descent;

    wi->dev_info.title_width = XTextWidth(gw->graph.titleFont, "8", 1);
    wi->dev_info.title_height = gw->graph.labelFont->max_bounds.ascent +
				gw->graph.labelFont->max_bounds.descent;
    wi->max_segs = 100;
    wi->Xsegs = (XSegment *)malloc(100 * sizeof(XSegment));

    wi->dev_info.xg_text = text_X;
    wi->dev_info.xg_seg = seg_X;
    wi->dev_info.xg_dot = dot_X;
    wi->dev_info.xg_end = (void (*)()) 0;

    make_markers(gw);

}

void text_X(w, x, y, text, just, style)
Widget w;
int x, y;			/* Text position (pixels) */
char *text;			/* Null terminated text   */
int just;			/* Justification (above)  */
int style;			/* Text style (above)     */
/*
 * This routine should draw text at the indicated position using
 * the indicated justification and style.  The justification refers
 * to the location of the point in reference to the text.  For example,
 * if just is T_LOWERLEFT,  (x,y) should be located at the lower left
 * edge of the text string.
 */
{
    GraphWidget gw = (GraphWidget) w;
    XCharStruct bb;
    int rx, ry, len, height, width, dir;
    int ascent, descent;
    XFontStruct *font;
    GC loc_gc;
    XGCValues gcvals;
    unsigned long gcmask;

    if ( gw->graph.title_gc == (GC) 0 ) {
	gcvals.font = gw->graph.titleFont->fid;
	gcmask = GCFont;
	gcvals.foreground = 1;
	gcmask |= GCForeground;
	gw->graph.title_gc = XCreateGC(XtDisplay(gw), XtWindow(gw), gcmask, &gcvals);
    }
    if ( gw->graph.label_gc == (GC) 0 ) {
	gcvals.font = gw->graph.labelFont->fid;
	gcmask = GCFont;
	gcvals.foreground = 1;
	gcmask |= GCForeground;
	gw->graph.label_gc = XCreateGC(XtDisplay(gw), XtWindow(gw), gcmask, &gcvals);
    }


    len = strlen(text);
    font = ((style == T_TITLE) ? gw->graph.titleFont : gw->graph.labelFont);
    loc_gc = ((style == T_TITLE) ? gw->graph.title_gc : gw->graph.label_gc);

    XTextExtents(font, text, len, &dir, &ascent, &descent, &bb);
    width = bb.rbearing - bb.lbearing;
    height = bb.ascent + bb.descent;

    switch (just) {
    case T_CENTER:
	rx = x - (width/2);
	ry = y - (height/2);
	break;
    case T_LEFT:
	rx = x;
	ry = y - (height/2);
	break;
    case T_UPPERLEFT:
	rx = x;
	ry = y;
	break;
    case T_TOP:
	rx = x - (width/2);
	ry = y;
	break;
    case T_UPPERRIGHT:
	rx = x - width;
	ry = y;
	break;
    case T_RIGHT:
	rx = x - width;
	ry = y - (height/2);
	break;
    case T_LOWERRIGHT:
	rx = x - width;
	ry = y - height;
	break;
    case T_BOTTOM:
	rx = x - (width/2);
	ry = y - height;
	break;
    case T_LOWERLEFT:
	rx = x;
	ry = y - height;
	break;
    }
    XDrawString(XtDisplay(gw), XtWindow(gw),
		loc_gc,
		rx, ry + bb.ascent, text, len);
}


void seg_X(w, ns, segs, width, style, lappr, color)
Widget w;
int ns;				/* Number of segments   */
XSegment *segs;			/* X array of segments  */
int width;			/* Width of lines       */
int style;			/* See above            */
int lappr;			/* Line appearence      */
int color;			/* Line color (if any)  */
/*
 * This routine draws a number of line segments at the points
 * given in `seglist'.  Note that contiguous segments need not share
 * endpoints but often do.  All segments should be `width' devcoords wide
 * and drawn in style `style'.  If `style' is L_VAR,  the parameters
 * `color' and `lappr' should be used to draw the line.  Both
 * parameters vary from 0 to 7.  If the device is capable of
 * color,  `color' varies faster than `style'.  If the device
 * has no color,  `style' will vary faster than `color' and
 * `color' can be safely ignored.  However,  if the
 * the device has more than 8 line appearences,  the two can
 * be combined to specify 64 line style variations.
 * Xgraph promises not to send more than the `max_segs' in the
 * xgOut structure passed back from xg_init().
 */
{
    GraphWidget gw = (GraphWidget) w;
    unsigned char dash_list[5];

    if (style == L_AXIS) {
	dash_list[0] = 1;
	dash_list[1] = 1;
	segGC(w, gw->graph.gridColor, LineOnOffDash, 0, dash_list, 2);
    }
    else if (style == L_ZERO) {
	segGC(w, gw->graph.zeroColor, LineSolid, 0, (char *) 0, 0);
    }
    else {
	/* Color and line style vary */
	lappr = (int)lappr/MAXCOL;
	if (lappr == 0) {
	    segGC(w, gw->graph.pix[color%MAXCOL], LineSolid, width, (char *) 0, 0);
	}
	else {
	    segGC(w, gw->graph.pix[color%MAXCOL], LineOnOffDash, width,
		       dashlist[lappr], 2);
	}
    }
    XDrawSegments(XtDisplay(gw), XtWindow(gw), gw->graph.seg_gc, segs, ns);
}

#define LAST_CHECK


#define dot_width 8
#define dot_height 8
static char dot_bits[] = {
   0x00, 0x3c, 0x7e, 0x7e, 0x7e, 0x7e, 0x3c, 0x00};

#define mark1_width 8
#define mark1_height 8
#define mark1_x_hot 3
#define mark1_y_hot 3
static char mark1_bits[] = {
   0x00, 0x00, 0x1c, 0x1c, 0x1c, 0x00, 0x00, 0x00};

#define mark2_width 8
#define mark2_height 8
#define mark2_x_hot 3
#define mark2_y_hot 3
static char mark2_bits[] = {
   0x00, 0x3e, 0x22, 0x22, 0x22, 0x3e, 0x00, 0x00};

#define mark3_width 8
#define mark3_x_hot 3
#define mark3_y_hot 3
static char mark3_bits[] = {
   0x00, 0x1c, 0x36, 0x22, 0x36, 0x1c, 0x00, 0x00};

#define mark4_width 8
#define mark4_height 8
#define mark4_x_hot 3
#define mark4_y_hot 3
static char mark4_bits[] = {
   0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 0x00};

#define mark5_width 8
#define mark5_height 8
#define mark5_x_hot 3
#define mark5_y_hot 3
static char mark5_bits[] = {
   0x00, 0x08, 0x14, 0x22, 0x14, 0x08, 0x00, 0x00};

#define mark6_width 8
#define mark6_height 8
#define mark6_x_hot 3
#define mark6_y_hot 3
static char mark6_bits[] = {
   0x00, 0x1c, 0x14, 0x1c, 0x14, 0x1c, 0x00, 0x00};

#define mark7_width 8
#define mark7_height 8
#define mark7_x_hot 3
#define mark7_y_hot 3
static char mark7_bits[] = {
   0x00, 0x1c, 0x2a, 0x36, 0x2a, 0x1c, 0x00, 0x00};

#define mark8_width 8
#define mark8_height 8
#define mark8_x_hot 3
#define mark8_y_hot 3
static char mark8_bits[] = {
   0x00, 0x3e, 0x1c, 0x08, 0x1c, 0x3e, 0x00, 0x00};

/* Sizes exported for marker drawing */
static unsigned int dot_w = dot_width;
static unsigned int dot_h = dot_height;
static unsigned int mark_w = mark1_width;
static unsigned int mark_h = mark1_height;
static int mark_cx = mark1_x_hot;
static int mark_cy = mark1_y_hot;


make_markers(w)
Widget w;
{
    GraphWidget gw = (GraphWidget) w;

    gw->graph.marker[0] = XCreateBitmapFromData(XtDisplay(gw),
	RootWindowOfScreen(XtScreen(gw)), mark1_bits, mark_w, mark_h);

    gw->graph.marker[1] = XCreateBitmapFromData(XtDisplay(gw),
	RootWindowOfScreen(XtScreen(gw)), mark2_bits, mark_w, mark_h);

    gw->graph.marker[2] = XCreateBitmapFromData(XtDisplay(gw),
	RootWindowOfScreen(XtScreen(gw)), mark3_bits, mark_w, mark_h);

    gw->graph.marker[3] = XCreateBitmapFromData(XtDisplay(gw),
	RootWindowOfScreen(XtScreen(gw)), mark4_bits, mark_w, mark_h);

    gw->graph.marker[4] = XCreateBitmapFromData(XtDisplay(gw),
	RootWindowOfScreen(XtScreen(gw)), mark5_bits, mark_w, mark_h);

    gw->graph.marker[5] = XCreateBitmapFromData(XtDisplay(gw),
	RootWindowOfScreen(XtScreen(gw)), mark6_bits, mark_w, mark_h);

    gw->graph.marker[6] = XCreateBitmapFromData(XtDisplay(gw),
	RootWindowOfScreen(XtScreen(gw)), mark7_bits, mark_w, mark_h);

    gw->graph.marker[7] = XCreateBitmapFromData(XtDisplay(gw),
	RootWindowOfScreen(XtScreen(gw)), mark8_bits, mark_w, mark_h);

}



void dot_X(w, x, y, style, type, color)
Widget w ;		/* Value set in xg_init    */
int x, y;			/* Location in pixel units */
int style;			/* Dot style               */
int type;			/* Type of marker          */
int color;			/* Marker color (if any)   */
/*
 * This routine should draw a marker at location `x,y'.  If the
 * style is P_PIXEL,  the dot should be a single pixel.  If
 * the style is P_DOT,  the dot should be a reasonably large
 * dot.  If the style is P_MARK,  it should be a distinguished
 * mark which is specified by `type' (0-7).  If the output
 * device is capable of color,  the marker should be drawn in
 * `color' (0-7) which corresponds with the color for xg_line.
 */
{
    GraphWidget gw = (GraphWidget) w;
    XGCValues gcvals;
    unsigned long gcmask;

    gcvals.foreground = gw->graph.pix[color%MAXCOL];
    gcvals.clip_mask = gw->graph.marker[type%MAXCOL];
    gcvals.clip_x_origin = (int) (x - mark_cx);
    gcvals.clip_y_origin = (int) (y - mark_cy);
    gcmask = GCForeground | GCClipMask | GCClipXOrigin | GCClipYOrigin;

    if (gw->graph.dot_gc == (GC) 0) {
	gw->graph.dot_gc = XCreateGC(XtDisplay(gw), XtWindow(gw),
				    gcmask, &gcvals);
    }
    else {
	XChangeGC(XtDisplay(gw), gw->graph.dot_gc, gcmask, &gcvals);
    }

    XFillRectangle(XtDisplay(gw), XtWindow(gw), gw->graph.dot_gc,
		   (int) (x - mark_cx), (int) (y - mark_cy), mark_w, mark_h);
}

