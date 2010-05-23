%{

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
/*   lexical.l                Version 4.1     */
/*   Last Modification : 8/18/89  16:35:44 */


#undef input
#define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(stdin))==10?(yylineno++,yytchar):yytchar)==EOF?1:yytchar)


%}

%a 6000
%o 4000
%Start bool init

D	[0-9]
E	[EeDd][+-]?{D}+
SIGN    ([+-]?)
W	[ \t\n]
ESC	\033
EndOF	{ESC}{ESC}
L	[a-zA-Z]
V	[/a-zA-Z][%a-zA-Z0-9"."_]*
STR	[a-zA-Z][%a-zA-Z0-9"."_]*
S       [a-zA-Z0-9". "_]*
DEL     [,.; ]
STRDEL  [\'\"]

%%

\{	{
#ifdef DEBUG
	printf("returning begin group char\n");
#endif
	BEGIN 0;
	return( BG_GRP );
	}

(\})|('\001')|(end)	{
#ifdef DEBUG
	printf("returning end group char\n");
#endif
	BEGIN 0;
	return( END_GRP );
	}

{W}+	{
	/*do nothing*/
	}

\#[^\n]*	{
#ifdef DEBUG
	printf("returning comment\n");
#endif
	return(COMMENT);
	}

;	{
#ifdef DEBUG
	printf("returning semi colon\n");
#endif
	BEGIN 0;
	return(EOL);
	}

card	{
#ifdef DEBUG
	printf("returning card id\n");
#endif
	BEGIN 0;
	return(CARD);
	}

(integer|int) {
#ifdef DEBUG
	printf("returning integer type parameter\n");
#endif
	BEGIN 0;
	yylval.ival = INT;
	return(PARTYPE);
	}

switch {
#ifdef DEBUG
	printf("returning choice type parameter\n");
#endif
	BEGIN 0;
	yylval.ival = CHOICE;
	return(PARTYPE);
	}

float	{
#ifdef DEBUG
	printf("returning real number parameter type\n");
#endif
	BEGIN 0;
	yylval.ival = REAL;
	return(PARTYPE);
	}

string  {
#ifdef DEBUG
	printf("returning string parameter type\n");
#endif
	BEGIN 0;
	yylval.ival = STR;
	return(PARTYPE);
	}

boolean {
#ifdef DEBUG
	printf("returning boolean parameter type\n");
#endif
	BEGIN 0;
	yylval.ival = BOOL;
	return(PARTYPE);
	}

numb({W}*)=({W}*)[0-9]*      {
	char *s;

	/*skip past equal*/
	for(s = yytext; ! isdigit( *s ); s++ );

	/*read in number*/
	sscanf(s, "%d", &yylval.ival);
#ifdef DEBUG
	printf("the number return is %d\n", yylval.ival);
#endif
	BEGIN 0;
	return( NUMB );
	}

units({W}*)=({W}*)\"[^"]*\"	{
	char *s;
	/*eat this to get the string portion*/
	s = yytext;
	s += 5;
	while( isspace( *s ) || ( *s == '=') ) s++;
	/*advance past " char*/
	s++;
	yytext[yyleng-1] = '\0';
	yylval.sval = (char *)malloc( strlen(s) + 1 );
	strcpy(yylval.sval, s);
#ifdef DEBUG
	printf("units return is %s\n", yylval.sval);
#endif
	BEGIN 0;
	return(UNITS);
	}

message({W}*)=({W}*)\"[^"]*\"	{
	char *s;
	s = yytext;
	s += 7;
	while( isspace( *s ) || ( *s == '=') ) s++;
	/*advance past " char*/
	s++;
	yytext[yyleng-1] = '\0';
	yylval.sval = (char *)malloc( strlen(s) + 1 );
	strcpy(yylval.sval, s);
#ifdef DEBUG
	printf("error message return is %s\n", yylval.sval);
#endif
	BEGIN 0;
	return(ERR_MSG);
	}

{SIGN}{D}+	{
	sscanf(yytext, "%d", &yylval.ival);
#ifdef DEBUG
	printf("integer value found %d\n", yylval.ival);
#endif
	return(INTEGER);
	}

(on?)|(tr?|tru?|true?) {
#ifdef DEBUG
	printf("boolean true value found\n");
#endif
	yylval.ival = TRUE;
	return(INTEGER);
	}

(of?|off?)|(fa?|fal?|fals?|false?)	{
#ifdef DEBUG
	printf("boolean false value found\n");
#endif
	yylval.ival = FALSE;
	return(INTEGER);
	}

{SIGN}{D}+"."{D}*({E})?	|
{SIGN}{D}*"."{D}+({E})?	|
{SIGN}{D}+({E})		{
	    sscanf(yytext, "%f", &yylval.dval);
#ifdef DEBUG
	    printf("real number return %f\n", yylval.dval);
#endif
	    return(RCONST);
	    }

error({W}*)=	{
	/*send into boolean mode, ignore*/
#ifdef DEBUG
	printf("returning start of boolean expression\n");
#endif
	BEGIN bool;
	return(BOOLSTART);
	}

<init>=	{
#ifdef DEBUG
	printf("returning equal sign in default mode\n");
#endif
	return('=');
	}

<bool>[()]	{
#ifdef DEBUG
	printf("returning paran %c\n", yytext[0]);
#endif
	return(yytext[0]);
	}

<bool>\> 	{
#ifdef DEBUG
	printf("returning greater than\n");
#endif
	return(GRT);
	}

<bool>\< 	{
#ifdef DEBUG
	printf("returning less than\n");
#endif
	return(LES);
	}

<bool>\>\= 	{
#ifdef DEBUG
	printf("returning greater than or equal\n");
#endif
	return(GRTE);
	}

<bool>\<\= 	{
#ifdef DEBUG
	printf("returning less than or equal\n");
#endif
	return(LESE);
	}

<bool>\|\| 	{
#ifdef DEBUG
	printf("returning or\n");
#endif
	return(OR);
	}

<bool>\&\& 	{
#ifdef DEBUG
	printf("returning and\n");
#endif
	return(AND);
	}

<bool>\= 	{
#ifdef DEBUG
	printf("returning equal\n");
#endif
	return(EQU);
	}

<bool>\!\= 	{
#ifdef DEBUG
	printf("returning not equal\n");
#endif
	return(NOTEQU);
	}

<bool>\! 	{
#ifdef DEBUG
	printf("returning not\n");
#endif
	return(NOT);
	}

<bool>\+ 	{
#ifdef DEBUG
	printf("returning plus\n");
#endif
	return(PLUS);
	}

<bool>\* 	{
#ifdef DEBUG
	printf("returning times\n");
#endif
	return(TIMES);
	}

<bool>{STR} 	{
#ifdef DEBUG
	printf("returning par name %s\n", yytext);
#endif
	yylval.sval = (char *)malloc( strlen(yytext) + 1 );
	strcpy(yylval.sval, yytext);
	return(PARNAME);
	}


{V}	{
#ifdef DEBUG
	printf("returning Name %s\n", yytext);
#endif
	yylval.sval = (char *)malloc( yyleng + 1 );
	strcpy(yylval.sval, yytext);
	BEGIN init;
	return(NAME);
	}

