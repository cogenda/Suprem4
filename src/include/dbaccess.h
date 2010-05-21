/*************************************************************************
 *									 *
 *     Copyright c 1991 The board of regents of the University of 	 *
 *                      Florida.  All rights reserved.			 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of the University of 	 *
 *     Florida.								 *
 *									 *
 *									 *
 *************************************************************************/
/*   dbaccess.h                Version 5.1     */
/*   Last Modification : 7/3/91 08:54:59 */

/************************************************************************
 *									*
 *	This file contains useful definitions of macros for data base	*
 *  access.								*
 *									*
 ************************************************************************/

/*big useful macros*/
#define set(A,B)	A->flags |= B
#define clr(A,B)	A->flags &= ~B
#define ask(A,B)	(A->flags & B)

/*global dirty variables*/
EXTERN int bc_dirty;		/*boundary codes for triangles*/
EXTERN int tripts;		/*some tris point to points*/
EXTERN int geom_dirty;
EXTERN int clkws_dirty;
EXTERN int nd2tri_dirty;
EXTERN int neigh_dirty;
EXTERN int edge_dirty;
EXTERN int nd2edg_dirty;
EXTERN int reg_dirty;
EXTERN int need_waste;

/*data base flag for everybody*/
#define KILL_ME     0x8000
#define KILL_LATER  0x4000

/*Data Base Flags for nodes*/
#define ND2EDG	0x0001
#define ND2TRI	0x0002
#define REG_BND	0x0004

/*Data Base Flags for points*/
#define SURFACE 0x0001
#define BACKSID 0x0002
#define SKELP 0x0004

/*Data Base Flags for triangles*/
#define TRIPTS	0x0001
#define GEOMDN	0x0002
#define CLKWS 	0x0004
#define NEIGHB  0x0008
#define CHPFIX  0x0010

/*Data Base Flags for edges*/
#define REGS	0x0001		/*edge points to skeleton regions*/
/*use geomdn for triangles*/
#define ESURF   0x0004
#define EBACK   0x0008
#define MARKED  0x0010

/*Data Base Flags for regions*/
#define EDG	0x0001
#define EXPOS   0x0002	/*is this region exposed to the surface*/
#define SKEL	0x0004	/* this is a skeleton region, unfilled*/
#define ETCHED  0x0008	/* don't etch regions multiple times*/

/*the code to represent non-existant sons or fathers in the element tree*/
#define NO_TRIANGLE -1

/*the code used by element to indicate a neumann edge*/
#define BC_OFFSET -1024
#define EXPOSED BC_OFFSET+2
#define BACKEDG BC_OFFSET+1


#define DBFUNC
#ifndef GEOMDB
#include "geom.h"
#endif

/*DATA BASE Status Variables*/
EXTERN int dbi;

/*POINT related functions*/
/*no arguments, returns error message*/
extern char *alloc_pt();	

/* number of cords, cord array, return created p number */
extern int mk_pt();		

#define num_nd(A)	pt[A]->nn
#define nd_pt(A,B)	pt[A]->nd[B]
#define cord_arr(A)	pt[A]->cord
#define cordinate(A,B)	pt[A]->cord[B]
#define xcord(A)	pt[A]->cord[0]
#define ycord(A)	pt[A]->cord[1]
#define zcord(A)	pt[A]->cord[2]
#define set_cord(A,B,C)	pt[A]->cord[B] = C
#define depth_cord(A)	pt[A]->cord[(mode==ONED)?0:1]
#define vel_arr(A)	pt[A]->vel
#define velocity(A,B)	pt[A]->vel[B]
#define xvel(A)	pt[A]->vel[0]
#define yvel(A)	pt[A]->vel[1]
#define zvel(A)	pt[A]->vel[2]

/*
 * These next two functions need to be tied tighter to the edge structure
 * an overall analysis of how these are used and where should improve
 * performance
 */
/*this returns the length between two points*/
extern double dist();
/*this return the total coupling between two nodes*/
extern float gimme_ehed();

#define add_nd_pt(A,B) pt[A]->nd[(pt[A]->nn)++] = B

/*is the point on the surface??*/
#define set_surf(A) pt[A]->flags |= SURFACE
#define clr_surf(A) pt[A]->flags &= ~SURFACE
#define is_surf(A) pt[A]->flags & SURFACE

