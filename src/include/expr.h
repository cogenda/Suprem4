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
/*   expr.h                Version 5.1     */
/*   Last Modification : 7/3/91 08:55:03 */

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

typedef int (* PTR_LEXFUNC)();
typedef char * (* PTR_EVALFUNC)();


/*define a maximum depth for the expression parser*/
#define STACK_DEPTH 100

/*define token values for the different type of lexical values*/
#define  EXPR  	0x0001
#define  OP1   	0x0002		/* plus/minus */
#define  OP2   	0x0004		/* multiply/divide */
#define  FN    	0x0008		/* miscellaneous functions */
#define  VFN	0x0010		/* vector valued functions */
#define  LPAR  	0x0020		/* left parenthesis */
#define  RPAR  	0x0040		/* right parenthesis */
#define  RCONST 0x0080		/* real constant value */
#define  SOLVAL 0x0100		/* solution value, As, B, etc. */
#define  TOPSTK 0x0200		/* top of stack value */
#define  EOI    0x0400		/* end of input marker */
#define  OP3    0x0800		/* exponentiation */
#define  COM    0x1000		/* comma operator for two arg functions*/
#define  STRING 0x2000		/* strings for functional use */

/*define numbers for functions*/
#define LOG10	1
#define LOG	2
#define EXP	3
#define ERF	4
#define ERFC	5
#define ABS     6
#define SQRT    7
#define SIGN    8

/*define numbers for vector functions*/
#define DO_ACTIVE  1
#define SCALE   2
#define GRADX   3
#define GRADY   4
#define DATA    5

/*define numbers for solution values*/
#define TIM  -1
#define X    -2
#define Y    -3
#define OXY  -4
#define DOP  -6
#define CIS  -7
#define CVS  -8
#define Z    -10
#ifdef DEVICE
#define ECON -11
#define EVAL -12
#define QFP -13
#define QFN -14
#endif

/*type def a relationship for the value field*/
typedef union {
    float dval;
    int ival;
    char *sval;
    struct vec_str *bval;
    } plt_val;

/*define a streucture to hold everything in*/
struct vec_str {
    plt_val value;
    int type;
    struct vec_str *right;
    struct vec_str *left;
    };


/*declare a structure for parsing*/
struct tok_str {
    int type;
    plt_val value;
    };

/*declare the global stack of the values to be handled*/
EXTERN struct tok_str stack[STACK_DEPTH];

/*current stack pointer*/
EXTERN int st_pnt;

/*the next input token read*/
EXTERN struct tok_str input_token;

extern char *parse_expr(), *eval_real(), *eval_vec();
extern float sol_interp();

/*diffuse stores this away for use in the selection of variables*/
EXTERN double last_temp;
