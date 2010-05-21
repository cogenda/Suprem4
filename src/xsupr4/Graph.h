/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


#ifndef _Graph_h
#define _Graph_h

#ifdef ardent
typedef void * XtPointer;
#endif

/*
 * Graph Widget public include file
 */

/*
 * This include not needed unless the application includes Intrinsic.h
 * after this file.   Anyway, it doesn't hurt.
 */
#include <X11/Core.h>

/* Resources:
 * Name		     Class		RepType		Default Value
 * ----		     -----		-------		-------------
 * (from RectObj)
 * ancestorSensitive
 * x		    Position		Int		0
 * y		    Position		Int		0
 * width	Dimension	Dimension	0
 * height	Dimension	Dimension	0
 * borderWidth	BorderWidth	Int
 * sensitive	Sensitive
 *
 * (from WindowObj)
 * screen		    Screen		Pointer		XtCopyScreen
 * depth		    Depth		Int		XtCopyFromParent
 * colormap	    Colormap	        Pointer	        XtCopyFromParent
 * background	    Background		Pixel		White
 * backgroundPixmap    Pixmap		Pixmap		XtUnspecifiedPixmap
 * borderColor	    BorderColor		Pixel		Black
 * borderPixmap	    BorderPixmap	Pixmap		XtUnspecifiedPixmap
 * mappedWhenManaged   MappedWhenManaged	Boolean		True
 * translations
 * accelerators
 *
 * (from Core)
 * none
 *
 */

/* 
 * This public structure is used as call_data to the callback.
 * It passes the x, y position of the cell toggled (in units of
 * cells, not pixels) and a mode flag that indicates whether the
 * cell was turned on (1) or off (0).
 */
/* Class record constants */

#define XtNTitleFont "titleFont"
#define XtNLabelFont "labelFont"
#define XtNZeroColor "zeroColor"
#define XtNGridColor "gridColor"
#define XtCTitleFont "TitleFont"
#define XtCLabelFont "LabelFont"
#define XtCZeroColor "ZeroColor"
#define XtCGridColor "GridColor"

#define XtCLineColor1 "LineColor1"
#define XtNLineColor1 "lineColor1"
#define XtCLineColor2 "LineColor2"
#define XtNLineColor2 "lineColor2"
#define XtCLineColor3 "LineColor3"
#define XtNLineColor3 "lineColor3"
#define XtCLineColor4 "LineColor4"
#define XtNLineColor4 "lineColor4"
#define XtCLineColor5 "LineColor5"
#define XtNLineColor5 "lineColor5"
#define XtCLineColor6 "LineColor6"
#define XtNLineColor6 "lineColor6"
#define XtCLineColor7 "LineColor7"
#define XtNLineColor7 "lineColor7"
#define XtCLineColor8 "LineColor8"
#define XtNLineColor8 "lineColor8"
#define XtNGraphWin    "GraphWin"
#define XtCGraphWin    "graphWin"

extern WidgetClass graphWidgetClass;

typedef struct _GraphClassRec *GraphWidgetClass;
typedef struct _GraphRec      *GraphWidget;

#endif /* _ORAGraph_h */
/* DON'T ADD STUFF AFTER THIS #endif */
