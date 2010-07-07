/*	makeframe.c		Version 5.1	*/
/*	Last Modification:  7/3/91 15:45:33		*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sysdep.h"
#include "bound.h"

#ifdef CONVEX
#include "constant.h"
#endif


#define CRITERION 1
#define OFFSET 30
#define	WHITE	254
#define BLACK	255
#define NITRIDE_COLOR 250
#define OXIDE_COLOR 5
#define MACOXIDE 255
#define MACWHITE 0
#define MACNITRIDE 250

/*
     input:
	data = array of data for entire array
	nplot= number of frames in conman output file tfile (tfile is default)
	nx= number of elements in the x direction
	ny= number of elements in the y direction
	nxfac= number of pixels per element in the x direction (4 is good)
	nyfac= number of pixels per element in the y direction (4 is good)
	name= name to call output frame files ('movie' is good)
*/
makeframe(data, NX, NY, NXFAC, NYFAC, name, min_value, mode,
    macfile)
float *data;
int NX;
int NY;
int NXFAC;
int NYFAC;
char *name;
float min_value;
int mode;  /* scaling mode - for some extra "hooks" */
int macfile;
{
float min, max,t, u, width;
int k, ix, iy, iix, iiy, fdd,value;
int NELX, NELY;
int fillx, filly;
float fvalue,value1,value2,value3,value4;
int iaddr, iaddr2, ixstart, iystart, ixstop, iystop, l, n, p;
unsigned char *array;
char *filename;
float OXIDE_VALUE;
float NITRIDE_VALUE;
float MAXCOLOR = 240;
float CLIPVALUE = 14.0;
filename = (char *)malloc(25*sizeof(char));

OXIDE_VALUE = min_value - 1.0;
NITRIDE_VALUE = min_value - 2.0;
NELX = NX/NXFAC;
NELY = NY/NYFAC;
fillx = NX % NXFAC;
filly = NY % NYFAC;
array = (unsigned char *) calloc( NX * NY , sizeof(char));

/* Set min and max equal to gmin and gmax for consistency in multiple files */
	min = gmin;
   	max = gmax;

 	for ( iy=0; iy<NELY; iy++)  {
	    for ( ix=0; ix<NELX; ix++)
	    {
		iaddr=ix+(NELX+1)*iy;
		value1=data[iaddr] ;
		if ( (value1 != min_value) && (value1 != OXIDE_VALUE)
			&& (value1 != NITRIDE_VALUE) )  {
		    iaddr=ix+1+(NELX+1)*iy;
		    value2=data[iaddr] ;
		    iaddr=ix+1+(NELX+1)*(iy+1);
		    value3=data[iaddr] ;
		    iaddr=ix+(NELX+1)*(iy+1);
		    value4=data[iaddr] ;
	  	    ixstart=ix*NXFAC;
		    iystart=iy*NYFAC;
                    ixstop=(ix+1)*NXFAC;
                    iystop=(iy+1)*NYFAC;
		    for ( iiy=iystart;iiy<iystop;iiy++)
		        for ( iix=ixstart;iix<ixstop;iix++)
		        {
                            t=((float)iix-(float)ixstart)/(float)NXFAC;
                            u=((float)iiy-(float)iystart)/(float)NYFAC;
			    iaddr2 = iix + iiy * NX;
                            fvalue = (1.-t)*(1.-u)*value1
                                  + t*(1.-u)     *value2
                                  + t*u          *value3
                                  + (1.-t)*u     *value4;
			    width = MAXCOLOR - (float)OFFSET;
			    if (mode)  {
				if (fabs((double)fvalue) < CLIPVALUE)
	value = OFFSET + (int)(width/2.0);
				else  {
				    if (fvalue < 0)
	value = OFFSET + (int)(((max-fvalue-28.0)*width)/(max-min-28.0));
				    else
	value = OFFSET + (int)(((max-fvalue)*width)/(max-min-28.0));
				}
			    }
			    else
                    value=(int)(OFFSET + ((fvalue-min) * width)/(max - min));


			    value=mysmooth(value);
		    	    array[iaddr2]=(unsigned char)value;
		        }
		} /* if value1 */
		else  {
		    if ( value1 == OXIDE_VALUE )  {
			if (macfile)
			    value = MACOXIDE;
			else
			    value = OXIDE_COLOR;
		    }
		    else if (value1 == NITRIDE_VALUE) {
			if (macfile)
			    value = MACNITRIDE;
			else
			    value = NITRIDE_COLOR;
		    }
		    else  {
			if (macfile)
			    value = MACWHITE;
			else
		    	    value = WHITE;
		    }
	  	    ixstart=ix*NXFAC;
		    iystart=iy*NYFAC;
                    ixstop=(ix+1)*NXFAC;
                    iystop=(iy+1)*NYFAC;
		    for ( iiy=iystart;iiy<iystop;iiy++)
		        for ( iix=ixstart;iix<ixstop;iix++)
		        {
			    iaddr2 = iix + iiy * NX;
		    	    array[iaddr2]=(unsigned char)value;
		        }
		}  /* if then else value1 */
	    } /* for ix */
    /* pad extra x-values with WHITE */
	    n = iy * NX + NELX * NXFAC;
	    for (k = 0; k < fillx; k++)
		if (macfile)
		    array[n + k] = MACWHITE;
		else
		    array[n + k] = WHITE;
 	} /* for iy */
	for (k = 0 ; k < filly; k++)    {
	    n = (NELY * NYFAC + k) * NX;
	    for (p = 0; p < NX; p++)  {
		if (macfile)
		    array[n + p] = MACWHITE;
		else
		    array[n + p] = WHITE;
	    }
	}
    /* pad extra y-values with WHITE */

	sprintf(filename,"%s(%d*%d)",name, NX, NY);
	fdd=open(filename,O_WRONLY | O_CREAT | O_TRUNC,0644);
	write(fdd,array,NX*NY);
	close(fdd);
        free( array );
	return(0);
}

mysmooth(value)
int value;
{
    int quotient;
    int remainder;

    quotient = value/CRITERION;
    remainder = value%CRITERION;
    quotient = ((float)remainder < (float)CRITERION/2.0) ?
	quotient : quotient + 1;
    return(quotient*CRITERION);
}

