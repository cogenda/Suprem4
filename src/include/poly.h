#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

#define MAXPOLY 5 
#define MAXGRAIN 100

struct poly_node {
int ndtyp;          /* 0: single node grain 1: grain boundary node 2: node in grain */ 
int alive;       /* 0: dead 1:alive */
int npt;        /* number of pointe */
int nnd;            /* number of node */ 
float mf;
float ff;
float sol[MAXIMP];
float gs;           /* initial grain size before grain growth */
float delgs;
int nxy;
int xy[10];
float dxy[10];
struct poly_node *nnp; /* next node pointer */
} *pnp;

struct grain {
int ngn;        /* number of grain */
int region;        /* poly region number involved */
float position[MAXDIM];   /* center of grain ; X , Y (2-D) */
float gs;          /* grain size */ 
struct poly_node *gn; /*  1st node in grain */
struct poly_node *gb; /* boundary node first pointer */ 
struct grain *grain; /* next grain pointer */
} *grain;

struct poly_str {
int n_poly;          /* poly_region number from the outside */
int n_reg;
float thick;        /* poly_region thickness */  
float avgs;         /* average grain size */
float avgs2;
float area;         /* polysilicon layer area */
double ncg[MAXIMP];       /* net impurity concentration ; As P Sb B */
double ncgw[MAXIMP];      /* net impurity concentration considering segregation */
double acg[MAXIMP];       /* active impurity concentration ; As P Sb B */
int nspace;
int ndiv;
int status;  /* 0: amorphous 1:slightly doped 2: heavily doped */ 
int ngrain;         /* number of grain included */
int nmgrain;
struct poly_node *gp;
struct grain *fgrn;   /* 1st grain pointer */
struct grain *lgrn;   /* last grain */
} poly[MAXPOLY];

EXTERN int n_poly;  /* present # of total poly region */
EXTERN int n_grain;  /* present # of total grain */ 
EXTERN int n_gnnd;
#define grain_growing (n_poly > 1)

EXTERN int npoly;   /* poly region # being considered now */ 
EXTERN int ngrain;  /* grain # being considered now */

typedef struct grain grain_typ;
typedef struct poly_node gnnd_typ; 
EXTERN gnnd_typ *lgp;

EXTERN float grain_grid;
EXTERN float grain_gdt;

EXTERN float ent_heat[4][3]; 
 /*      2.7500E+00 , -4.4000E-01 , 2.6400E+15,  */
 /*      1.9000E+00 , -4.5200E-01 , 2.6400E+15,  */
 /*      0.0000E+00 ,  0.0000E+00 , 2.6400E+15,  */
 /*      2.0000E-01 , -3.8000E-01 , 2.6400E+15   */
 /*     };  */
#define entrop(A) ent_heat[A][0];
#define heat(A) ent_heat[A][1];
#define Qsite(A) ent_heat[A][2];
