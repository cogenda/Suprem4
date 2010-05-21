h43119
s 00000/00000/00022
d D 2.1 93/02/03 15:39:48 suprem45 2 1
c Iniital IV.GS release
e
s 00022/00000/00000
d D 1.1 93/02/03 15:38:31 suprem45 1 0
c date and time created 93/02/03 15:38:31 by suprem45
e
u
U
f e 0
t
T
I 1
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
E 1
