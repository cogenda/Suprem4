/*	string_off.c		Version 5.1		*/
/*	Last Modification:	7/3/91 15:40:55		*/


/*----------------------------------------------------------------------
**  Copyright 1990 by
**  The Board of Trustees of the Leland Stanford Junior University
**  All rights reserved.
**
**  This routine may not be used without the prior written consent of
**  the Board of Trustees of the Leland Stanford University.
**----------------------------------------------------------------------
**/

/*	string_offset.c
 *	goodwin chin
 */

#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "constant.h"
#include "refine.h"
#define BUFFERSIZE 80

string_offset(infile, poffset_array)
char *infile;
struct line **poffset_array;
{
    extern char *fgets();
    char *buffer;
    int nump;
    float xcord;
    float ycord;
    struct line *offset_array;
    int maxop;
    FILE *inf;

    maxop = 100;
    *poffset_array = salloc( struct line, maxop );
    offset_array = *poffset_array;


    if ((buffer = malloc(BUFFERSIZE)) == NULL)
	panic("can't malloc space for buffer\n");

    if ((inf = fopen(infile, "r")) == NULL)
	panic("can't open for read in gen_file_offset\n");
    else  {

	/* read in data */
	nump = 0;

	while ((buffer = fgets(buffer, BUFFERSIZE, inf)) != NULL)  {
	    sscanf(buffer, "%f %f", &xcord, &ycord);
	    offset_array[nump].x = xcord * 1.0e-4;
	    offset_array[nump].y = ycord * 1.0e-4;
	    offset_array[nump].map = 0;
	    offset_array[nump].p = -1;
	    nump++;
	    if ( nump == maxop ) {
		maxop += 100;
		*poffset_array = sralloc( struct line, maxop, offset_array );
		offset_array = *poffset_array;
	    }
	}
	free(buffer);
	fclose(inf);
    }
    return(nump);
}

