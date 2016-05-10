
# line 2 "freadin.y"
/*
  feradin.y --- yacc program for fread
  Copyright (C) 2000  A. Tamii
  Author:   A. Tamii
  Facility: Department of Physics, University of Tokyo
  Version 1.00 04-JUN-2000 by A. Tamii
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "freadin.h"
#include "histogram.h"

extern vrec_p slist;

# line 20 "freadin.y"
typedef union  {
	struct  vrec   *vrecp;
} YYSTYPE;
# define NAME 257
# define NAME2 258
# define NUM 259
# define TDEF 260
# define TH1 261
# define TH2 262
# define TCOM 263
# define STR 264
# define TUNDEF 265
# define TNODE 266
# define TUMIN 267
# define TFUNC 268
# define TARRAY 269
# define TELM 270
# define TLOR 271
# define TLAND 272
# define TEQ 273
# define TNEQ 274
# define TLE 275
# define TGE 276
# define UMINUS 277
# define UPLUS 278
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;
# define YYERRCODE 256

# line 123 "freadin.y"

extern int   line;
extern char  yytext[];

/* yyerror */
void yyerror(s)
		 char  *s;
{
	showerr("line %d: parse error at %s.\n", line, yytext);
}

/* new_vrecp --- create new variable table */
vrec_p new_vrecp()
{
	vrec_p  vp;
	vp = (vrec_p)malloc(sizeof(struct vrec));
	if(vp==(vrec_p)NULL){
		showerr("new_vrecp: malloc: %s", strerror(errno));
		exit(1);
	}
	vp->vtype = TUNDEF;
	vp->vref = 0;
	dr_set_nothing(&vp->dval);
	vp->sval = (char*)NULL;
	vp->arg1 = vp->arg2 = (vrec_p)NULL;
	vp->eval = EVAL_NONE;
	vp->neval = 0;
	return(vp);
}
		
/* free_vrecp --- free variable table */
void free_vrecp(v)
		 vrec_p v;
{
	vrec_p  v2;
	if(!v) return;
	if(v2=v->arg1){
		v->arg1 = (vrec_p)NULL;
		free_vrecp(v2);
	}
	if(v2=v->arg2){
		v->arg2 = (vrec_p)NULL;
		free_vrecp(v2);
	}
	if(v->sval){
		free(v->sval);
		v->sval = (char*)NULL;
	}
	free(v);
}
		
/* str_token --- get string of the token */
char *str_token(vtype)
		 int vtype;
{
	static char str[256];
	switch(vtype){
	case NUM:     return("NUM");
	case STR:     return("STR");
	case NAME:   	return("NAME");
	case NAME2:  	return("NAME2");
	case '!':     return("!");
	case '~':     return("~");
	case '|':     return("|");
	case '&':     return("&");
	case '<':     return("<");
	case '>':     return(">");
	case '+':     return("+");
	case '-':     return("-");
	case '*':     return("*");
	case '/':     return("/");
	case '=':     return("=");
	case TUMIN:   return("MINUS");
	case TLOR:    return("||");
	case TLAND:   return("&&");
	case TEQ:     return("==");
	case TNEQ:    return("!=");
	case TLE:     return("<=");
	case TGE:     return(">=");
	case TFUNC:   return("FUNCTION");
	case TNODE:   return("NODE");
	case TUNDEF:  return("UNDEFINED");
	case TARRAY:  return("ARRAY");
	case TELM:    return("ELEMENT");
	default:      return("UNKOWN");
	}
	return((char*)str);
}

/* str_vtype --- get string of the vrecp */
char *str_vtype(v)
		 vrec_p  v;
{
	static char str[256];
	switch(v->vtype){
	case NAME:
	case NAME2:
		if(v->sval){
			sprintf(str,"(%s)",v->sval);
			return(str);
		}
		else
			return(str_token(v->vtype));
	case TARRAY:
	case TFUNC:
		if(v->arg1->sval){
			sprintf(str,"(%s)",v->arg1->sval);
			return(str);
		}
		else
			return(str_token(v->vtype));
	case NUM:
		sprintf(str,"%5.3f",v->dval);
		return(str);
	default:
		return(str_token(v->vtype));
	}
}

