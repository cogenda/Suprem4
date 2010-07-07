/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * Graph.c - bitmap editor widget.
 */


#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

#define __UNIXOS2__
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>



#include "GraphP.h"

#define INTERNAL_WIDTH    2
#define INTERNAL_HEIGHT 4

static XtResource resources[] = {
     {
        XtNTitleFont,
        XtCTitleFont,
        XtRFontStruct,
        sizeof(XFontStruct *),
        XtOffset(GraphRec *, graph.titleFont),
        XtRString,
        XtDefaultFont
     },
     {
        XtNLabelFont,
        XtCLabelFont,
        XtRFontStruct,
        sizeof(XFontStruct *),
        XtOffset(GraphRec *, graph.labelFont),
        XtRString,
        XtDefaultFont
     },
     {
        XtNGridColor,
        XtCGridColor,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.gridColor),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNZeroColor,
        XtCZeroColor,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.zeroColor),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNLineColor1,
        XtCLineColor1,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.pix[0]),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNLineColor2,
        XtCLineColor2,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.pix[1]),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNLineColor3,
        XtCLineColor3,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.pix[2]),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNLineColor4,
        XtCLineColor4,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.pix[3]),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNLineColor5,
        XtCLineColor5,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.pix[4]),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNLineColor6,
        XtCLineColor6,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.pix[5]),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNLineColor7,
        XtCLineColor7,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.pix[6]),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNLineColor8,
        XtCLineColor8,
        XtRPixel,
        sizeof(Pixel),
        XtOffset(GraphRec *, graph.pix[7]),
        XtRString,
        XtDefaultForeground
     },
     {
        XtNGraphWin,
        XtCGraphWin,
        XtRString,
        sizeof(GraphWin *),
        XtOffset(GraphRec *, graph.wi),
        XtRString,
        NULL,
     },
};


/* Declaration of methods */

static void Initialize();
static void GraphRedisplay();
static void Destroy();
static void Resize();
static void DrawCell();
static Boolean SetValues();
static XtGeometryResult QueryGeometry();

static char defaultTranslations[] =
        "<Btn1Down>:    DrawCell()              \n\
	<Btn1Up>:	DrawCell()		\n\
        <Btn1Motion>:   DrawCell()";

static XtActionsRec actions[] = {
	{"DrawCell", DrawCell},
};
static void tildeExpand(char *, char *);

/* these Core methods not needed by Graph:
 *
 * static void ClassInitialize();
 * static void Realize();
 */

GraphClassRec graphClassRec = {
    {
    /* core_class fields */
#ifdef ardent
    /* superclass	  	 */ (WidgetClass) &widgetClassRec,
#else
    /* superclass	  	 */ (WidgetClass) &coreClassRec,
#endif
    /* class_name	  	 */ "Graph",
    /* widget_size	  	 */ sizeof(GraphRec),
    /* class_initialize   	 */ NULL,
    /* class_part_initialize	 */ NULL,
    /* class_inited       	 */ FALSE,
    /* initialize	  	 */ Initialize,
    /* initialize_hook		 */ NULL,
    /* realize		  	 */ XtInheritRealize,
    /* actions		  	 */ actions,
    /* num_actions	  	 */ XtNumber(actions),
    /* resources	  	 */ resources,
    /* num_resources	  	 */ XtNumber(resources),
    /* xrm_class	  	 */ NULLQUARK,
    /* compress_motion	  	 */ TRUE,
#ifdef ardent
    /* compress_exposure  	 */ TRUE,
#else
    /* compress_exposure  	 */ XtExposeCompressMultiple,
#endif
    /* compress_enterleave	 */ TRUE,
    /* visible_interest	  	 */ FALSE,
    /* destroy		  	 */ Destroy,
    /* resize		  	 */ Resize,
    /* expose		  	 */ GraphRedisplay,
    /* set_values	  	 */ SetValues,
    /* set_values_hook		 */ NULL,
    /* set_values_almost	 */ XtInheritSetValuesAlmost,
    /* get_values_hook		 */ NULL,
    /* accept_focus	 	 */ NULL,
    /* version			 */ XtVersion,
    /* callback_private   	 */ NULL,
    /* tm_table		   	 */ defaultTranslations,
    /* query_geometry		 */ QueryGeometry,
    /* display_accelerator       */ XtInheritDisplayAccelerator,
    /* extension                 */ NULL
    },
    {
    /* dummy_field               */ 0,
    },
};

