/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Shell.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>



extern XtAppContext help_context;

static destroy_help(widget, client_data, call_data)
Widget widget;
XtPointer client_data;   /* cast to bigBitmap */
XtPointer call_data;    /* unused */
{

    XtDestroyApplicationContext(help_context);
    help_context = NULL;
}


help_win(widget, client_data, call_data)
Widget widget;
XtPointer client_data;   /* cast to bigBitmap */
XtPointer call_data;    /* unused */
{
    Widget tL1, vpane, buttonbox;
    Widget close, txt;
    int one = 1;
    char *argv[2];
    Display * dpy;
	
    if (help_context != NULL) return;
    argv[0] = "xsuprem";
    argv[1] = NULL;

    help_context = XtCreateApplicationContext();

    dpy = XtOpenDisplay(help_context, (String) NULL, NULL, "XSuprem",
			NULL, 0, &one, argv);

    if (dpy == NULL)
	XtErrorMsg("invalidDisplay","xtInitialize","XtToolkitError",
                   "Can't Open display", (String *) NULL, (Cardinal *)NULL);

    tL1 = XtVaAppCreateShell( "XSupHelp", "XSuprem", 
			    applicationShellWidgetClass, dpy,
			    XtNscreen, (XtArgVal)DefaultScreenOfDisplay(dpy),
			    NULL );
   
    
    vpane = XtVaCreateManagedWidget("vpane", panedWidgetClass, tL1, NULL);
    buttonbox = XtVaCreateManagedWidget("buttonbox", boxWidgetClass, vpane, NULL);

    /*Make the Help Button*/
    close = XtVaCreateManagedWidget("Cancel", commandWidgetClass, 
					      buttonbox, NULL);
    XtAddCallback(close, XtNcallback, destroy_help, NULL);

    txt = XtVaCreateManagedWidget("help", asciiTextWidgetClass, vpane, 
				  XtNscrollVertical, XawtextScrollAlways,
				  XtNtype, XawAsciiFile,
				  /*XtNstring, "/home/helios0/law/src/xgraph/help",*/
				  NULL);

    XtRealizeWidget(tL1);
}

