static char sccsid[]="$Header: ig2_meshio.c rev 5.1 7/3/91 08:30:45";
/*----------------------------------------------------------------------
 *
 * ig2_io -   Reads/writes iggi-II files.
 *
 * Copyright c 1985 The board of trustees of the Leland Stanford
 *                  Junior University. All rights reserved.
 * This subroutine may not be used outside of the SUPREM4 computer
 * program without the prior written consent of Stanford University.
 *
 * Original: CSR Nov85 (tri program)
 * Modified: CSR Mar85 (do triangle codes on the way out)
 * Hacked  : CSR May85 (version for use in Suprem 4 - prior to making
 *			all this stuff rational)
 * Modified: MEL Apr86 (added the node save for the structure card)
 *                     (decided to postpone rationality a little longer)
 *---------------------------------------------------------------------*/
#include "global.h"
#include "constant.h"
#include "dbaccess.h"
#include <stdio.h>
#include <ctype.h>
#include "material.h"
#include "impurity.h"
#include "sysdep.h"
#include "expr.h"	/*for last_temp*/

#define nop(J,I) nd[ tri[I]->nd[J] ]->pt
#define ngh(J,I) tri[I]->nb[J]

/*-----------------IG2_READ---------------------------------------------
 * Do the read.
 * Bugs: completely loses it on synchronization errors. Should do this
 *       in a line-oriented instead of token-oriented fashion. Some day.
 * Warnings: user input point / triangle / node numbers must be consecutive.
 *----------------------------------------------------------------------*/