WidgetClass graphWidgetClass = (WidgetClass) & graphClassRec;





void associate(gw, wi)
GraphWidget gw;
GraphWin *wi;
{
    gw->graph.wi = wi;
}

/* ARGSUSED */
static void
Initialize(treq, tnew, args, num_args)
Widget treq, tnew;
ArgList args;
Cardinal *num_args;
{
    GraphWidget new = (GraphWidget) tnew;

    if (new->core.width == 0) {
	new->core.width = 512;
    }

    if (new->core.height == 0) {
	new->core.height = 512;
    }

    /*clear initial stuff*/
    new->graph.echo_gc = (GC) 0;
    new->graph.seg_gc = (GC) 0;
    new->graph.dot_gc = (GC) 0;
    new->graph.title_gc = (GC) 0;
    new->graph.label_gc = (GC) 0;

    set_X( new );
}


/* ARGSUSED */
static void
GraphRedisplay(w, event)
Widget w;
XEvent *event;
{
    GraphWidget gw = (GraphWidget) w;
    GraphWin *wi = gw->graph.wi;

    if (!XtIsRealized(gw)) return;
    do_redraw(gw, wi);
}

/* ARGSUSED */
static Boolean
SetValues(current, request, new, args, num_args)
Widget current, request, new;
ArgList args;
Cardinal *num_args;
{
    GraphWidget curcw = (GraphWidget) current;
    GraphWidget newcw = (GraphWidget) new;
    Boolean do_redisplay = False;

}


static void
Destroy(w)
Widget w;
{
	GraphWidget cw = (GraphWidget) w;
}


/* ARGSUSED */
static void
Resize(w)
Widget w;
{
	GraphWidget gw = (GraphWidget) w;
	set_X(gw);
}

static XtGeometryResult QueryGeometry(w, proposed, answer)
Widget w;
XtWidgetGeometry *proposed, *answer;
{
	GraphWidget cw = (GraphWidget) w;

    return XtGeometryYes;
}

#define SetBox( A, B ) \
if (A.stx < A.crx) {  \
    B.x = A.stx;  \
    B.width = A.crx - A.stx; \
} \
else { \
    B.x = A.crx; \
    B.width = A.stx - A.crx; \
} \
if (A.sty < A.cry) { \
    B.y = A.sty; \
    B.height = A.cry - A.sty; \
} \
else { \
    B.y = A.cry; \
    B.height = A.sty - A.cry; \
}

#define TRANX(xval) \
(((double) ((xval) - wi->XOrgX)) * wi->XUnitsPerPixel + wi->UsrOrgX)

#define TRANY(yval) \
(wi->UsrOppY - (((double) ((yval) - wi->XOrgY)) * wi->YUnitsPerPixel))

