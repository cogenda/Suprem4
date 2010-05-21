/* 
 * GraphP.h - Private definitions for Graph widget
 */

#ifndef _ORAGraphP_h
#define _ORAGraphP_h

/*
 * This include not needed unless the .c file includes IntrinsicP.h
 * after this file.   Anyway, it doesn't hurt.
 */
#include <X11/CoreP.h>

/*
 * This one is always needed!
 */
#include "Graph.h"
#include "DataRec.h"

/* New fields for the Graph widget class record */

typedef struct {
	int make_compiler_happy;	/* keep compiler happy */
} GraphClassPart;

/* Full class record declaration */
typedef struct _GraphClassRec {
    CoreClassPart	core_class;
    GraphClassPart	graph_class;
} GraphClassRec;

extern GraphClassRec graphClassRec;
#define MAXCOL 8

/* New fields for the Graph widget record */
typedef struct {
    /* resources */
    XtCallbackList callback;	/* application installed callback function(s) */
    XFontStruct *titleFont;
    XFontStruct *labelFont;
    Pixel gridColor;
    Pixel zeroColor;
    Pixel pix[MAXCOL];		/*line colors*/
    GraphWin *wi;
    
    /* private state */
    int stx, sty;		/* start of zoom box */
    int crx, cry;		/* end of zoom box */
    Pixmap marker[MAXCOL];	/* line markers */
    GC		title_gc;	/* text label graphic context*/
    GC		label_gc;	/* text label graphic context*/
    GC		seg_gc;		/* segment drawing context*/
    GC		dot_gc;		/* dots */
    GC		echo_gc;	/* for the outlines */
} GraphPart;

/*
 * Full instance record declaration
 */
typedef struct _GraphRec {
    CorePart		core;
    GraphPart		graph;
} GraphRec;

#endif /* _ORAGraphP_h */