ig2_read (name, flip, scale)
    char *name;		/* where the data is hiding */
    int flip;		/* flip y coords? */
    float scale;	/* scale factor */
{

    char iline[BUFSIZ];
    char flag[2], *alloc_nd(), *ip;
    int i, ierr, line, n, a, r, p, t;
    float cord[MAXDIM];
    int vert[MAXVRT], bc[MAXSID];
    int vertf[MAXVRT], bcf[MAXSID], b;
    FILE *lu;
    double x, scal[3];

    extern check_active();

    scal[0] = 1e-4*scale,
    scal[1] = 1e-4*((flip)? -scale : scale),
    scal[2] = 1e-4*scale;

    /* See if the file really exists */
    if ((lu = fopen(name, "r")) == NULL) {
	fprintf(stderr, "ig2_read: cannot open %s\n", name);
	return (-1);
    }

    /* Junk the previous mesh */
    dis_all();

    /*default to reading in a two dimensional mesh*/
    set_dim( 2 );

#define SKIP(I) while(isspace(*I)) I++; while(!isspace(*I)) I++;

    /* Read */
    line = ierr = 0;
    while (fgets( iline, BUFSIZ, lu) != NULL) {
	line++;
	sscanf( iline, "%1s", flag);

	switch(flag[0]) {

	case 'D' :
	    /*read the dimensionality*/
	    if ((sscanf(iline, "D %d %d %d", &mode, &nvrt, &nedg)) != 3)
		{ ugh (line,"incomplete dimensional terms",ierr++); }
	    set_dim( mode );
	    break;

	case 'c' :
	    /*read the line data*/
	    if ( sscanf(iline,"c %d", &i ) != 1 )
		ugh( line, "incomplete point line", ierr++ );

	    /*read the solution values*/
	    ip = iline + 1;
	    SKIP( ip );

	    for(i = 0; i < mode; i++) {
		if ( sscanf( ip, "%e", &(cord[i]) ) != 1 ) {
		    ugh( line, "incomplete point line", ierr++ );
		    break;
		}
		cord[i] *= scal[i];

		/*advance past the characters*/
		SKIP(ip);
	    }

	    (void)mk_pt(mode, cord);
	    break;

	case 'e':
	    /* We don't use edge data but we have to get past it. */
	    if ((sscanf(iline,"e %d", &i)) != 1)  {
		ugh( line, "incomplete edge line", ierr++ );
		break;
	    }
	    ip = iline + 1;
	    SKIP(ip);

	    for(i = 0; i < mode; i++) {
		if ( sscanf( ip, "%d", &a ) != 1 ) {
		    ugh( line, "incomplete edge line", ierr++ );
		    break;
		}

		/*advance past the characters*/
		SKIP(ip);
	    }
	    if ( sscanf( ip, "%d", &a ) != 1 ) {
		ugh( line, "incomplete edge line", ierr++ );
		break;
	    }


	    break;

	case 'r' :
	    /* Note the infamous down by -1 kludge */
	    if ((sscanf(iline,"r %d %d", &i, &n)) != 2)
		ugh(line, "need 2 items", ierr++);
	    else {
		(void)mk_reg(n);
		if (num_reg != i) ugh(line, "region sync error", ierr++);
	    }
	    break;

	case 'b' :
	    if (num_reg == 0)
		ugh(line,"no region yet",ierr++);
	    else
		if ((sscanf(iline,"b %d", &i)) != 1)
		    ugh (line, "need 1 item", ierr++);

		/* This code is borrowed from iggi2, which uses edges.
		   Suprem-IV doesn't (yet...)
		else
		    if (err = ad_edge(num_reg,i,reg[num_reg]->bnd,MAYBE,BEFORE))
			    ugh (line, err, ierr++);
		*/
	    break;

	case 't' :
	    /*read the element data*/
	    if ( sscanf(iline,"t %d %d", &n, &r ) != 2 )
		ugh( line, "incomplete node line", ierr++ );
	    r--;

	    /*skip over what's been read*/
	    ip = iline + 1;
	    SKIP(ip); SKIP(ip);

	    /*read the node numbers*/
	    for(i = 0; i < nvrt; i++) {
		if ( sscanf( ip, "%d", &(vert[i]) ) != 1 ) {
		    ugh( line, "incomplete node line", ierr++ );
		    break;
		}
		vert[i]--;

		/*skip a bit*/
		SKIP(ip);
	    }

	    /*read the edge codes*/
	    for(i = 0; i < nedg; i++) {
		if ( sscanf( ip, "%d", &(bc[i]) ) != 1 ) {
		    ugh( line, "incomplete node line", ierr++ );
		    break;
		}
		if (bc[i] > 0 ) bc[i]--;

		/*skip a bit*/
		SKIP(ip);
	    }

            if(flip)
            {
              for(i = 0; i < nvrt; i++)
                vertf[i] = vert[nvrt-1-i];
              for(i = 0; i < nedg; i++)
              {
                b = bc[nedg-1-i];
                if(b == -1022)
                  bcf[i] = -1023;
                else if(b == -1023)
                  bcf[i] = -1022;
                else
                  bcf[i] = b;
              }
            }

	    /*final true is for point creation, rather than node*/
            if(flip)
	      t = mk_ele(nvrt, vertf, nedg, bcf, r, TRUE);
            else
              t = mk_ele(nvrt, vert, nedg, bc, r, TRUE);

            /*optional data on sons and fathers*/
	    if ( sscanf(ip,"%d %d", &n, &r ) == 2 ) {
		set_father(t, n);
		set_offspr(t, r);
	    }

	    break;

	case 's' :
	    /*read in the number of solutions and their impurity numbers*/
	    if ( sscanf(iline,"s %d", &n_imp) != 1 )
		ugh( line, "no impurities on the s line", ierr++ );

	    /*loop to read in all the solution numbers*/
	    /*position ip after s %d*/
	    ip = iline+1;
	    SKIP(ip);

	    /* enter loop sitting on first character after digit */
	    for(i = 0; i < n_imp; i++) {

		/* better be a number */
		if ( sscanf(ip," %d", &(soltoimp[i])) != 1) {
		    ugh(line,"less data than specified impurity number",ierr++);
		    break;
		}
		imptosol[ soltoimp[i] ] = i;

		/* move up ip - depends on \n at end of line */
		SKIP(ip);
	    }
	    break;

	case 'I' :
	    if ( sscanf(iline,"I %d", &n_imp) != 1 )
		ugh( line, "no impurities on the I line", ierr++ );

	    /*loop to read in all the solution numbers*/
	    /*position ip after s %d*/
	    ip = iline+1;
	    SKIP(ip);

	    /* enter loop sitting on first character after digit */
	    for(i = 0; i < n_imp; i++) {

		/* better be a number */
		if ( sscanf(ip," %d", &n) != 1) {
		    ugh(line,"less data than specified impurity number",ierr++);
		    break;
		}
		if (n) SET_FLAGS(soltoimp[i], IMPLANTED_IMP);
		else CLEAR_FLAGS(soltoimp[i], IMPLANTED_IMP);

		/* move up ip - depends on \n at end of line */
		SKIP(ip);
	    }
	    break;

	case 'n' :
	    /*read the line data*/
	    if ( sscanf(iline,"n %d %d", &i, &n ) != 2 )
		ugh( line, "incomplete node line", ierr++ );

	    p = mk_nd(i, n);

	    /*read the solution values*/
	    ip = iline + 1;
	    SKIP(ip);
	    SKIP(ip);

	    for(i = 0; i < num_sol(p); i++) {
		if ( sscanf( ip, "%le", &x ) != 1 ) {
		    ugh( line, "incomplete node line", ierr++ );
		    break;
		}
		set_sol_nd(p, i, x);
		SKIP(ip);
	    }
	    break;

	case 'M':
	    if ( sscanf( iline, "M %d %le", &sub_ornt, &last_temp) < 1 )
		 ugh( line, "no orientation given", ierr++);
	    if (last_temp == 0.0) last_temp = 1173.0;
	    break;

	default :
	    /* ignore this line */
	    break;
	}
    } /* Next line */

    fclose(lu);

    if (np == 0 || ne==0 || num_reg==0)
	ugh(line, "mesh is not complete!", ierr++);

    check_active();

    return(-ierr);
}

