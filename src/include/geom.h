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
/*   geom.h                Version 5.1     */
/*   Last Modification : 7/3/91 08:55:05 */

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

#define GEOMDB
#ifndef DBFUNC
#include "dbaccess.h"
#endif

#ifndef SKELDB
#include "skel.h"
#endif


/************************************************************************
 *									*
 *	This file contains the structure definitions of the three 	*
 *  fundamental units used to represent data in SUPREM IV.  These are	*
 *  points, nodes, and triangles.  Definitions:				*
 *									*
 *  Point:  A physical location in the mesh.  Each grid point 		*
 *	represents a single location at which the partial differential	*
 *	equations are solved.  Each physical point will have one or 	*
 *	more nodes associated with it, which represent the data at that	*
 *	point in each of the materials which may meet at it.		*
 *									*
 *  Nodes:  Each node represents the solution in a particular material	*
 *	at a physical point.  A given node may represent the Silicon	*
 *	values at a point with coordintes (0,0), an entirely different	*
 *	node will represent the Silicon Dioxide values at point (0,0).	*
 *									*
 *  Triangles:  Each triangle is in a single material and has a set of	*
 *	three physical points associated with its vertices.		*
 *									*
 ************************************************************************/

/*a structure to hold connectivity*/
struct list_str {
    short num;		/*the number in the list*/
    short all;		/*the amount allocated*/
    int *list;		/*the number in the list*/
};

/*a structure to hold connectivity*/
struct box_str {
    short num;		/*the number in the list*/
    short all;		/*the amount allocated*/
    float *list[MAXDIM];	/*the coordinates of each point in the list*/
};

/*define a structure of the physical point information*/
struct pt_str {
    float cord[MAXDIM];		/*the coordinate value of the point*/
    float cordo[MAXDIM];	/*the initial coordinates of the point*/
    float vel[MAXDIM];		/*the current velocity*/
    float ovel[MAXDIM];		/*the last velocity*/
    float spac;			/*local spacing*/
    int flags;			/*set of bits to indicate special conditions*/
    int nn;			/*the number of nodes at this point*/
    int nd[MAXMAT];		/*indices into the node structure*/
};
typedef struct pt_str pt_typ;

/*define a structure for the logical nodes*/
struct nd_str {
    double sol[MAXIMP];		/*the solution values for each stream*/
    int mater;			/*the material index for this node*/
    double step;		/*the step the node was added at*/
    double time;		/*the time of the step it was added*/
    struct list_str ltri;	/*the list of triangles*/
    struct list_str edg;	/*the list of edges*/
    int pt;			/*the index of the physical point*/
    int flags;			/*set of bits to indicate special conditions*/
};
typedef struct nd_str nd_typ;

EXTERN int nvrt;		/*actual number of vertices*/
EXTERN int nedg;		/*actual number of edges*/

/*define the structure to contain the element information*/
struct tri_str {
    int nd[MAXVRT];		/*the index to the nodes in the element*/
    int nb[MAXSID];		/*the neighbor element*/
    int edg[MAXSID];		/*the edges for each triangle*/
    float ehed[MAXSID];	/*the coupling coefficients*/
    float d[MAXSID];		/*the length of the sides - boundary conditions*/
    float earea[MAXVRT];	/*the area of the element for each node*/
    float sig[MAXVRT];	/*the per-element stresses*/
    int regnum;		/*the region number of the element*/
    int fath;		/*the father of this element*/
    int son;		/*the first son of this element*/
    int level;		/*the height in the tree (redundant but fun)*/
    int flags;		/*set of bits to indicate special conditions*/
};
typedef struct tri_str tri_typ;

/*define a structure for region data*/
struct edg_str {
    int nd[2];		/*nodes at ends of the edge*/
    struct list_str ele;		/*element list*/
    struct list_str skel;		/*skeleton list*/
    int flags;		/*data base flags*/
    float cpl;		/*the coupling coefficient along the edge*/
    float len;		/*the length of the edge*/
    int u, l;		/*the location of lower and upper couplings*/
};
typedef struct edg_str edg_typ;

/*define a structure for region data*/
struct reg_str {
    int mater;		/*material type of region*/
    int flags;		/*data base flags*/
    double thick;	/*thickness of deposited layer*/
    struct list_str edg;
    struct list_str tri;
    struct list_str fac;
    struct list_str nd;
    int sreg;		/*the skeleton index*/
    };
typedef struct reg_str reg_typ;

#define EC(X) if (err = X) return(err);

EXTERN struct pt_str  **pt;
EXTERN struct nd_str **nd;
EXTERN struct tri_str **tri;
EXTERN struct edg_str **edg;
EXTERN struct reg_str *reg[MAXREG];

extern float area_tri();

/*should be in the geom stuff*/
EXTERN int nreg;	/*the number of regions*/
EXTERN int ned;		/*the number of edges*/
EXTERN int ne;		/*number of elements*/
EXTERN int np;		/*number of points*/
EXTERN int nn;		/*the number of nodes*/

/*the step number*/
EXTERN int process_step;
