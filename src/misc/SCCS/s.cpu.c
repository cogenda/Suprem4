h01964
s 00000/00000/00068
d D 2.1 93/02/03 15:25:07 suprem45 2 1
c Initial IV.GS release
e
s 00068/00000/00000
d D 1.1 93/02/03 15:24:14 suprem45 1 0
c date and time created 93/02/03 15:24:14 by suprem45
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
 *************************************************************************/
/*   cpu.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:41:05 */

#include <stdio.h>
#include <math.h>
#include "global.h"
#include "shell.h"

extern long time();


/************************************************************************
 *									*
 *	cpu( par, param ) - this routine sets up a log file cpu 	*
 *  times to be stored.							*
 *									*
 *	Original :	Mark E. Law	Oct, 1984				
 *									*
 ************************************************************************/
cpu(par, param )
char *par;
int param;
{
    char *f;
    int on;
    long i;
    
    f  = get_string( param, "cpufile" );
    on = get_bool( param, "log" );

    /*if we are turning logging on, set everything up*/
    if ( on ) {
	/*if no name given, usr stdout*/
	if (f == NULL) 
	    cpufile = stdout;
	else {
	    if ((cpufile = fopen(f, "a")) == NULL) {
		fprintf(stderr, "error on open of cpu file %s\n", f);
		return(-1);
	    }
#           ifdef BSD
	    setlinebuf( cpufile);
#           endif
	}
	/*write a header into it*/
	fprintf(cpufile, "\n\nSUPREM IV cpu usage summary");

	i = time((long *)0);
	f = (char *)ctime(&i);
	fprintf(cpufile, "\t%s\n", f);
    }
    else  {
	/*turn it all off*/
	if (cpufile != NULL)
	    if (cpufile != stdout)
		fclose(cpufile);
	cpufile = NULL;
    }
    return(0);
}
E 1
