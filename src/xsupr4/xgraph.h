/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

XtAppContext app_context;
XtAppContext help_context;
GraphWidget graph;

GraphWin *newwin;

DataSet *curspot;
PointList *curpt;

double xg_xscl;
double xg_yscl;

int newGroup;

Widget global;

int toolkitinit;
