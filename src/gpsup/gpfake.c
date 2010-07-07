
#include <stdio.h>
#include <math.h>


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

void xgPlotInit() {}

void check_x() {}

void xgSetBounds(double lowX, double upX, double lowY, double upY) {}

void xgLogAxis(int xlog, int ylog){}

#define STRDUP(str)       (char *)(strcpy(malloc((unsigned) (strlen(str)+1)), (str)))

void xgAxisLabels(char *xl, char *yl, char *ti) {}

int xgNewSet() { return 0; }

int xgNewSetLT(int line_spec) {return 0;}

void xgSetName(char * name) {}

void xgSetValue(double val) {}


void xgSetMark(int mark) {}


void xgNewGroup() {}

#define INITSIZE 4
void xgPoint(double xval, double yval) {}

void xgLabel(char *lab, double x, double y) {}


void xgSetScale(double x, double y) {}

void xgClear() {}


void xgUpdate( int reset_axis ) {}


void axis_2d(float xmin, float xmax, float ymin, float ymax, float xscale, float yscale, float pxmax, float pymax, 
	int flip, char *xlab, char *ylab, int ylog)
{}


#undef gmove
#undef gdraw

void gmove(float x, float y) {}

void gdraw(float x, float y) {}
