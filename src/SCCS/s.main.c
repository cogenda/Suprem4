h03926
s 00003/00003/00215
d D 2.2 93/04/23 12:34:53 suprem45 3 2
c changed silicon cmd to isilicon
e
s 00039/00025/00179
d D 2.1 93/02/03 14:02:27 suprem45 2 1
c Initial IV.GS release
e
s 00204/00000/00000
d D 1.1 93/02/03 13:50:40 suprem45 1 0
c date and time created 93/02/03 13:50:40 by suprem45
e
u
U
f e 0
t
T
I 1
/*************************************************************************
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *     Copyright c 1991 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *									 *
 *************************************************************************/
D 3
/*   %M%                Version %I%     */
/*   Last Modification : %G% %U% */
E 3
I 3
/*   main.c                Version 2.1     */
/*   Last Modification : 2/3/93 14:02:27 */
E 3

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#define STATIC_ALLOCATION_TIME
#include "global.h"
#include "check.h"
#include "constant.h"
#include "dbaccess.h"
#include "diffuse.h"
#include "impurity.h"
#include "defect.h"
#include "key.h"
#include "material.h"
#include "matrix.h"
#include "plot.h"
#include "regrid.h"
#include "shell.h"
#include "implant.h"
#include "sysdep.h"
#include "expr.h"
#include "refine.h"
#include "poly.h"
#ifdef DEVICE
#include "device.h"
#endif

/*signal location of return*/
extern onintr();

/*every command subroutine has to be declared here*/
extern echo(), cpu(), man(), plot_2d();
extern option(), method(), diffuse(), implant();
extern sel_var(), contour(), plot_1d(), print_1d();
extern vacancy(), interstitial(), antimony();
extern arsenic(), boron(), regrid(), coeffox();
extern phosphorus(), rregion(), redge(), rline();
extern initialize(), structure();
extern user_deposit(), user_etch();
extern gold(), draw_lab();
extern stress_analysis();
extern c_mater(), cesium();
extern uPause(), trap(), user_print(), profile();
extern mode_cmd();
extern device();
I 2
extern beryllium(), magnesium(), selenium(), silicon(), tin();
extern germanium(), zinc(), carbon(), generic();
E 2

struct command_table command[NUMCMD] = {
D 2
    "echo", echo, NULL,			"man", man, NULL, 			
    "initialize", initialize, NULL,	"cpulog", cpu, NULL,		
    "plot.2d", plot_2d, NULL, 		"option", option, NULL,		
    "method", method, NULL,		"diffuse", diffuse, NULL,		
    "implant", implant, NULL, 		"select", sel_var, NULL,		
    "contour", contour, NULL,		"plot.1d", plot_1d, NULL,		
    "print.1d", print_1d, NULL,		"vacancy", vacancy, NULL,	
    "interstitial", interstitial, NULL, "antimony", antimony, NULL,	
    "arsenic", arsenic, NULL, 		"boron", boron, NULL,		
    "oxide", coeffox, NULL,	
    "phosphorus", phosphorus, NULL, 	"region", rregion, NULL,	
E 2
I 2
    "echo", echo, NULL,			"man", man, NULL, 
    "initialize", initialize, NULL,	"cpulog", cpu, NULL,
    "plot.2d", plot_2d, NULL, 		"option", option, NULL,
    "method", method, NULL,		"diffuse", diffuse, NULL,
    "implant", implant, NULL, 		"select", sel_var, NULL,
    "contour", contour, NULL,		"plot.1d", plot_1d, NULL,
    "print.1d", print_1d, NULL,		"vacancy", vacancy, NULL,
    "interstitial", interstitial, NULL, "antimony", antimony, NULL,
    "arsenic", arsenic, NULL, 		"boron", boron, NULL,
    "oxide", coeffox, NULL,
    "phosphorus", phosphorus, NULL, 	"region", rregion, NULL,
E 2
    "boundary", redge, NULL,		"line", rline, NULL,
    "structure", structure, NULL,	"deposit", user_deposit, NULL,
D 2
    "etch", user_etch, NULL,		"gold", gold, NULL,		
    "stress", stress_analysis, NULL,	"material", c_mater, NULL,	
E 2
I 2
    "etch", user_etch, NULL,		"gold", gold, NULL,
    "stress", stress_analysis, NULL,	"material", c_mater, NULL,
E 2
    "cesium", cesium, NULL,		"pause", uPause, NULL,
    "trap", trap, NULL,			"printf", user_print, NULL,
D 2
    "profile", profile, NULL,		"mode", mode_cmd, NULL,			
    "device", device, NULL,		"", NULL, 45,			
    "", NULL, 44,			"", NULL, 45,			
    "", NULL, 44,			"", NULL, 45,			
    "", NULL, 44,			"", NULL, 45,			
    "", NULL, 46,			"", NULL, 47,			
    "", NULL, 46,			"", NULL, 47,			
    "", NULL, 46,
    "", NULL, 48,			"", NULL, 49};			