/*point on the backside*/
#define set_back(A) pt[A]->flags |= BACKSID
#define clr_back(A) pt[A]->flags &= ~BACKSID
#define is_back(A) pt[A]->flags & BACKSID
#define mk_surf(A) {set_surf(A);if(node_mat(nd_pt(A,0),GAS)==-1)(void)mk_nd(A,GAS);}

/*returns the number of neighbors to a point*/
extern int num_neigh_pt();
/*returns the specific neighbor of a point*/
extern int neigh_pt();

/*spacing operations*/
#define get_space(A)	pt[A]->spac
#define set_space(A,B)	pt[A]->spac = B

#define dead_pt(N)  ask(pt[N], KILL_ME)
#define fing_pt(N)  set(pt[N], KILL_ME); need_waste = TRUE

#define pdist(A,B)	dist( cord_arr(A), cord_arr(B) )

/*NODE related functions*/
/*no arguments, returns error message*/
extern char *alloc_nd();	

/*parent point, material, returns created node number*/
extern int mk_nd();		

#define pt_nd(A)	nd[A]->pt
#define mat_nd(A)	nd[A]->mater

#define num_tri_nd(A)	nd[A]->ltri.num

#define tri_nd(A,B)	nd[A]->ltri.list[B]
#define set_tri_nd(A,B,C)	nd[A]->ltri.list[B] = C
#define add_tri_nd(A,B)		add_list( &(nd[A]->ltri), B )
#define sub_tri_nd(A,B)		sub_list( &(nd[A]->ltri), B );

/*node and triangle*/

#define num_sol(A)	n_imp
#define sol_nd(A,B)	nd[A]->sol[B]
#define set_sol_nd(A,B,C)	nd[A]->sol[B] = C

#define copy_sol(A,B) for(dbi=0;dbi<n_imp;dbi++) nd[B]->sol[dbi]=nd[A]->sol[dbi];

#define num_edge_nd(A)	nd[A]->edg.num
#define edge_nd(A,B)	nd[A]->edg.list[B]
#define add_edge_nd(A,B)	add_list(&(nd[A]->edg), B)
#define set_edge_nd(A,B,C)	nd[A]->edg.list[B] = C
#define sub_edge_nd(A,B)	sub_list(&(nd[A]->edg), B)

/*these two are messy, we need a better way to do them*/
/*returns the number of neighbors to a node*/
extern int num_neigh_nd();
/*returns the specific neighbor of a node*/
extern int neigh_nd();

#define dead_nd(N)  ask(nd[N], KILL_ME)
#define fing_nd(N)  set(nd[N], KILL_ME); need_waste = TRUE

/*returns the distance between two nodes*/
#define ndist(A,B)	dist( cord_arr(pt_nd(A)), cord_arr(pt_nd(B)) )



/*TRIANGLE related functions*/
extern char *alloc_tri();	/*no arguments*/
/*num vert, verts, num edges, edges, region, pts or nds?, return tri */
extern int mk_ele();

/*num vert, verts, region, return tri */
extern int mk_ele_nd();
extern int mk_ele_pt();

/*for looping....*/
#define done_tri(A)	A < ne
#define next_tri(A)	for(A++; (A < ne) && (tri[A]->son != NO_TRIANGLE) && \
				 (dead_tri(A)); A++)

/*vertex functions*/
#define num_vert(A)		nvrt
#define vert_tri(A,B)		tri[A]->nd[B]
#define set_vert_tri(A,B,C)	tri[A]->nd[B] = C

/*the number and type of faces*/
#define num_face(A)		nvrt
#define num_nd_fc(A,B)		((mode==ONED)?1:2)
#define nd_face_ele(A,B,C)	tri[A]->nd[(B+(C+1))%nvrt]
#define neigh_fc(A,B)		tri[A]->nb[B]

#define set_face_exp(A,B)	tri[A]->nb[B] = EXPOSED
#define is_face_exp(A,B)	tri[A]->nb[B] == EXPOSED

#define set_face_bck(A,B)	tri[A]->nb[B] = BACKEDG
#define is_face_bck(A,B)	tri[A]->nb[B] == BACKEDG

/*some neighbor stuff across the face*/
#define neigh_t(A,B)	tri[A]->nb[B]
#define set_neigh_t(A,B,C)	tri[A]->nb[B] = C


/*the number and type of edges*/
#define num_edge(A)		((mode==ONED)?1:3)
#define edg_ele(A,B)		tri[A]->edg[B]
#define set_edg_ele(A,B,C)	tri[A]->edg[B] = C

