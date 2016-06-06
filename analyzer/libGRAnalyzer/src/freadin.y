%{
/*
  feradin.y --- yacc program for fread
  Copyright (C) 2000  A. Tamii
  Author:   A. Tamii
  Facility: Department of Physics, University of Tokyo
  Version 1.00 04-JUN-2000 by A. Tamii
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "freadin.h"
#include "histogram.h"

extern vrec_p slist;
vrec_p add_node(int ope, vrec_p arg1, vrec_p arg2);
%}

%union {
	struct  vrec   *vrecp;
}
%token <vrecp> NAME NAME2
%token <vrecp> NUM
%token <vrecp> TDEF TH1 TH2 TCOM TSEP
%token <vrecp> STR
%token TUNDEF TNODE
%token TUMIN 
%token TFUNC TARRAY TELM
%token TLOR TLAND TEQ TNEQ TLE TGE
%right '='
%left ','
%left TLOR
%left TLAND
%left '|'
%left '^'
%left '&'
%left TEQ TNEQ
%left '<' TLE '>' TGE
%left TSL TSR
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS UPLUS
%right '!'
%right '~'

%type <vrecp> statement_list statement expr var elmnts number
%%
statement_list: '\n' {}
              | statement  '\n' {}
              | statement_list '\n' {}
              | statement_list statement '\n' {}
              ;

statement: number TDEF NAME '=' expr {
						 def($1,$3,0,$5);
             }
         | number TDEF NAME '=' STR {
						 def($1,$3,0,$5);
             }
         | number TDEF NAME '[' expr ']' '=' expr {
						 def($1,$3,$5,$8);
             }
         | number TDEF NAME '=' '{' elmnts '}' {
						 def($1,$3,0,$6);
             }
         | number TDEF NAME '[' expr ']' '=' '{' elmnts '}' {
						 def($1,$3,$5,$9);
             }
         | number TH1 NAME TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP
                  STR TSEP STR {
						 hist1($1,$3,$5,$7,$9,$11,$13,$15,$17,0);
             }
         | number TH1 NAME TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP
                  STR TSEP STR TSEP {
						 hist1($1,$3,$5,$7,$9,$11,$13,$15,$17,0);
             }
         | number TH1 NAME TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP
                  STR TSEP STR TSEP var {
						 hist1($1,$3,$5,$7,$9,$11,$13,$15,$17,$19);
             }
         | number TH2 NAME TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP STR {
						 hist2($1,$3,$5,$7,$9,$11,$13,$15,$17,$19,$21,$23,$25,0);
             }
         | number TH2 NAME TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP STR TSEP {
						 hist2($1,$3,$5,$7,$9,$11,$13,$15,$17,$19,$21,$23,$25,0);
             }
         | number TH2 NAME TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP STR TSEP
                  var TSEP var TSEP var TSEP var TSEP STR TSEP var {
						 hist2($1,$3,$5,$7,$9,$11,$13,$15,$17,$19,$21,$23,$25,$27);
             }
         | number TCOM {com($1);}
         ;

/*
ref : NUM                    { $$ = $1; }
    | NAME                   { $$ = $1; $1->vtype = NAME2;}
    | NAME '[' expr ']'      { $$ = add_node(TARRAY,$1,$3); }
*/

/*
var : '(' expr ')'           { $$ = $2; }
    | NAME '[' expr ']'      { $$ = add_node(TARRAY,$1,$3); }
    | NAME                   { $$ = $1; $1->vtype = NAME2; }
    | number                 { $$ = $1; }
    | expr                   { $$ = $1; }
    ;
*/

var : expr                   { $$ = $1; }

elmnts : expr ',' elmnts     { $$ = add_node(TELM,$1,$3); }
       | expr                { $$ = $1; }
       ;

expr: expr TLOR expr         { $$ = add_node(TLOR,$1,$3); }
    | expr TLAND expr        { $$ = add_node(TLAND,$1,$3); }
    | expr '|' expr          { $$ = add_node('|',$1,$3); }
    | expr '^' expr          { $$ = add_node('^',$1,$3); }
    | expr '&' expr          { $$ = add_node('&',$1,$3); }
    | expr TEQ expr          { $$ = add_node(TEQ,$1,$3); }
    | expr TNEQ expr         { $$ = add_node(TNEQ,$1,$3); }
    | expr TLE expr          { $$ = add_node(TLE,$1,$3); }
    | expr TGE expr          { $$ = add_node(TGE,$1,$3); }
    | expr '<' expr          { $$ = add_node('<',$1,$3); }
    | expr '>' expr          { $$ = add_node('>',$1,$3); }
    | expr TSL expr          { $$ = add_node(TSL,$1,$3); }
    | expr TSR expr          { $$ = add_node(TSR,$1,$3); }
    | expr '+' expr          { $$ = add_node('+',$1,$3); }
    | expr '-' expr          { $$ = add_node('-',$1,$3); }
    | expr '*' expr          { $$ = add_node('*',$1,$3); }
    | expr '/' expr          { $$ = add_node('/',$1,$3); }
    | expr '%' expr          { $$ = add_node('%',$1,$3); }
    | '-' expr %prec UMINUS  { $$ = add_node(TUMIN,$2,0); } 
    | '+' expr %prec UPLUS   { $$ = $2; } 
    | '!' expr %prec UMINUS  { $$ = add_node('!',$2,0); } 
    | '~' expr %prec UMINUS  { $$ = add_node('~',$2,0); } 
    | '(' expr ')'           { $$ = $2; }
    | NAME '[' expr ']'      { $$ = add_node(TARRAY,$1,$3); }
/*  | NAME '(' expr ')'      { $$ = add_node(TFUNC,$1,$3); } */
    | NAME '(' elmnts ')'    { $$ = add_node(TFUNC,$1,$3); }
/*  | NAME                   { $$ = $1; } */
    | NAME                   { $$ = $1; $1->vtype = NAME2; }
    | NUM                    { $$ = $1; }
    ;

number: NUM                  { $$ = $1; }
      | '+' NUM %prec UPLUS  { $$ = $2; }
      | '-' NUM %prec UMINUS { $$ = $2; $$->dval = -$$->dval; }
      ;
%%
extern int   line;
extern char  *yytext;

/* yyerror */
int yyerror(s)
		 char  *s;
{
	extern int parse_error;
	showerr("line %d: parse error at '%s'.\n", line, yytext);
	parse_error++;
  return 0;
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
	if((v2=v->arg1)){
		v->arg1 = (vrec_p)NULL;
		free_vrecp(v2);
	}
	if((v2=v->arg2)){
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
	case '^':     return("^");
	case '&':     return("&");
	case '<':     return("<");
	case '>':     return(">");
	case TSL:     return("<<");
	case TSR:     return(">>");
	case '+':     return("+");
	case '-':     return("-");
	case '*':     return("*");
	case '/':     return("/");
	case '%':     return("%");
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
vrec_p add_node(ope, arg1, arg2)
		 int      ope;
		 vrec_p   arg1, arg2;
{
	vrec_p  vp;
#if DEBUG_YACC
	fprintf(stderr, "add_node: token: %s\n", str_token(ope));
#endif
	vp = new_vrecp();
	vp->vtype = TNODE;
	switch(ope){
	case TUMIN:
	case '!':
	case '~':
		if(arg1==(vrec_p)NULL){
			showerr("add_node: illegal argument for token %s\n", str_token(ope));
			break;
		}
		vp->vtype = ope;
		vp->arg1 = arg1;
		break;
	case '|':
	case '^':
	case '&':
	case '<':
	case '>':
	case TSL:
	case TSR:
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
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