E 2
I 2
    "profile", profile, NULL,		"mode", mode_cmd, NULL,	
    "device", device, NULL,		"beryllium", beryllium, NULL,
    "magnesium", magnesium, NULL,	"selenium", selenium, NULL,
D 3
    "silicon", silicon, NULL,		"tin", tin, NULL,
E 3
I 3
    "isilicon", silicon, NULL,		"tin", tin, NULL,
E 3
    "germanium", germanium, NULL,	"zinc", zinc, NULL,
    "carbon", carbon, NULL,		"generic", generic, NULL,
    "", NULL, 45,			"", NULL, 46,			
    "", NULL, 47,			"", NULL, 48,			
    "", NULL, 49};			
E 2

main(argc, argv)
int argc;
char **argv;
{
    int i;
    char dot_name[80], *t;

I 2
/*
    (void)malloc_debug(2);
 */

    /* this is for debugging only */
    if (0) { double x; void pa(); pa(&x, 0, 0); }

E 2
    /*line at a time buffering*/
#ifdef BSD
    setlinebuf(stdout);
    setlinebuf(stderr);
#endif

    /*initialize the file pointers, which are defined on the compile line*/
    if (t = (char *)getenv("SUP4MANDIR")) 
	MANDIR = t;
    else
	MANDIR = MANLOC;
I 2

E 2
    if (t = (char *)getenv("SUP4KEYFILE")) 
	KEYFILE = t;
    else
	KEYFILE = KEYLOC;
I 2

E 2
    if (t = (char *)getenv("SUP4MODELRC")) 
	MODELFILE = t;
    else
	MODELFILE = MODELLOC;
I 2

E 2
    if (t = (char *)getenv("SUP4IMPDATA")) 
	IMP_FILE = t;
    else
	IMP_FILE = IMPLOC;

    /* Parser initialization (verbose is in global.h, not in the parser) */
D 2
    strcpy( VersionString, "SUPREM-IV A.9130");
E 2
I 2
    strcpy( VersionString, "SUPREM-IV.GS B.9305");
E 2
    fprintf(stdout, "%s\n", VersionString);
D 2
    fprintf(stdout, "\t(c) 1991 Stanford University\n");
E 2
I 2
    fprintf(stdout, "\t(c) 1991-1993 Stanford University\n");
E 2
    fprintf(stdout, "\t(c) 1991 University of Florida\n");
    fprintf(stdout, "Reading Models...\n");
    parser_boot( KEYFILE, "SUPREM4 ");
    verbose = V_CHAT;

    /*initialize the diffusion co_efficients and routine pointers*/
    diffuse_init();
#ifdef DEVICE
    device_init();
#endif
    vxmin = vymin = 0.0;	vxmax = vymax = 1.0;

    /*plot initialization*/
    xgPlotInit();

    /*some grid initialization*/
    rect_boot();

    /*plot initialization*/
    title = salloc(char, strlen(VERSION)+1);
    strcpy(title, VERSION);

    /*ready to start reading input files!*/

    /*read the modelrc file*/
    do_source(MODELFILE, NULL, FALSE, /*report errors*/ TRUE);

    /*read the .supremrc file*/
D 2
    strcpy(dot_name, getenv("HOME")); strcat(dot_name, "/.supremrc");
E 2
I 2
    strcpy(dot_name, (char *)getenv("HOME")); strcat(dot_name, "/.supremrc");
E 2
    do_source(dot_name, NULL, FALSE, /*report errors*/ FALSE);
    do_source(".supremrc", NULL, FALSE, /*report errors*/ FALSE);

    /*do any command line files*/
    for(i = 1; i < argc; i++) {
	do_source(argv[i], NULL, FALSE, /*report errors*/ TRUE );
    }

    /*main command parsing loop*/
    if ( argc < 2 )
	while ( (i = yyparse()) != -1) check_x();

    /*if we are logging cpu stats, close the file*/
    if (cpufile != NULL)
	fclose(cpufile);
I 2

    xgUpdate(1);
E 2


}






/************************************************************************
 *									*
 *	mode( par, param ) - This card controls the computation mode.	*
 *  It gets its card so that it doesn't get used by mistake.		*
 *									*
 *  Original:	MEL	10/84						*
 *									*
 ************************************************************************/
mode_cmd( par, param )
char *par;
int param;
{
#   define CHOSEN(x) (is_specified( param, x) && get_bool( param, x))

    /*how much barfola the user wants*/
    if( CHOSEN("one.dim")) {
	set_dim( 1 );
    }
    if( CHOSEN("two.dim")) {
       set_dim( 2 );
    }
    
    return(0);
}

E 1
