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
/*   shell.h                Version 5.1     */
/*   Last Modification : 7/3/91  08:55:21 */


#ifdef STATIC_ALLOCATION_TIME
#define EXTERN
#else
#define EXTERN extern
#endif

/*typedef a pointer to a function, (it works, trust me)*/
typedef int (* PTR_FUNC)();

EXTERN char *prompt;

/*these variables are used for input processing*/
EXTERN char *buffer;	/*the input buffer pointer*/
EXTERN int bufptr;	/*the current input buffer pointer*/
EXTERN int buflen;	/*the current amount of space buffer is decalared to*/

/*these are used in input munging as well*/
EXTERN char *supbuf;	/*the input buffer pointer*/
EXTERN int supbpt;	/*the current input buffer pointer*/
EXTERN int supbln;	/*the current amount of space buffer is decalared to*/

EXTERN char *echo_buffer;	/*the input buffer pointer*/
EXTERN int echo_bufptr;		/*the current input buffer pointer*/
EXTERN int echo_buflen;		/*the current amount of space buffer is decalared to*/

/*return values for prompting*/
#define BACKGROUND 1
#define PROMPT 2


/*define types of parameters that may be recieved*/
#define IVAL 1
#define DVAL 2
#define SVAL 3

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/*declare a function to parse a string and return a stream*/
extern FILE *file_parse();

/*this file is the standard input path for all input*/
EXTERN FILE *in_file ;

/*this structure is for use in the macro preprocessor*/
struct macro_table {
    char *name;		/*the name of the macro*/
    char *args;		/*the argument list if any for this macro*/
    char *replace;	/*the replacement string*/
    struct macro_table *next;	/*line pointer*/
    };

EXTERN struct macro_table *macro;

/*set some performance vairables*/
EXTERN int noexecute;
EXTERN int interactive;
EXTERN int echoall;

/*this set of data is used in looping*/
/*store is a pointer to a pointer for storage mode on input*/
/*all data will be stored into this pointer*/
EXTERN char *store[10];
EXTERN int store_len[10];
EXTERN int store_ptr[10];
EXTERN int depth;

/*this structure and information is used to keep pointers to functions*/
struct command_table {
	char *name;
	PTR_FUNC func;
	int param;
	};

#define NUMCMD 50
extern struct command_table command[NUMCMD];

/*this table is used only with the fortran version*/
struct fort_str {
    int keyid;
    char *par;
    int back;
    char *rdir;
    int intr;
    struct fort_str *next;
};

/*pointers to the top and bottom of the command stack*/
EXTERN struct fort_str *fort_top, *fort_end;

/*file pointer for cpu file statistics*/
EXTERN FILE *cpufile;
