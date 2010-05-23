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
#include <time.h>

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
int cpu(char *par, int param )
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

	i = time(NULL);
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
