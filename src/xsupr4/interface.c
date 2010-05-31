/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */



/*
 * xbitmap1.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include "GraphP.h"
#include "xgraph.h"


#define MAX(A,B) (A>B)?A:B;


GraphWin *make_datawin()
{
    GraphWin *tmp;

    tmp = (GraphWin *)malloc(sizeof(GraphWin));
    tmp->logx = tmp->logy = 0;
    tmp->Data = NULL;
    tmp->Label = NULL;
    tmp->Xsegs = NULL;
    tmp->max_segs = 0;
    tmp->Title = tmp->XUnitText = tmp->YUnitText = NULL;

    return(tmp);
}

xgPlotInit()
{
    help_context = app_context = NULL;
    newwin = NULL;
    toolkitinit = FALSE;
}

check_x()
{
    XEvent event_return;

    while( app_context && XtAppPending(app_context)) {
	    XtAppNextEvent(app_context, &event_return);
	    XtDispatchEvent(&event_return);
    }
    while( help_context && XtAppPending(help_context)) {
	    XtAppNextEvent(help_context, &event_return);
	    XtDispatchEvent(&event_return);
    }
}


void xgSetBounds(lowX, upX, lowY, upY)
double lowX, lowY;		/* Lower left corner  */
double upX, upY;		/* Upper right corner */
{
    double pad;

    if ( newwin == NULL) {
	newwin = make_datawin();
    }

    newwin->loX = lowX;
    newwin->hiX = upX;
    newwin->loY = lowY;
    newwin->hiY = upY;

    /* Increase the padding for aesthetics */
    if (newwin->hiX - newwin->loX == 0.0) {
	pad = MAX(0.5, fabs(newwin->hiX/2.0));
	newwin->hiX += pad;
	newwin->loX -= pad;
    }
    if (newwin->hiY - newwin->loY == 0) {
	pad = MAX(0.5, fabs(newwin->hiY/2.0));
	newwin->hiY += pad;
	newwin->loY -= pad;
    }

    /* Add 10% padding to bounding box (div by 20 yeilds 5%) */
    pad = (newwin->hiX - newwin->loX) / 20.0;
    newwin->loX -= pad;  newwin->hiX += pad;
    pad = (newwin->hiY - newwin->loY) / 20.0;
    newwin->loY -= pad;  newwin->hiY += pad;

    newwin->llx = newwin->loX;
    newwin->urx = newwin->hiX;
    newwin->lly = newwin->loY;
    newwin->ury = newwin->hiY;
}


void xgLogAxis(xlog, ylog)
int xlog, ylog;
{
    if ( newwin == NULL) {
	newwin = make_datawin();
    }
    newwin->logx = xlog;
    newwin->logy = ylog;
}

#define STRDUP(str)       (strcpy(malloc((unsigned) (strlen(str)+1)), (str)))


void xgAxisLabels(xl, yl, ti)
char *xl, *yl, *ti;
{
    if ( newwin == NULL) {
	newwin = make_datawin();
    }
    if ( newwin->XUnitText ) {free(newwin->XUnitText); newwin->XUnitText=NULL;}
    if ( newwin->YUnitText ) {free(newwin->YUnitText); newwin->YUnitText=NULL;}
    if ( newwin->Title ) {free(newwin->Title); newwin->Title=NULL;}

    newwin->XUnitText = STRDUP(xl);
    newwin->YUnitText = STRDUP(yl);
    newwin->Title = STRDUP(ti);
}



int xgNewSet()
/*
 * Set up new dataset.  Will return zero if there are too many data sets.
 */
{
    DataSet *loc;

    if ( newwin == NULL) {
	newwin = make_datawin();
    }

    if (newwin->Data == NULL) {
	newwin->Data = loc = (DataSet *)malloc(sizeof(DataSet));
    }
    else {
	/*walk to the end of the Data linked list*/
	for(loc = newwin->Data; loc->next != NULL; loc = loc->next);

	loc->next = (DataSet *)malloc(sizeof(DataSet));
	loc = loc->next;
    }

    curspot = loc;
    curspot->next = (DataSet *)NULL;
    curspot->list = (PointList *)NULL;
    curspot->marker  = 0;
    curspot->setName = (char *)NULL;
    curpt = NULL;
    newGroup = 1;
}


void xgSetName(name)
char *name;			/* New set name */
/*
 * Sets the name of a data set.  Automatically makes a copy.
 */
{
    curspot->setName = STRDUP(name);
}

void xgSetValue(val)
double val;
{
    curspot->setValue = val;
}


void xgSetMark(mark)
int mark;
{
    curspot->marker = mark;
}


void xgNewGroup()
/*
 * Set up for reading new group of points within a dataset.
 */
{
    newGroup = 1;
}

