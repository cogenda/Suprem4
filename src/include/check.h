/*************************************************************************
 *									 *
 *   Original : MEL         Stanford University        Sept, 1984	 *
 *									 *
 *     Copyright c 1984 The board of trustees of the Leland Stanford 	 *
 *                      Junior University. All rights reserved.		 *
 *     This subroutine may not be used outside of the SUPREM4 computer	 *
 *     program without the prior written consent of Stanford University. *
 *									 *
 *************************************************************************/
/*   check.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:54:56 */

/************************************************************************
 *									*
 *	check.h - this file contains common definitions for the 	*
 *  parameter parser.  							*
 *									*
 ************************************************************************/

#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

#define NUMPAR 100	/*the maximum number of parameters for a command*/

/*define set values for data types, also used for interactive routines*/
/*flags defined here to be used in type var as well*/
#define INDEX 0x003F
#define REAL 0x0040
#define INT  0x0080
#define STR  0x0100
#define BOOL 0x0200
#define COMM 0x0400
#define CHOICE 0x1000
#define SPECIFIED 0x0800
#define MASK 0x083F

/*define types used in the boolean trees*/
#define OPER 1
#define CONSTREAL 2
#define PARVAL 3

/*define a type for the data in a table field*/
typedef union {
    int ival;
    float dval;
    char *sval;
    } val_str;

/*define the boolean expression structure*/
struct bool_exp {
    val_str value;		/*the value at this node*/
    int type;			/*the type of value that was*/
    struct bool_exp *right;	/*the right hand son*/
    struct bool_exp *left;	/*the left hand son*/
    };

/*structure to store away an individual parameter*/
struct par_str {
    char name[12];		/*the parameter name*/
    char *units;		/*the units to specify the parameter in*/
    val_str def;		/*the parameter default value*/
    val_str value;		/*the parameter value*/
    struct bool_exp *bexp;	/*the parameter legality function*/
    int type;			/*type of the parameter*/
    char *err_msg;		/*pointer to the error message at this node*/
    struct par_str **param;	/*list of sub parameters*/
    };

EXTERN struct par_str *cards[NUMPAR];
