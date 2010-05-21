h58536
s 00000/00000/00035
d D 2.1 93/02/03 15:23:16 suprem45 2 1
c Initial IV.GS release
e
s 00035/00000/00000
d D 1.1 93/02/03 15:22:15 suprem45 1 0
c date and time created 93/02/03 15:22:15 by suprem45
e
u
U
f e 0
t
T
I 1
/*
 * simpl.h
 *	Definitions for SIMPL cross-sectional profile.
 *
 * VERSION:
 *	Release 1, 10-15-1987
 *
 * Copyright (C) 1987  U. C. Berkeley SAMPLE Group
 */


/*
 * Profile information.
 */

#define	GRID_X_SIZE			150
#define	GRID_Y_SIZE			100

#define	NO_DOPING			0.0
#define	NO_GRID				-1.0

#define	DOPING_EXTREME			1e30
#define	PROFILE_BOUND			0.0


/*
 * Profile polygon structure.
 */

typedef struct simpl_polygon {
	char name[NAME_SIZE];		/* Polygon layer name. */
	struct float_path *path;	/* Polygon path. */
	struct simpl_polygon *next;	/* Pointer to next layer. */
} simplPolygon;

E 1
