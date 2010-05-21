h17082
s 00000/00000/00065
d D 2.1 93/02/03 15:37:21 suprem45 2 1
c Initial IV.GS release
e
s 00065/00000/00000
d D 1.1 93/02/03 15:35:47 suprem45 1 0
c date and time created 93/02/03 15:35:47 by suprem45
e
u
U
f e 0
t
T
I 1
/*************************************************************************
 *									 *
 *   Original : CSR         Stanford University        Jan, 1987 	 *
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   parser_boot.c                Version 5.1     */
/*   Last Modification : 7/3/91  08:28:14 */

#include <stdio.h>
#include "check.h"
#include "constant.h"
#include "dbaccess.h"
#include "global.h"
#include "key.h"
#include "shell.h"
#include "sysdep.h"
#include "expr.h"

/************************************************************************
 *									*
 *	parser_boot - do all the bits and pieces needed to get the      *
 *			parser going					*
 *	Orignal: CSR Thu Jan 15 17:07:46 PST 1987			*
 ************************************************************************/
parser_boot( Pkeyfile, Pprompt)
     char *Pkeyfile;
     char *Pprompt;
{
    int i;

    /*set up the input buffer*/
    buflen = 160;
    buffer = scalloc(char , buflen);
    bufptr = 0;
    echo_buflen = 160;
    echo_buffer = scalloc(char , echo_buflen);
    echo_bufptr = 0;
    supbln = 160;
    supbuf = scalloc(char , echo_buflen);
    supbpt = 0;

    /*initialize shell variables*/
    interactive = FALSE;
    noexecute = FALSE;
    depth = -1;

    /*read the unformatted key file*/
    if ( read_ukfile( Pkeyfile ) == -1 ) {
	fprintf(stderr, "unable to read or open the unformatted key file %s\n",			Pkeyfile);
	exit( -1 );
    }

    /*set up the prompt*/
    prompt = salloc(char , strlen(Pprompt) +1);
    strcpy(prompt, Pprompt);

    in_file = stdin;
    for(i = 0; i < 10; i++) store[i] = NULL;

}
E 1