static void DrawCell(w, event)
Widget w;
XEvent *event;
{
    GraphWidget gw = (GraphWidget) w;
    GraphWin *wi = gw->graph.wi;
    unsigned long gcmask;
    XGCValues gcvals;
    XRectangle boxEcho;
    double t, lx, ly, hx, hy;
    XExposeEvent fake_event;

    if ( gw->graph.echo_gc == (GC) 0 ) {
	gcmask = GCForeground | GCFunction;
	gcvals.foreground = 1;
	gcvals.function = GXxor;
	gw->graph.echo_gc = XCreateGC(XtDisplay(gw), XtWindow(gw), gcmask, &gcvals);
    }

    if (event->type == ButtonPress) {
	gw->graph.crx = gw->graph.stx = ((XButtonEvent *)event)->x;
	gw->graph.cry = gw->graph.sty = ((XButtonEvent *)event)->y;
    }
    else if ( event->type == MotionNotify ) {
	/*undraw the old*/
	SetBox( gw->graph, boxEcho );
	XDrawRectangles(XtDisplay(gw), XtWindow(gw), gw->graph.echo_gc, &boxEcho, 1);

	/*draw the new*/
	gw->graph.crx = ((XMotionEvent *)event)->x;
	gw->graph.cry = ((XMotionEvent *)event)->y;
	SetBox( gw->graph, boxEcho );
	XDrawRectangles(XtDisplay(gw), XtWindow(gw), gw->graph.echo_gc, &boxEcho, 1);
    }
    else if ( event->type == ButtonRelease ) {
	SetBox( gw->graph, boxEcho );
	XDrawRectangles(XtDisplay(gw), XtWindow(gw), gw->graph.echo_gc, &boxEcho, 1);

	/*scale the cordinates back*/
	lx = TRANX(gw->graph.crx);
	ly = TRANY(gw->graph.cry);
	hx = TRANX(gw->graph.stx);
	hy = TRANY(gw->graph.sty);

	/*adjust*/
	if (lx > hx) {t = lx; lx = hx; hx = t;}
	if (ly > hy) {t = ly; ly = hy; hy = t;}
	gw->graph.wi->loX = lx;
	gw->graph.wi->loY = ly;
	gw->graph.wi->hiX = hx;
	gw->graph.wi->hiY = hy;
	XClearWindow(XtDisplay(gw), XtWindow(gw));
	GraphRedisplay(gw, &fake_event);
    }
}


/* ARGSUSED */
void
unzoom(widget, client_data, call_data)
Widget widget;
XtPointer client_data;   /* cast to graph widget */
XtPointer call_data;	/* unused */
{
    GraphWidget gw = (GraphWidget) client_data;
    XExposeEvent fake_event;

    gw->graph.wi->loX = gw->graph.wi->llx;
    gw->graph.wi->loY = gw->graph.wi->lly;
    gw->graph.wi->hiX = gw->graph.wi->urx;
    gw->graph.wi->hiY = gw->graph.wi->ury;
    XClearWindow(XtDisplay(gw), XtWindow(gw));
    GraphRedisplay(gw, &fake_event);

}

#include "HardInfo.h"
#define MAXBUFSIZE 256


extern int hpglInit();
extern int psInit();
extern int idrawInit();

struct hard_dev hd[] = {
    { "Postscript", psInit, "lpr -P%s", "xgraph.ps", "lw1",
        19.0, "Times-Bold", 18.0, "Times-Roman", 12.0, NO },
    { "Idraw", idrawInit,
	"cat > /usr/tmp/idraw.tmp.ps; %s /usr/tmp/idraw.tmp.ps&",
	"~/.clipboard", "/usr/local/idraw", 19.0, "Times-Bold", 18.0,
	"Times-Roman", 12.0, NONE },
    { "HPGL", hpglInit, "lpr -P%s", "xgraph.hpgl", "paper",
	    27.5, "1", 14.0, "1", 12.0, NONE }

};


/* ARGSUSED */
void do_cn(widget, client_data, call_data)
Widget widget;
XtPointer client_data;   /* cast to bigBitmap */
XtPointer call_data;	/* unused */
{
    print_calldata *foo = (print_calldata *)client_data;
    XtPopdown(foo->print_shell);
}


#define RND(val)        ((int) ((val) + 0.5))


