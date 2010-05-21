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

