h33582
s 00005/00007/00094
d D 2.1 93/02/03 15:25:07 suprem45 2 1
c Initial IV.GS release
e
s 00101/00000/00000
d D 1.1 93/02/03 15:24:13 suprem45 1 0
c date and time created 93/02/03 15:24:13 by suprem45
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
/*   bsd.c                Version 5.1     */
/*   Last Modification : 7/3/91 08:08:38 */

#include <stdio.h>


/************************************************************************
 *									*
 *	This file contains routines which, for one reason or another	*
 *  have to be added to the BSD codes and not the HPUX code.		*
 *									*
 ************************************************************************/

I 2
#ifdef NOSTRTOK
E 2

D 2

E 2
/************************************************************************
 *									*
 *	strtok(s1, s2) - This routine is shamelessly based on the 	*
 *  routine provided with the HPUX string(3C) library.  It is not 	*
 *  in the Berkeley library, therefore this copy.  The routine takes	*
 *  s1 to be a series of tokens separated by one or more of the chars	*
 *  in s2.  It returns a pointer to the first token and writes a \0 in	*
 *  the string at the end of the token.  Subsequent calls with NULL for	*
 *  s1, continue to use the previous string.  If no tokens remain, a	*
 *  Null is returned.							*
 *									*
 *  Original:	MEL	12/84						*
 *									*
 ************************************************************************/
char *strtok(news1, s2)
char *news1, *s2;
{
    static char *s1;
    char *start, *t;

    /*set up a new string to work on, if required*/
    if (news1 != NULL)
	s1 = news1;

    /*advance the pointer past any token separators*/
    for ( ; *s1 ; s1++) {
	for (t = s2; *t; t++)
	    if (*s1 == *t) goto found;
	break;
	found:;
    }

    /*we are now at the beginning of the token*/
    if ( ! *s1 )
	start = NULL;
    else {
	start = s1;

	/*now find the end of the token*/
	for (; *s1; s1++)
	    for (t = s2; *t; t++)
		if (*t == *s1) goto break2b;
	break2b:

	/*put in the null*/
	if ( *s1 )
	    *s1++ = '\0';
    }

    return(start);
}

I 2
#endif /* NOSTRTOK */
E 2

I 2
#ifdef NOYYPARSE

E 2
/************************************************************************
 *									*
 *	yyerror - error printer for the yacc routines, normally it is	*
 *  found in the library liby.a which is unexiplicably missing from BSD	*
 *									*
 ************************************************************************/
yyerror(s)
char *s;
{
    fprintf(stderr, "%s", s);
}

D 2
pl_interface() {return(0);}
double do_poly() {return(1.0e37);}
poly_gs() {}
local_normal2() {}
poly_init() {}
grain_gs() {}
E 2
I 2
#endif /* NOYYPARSE */
E 2
E 1