#define INITSIZE 4
/*
 * Adds a new point to the current group of the current
 * data set.
 */
void xgPoint(double xval, double yval)
{
    if ( curpt == NULL ) {
	curspot->list = (PointList *)malloc(sizeof(PointList));
	curpt = curspot->list;
	curpt->numPoints = 0;
	curpt->allocSize = INITSIZE;
	curpt->xvec = (double *) malloc((unsigned)(INITSIZE * sizeof(double)));
	curpt->yvec = (double *) malloc((unsigned)(INITSIZE * sizeof(double)));
	curpt->next = (PointList *) 0;
	newGroup = 0;
    }
    else if (newGroup) {
	curpt->next = (PointList *) malloc(sizeof(PointList));
	curpt = curpt->next;
	curpt->numPoints = 0;
	curpt->allocSize = INITSIZE;
	curpt->xvec = (double *) malloc((unsigned)(INITSIZE * sizeof(double)));
	curpt->yvec = (double *) malloc((unsigned)(INITSIZE * sizeof(double)));
	curpt->next = (PointList *) 0;
	newGroup = 0;
    }

    if (curpt->numPoints >= curpt->allocSize) {
	curpt->allocSize *= 2;
	curpt->xvec = (double *) realloc((char *) curpt->xvec,
					   (unsigned) (curpt->allocSize *
						       sizeof(double)));
	curpt->yvec = (double *) realloc((char *) curpt->yvec,
					   (unsigned) (curpt->allocSize *
						       sizeof(double)));
    }

    curpt->xvec[curpt->numPoints] = xval * newwin->xg_xscl;
    curpt->yvec[curpt->numPoints] = yval * newwin->xg_yscl;

    (curpt->numPoints)++;
}


void xgLabel(lab, x, y)
char *lab;
double x, y;
{
    LabelList *spot;

    if ( newwin == NULL) {
	newwin = make_datawin();
    }

    if ( newwin->Label == NULL ) {
	newwin->Label = (LabelList *)malloc(sizeof(LabelList));
	spot = newwin->Label;
    }
    else {
	for( spot = newwin->Label; spot->next; spot = spot->next );
	spot->next = (LabelList *)malloc(sizeof(LabelList));
	spot = spot->next;
    }

    spot->x = x * newwin->xg_xscl;
    spot->y = y * newwin->xg_yscl;
    spot->just = T_CENTER;
    spot->style = T_AXIS;
    spot->label = STRDUP(lab);
    spot->next = NULL;
}


void xgSetScale(x,y)
double x, y;
{
    if ( newwin == NULL) {
	newwin = make_datawin();
    }
    newwin->xg_xscl = x;
    newwin->xg_yscl = y;
}

void xgClear()
{
    int idx;
    DataSet *dloc, *dtmp;
    PointList *spot, *tmp;
    LabelList *spl, *tl;

    if ( newwin == NULL) return;

    if (newwin->Title) {free(newwin->Title); newwin->Title=NULL;}
    if (newwin->XUnitText) {free(newwin->XUnitText); newwin->XUnitText=NULL;}
    if (newwin->YUnitText) {free(newwin->YUnitText); newwin->YUnitText=NULL;}

    /* Initialize the data sets */
    for (dloc = newwin->Data; dloc; ) {
	for(spot = dloc->list; spot; ) {
	    tmp = spot;
	    spot = spot->next;
	    free(tmp->xvec); tmp->xvec=NULL;
	    free(tmp->yvec); tmp->yvec=NULL;
	    free(tmp);
	}
	if (dloc->setName) {free(dloc->setName); dloc->setName=NULL;}
	dtmp = dloc;
	dloc = dloc->next;
	free(dtmp);
    }
    newwin->Data = NULL;

    /*free the label space*/
    spl = newwin->Label;
    while(spl) {
	tl = spl;
	spl = spl->next;
	free(tl->label); tl->label=NULL;
	free(tl);
    }
    newwin->Label=NULL;

    /*clear the scaling factors*/
    newwin->xg_xscl = 1.0;
    newwin->xg_yscl = 1.0;

    XClearWindow(XtDisplay(graph), XtWindow(graph));
}


void xgUpdate( reset_axis )
int reset_axis;
{
    int flags;
    void Traverse();

    if ( reset_axis && app_context ) {
	XClearWindow(XtDisplay(graph), XtWindow(graph));
    }

    if ( app_context == NULL ) {
	make_graph(newwin);
    }
    do_redraw(graph, newwin);
}



gdraw(x,y)
double x,y;
{
    xgPoint(x, y);
}

gmove(x,y)
double x,y;
{
    xgNewGroup();
    xgPoint(x, y);
}
