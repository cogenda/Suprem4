h18571
s 00000/00000/00032
d D 2.1 93/02/03 15:13:29 suprem45 2 1
c Initial IV.GS release
e
s 00032/00000/00000
d D 1.1 93/02/03 15:12:11 suprem45 1 0
c date and time created 93/02/03 15:12:11 by suprem45
e
u
U
f e 0
t
T
I 1
/*
 * regrid.h 5.1   7/3/91 08:55:20
 * Storage for grid refinement.
 * During tree-structured regrid, a new copy of the whole 
 * point-node-tritree structure
 * is built, and when finished the old one is free'd and global arrays
 * pointed at the new one.
 *
 * Also has stuff for moving grid update
 */

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

/* Describe a polygon by a linked list */
struct polyel {
    int nd;		/* Node number */
    int nb;		/* Neighbor element on clockwise side of that node */
    int nx;		/* Which neighbor of that element needs to be fixed */
    struct polyel *prev, *next;
};

#define twhich(T,N) ((tri[T]->nb[0] == (N) ? 0 : \
		     (tri[T]->nb[1] == (N) ? 1 : \
		     (tri[T]->nb[2] == (N) ? 2 : NO_TRIANGLE))))
#define nwhich(T,N) ((tri[T]->nd[0] == (N) ? 0 : \
		     (tri[T]->nd[1] == (N) ? 1 : \
		     (tri[T]->nd[2] == (N) ? 2 : NO_TRIANGLE))))

E 1
