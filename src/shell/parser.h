
/* A Bison parser, made by GNU Bison 2.4.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PARAMETER = 258,
     COMMAND = 259,
     NAME = 260,
     LIST = 261,
     SOURCE = 262,
     EOL = 263,
     ENDFILE = 264,
     QUIT = 265,
     BACK = 266,
     REDIRECT = 267,
     BANG = 268,
     HELP = 269,
     BG_GRP = 270,
     END_GRP = 271,
     DELIMIT = 272,
     DEFINE = 273,
     COMMENT = 274,
     UNDEF = 275,
     SET = 276,
     UNSET = 277,
     FOR = 278,
     END = 279,
     CHDIR = 280
   };
#endif
/* Tokens.  */
#define PARAMETER 258
#define COMMAND 259
#define NAME 260
#define LIST 261
#define SOURCE 262
#define EOL 263
#define ENDFILE 264
#define QUIT 265
#define BACK 266
#define REDIRECT 267
#define BANG 268
#define HELP 269
#define BG_GRP 270
#define END_GRP 271
#define DELIMIT 272
#define DEFINE 273
#define COMMENT 274
#define UNDEF 275
#define SET 276
#define UNSET 277
#define FOR 278
#define END 279
#define CHDIR 280




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 56 "shell/parser.y"

    int ival;
    char *sval;
    


/* Line 1676 of yacc.c  */
#line 109 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


