h32030
s 00000/00000/00035
d D 2.1 93/02/03 15:39:46 suprem45 2 1
c Iniital IV.GS release
e
s 00035/00000/00000
d D 1.1 93/02/03 15:38:30 suprem45 1 0
c date and time created 93/02/03 15:38:30 by suprem45
e
u
U
f e 0
t
T
I 1
#define DEVICE 1
#define FILEN 2

#define PS 1
#define IDRAW 2
#define HPGL 3

typedef struct print_info {
    Widget dev_file;
    Widget dev_type;
    Widget dev_name;
    GraphWidget graph;
    Widget print_shell;
    Widget do_include;
} print_calldata;


#define MFNAME 25

typedef enum hard_dev_docu_defn { NONE, NO, YES } hard_dev_docu;

typedef struct hard_dev {
    char *dev_name;		/* Device name                */
    int (*dev_init)();		/* Initialization function    */
    char *dev_spec;		/* Default pipe program       */
    char dev_file[MFNAME];	/* Default file name          */
    char dev_printer[MFNAME];	/* Default printer name       */
    double dev_max_dim;		/* Default maximum dimension (cm)    */
    char dev_title_font[MFNAME];/* Default name of title font        */
    double dev_title_size;	/* Default size of title font (pnts) */
    char dev_axis_font[MFNAME];	/* Default name of axis font         */
    double dev_axis_size;	/* Default size of axis font (pnts)  */
    hard_dev_docu dev_docu;	/* Document predicate                */
};

E 1
