%{
/*
  freadin.lex --- lex program for fread
  Copyright (C) 2000  A. Tamii
  Author:   A. Tamii
  Facility: Department of Physics, University of Tokyo
  Version 1.00 04-JUN-2000 by A. Tamii
*/
#include <string.h>
#include <math.h>
#include "freadin.tab.h"
#include "freadin.h"
#include "analyzer.h"
extern int line;
#undef input
#undef unput

#ifdef AIX
#undef YYLMAX
#define YYLMAX 1000
#endif

int finput(char *buf, int *result, int max_size);

#if defined(linux) || defined(Darwin)
#define YY_INPUT(buf,result,max_size)\
  { \
		int res; \
		finput((char*)buf,(int*)&res,(int)max_size);	\
		result = res;																	\
	}
#endif

fin_t fin;
extern FILE *findef;
extern FILE *fout;
static int  end_input=0;
char name[256];
int  i;
%}

%%


[#][^\n]*\n   {line++; return '\n';}

\/\/.*\n    {line++;}

[ \t] ;

[0-9]+ {
	vrec_p   vp;
#if DEBUG_LEX
	fprintf(stderr, "NUM: %s\n", yytext);
#endif
	yylval.vrecp = vp = new_vrecp();
  vp->vtype = NUM;
	vp->dval = atof(yytext);
	return NUM;
  }

[0-9]+\.[0-9]* {
	vrec_p   vp;
#if DEBUG_LEX
	fprintf(stderr, "NUM: %s\n", yytext);
#endif
	yylval.vrecp = vp = new_vrecp();
  vp->vtype = NUM;
	vp->dval = atof(yytext);
	return NUM;
  }

[0-9\.]+[eE][+-]?[0-9]+ {
	vrec_p   vp;
#if DEBUG_LEX
	fprintf(stderr, "NUM: %s\n", yytext);
#endif
	yylval.vrecp = vp = new_vrecp();
  vp->vtype = NUM;
	vp->dval = atof(yytext);
	return NUM;
  }

(0[xX][0-9a-fA-F]+) {
	vrec_p   vp;
#if DEBUG_LEX
	fprintf(stderr, "NUM: %s\n", yytext);
#endif
	yylval.vrecp = vp = new_vrecp();
  vp->vtype = NUM;
	vp->dval = (double)strtol(yytext,(char**)NULL,16);
	return NUM;
  }

(0o[0-7]+) {
	vrec_p   vp;
#if DEBUG_LEX
	fprintf(stderr, "NUM: %s\n", yytext);
#endif
	yylval.vrecp = vp = new_vrecp();
  vp->vtype = NUM;
	vp->dval = (double)strtol(yytext,(char**)NULL,8);
	return NUM;
  }

(0[bB][01]+) {
	vrec_p   vp;
#if DEBUG_LEX
	fprintf(stderr, "NUM: %s\n", yytext);
#endif
	yylval.vrecp = vp = new_vrecp();
  vp->vtype = NUM;
	vp->dval = (double)strtol(yytext,(char**)NULL,2);
	return NUM;
  }

def/[^a-zA-Z0-9_.] {
#if DEBUG_LEX
	fprintf(stderr, "def: %s\n", yytext);
#endif
	return TDEF;
  }

hist1/[^a-zA-Z0-9_.] {
#if DEBUG_LEX
	fprintf(stderr, "TH1: %s\n", yytext);
#endif
	return TH1;
  }

hist2/[^a-zA-Z0-9_.] {
#if DEBUG_LEX
	fprintf(stderr, "TH2: %s\n", yytext);
#endif
	return TH2;
  }

com.*/\n {
#if DEBUG_LEX
	fprintf(stderr, "COM: %s\n", yytext);
#endif
	return TCOM;
  }

&&   {
#if DEBUG_LEX
	fprintf(stderr, "TLAND: %s\n", yytext);
#endif
	return TLAND;
  }

\|\| {
#if DEBUG_LEX
	fprintf(stderr, "TLOR: %s\n", yytext);
#endif
	return TLOR;
  }

==   {
#if DEBUG_LEX
	fprintf(stderr, "TEQ: %s\n", yytext);
#endif
	return TEQ;
  }

!=   {
#if DEBUG_LEX
	fprintf(stderr, "TNQ: %s\n", yytext);
#endif
	return TNEQ;
  }

(\>=|=\>) {
#if DEBUG_LEX
	fprintf(stderr, "TGE: %s\n", yytext);
#endif
	return TGE;
  }

(\<=|=\<) {
#if DEBUG_LEX
	fprintf(stderr, "TLE: %s\n", yytext);
#endif
	return TLE;
  }

(\<\<) {
#if DEBUG_LEX
	fprintf(stderr, "TSL: %s\n", yytext);
#endif
	return TSL;
  }

(\>\>) {
#if DEBUG_LEX
	fprintf(stderr, "TSR: %s\n", yytext);
#endif
	return TSR;
  }

[A-Za-z_][_A-Za-z0-9]* {
	vrec_p   vp;
#if DEBUG_LEX
	fprintf(stderr, "NAME: %s\n", yytext);
#endif
	yylval.vrecp = vp = new_vrecp();
  vp->vtype = NAME;
	vp->sval = strdup(yytext);
	return NAME;
  }

\"[^"\n]*["] {
  char *str;
	vrec_p   vp;
#if DEBUG_LEX
	fprintf(stderr, "STR: %s\n", yytext);
#endif
	yylval.vrecp = vp = new_vrecp();
  vp->vtype = STR;
	vp->sval = str = strdup(&yytext[1]);
	str[strlen(str)-1] = 0x00;
	return STR;
  }

\; {
  return TSEP;
  }

\n  {
	line++;
	return yytext[0];
}

. {
#if DEBUG_LEX
	fprintf(stderr, "OPE: %s\n", yytext);
#endif
	return yytext[0];
  }

%%
#if defined(linux) || defined(Darwin)
int finput(buf,res,max_size)
		 char *buf;
		 int  *res, max_size;
{
	int c;
	switch(fin.type){
	case FIN_NONE:
		*res = YY_NULL;
		break;
	case FIN_FILE:
		if(findef && (c=getc(fin.file)) != EOF){
			buf[0] = c;
			*res = 1;
		}else{
			*res = YY_NULL;
		}
		break;
	case FIN_STR:
		c = fin.pstr[fin.istr];
		if(c){
			buf[0] = c;
			*res = 1;
			fin.istr++;
		}else{
			*res = YY_NULL;
		}
		break;
	}
	return(*res);
}

#endif

#if defined(OSF1) || defined(AIX)
static int input()
{
	int c;
	switch(fin.type){
	case FIN_FILE:
		if(findef && (c=getc(fin.file)) != EOF){
			return(c);
		}
		break;
	case FIN_STR:
		c = fin.pstr[fin.istr];
		if(c){
			fin.istr++;
			return(c);
		}
		break;
	case FIN_NONE:
	default:
		break;
	}
	return(0);
}

static void unput(c)
		 int  c;
{
	switch(fin.type){
	case FIN_NONE:
		break;
	case FIN_FILE:
		if(c && c!=EOF)
		ungetc(c, findef);
		break;
	case FIN_STR:
		if(fin.istr)
			fin.istr--;
		break;
	}
}

#endif

int yywrap(){
  return(1);
}
/*
Local Variables:
mode: C
tab-width: 2
End:
*/