/* ARGSUSED */
void do_pr(widget, client_data, call_data)
Widget widget;
XtPointer client_data;   /* cast to bigBitmap */
XtPointer call_data;	/* unused */
{
    String str;
    int dev1, type;
    Arg args[1];
    char * value;
    print_calldata *foo = (print_calldata *)client_data;
    GraphWidget gw = foo->graph;
    GraphWin *curWin = (GraphWin *)gw->graph.wi;
    GraphWin thisWin;
    FILE *out_stream;
    char buf[MAXBUFSIZE], err[MAXBUFSIZE], ierr[MAXBUFSIZE];
    char tilde[MAXBUFSIZE*10];
    int final_w, final_h, flags;
    int inc;
    double ratio;
    char *userinfo;
    XExposeEvent fake_event;


    dev1 = (int )XawToggleGetCurrent(foo->dev_file);
    type = (int )XawToggleGetCurrent(foo->dev_type) - 1;
    inc  = (int )XawToggleGetCurrent(foo->do_include);

    XtSetArg(args[0], XtNstring, &value);
    XtGetValues( foo->dev_name, args, 1);
    XtPopdown(foo->print_shell);

    if ( type > 3 ) {
	do_error("Device is not yet supported");
	return;
    }

    if (dev1 == DEVICE) {
	(void) sprintf(buf, hd[type].dev_spec, value);
	out_stream = popen(buf, "w");
	if (!out_stream) {
	    do_error(sprintf(err, "Unable to issue command:\n  %s\n", buf));
	    return;
	}
    }
    else if (dev1 == FILEN) {
	tildeExpand(tilde, value);
	out_stream = fopen(tilde, "w");
	if (!out_stream) {
	    do_error(sprintf(err, "Unable to open file `%s'\n", tilde));
	    return;
	}
    }

    thisWin = *curWin;
    ratio = ((double) thisWin.dev_info.area_w) /
      ((double) thisWin.dev_info.area_h);
    if (thisWin.dev_info.area_w > thisWin.dev_info.area_h) {
	final_w = RND(hd[type].dev_max_dim * 10000.0);
	final_h = RND(hd[type].dev_max_dim/ratio * 10000.0);
    } else {
	final_w = RND(hd[type].dev_max_dim * ratio * 10000.0);
	final_h = RND(hd[type].dev_max_dim * 10000.0);
    }
    ierr[0] = '\0';
    flags = 0;
    if (hd[type].dev_docu && inc) flags |= D_DOCU;
    if (hd[type].dev_init(out_stream, final_w, final_h,
			hd[type].dev_title_font, hd[type].dev_title_size,
		        hd[type].dev_axis_font, hd[type].dev_axis_size,
			flags, &(thisWin.dev_info), &userinfo, ierr)) {
	do_redraw(userinfo, &thisWin);
	if (thisWin.dev_info.xg_end) {
	    thisWin.dev_info.xg_end(userinfo);
	}
    } else {
	do_error(ierr);
    }
    if (hd[type].dev_spec) {
	(void) pclose(out_stream);
    } else {
	(void) fclose(out_stream);
    }

}

static void tildeExpand(out, in)
char *out;			/* Output space for expanded file name */
char *in;			/* Filename with tilde                 */
/*
 * This routine expands out a file name passed in `in' and places
 * the expanded version in `out'.  It returns `out'.
 */
{
    char username[50], *userPntr;
    struct passwd *userRecord;

    out[0] = '\0';

    /* Skip over the white space in the initial path */
    while ((*in == ' ') || (*in == '\t')) in++;

    /* Tilde? */
    if (in[0] == '~') {
	/* Copy user name into 'username' */
	in++;  userPntr = &(username[0]);
	while ((*in != '\0') && (*in != '/')) {
	    *(userPntr++) = *(in++);
	}
	*(userPntr) = '\0';
	/* See if we have to fill in the user name ourselves */
	if (strlen(username) == 0) {
	    userRecord = getpwuid(getuid());
	} else {
	    userRecord = getpwnam(username);
	}
	if (userRecord) {
	    /* Found user in passwd file.  Concatenate user directory */
	    strcat(out, userRecord->pw_dir);
	}
    }

    /* Concantenate remaining portion of file name */
    strcat(out, in);
}

void add_label(widget, client_data, call_data)
Widget widget;
XtPointer client_data;   /* cast to bigBitmap */
XtPointer call_data;    /* unused */
{
    print_calldata *foo = (print_calldata *)client_data;
    GraphWidget gw = foo->graph;
    Arg args[1];
    char *x, *y, *t;

    XtSetArg(args[0], XtNstring, &x);
    XtGetValues(foo->dev_file, args, 1);
    XtSetArg(args[0], XtNstring, &y);
    XtGetValues(foo->dev_type, args, 1);
    XtSetArg(args[0], XtNstring, &t);
    XtGetValues(foo->dev_name, args, 1);

    xgAxisLabels(x, y, t);

    XClearWindow(XtDisplay(gw), XtWindow(gw));
    do_redraw(gw, gw->graph.wi);

}


