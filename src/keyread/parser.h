
typedef union   {
    int ival;
    float dval;
    char *sval;
    struct bool_exp *bval;
    } YYSTYPE;
extern YYSTYPE yylval;
# define CARD 257
# define BG_GRP 258
# define END_GRP 259
# define PARTYPE 260
# define EOL 261
# define BOOLSTART 262
# define GRT 263
# define LES 264
# define GRTE 265
# define LESE 266
# define OR 267
# define AND 268
# define EQU 269
# define NOTEQU 270
# define NOT 271
# define PLUS 272
# define TIMES 273
# define RCONST 274
# define STRING 275
# define INTEGER 276
# define NUMB 277
# define NAME 278
# define UNITS 279
# define ERR_MSG 280
# define COMMENT 281
# define PARNAME 282