ugh(line,s,ierr)
    char *s;
    int line,ierr;
{
    fprintf (stderr,"Input mesh error in line %d: %s \0",line,s);
}


/*-----------------IG2_WRITE--------------------------------------------
 * Write an IGGI-II format mesh.
 *----------------------------------------------------------------------*/
int ig2_write (name, lflip, scale)
    char *name;
    int lflip;
    float scale;
{
    FILE *lu;
    double scal[3];
    int ip, ir, it, is, in, i, bc;

    scal[0] = 1e+4*scale;
    scal[1] = 1e+4*((lflip)? -scale : scale);
    scal[2] = 1e+4*scale;


    /* If the file is stuck it ain't worth a <beep> */
    if ((lu = fopen(name, "w")) == NULL) {
	fprintf(stderr, "ig2_write: cannot open %s\n", name);
	return(-1);
    }

    /* The 0th step is to print what version of the program is being abused */
    fprintf( lu, "v %s\n", VERSION);

    /* write out the dimensionality*/
    fprintf( lu, "D %d %d %d\n", mode, nvrt, nedg );

    /* The next is to print the points one by one */
    for (ip = 0; ip < np; ip++) {
	fprintf (lu, "c %d", ip+1);
	for(i = 0; i < mode; i++) fprintf(lu, " %g", cordinate(ip,i)*scal[i]);
	fprintf (lu, "  0\n" );
    }

    /* Then the regions */
    for (ir = 0; ir < num_reg; ir++) {
	fprintf (lu, "r %d   %d\n", ir+1, mat_reg(ir));
    }

    /* Then the triangles */

    /*This brutal hack accounts for the down by one problem*/
#  define tcode(ie,j) ((neigh_t(ie,j) >= 0)? neigh_t(ie,j)+1 : neigh_t(ie,j))

    for(it = 0; it < ne; it++) {
	fprintf (lu, "t %d %d ", it+1, reg_tri(it) + 1);

        if(lflip)
        {
          for(i=num_vert(it)-1; i>=0; i--)
	    fprintf(lu,"%d ",pt_nd(vert_tri(it,i))+1);
          for(i=num_face(it)-1; i>=0; i--)
          {
            /* exchange bot/top boundary code */
            bc = tcode(it, i);
            if(bc == -1022)
              fprintf (lu, "%d ",  -1023 );
            else if( bc == -1023)
              fprintf (lu, "%d ",  -1022 );
            else
              fprintf (lu, "%d ",  bc );
          }
        }else
        {
          for(i=0; i < num_vert(it); i++)
            fprintf(lu,"%d ",pt_nd(vert_tri(it,i))+1);
          for(i=0; i < num_face(it); i++)
            fprintf (lu, "%d ", tcode(it, i) );
        }
	fprintf(lu, "%d %d ", father(it), offspr(it));
	fprintf( lu, "\n" );
    }

    /* print any model information that needs to be stored
       (this could be a monster if we all pull together as a team) */
    fprintf( lu, "M %d %e\n", sub_ornt, last_temp);

    /* print the node info line */
    fprintf( lu, "s %d  ", n_imp );
    for( is = 0; is < n_imp; is++ ) fprintf(lu, " %d", soltoimp[is]);
    fprintf( lu, "\n" );

    /* print the node data for all the nodes */
    for( in = 0; in < nn; in++ ) {
	fprintf( lu, "n %d   %d  ", pt_nd(in), mat_nd(in) );
	for( is = 0; is < num_sol(in); is++ ) fprintf(lu," %e", sol_nd(in,is));
	fprintf( lu, "\n" );
    }

    fprintf( lu, "I %d  ", n_imp );
    for( i = 0; i < n_imp; i++ ) fprintf(lu, " %d", IS_IMPLANTED(soltoimp[i]));
    fprintf( lu, "\n" );

    /* Bye now. */
    fclose (lu);
    return(0);
}
