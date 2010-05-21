
typedef struct point_list {
    int numPoints;		/* Number of points in group */
    int allocSize;		/* Allocated size            */
    double *xvec;		/* X values                  */
    double *yvec;		/* Y values                  */
    struct point_list *next;	/* Next set of points        */
} PointList;

typedef struct new_data_set {
    char *setName;		/* Name of data set     */
    double setValue;		/* Value for data set */
    PointList *list;		/* List of point arrays */
    int marker;			/* Marker for this set? */
    struct new_data_set *next;
} DataSet;

typedef struct label_list_str {
    char *label;
    double x, y;
    int just;
    int style;
    struct label_list_str *next;
    } LabelList;


/* Passed device option flags */
#define D_DOCU		0x01

/* Returned device capability flags */
#define D_COLOR		0x01

/* Text justifications */
#define T_CENTER	0
#define T_LEFT		1
#define T_UPPERLEFT	2
#define T_TOP		3
#define T_UPPERRIGHT	4
#define T_RIGHT		5
#define T_LOWERRIGHT	6
#define T_BOTTOM	7
#define T_LOWERLEFT	8

/* Text styles */
#define T_AXIS		0
#define T_TITLE		1

/* Line Styles */
#define L_AXIS		0
#define L_ZERO		1
#define L_VAR		2

/* Marker Styles */
#define P_PIXEL		0
#define P_DOT		1
#define P_MARK		2

/* Output device information returned by initialization routine */

typedef struct xg_out {
    int dev_flags;		/* Device characteristic flags           */
    int area_w, area_h;		/* Width and height in pixels            */
    int bdr_pad;		/* Padding from border                   */
    int axis_pad;		/* Extra space around axis labels        */
    int tick_len;		/* Length of tick mark on axis           */
    int legend_pad;		/* Top of legend text to legend line     */
    int axis_width;		/* Width of big character of axis font   */
    int axis_height;		/* Height of big character of axis font  */
    int title_width;		/* Width of big character of title font  */
    int title_height;		/* Height of big character of title font */

    void (*xg_init)();		/* Draws text at a location              */
    void (*xg_text)();		/* Draws text at a location              */
    void (*xg_seg)();		/* Draws a series of segments            */
    void (*xg_dot)();		/* Draws a dot or marker at a location   */
    void (*xg_end)();		/* Stops the drawing sequence            */

} xgOut;

typedef struct local_win {
    double llx, lly, urx, ury;  /* Bounding box of all data */
    double loX, loY, hiX, hiY;	/* Local bounding box of window         */
    int XOrgX, XOrgY;		/* Origin of bounding box on screen     */
    int XOppX, XOppY;		/* Other point defining bounding box    */
    double UsrOrgX, UsrOrgY;	/* Origin of bounding box in user space */
    double UsrOppX, UsrOppY;	/* Other point of bounding box          */
    double XUnitsPerPixel;	/* X Axis scale factor                  */
    double YUnitsPerPixel;	/* Y Axis scale factor                  */
    double xg_xscl, xg_yscl;	/* Scaling Factors */
    int logx, logy;
    DataSet	*Data;		/*Data for the plot window*/
    LabelList	*Label;		/*LabelLists in the window*/
    XSegment *Xsegs;
    int max_segs;		/* Maximum number of segments in group   */
    char *Title;
    char *XUnitText;
    char *YUnitText;
    xgOut dev_info;
} GraphWin;