/* add_node --- add a node to variable tree */
vrec_p add_node(ope, arg1, arg2, arg3)
		 int      ope;
		 vrec_p   arg1, arg2, arg3;
{
	vrec_p  vp;
#if DEBUG_YACC
	fprintf(stderr, "add_node: token: %s\n", str_token(ope));
#endif
	vp = new_vrecp();
	vp->vtype = TNODE;
	switch(ope){
	case '!':
	case '~':
	case TUMIN:
		if(arg1==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s\n", str_token(ope));
			break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		break;
	case '|':
	case '&':
	case '<':
	case '>':
	case '+':
	case '-':
	case '*':
	case '/':
	case TLOR:
	case TLAND:
	case TEQ:
	case TNEQ:
	case TLE:
	case TGE:
	case '=':
	case TELM:
		if(arg1==(vrec_p)NULL || arg2==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s\n", str_token(ope));
			break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		vp->arg2 = arg2;
		break;
	case TNODE:
		showerr("add_node: illegal token %s.", str_token(ope));
		break;
	case TUNDEF:
		showerr("add_node: illegal token %s.", str_token(ope));
		break;
	case TFUNC:
		if(arg1==(vrec_p)NULL || arg2==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s.", str_token(ope));
		  break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		vp->arg2 = arg2;
		break;
	case TARRAY:
		if(arg1==(vrec_p)NULL || arg2==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s.", str_token(ope));
		  break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		vp->arg2 = arg2;
		break;
	default:
		showerr("add_node: unknown token %d.\n", ope);
		break;
	}
	return(vp);
}
/*
Local Variables:
mode: C
tab-width: 2
End:
*/

static const yytabelem yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 44
# define YYLAST 454
static const yytabelem yyact[]={

    43,    18,    17,    93,    52,    50,    43,    51,    14,    53,
    52,    50,    43,    51,    13,    53,    52,    50,    16,    51,
   100,    53,    48,    82,    49,    62,    38,    87,    48,    54,
    49,     1,    19,    15,    48,    43,    49,     0,    84,    52,
    50,    43,    51,     8,    53,    52,    50,    83,    51,    43,
    53,     0,    65,    52,    50,    52,    51,    48,    53,    49,
    53,    92,    20,    48,     5,    49,     6,    89,     0,    52,
    50,    48,    51,    49,    53,    43,    61,     0,     0,    52,
    50,    30,    51,     2,    53,     7,    42,    48,    32,    49,
     0,    29,    42,    28,     0,     0,    43,    48,    42,    49,
    52,    50,    43,    51,     0,    53,    52,    50,     0,    51,
    43,    53,     0,    91,    52,    50,     0,    51,    48,    53,
    49,    42,     0,     0,    48,    98,    49,    42,     0,    30,
    63,     0,    48,    43,    49,    42,    32,    52,    50,    29,
    51,    28,    53,    22,    52,    50,     5,    51,     6,    53,
     9,    10,    11,    12,     0,    48,     0,    49,    30,     0,
     0,    42,    48,     0,    49,    32,     0,     0,    29,     0,
    28,    95,    52,    50,    31,    51,     0,    53,    24,     3,
     3,     0,    42,     0,     0,     0,     0,     0,    42,     0,
     0,     0,     0,     0,     0,     0,    42,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
     0,     0,    31,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    40,    41,    44,    45,    46,    47,    40,
    41,    44,    45,    46,    47,    40,    41,    44,    45,    46,
    47,    31,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    40,    41,
    44,    45,    46,    47,    40,    41,    44,    45,    46,    47,
     4,     0,    40,    41,    44,    45,    46,    47,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    44,    45,    46,    47,     0,    33,     0,    34,    40,    41,
    44,    45,    46,    47,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
    41,    44,    45,    46,    47,    21,    41,    44,    45,    46,
    47,     0,     0,     0,     0,    44,    45,    46,    47,     0,
     0,     0,     0,    33,    25,    34,     0,    36,     0,     0,
    23,    39,     4,     0,     0,    55,     0,     0,    44,    45,
    46,    47,    64,     0,     0,    67,     0,    46,    47,     0,
     0,     0,    33,     0,    34,    26,    35,     0,    37,     0,
     0,     0,     0,     0,    56,    57,    58,    59,    60,     0,
    88,     0,     0,    90,    66,     0,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
     0,     0,     0,     0,    96,     0,    97,    85,    86,     0,
     0,     0,     0,    99,     0,   101,     0,   102,   103,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,    94 };
static const yytabelem yypact[]={

    21,    21,    33,  -110, -1000,  -245,  -251,    23, -1000,  -239,
  -262,  -263, -1000, -1000, -1000, -1000,   -29,   103,   103,    96,
   125,   103,   125,   -65, -1000,   103,    58,   125,   125,   125,
   125,   125,   125,   -15, -1000,    37,   103,    11,   125,   103,
   125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
   125,   125,   125,   125,  -102,     3, -1000, -1000, -1000, -1000,
    -3,   125,   125,   -34,   103, -1000,   -26,   103,    64,    72,
    95,    27,   102,   102,   130,   130,   130,   130,    13,    13,
 -1000, -1000, -1000,   125, -1000,   -32,   -38,    48,   103, -1000,
   103, -1000, -1000, -1000,    58,   125, -1000,   103,  -105,   103,
 -1000,   103,   103, -1000 };
static const yytabelem yypgo[]={

     0,    31,    83,   365,   335,    29,   178 };
static const yytabelem yyr1[]={

     0,     1,     1,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     4,     4,     4,     4,     5,     5,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     6,     6,     6 };
static const yytabelem yyr2[]={

     0,     5,     7,    11,    17,    15,    21,    15,    17,    23,
    25,     5,     7,     9,     3,     3,     7,     3,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     5,     5,     5,     5,     7,     9,     9,     3,
     3,     3,     5,     5 };
static const yytabelem yychk[]={

 -1000,    -1,    -2,    -6,   259,    43,    45,    -2,    10,   260,
   261,   262,   263,   259,   259,    10,   257,   264,   264,    61,
    91,    -4,    40,   257,    -6,    -4,    -3,   123,    45,    43,
    33,   126,    40,   257,   259,    -3,    -4,    -3,    91,    -4,
   271,   272,   124,    38,   273,   274,   275,   276,    60,    62,
    43,    45,    42,    47,    -5,    -3,    -3,    -3,    -3,    -3,
    -3,    91,    40,    93,    -4,    41,    -3,    -4,    -3,    -3,
    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,    -3,
    -3,    -3,   125,    44,    41,    -3,    -3,    61,    -4,    93,
    -4,    -5,    93,    41,    -3,   123,    -4,    -4,    -5,    -4,
   125,    -4,    -4,    -4 };
static const yytabelem yydef[]={

     0,    -2,     0,     0,    41,     0,     0,     0,     1,     0,
     0,     0,    11,    42,    43,     2,     0,     0,     0,     0,
     0,     0,     0,    14,    15,     0,     3,     0,     0,     0,
     0,     0,     0,    39,    40,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    17,    32,    33,    34,    35,
     0,     0,     0,     0,     0,    12,     0,     0,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
    30,    31,     5,     0,    36,     0,     0,     0,     7,    13,
     0,    16,    37,    38,     4,     0,     8,     0,     0,     0,
     6,     0,     9,    10 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

yytoktype yytoks[] =
{
	"NAME",	257,
	"NAME2",	258,
	"NUM",	259,
	"TDEF",	260,
	"TH1",	261,
	"TH2",	262,
	"TCOM",	263,
	"STR",	264,
	"TUNDEF",	265,
	"TNODE",	266,
	"TUMIN",	267,
	"TFUNC",	268,
	"TARRAY",	269,
	"TELM",	270,
	"TLOR",	271,
	"TLAND",	272,
	"TEQ",	273,
	"TNEQ",	274,
	"TLE",	275,
	"TGE",	276,
	"=",	61,
	",",	44,
	"|",	124,
	"&",	38,
	"<",	60,
	">",	62,
	"+",	43,
	"-",	45,
	"*",	42,
	"/",	47,
	"UMINUS",	277,
	"UPLUS",	278,
	"!",	33,
	"~",	126,
	"-unknown-",	-1	/* ends search */
};

char * yyreds[] =
{
	"-no such reduction-",
      "statement_list : statement '\n'",
      "statement_list : statement_list statement '\n'",
      "statement : number TDEF NAME '=' expr",
      "statement : number TDEF NAME '[' expr ']' '=' expr",
      "statement : number TDEF NAME '=' '{' elmnts '}'",
      "statement : number TDEF NAME '[' expr ']' '=' '{' elmnts '}'",
      "statement : number TH1 STR var var var var",
      "statement : number TH1 STR var var var var var",
      "statement : number TH2 STR var var var var var var var var",
      "statement : number TH2 STR var var var var var var var var var",
      "statement : number TCOM",
      "var : '(' expr ')'",
      "var : NAME '[' expr ']'",
      "var : NAME",
      "var : number",
      "elmnts : expr ',' elmnts",
      "elmnts : expr",
      "expr : expr TLOR expr",
      "expr : expr TLAND expr",
      "expr : expr '|' expr",
      "expr : expr '&' expr",
      "expr : expr TEQ expr",
      "expr : expr TNEQ expr",
      "expr : expr TLE expr",
      "expr : expr TGE expr",
      "expr : expr '<' expr",
      "expr : expr '>' expr",
      "expr : expr '+' expr",
      "expr : expr '-' expr",
      "expr : expr '*' expr",
      "expr : expr '/' expr",
      "expr : '-' expr",
      "expr : '+' expr",
      "expr : '!' expr",
      "expr : '~' expr",
      "expr : '(' expr ')'",
      "expr : NAME '[' expr ']'",
      "expr : NAME '(' expr ')'",
      "expr : NAME",
      "expr : NUM",
      "number : NUM",
      "number : '+' NUM",
      "number : '-' NUM",
};
#endif /* YYDEBUG */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/* @(#)yaccpar	1.3  com/cmd/lang/yacc,3.1, 9/7/89 18:46:37 */
/*
** Skeleton parser driver for yacc output
*/

/*
** yacc user known macros and defines
*/
#ifdef YYSPLIT
#   define YYERROR	return(-2)
#else
#   define YYERROR	goto yyerrlab
#endif

#define YYACCEPT	return(0)
#define YYABORT		return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yychar >= 0 || ( yyr2[ yytmp ] >> 1 ) != 1 )\
	{\
		yyerror( "syntax error - cannot backup" );\
		goto yyerrlab;\
	}\
	yychar = newtoken;\
	yystate = *yyps;\
	yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yyerrflag)
#ifndef YYDEBUG
#	define YYDEBUG	1	/* make debugging available */
#endif

/*
** user known globals
*/
int yydebug;			/* set to 1 to get debugging */

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

#ifdef YYSPLIT
#   define YYSCODE { \
			extern int (*yyf[])(); \
			register int yyret; \
			if (yyf[yytmp]) \
			    if ((yyret=(*yyf[yytmp])()) == -2) \
				    goto yyerrlab; \
				else if (yyret>=0) return(yyret); \
		   }
#endif

/*
** local variables used by the parser
 * these should be static in order to support
 * multiple parsers in a single executable program. POSIX 1003.2-1993
 */
static YYSTYPE yyv[ YYMAXDEPTH ];	/* value stack */
static int yys[ YYMAXDEPTH ];		/* state stack */

static YYSTYPE *yypv;			/* top of value stack */
static YYSTYPE *yypvt;			/* top of value stack for $vars */
static int *yyps;			/* top of state stack */

static int yystate;			/* current state */
static int yytmp;			/* extra var (lasts between blocks) */

/*
** global variables used by the parser - renamed as a result of -p
*/
int yynerrs;			/* number of errors */
int yyerrflag;			/* error recovery flag */
int yychar;			/* current input token number */

/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register int *yy_ps;		/* top of state stack */
		register int yy_state;		/* current state */
		register int  yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#if YYDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yydebug )
		{
			register int yy_i;

			printf( "State %d, token ", yy_state );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ++yy_ps >= &yys[ YYMAXDEPTH ] )	/* room on stack? */
		{
			yyerror( "yacc stack overflow" );
			YYABORT;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#if YYDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yytmp = yychar < 0;
#endif
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		/* reached EOF */
#if YYDEBUG
		if ( yydebug && yytmp )
		{
			register int yy_i;

			printf( "Received token " );
			if ( yychar == 0 )
				printf( "end-of-file\n" );
			else if ( yychar < 0 )
				printf( "-none-\n" );
			else
			{
				for ( yy_i = 0; yytoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( yytoks[yy_i].t_val == yychar )
						break;
				}
				printf( "%s\n", yytoks[yy_i].t_name );
			}
		}
#endif /* YYDEBUG */
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	/*valid shift*/
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
#if YYDEBUG
			yytmp = yychar < 0;
#endif
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		/* reached EOF */
#if YYDEBUG
			if ( yydebug && yytmp )
			{
				register int yy_i;

				printf( "Received token " );
				if ( yychar == 0 )
					printf( "end-of-file\n" );
				else if ( yychar < 0 )
					printf( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						yytoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( yytoks[yy_i].t_val
							== yychar )
						{
							break;
						}
					}
					printf( "%s\n", yytoks[yy_i].t_name );
				}
			}
#endif /* YYDEBUG */
			/*
			** look through exception table
			*/
			{
				register const int *yyxi = yyexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yyerrflag )
			{
			case 0:		/* new error */
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
						yychk[yyact[yy_n]] == YYERRCODE)					{
						/*
						** simulate shift of "error"
						*/
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#if YYDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
					if ( yydebug )
						printf( _POP_, *yy_ps,
							yy_ps[-1] );
#	undef _POP_
#endif
					yy_ps--;
					yy_pv--;
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#if YYDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yydebug )
				{
					register int yy_i;

					printf( "Error recovery discards " );
					if ( yychar == 0 )
						printf( "token end-of-file\n" );
					else if ( yychar < 0 )
						printf( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							yytoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( yytoks[yy_i].t_val
								== yychar )
							{
								break;
							}
						}
						printf( "token %s\n",
							yytoks[yy_i].t_name );
					}
				}
#endif /* YYDEBUG */
				if ( yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yychar = -1;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#if YYDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yydebug )
			printf( "Reduce by (%d) \"%s\"\n",
				yy_n, yyreds[ yy_n ] );
#endif
		yytmp = yy_n;			/* value to switch over */
		yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If yyr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register int yy_len = yyr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	/* $$ = $1 */
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	/*
	** code supplied by user is placed in this switch
	*/

		switch(yytmp){

case 1:
# line 47 "freadin.y"
{} /*NOTREACHED*/ break;
case 2:
# line 48 "freadin.y"
{} /*NOTREACHED*/ break;
case 3:
# line 51 "freadin.y"
{
						 def(yypvt[-4].vrecp,yypvt[-2].vrecp,0,yypvt[-0].vrecp);
             } /*NOTREACHED*/ break;
case 4:
# line 54 "freadin.y"
{
						 def(yypvt[-7].vrecp,yypvt[-5].vrecp,yypvt[-3].vrecp,yypvt[-0].vrecp);
             } /*NOTREACHED*/ break;
case 5:
# line 57 "freadin.y"
{
						 def(yypvt[-6].vrecp,yypvt[-4].vrecp,0,yypvt[-1].vrecp);
             } /*NOTREACHED*/ break;
case 6:
# line 60 "freadin.y"
{
						 def(yypvt[-9].vrecp,yypvt[-7].vrecp,yypvt[-5].vrecp,yypvt[-1].vrecp);
             } /*NOTREACHED*/ break;
case 7:
# line 63 "freadin.y"
{
						 hist1(yypvt[-6].vrecp,yypvt[-4].vrecp,yypvt[-3].vrecp,yypvt[-2].vrecp,yypvt[-1].vrecp,yypvt[-0].vrecp,0);
             } /*NOTREACHED*/ break;
case 8:
# line 66 "freadin.y"
{
						 hist1(yypvt[-7].vrecp,yypvt[-5].vrecp,yypvt[-4].vrecp,yypvt[-3].vrecp,yypvt[-2].vrecp,yypvt[-1].vrecp,yypvt[-0].vrecp);
             } /*NOTREACHED*/ break;
case 9:
# line 69 "freadin.y"
{
						 hist2(yypvt[-10].vrecp,yypvt[-8].vrecp,yypvt[-7].vrecp,yypvt[-6].vrecp,yypvt[-5].vrecp,yypvt[-4].vrecp,yypvt[-3].vrecp,yypvt[-2].vrecp,yypvt[-1].vrecp,yypvt[-0].vrecp,0);
             } /*NOTREACHED*/ break;
case 10:
# line 72 "freadin.y"
{
						 hist2(yypvt[-11].vrecp,yypvt[-9].vrecp,yypvt[-8].vrecp,yypvt[-7].vrecp,yypvt[-6].vrecp,yypvt[-5].vrecp,yypvt[-4].vrecp,yypvt[-3].vrecp,yypvt[-2].vrecp,yypvt[-1].vrecp,yypvt[-0].vrecp);
             } /*NOTREACHED*/ break;
case 11:
# line 75 "freadin.y"
{com(yypvt[-1].vrecp);} /*NOTREACHED*/ break;
case 12:
# line 84 "freadin.y"
{ yyval.vrecp = yypvt[-1].vrecp; } /*NOTREACHED*/ break;
case 13:
# line 85 "freadin.y"
{ yyval.vrecp = add_node(TARRAY,yypvt[-3].vrecp,yypvt[-1].vrecp); } /*NOTREACHED*/ break;
case 14:
# line 86 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; yypvt[-0].vrecp->vtype = NAME2; } /*NOTREACHED*/ break;
case 15:
# line 87 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; } /*NOTREACHED*/ break;
case 16:
# line 90 "freadin.y"
{ yyval.vrecp = add_node(TELM,yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 17:
# line 91 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; } /*NOTREACHED*/ break;
case 18:
# line 94 "freadin.y"
{ yyval.vrecp = add_node(TLOR,yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 19:
# line 95 "freadin.y"
{ yyval.vrecp = add_node(TLAND,yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 20:
# line 96 "freadin.y"
{ yyval.vrecp = add_node('|',yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 21:
# line 97 "freadin.y"
{ yyval.vrecp = add_node('&',yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 22:
# line 98 "freadin.y"
{ yyval.vrecp = add_node(TEQ,yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 23:
# line 99 "freadin.y"
{ yyval.vrecp = add_node(TNEQ,yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 24:
# line 100 "freadin.y"
{ yyval.vrecp = add_node(TLE,yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 25:
# line 101 "freadin.y"
{ yyval.vrecp = add_node(TGE,yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 26:
# line 102 "freadin.y"
{ yyval.vrecp = add_node('<',yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 27:
# line 103 "freadin.y"
{ yyval.vrecp = add_node('>',yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 28:
# line 104 "freadin.y"
{ yyval.vrecp = add_node('+',yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 29:
# line 105 "freadin.y"
{ yyval.vrecp = add_node('-',yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 30:
# line 106 "freadin.y"
{ yyval.vrecp = add_node('*',yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 31:
# line 107 "freadin.y"
{ yyval.vrecp = add_node('/',yypvt[-2].vrecp,yypvt[-0].vrecp); } /*NOTREACHED*/ break;
case 32:
# line 108 "freadin.y"
{ yyval.vrecp = add_node(TUMIN,yypvt[-0].vrecp,0); } /*NOTREACHED*/ break;
case 33:
# line 109 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; } /*NOTREACHED*/ break;
case 34:
# line 110 "freadin.y"
{ yyval.vrecp = add_node('!',yypvt[-0].vrecp,0); } /*NOTREACHED*/ break;
case 35:
# line 111 "freadin.y"
{ yyval.vrecp = add_node('~',yypvt[-0].vrecp,0); } /*NOTREACHED*/ break;
case 36:
# line 112 "freadin.y"
{ yyval.vrecp = yypvt[-1].vrecp; } /*NOTREACHED*/ break;
case 37:
# line 113 "freadin.y"
{ yyval.vrecp = add_node(TARRAY,yypvt[-3].vrecp,yypvt[-1].vrecp); } /*NOTREACHED*/ break;
case 38:
# line 114 "freadin.y"
{ yyval.vrecp = add_node(TFUNC,yypvt[-3].vrecp,yypvt[-1].vrecp); } /*NOTREACHED*/ break;
case 39:
# line 115 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; } /*NOTREACHED*/ break;
case 40:
# line 116 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; } /*NOTREACHED*/ break;
case 41:
# line 119 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; } /*NOTREACHED*/ break;
case 42:
# line 120 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; } /*NOTREACHED*/ break;
case 43:
# line 121 "freadin.y"
{ yyval.vrecp = yypvt[-0].vrecp; yyval.vrecp->dval = -yyval.vrecp->dval; } /*NOTREACHED*/ break;
}


	goto yystack;		/* reset registers in driver code */
}