/*material stuff*/
#define reg_tri(A)		tri[A]->regnum
#define mat_tri(A)		reg[ tri[A]->regnum ]->mater
#define same_mat(A,B)	((A >= 0) && (B >= 0) && (mat_tri(A) == mat_tri(B)))

#define exists(X) ((X) != NO_TRIANGLE)
#define root(X) ((X)->fath == NO_TRIANGLE)
#define green(X) ((X)->fath < 0 && (X)->fath != NO_TRIANGLE)
#define reglr(X) (!(green(X)))
#define leaf(X) ((X)->son == NO_TRIANGLE)
#define father(X) (tri[X]->fath)
#define offspr(X) (tri[X]->son)
#define set_father(X,B) tri[X]->fath = B
#define set_offspr(X,B) tri[X]->son = B

/*coupling stuff*/
extern float ehed_tri();
extern float vol_ele();

/*this takes three coordinate pairs*/
extern float area_tri();

#define dead_tri(N)  ask(tri[N], KILL_ME)
#define fing_tri(N)  set(tri[N], KILL_ME); need_waste = TRUE

/*EDGE STUFF*/
/*n1, n2 returns edge number*/
extern mk_edg();
#define nd_edg(A,B)		edg[A]->nd[B]
#define pt_edg(A,B)		pt_nd(edg[A]->nd[B])
#define num_tri_edg(A)		edg[A]->ele.num
#define num_skel_edg(A)		edg[A]->skel.num
#define tri_edg(A,B)		edg[A]->ele.list[B]
#define ele_edg(A,B)		edg[A]->ele.list[B]
#define skel_edg(A,B)		edg[A]->skel.list[B]
#define cpl_edg(A)		edg[A]->cpl
#define len_edg(A)		edg[A]->len
#define set_cpl_edg(A,B,C)	edg[A]->cpl = C
#define set_len_edg(A,B,C)	edg[A]->len = C
#define num_ele_edg(A)		edg[A]->ele.num
#define add_ele_edg(A,B)	add_list(&(edg[A]->ele), B)
#define sub_ele_edg(A,B)	sub_list(&(edg[A]->ele), B)
#define add_skel_edg(A,B)	add_list(&(edg[A]->skel), B)
#define sub_skel_edg(A,B)	sub_list(&(edg[A]->skel), B)
#define is_border(A)	((num_tri_edg(A) == 1) || \
		(reg_tri(tri_edg(e,0)) != (reg_tri(tri_edg(e,1)))))

#define dead_edg(N)  ask(edg[N], KILL_ME)
#define fing_edg(N)  set(edg[N], KILL_ME); need_waste = TRUE; edge_dirty = TRUE
extern double l_edge();

/*REGION STUFF*/
#define num_reg 	nreg
extern mk_reg();	
#define mat_reg(A)	reg[A]->mater

#define num_edg_reg(A)	reg[A]->edg.num
#define edg_reg(A,B)	reg[A]->edg.list[B]
#define add_edg_reg(A,B)	add_list(&(reg[A]->edg), B)
#define sub_edg_reg(A,B)	add_list(&(reg[A]->edg), B)

#define num_bnd_reg(A)	reg[A]->fac.num
#define add_bnd_reg(A,B,C)		add_list(&(reg[A]->fac), (B<<4)|(C&0x0F))
#define bnd_reg(A,B,T,F)	T = reg[A]->fac.list[B] >> 4; F = reg[A]->fac.list[B] & 0x0F

#define num_tri_reg(A)	reg[A]->tri.num
#define tri_reg(A,B)	reg[A]->tri.list[B]
#define add_tri_reg(A,B)	add_list(&(reg[A]->tri), B)
#define sub_tri_reg(A,B)	sub_list(&(reg[A]->tri), B)

#define num_nd_reg(A)	reg[A]->nd.num
#define nd_reg(A,B)	reg[A]->nd.list[B]
#define add_nd_reg(A,B)	add_list(&(reg[A]->nd), B)
#define sub_nd_reg(A,B)	sub_list(&(reg[A]->nd), B)

#define thick_reg(A)	reg[A]->thick

#define dead_reg(N)  ask(reg[N], KILL_ME)
#define fing_reg(N)  set(reg[N], KILL_ME); need_waste = TRUE
