/*
  histogram.c ---- histogrammer subroutines
  Version 1.00  02-JUN-2000  by A. Tamii
  Version 1.80  28-JUL-2001  by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>

#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"
#include "freadin.tab.h"

extern int pflag;    /* parent process  flag */
extern int cflag;    /* child process flag */

#define DR_N_ADD   100

typedef struct hist_t{
	int     id;         /* histogram ID */
	int     type;       /* type of the histogram (enum hist_type) */
	char    *sname;     /* short hame of the histogram */
	char    *name;      /* name (title) of the histogram */
	vrec_p  v1;         /* x variable */
	vrec_p  min1;       /* x minimum */
	vrec_p  max1;       /* x maximum */
	vrec_p  nbin1;      /* x number of bins */
	char    *xlabel;    /* x label */
	vrec_p  v2;         /* y variable */
	vrec_p  min2;       /* y minimum */
	vrec_p  max2;       /* y maximum */
	vrec_p  nbin2;      /* y number of bins */
	char    *ylabel;    /* y label */
	vrec_p  gate;       /* gate variable */
} hist_t, *hist_p;

hist_p hists = (hist_p)NULL;
int    nhists=0;
int    histid=1;
int    neval=0;
int    level=0;

enum hist_type {
	HIST_NONE=0,     /* no histogram */
	HIST_1D,         /* 1-dimensional histogram */
	HIST_2D          /* 2-dimensional histogram */
};

typedef struct func_t {
	char    *name;
	double  (*func)();
} func_t;

func_t *funcs;
int nfunc = 0;

dr_t dr_data;

char *spf(fmt)
		 char *fmt;
{
	static char spf_str[256];
	va_list args;
	va_start(args,fmt);
	vsprintf(spf_str,fmt,args);
	va_end(args);
	return(spf_str);
}	

int dr_add(dr)
     dr_t *dr;
{
  dr_rec_t *r;
  int      i;
#if 0
		/* 28-JUL-2001
       The procedure dr_get_r() is changed. Owing the change,
			 I think the mechanism of dr_add() is working good.
			 See dr_get_r();
		*/
#if 1
		/* 28-JUL-2001
			 The following machanism of enlarging dr->rec size does not work good.
			 The change of the memory position of dr->rec can cause a serious problem.
		*/
	if(dr->nmax){
		fprintf(stderr, "The memory size for variables is not enough.\n");
		fprintf(stderr, "Please enlarge DR_N_ADD in histogram.c .\n");
		exit(1);
	}
#endif
#endif
  dr->nmax += DR_N_ADD;
  r = (dr_rec_t*)realloc(dr->rec, sizeof(dr_rec_t)*dr->nmax);
  if(r == (dr_rec_t*)NULL){
    fprintf(stderr, "dr_add: could not allocate memory.\n");
    dr->nmax -= DR_N_ADD;
    return(-1);
  }
  dr->rec = r;
  r = &r[dr->nmax-DR_N_ADD];
  for(i=0; i<DR_N_ADD; i++){
    r->type = DR_UNDEF;
    r->imin = 0;
    r->imax = 0;
    r->amin = 0;
    r->amax = 0;
    r->nvalue = 0;
    r->nam = (char*)NULL;
    r->value = (double*)NULL;
		r->nvrec = 0;
		r->vrec = (vrec_p*)NULL;
    r->nreg = 0;
    r->reg = (int**)NULL;
		r->eval = EVAL_NONE;
		r->neval = 0;
    r++;
  }
  return(0);
}

int dr_register(dr,h)
     dr_t  *dr;
     int   *h;
{
  int  n, **p;
  dr_rec_t *r;
  if(dr->n <= *h)
    return(-1);
  r = &dr->rec[*h];
  n = r->nreg+1;
  p = (int**)realloc(r->reg,sizeof(int*)*n);
  if(p==(int**)NULL){
    fprintf(stderr, "dr_register: could not allocate memory (%d).\n", n);
    return(-1);
  }
  r->reg = p;
  p[r->nreg++] = h;
    return 0;
}

int dr_rec_init(dr,h,nam)
     dr_t *dr;
     int  *h;
     char *nam;
{
  int   i, n;
  dr_rec_t *r;

  if(*h>=dr->nmax){
    while(*h>=dr->nmax){
      if(dr_add(dr)){
				return(-1);
      }
    }
    dr->n = *h+1;
  }else if(*h>=dr->n){
    dr->n = *h+1;
  }else{
    if(r->nam){
      free(r->nam);
      r->nam = (char*)NULL;
    }
    if(r->value){
      free(r->value);
      r->value = (double*)NULL;
    }
  }
	
  r = &dr->rec[*h];
  r->nam = (char*)malloc(strlen(nam)+1);
  if(r->nam==(char*)NULL){
    fprintf(stderr, "dr_rec_init: could not allocate memory.\n");
    return(-1);
  }
  strcpy(r->nam, nam);
  dr_register(dr,h);
  return(0);
}

int dr_rec_enlarge(r,index)
     dr_rec_t  *r;
     int       index;
{
  int    i, k, n, nmin, nmax;
  double *p, *s, *d;
  if(r->imin == r->imax){
    p = (double*)realloc(r->value, sizeof(double)*1);
    if(p==(double*)NULL){
      fprintf(stderr, "dr_rec_enlarge: could not allocate memory.\n");
      return(-1);
    }
    r->value = p;
    dr_set_nothing(r->value);
    r->imin = index;
    r->imax = index+1;
    r->nvalue = 1;
  }else if(index < r->imin){
    nmin = index;
    nmax = r->imax;
    k = r->imax - r->imin;
    n = nmax - nmin;
    p = (double*)realloc(r->value, sizeof(double)*n);
    if(p==(double*)NULL){
      fprintf(stderr, "dr_rec_enlarge: could not allocate memory.\n");
      return(-1);
    }
    r->value = p;
    d = &p[n-1];
    s = &p[k-1];
    for(i=0; i<k; i++)
      *d-- = *s--;
    for(; i<n; i++)
      dr_set_nothing(d--);
    r->imin = nmin;
    r->imax = nmax;
    r->nvalue = n;
  }else if(r->imax <= index){
    nmin = r->imin;
    nmax = index+1;
    k = r->imax - r->imin;
    n = nmax - nmin;
    p = (double*)realloc(r->value, sizeof(double)*n);
    if(p==(double*)NULL){
      fprintf(stderr, "dr_rec_enlarge: could not allocate memory.\n");
      return(-1);
    }
    r->value = p;
    d = &p[k];
    for(i=k; i<n; i++)
      dr_set_nothing(d++);
    r->imin = nmin;
    r->imax = nmax;
    r->nvalue = n;
  }
  return(0);
}
     
int dr_rec_search(dr,nam)
     dr_t  *dr;
     char  *nam;
{
  dr_rec_t *r;
  int      i;
  char     c;
	if(nam==(char*)NULL)
		return(0);
  c = *nam;
  r = dr->rec;
  for(i=1; i<dr->n; i++){
    r++;
    if(*(r->nam)==c){
      if(!strcmp(r->nam,nam))
	return(i);
    }
  }
  return(0);
}

int dr_set_r(dr,h,nam,index,value)
     dr_t  *dr;
     int   *h;
     char const *nam;
     int   index;
     double value;
{
  dr_rec_t  *r;
  int       n;
  
#if 0
  fprintf(stderr, "dr_set_r: %s %d %f\n", nam, index, value);
#endif

  /* check the record is already exists */
  if(*h==0){
    *h = dr_rec_search(dr,nam);
    if(*h==0){
      *h = dr->n;
      if(dr_rec_init(dr,h,nam)){
				*h = 0;
				return(-1);
      }
    }else{
      dr_register(dr,h);
    }
  }
  
  /* check the range of index  */
  if(index<DR_MIN_ARRAY_INDEX || DR_MAX_ARRAY_INDEX <= index){
    fprintf(stderr, "dr_r: Too large/small index (%d).\n", index);
    return(-1);
  }

  r = &dr->rec[*h];
  if(index < r->imin || r->imax <= index )
    if(dr_rec_enlarge(r, index))
      return(-1);

  r->value[index-r->imin] = value;
  if(r->amin==r->amax){
    r->amin = index;
    r->amax = index+1;
  }else if(index<r->amin)
    r->amin = index;
  else if(r->amax<=index)
    r->amax = index+1;
  r->type = DR_USER;
  return(0);
}

int dr_set_vrec(dr,h,nam,v)
     dr_t  *dr;
		 int   *h;
     char const *nam;
     vrec_p v;
{
  dr_rec_t  *r;
  int       n;
	vrec_p    *p;
  
  /* check the record is already exists */
	if(*h==0){
		*h = dr_rec_search(dr,nam);
		if(*h==0){
			*h = dr->n;
			if(dr_rec_init(dr,h,nam))
				return(-1);
		}
	}
  
  r = &dr->rec[*h];
	p = (vrec_p*)realloc(r->vrec,(r->nvrec+1)*sizeof(vrec_p));
	if(p==(vrec_p*)NULL){
		fprintf(stderr, "dr_set_vrec: could not allocate memory.\n");
		return(-1);
	}

	p[r->nvrec++] = v;
	r->vrec = p;
  return(0);
}

dr_rec_t *dr_get_drec(dr,h,nam)
     dr_t  *dr;
		 int   *h;
     char  *nam;
{
  
  /* check the record is already exists */
	if(*h==0){
		*h = dr_rec_search(dr,nam);
		if(*h==0){
			*h = dr->n;
			if(dr_rec_init(dr,h,nam))
				return((dr_rec_t*)NULL);
		}
	}
  
  return(&dr->rec[*h]);
}

double dr_get_r(dr,h,nam,index)
     dr_t  *dr;
     int   *h;
     char const *nam;
     int   index;
{
  dr_rec_t  *r;
  int       i, j, n;
  double    res;
	vrec_p    p, v;
	double    d1, d2;
	int       m;
  /* for DEBUG_CYCLIC */
	static int       n_eval=0;
	static char      str_eval[1024] = {0};
	static int       str_eval_pos[128];

  
  dr_set_nothing(&res);

  /* check the record is already exists */
  if(*h==0){
    *h = dr_rec_search(dr,nam);
    if(*h==0){
#if 0 /* changed on 03-AUG-2000 */
      return(res);
#else
      *h = dr->n;
      if(dr_rec_init(dr,h,nam)){
				*h = 0;
				return(res);
      }
#endif
    }else{
      dr_register(dr,h);
    }
  }
  
  r = &dr->rec[*h];

	if(r->neval < neval){
		if(r->eval==EVAL_UNDER){
			fprintf(stderr, "Cyclic evaluation of variable '%s'.\n", r->nam);
			if(DEBUG_CYCLIC){
				fprintf(stderr, "%s\n", str_eval);
			}
			return(res);
		}
		r->eval = EVAL_UNDER;

		/* do evaluation */
		if(DEBUG_CYCLIC){
			i = strlen(str_eval);
			str_eval_pos[n_eval++] = i;
			strcpy(&str_eval[i], "->");
			strcat(&str_eval[i], r->nam);
		}
		for(i=0; i<r->nvrec; i++){
			p = r->vrec[i];
			d1 = p->arg1 ? eval(p->arg1) : 0.;
			if(dr_exists(d1)){
				v = p->arg2;
				while(v && v->vtype==TELM){
					d2 = eval(v->arg1);
					if(dr_exists(d2))
						dr_set_r(&dr_data, h, nam, (int)d1, d2);
					d1 += 1.;
					v = v->arg2;
				}
				d2=eval(v);
				if(dr_exists(d2))
					dr_set_r(&dr_data, h, nam, (int)d1, d2);
			}
		}
		/* re-get the pointer r because the position of r might have been changed */
		/* chaned on 28-JUL-2001. See dr_get_r() */
		r = &dr->rec[*h];

		r->eval = EVAL_NONE;
		r->neval = neval;
		if(DEBUG_CYCLIC){
			str_eval[str_eval_pos[--n_eval]] = 0x00;
		}
	}

	/* check the range of indices */
  if(index < r->imin || r->imax <= index )
    return(res);

  return(r->value[index-r->imin]);
}

vrec_p dr_get_vrec_r(dr,h,nam,index)
     dr_t  *dr;
     int   *h;
     char const *nam;
     int   index;
{
  vrec_p    res;
  dr_rec_t  *r;
  
  res = (vrec_p)NULL;
  
	/* do evaluation */
	(void)dr_get_r(dr,h,nam,index);
  r = &dr->rec[*h];

	/* check the range of indices */
  if(index < r->imin || r->imax <= index )
    return(res);

  return(r->vrec[index-r->imin]);
}

/*
	 get string value
   The usage of a string value is very restricted
   No substituion is allowed, i.e. you cannot get the value of 'a' from
	 definitions of
	   a = b
		 b = "string" .
   Moreover only one definition of a  string value is allowed.
*/
char *dr_get_sval_r(dr,h,nam,index)
     dr_t  *dr;
     int   *h;
     char const *nam;
     int   index;
{
  vrec_p    v;
  dr_rec_t  *r;
  
	/* do evaluation */
	(void)dr_get_r(dr,h,nam,index);
  r = &dr->rec[*h];

	/* check the range of indices */
  if(index < r->imin || r->imax <= index )
    return((char*)NULL);
	
	v = r->vrec[index-r->imin]->arg2;
	if(v){
		return(v->sval);
	}else{
		return((char*)NULL);
	}
}

/* get minimum index number */
int dr_min_r(dr,h,nam,dummy)
     dr_t  *dr;
     int   *h;
     char const *nam;
		 int   dummy;
{
  /* check the record is already exists */
  if(*h==0){
    *h = dr_rec_search(dr,nam);
    if(*h==0){
      return(0);
    }else{
      dr_register(dr,h);
    }
  }
  
  /* do evaluation (in order to determine min and max)*/
	(void)dr_get_r(dr,h,nam,dummy);
	
  return(dr->rec[*h].amin);
}

/* get maximum index number */
int dr_max_r(dr,h,nam,dummy)
     dr_t  *dr;
     int   *h;
     char const *nam;
		 int   dummy;
{
  if(*h==0){
    *h = dr_rec_search(dr,nam);
    if(*h==0){
      return(0);
    }else{
      dr_register(dr,h);
    }
  }
  
  /* do evaluation (in order to determine min and max)*/
	(void)dr_get_r(dr,h,nam,dummy);

  return(dr->rec[*h].amax);
}

/* get reference number */
int dr_ref_r(dr,h,nam)
     dr_t  *dr;
     int   *h;
     char const *nam;
{
	*h = dr_rec_search(dr,nam);
	if(*h==0){
		*h = dr->n;
		if(dr_rec_init(dr,h,nam)){
			*h = 0;
			return(-1);
		}
	}else{
		dr_register(dr,h);
	}
    return 0;
}

int dr_append_r(dr,h,nam,index,value)
     dr_t  *dr;
     int   *h;
     char const *nam;
		 int   index;  /* dummy */
     double value;
{
  int       n;
  n = dr_max_r(dr,h,nam,0);
  return(dr_set_r(dr,h,nam,n,value));
}

/* get number of indices */
int dr_nval_r(dr,h,nam,index)
     dr_t  *dr;
     int   *h;
     char const *nam;
		 int   index;
{
  if(*h==0){
    *h = dr_rec_search(dr,nam);
    if(*h==0){
      return(0);
    }else{
      dr_register(dr,h);
    }
  }
  
  return(dr->rec[*h].nvalue);
}

int dr_clear_r(dr)
     dr_t  *dr;
{
  int    i, j, m, n;
  dr_rec_t *r;
  double   *p;
  n = dr->n;
  r = &dr->rec[1];
  for(i=1; i<n; i++, r++){
    if(!r->value)
      continue;
    m = r->amax;
    p = &r->value[r->amin-r->imin];
    for(j=r->amin; j<m; j++)
      dr_set_nothing(p++);
    r->amin = r->amax = 0;
  }
  return(0);
}

/* initialize dr records                         */
/*  1) clear the reference to variables          */
/*  2) clear and free the variables definitions  */
/*  3) clear and free user definitions (vrec)    */
int dr_reinit_r(dr)
     dr_t  *dr;
{
  int    i, j, n;
  dr_rec_t *r;
  double   *p;

  if(!dr->rec) return(0);

#if DEBUG_DEFINITION
	fprintf(stderr, "Clear all the definitions.\n");
#endif

	/* at first, do clear references (before memories are freed) */
	n = dr->n;
	r = &dr->rec[n-1];
	for(i=1; i<n; i++, r--){
		if(r->reg){
			for(j=0; j<r->nreg; j++){
				*(r->reg[j]) = 0;
			}
			free(r->reg);
			r->reg = (int**)NULL;
			r->nreg = 0;
		}
  }

	/* clear and free each dr_rec and vrec */
	n = dr->n;
	r = &dr->rec[n-1];
	for(i=1; i<n; i++, r--){
		if(r->nam){
			free(r->nam);
			r->nam = (char*)NULL;
		}
		if(r->value){
			free(r->value);
			r->value = (double*)NULL;
			r->nvalue = 0;
		}
		if(r->vrec){
			for(j=0; j<r->nvrec; j++){
				free_vrecp(r->vrec[j]);
				r->vrec[j] = (vrec_p)NULL;
			}
			free(r->vrec);
			r->vrec = (vrec_p*)NULL;
			r->nvrec = 0;
		}
		r->imin = r->imax = r->amin = r->amax = 0;
	}

	/* clear dr record */
	free(dr->rec);
	dr->rec = (dr_rec_t*)NULL;
	dr->n = 1;
	dr->nmax = 0;
	level = 0;
  return(0);
}

int dr_add_func(nam,func)
		 char   *nam;
		 double (*func)();
{
	int      i;
	func_t  *p;
	for(i=0; i<nfunc; i++){
		if(!strcmp(funcs[i].name,nam)){
			fprintf(stderr, "Warning: replace the function '%s'.", nam);
			funcs[i].func = func;
			return(0);
		}
	}
	funcs = (func_t*)realloc(funcs,sizeof(func_t)*(nfunc+1));
	if(funcs==(func_t*)NULL){
		fprintf(stderr, "dr_add_func: could not allocate memory.\n");
		return(-1);
	}
	funcs[i].name = strdup(nam);
	funcs[i].func = func;
	nfunc++;
	return(0);
}


void *dr_get_func_r(h,nam)
		 int   *h;
     char  *nam;
{
	int   i;
	char  c;
	if(*h){
		return (funcs[*h].func);
	}else{
		c = nam[0];
		for(i=0; i<nfunc; i++){
			if(*(funcs[i].name)==c){
				if(!strcmp(funcs[i].name,nam)){
					*h = i;
					return (funcs[i].func);
				}
			}
		}
	}
	fprintf(stderr, "No function named '%s'.\n", nam);
	return(NULL);
}

double dr_func_r(dr,h,nam,args)
     dr_t  *dr;
     int   *h;
     char  *nam;
     vrec_p args;
{
#define MaxNArgs 4
	double res;
	double (*func)();
	double arg[MaxNArgs];
	vrec_p v[MaxNArgs];
	vrec_p vp;
	int    i, ieval;

	/* get function */
  *(void **)&func = dr_get_func_r(h,nam);
	if(!func){
		dr_set_nothing(&res);
		return(res);
	}

	/* initialize arguments */
	for(i=0; i<MaxNArgs; i++){
		dr_set_nothing(&arg[i]);
		v[i] = (vrec_p)NULL;
	}

	/* extract arguments */
	vp = args;
	for(i=0; i<MaxNArgs; i++){
		if(vp->vtype!=TELM){
			/* get an argument and end */
			arg[i] = eval(vp);
			v[i] = vp;
			break;
		}
		/* get an argument */
		arg[i] = eval(vp->arg1);
		v[i]   = vp->arg1;
#if 0  /* 14-OCT-2003 */
		if(dr_is_nothing(arg[i]))
			break;
#endif
		/* advance to the next argument */
		vp   = vp->arg2;
	}

#if 0  /* 14-OCT-2003 */
	if(dr_is_nothing(arg[i])){
		/* abort calculation */
		return(arg[i]);
	}
#endif

#if !(MaxNArgs-4)
	res = (*func)(arg[0],arg[1],arg[2],arg[3],v[0],v[1],v[2],v[3]);
#else
  ERROR;
/*
	The above function call must be changed.
	Functions which utilized v-arguments must be appropriatedly changed.
*/
#endif

  /* check the value is constant or not */
  ieval = EVAL_CONST;
  for(i=0; i<MaxNArgs; i++){
		if(!v[i]) break;
		if(v[i]->eval!=EVAL_CONST){
			ieval = EVAL_NONE;
			break;
		}
	}
	args->eval = ieval;
  return(res);
}

void dr_show_r(dr)
     dr_t  *dr;
{
  int      i, j, min, max;
  double   *p;
  dr_rec_t *r;
  printf("[Dump of dr_data]\n");
  if(dr==(dr_t*)NULL){
    fprintf(stderr, "dr_show: Null pointer.\n");
    return;
  }
  r = dr->rec;
  if(r==(dr_rec_t*)NULL){
#if 0
    fprintf(stderr, "dr_show: Null dre->rec pointer.\n");
#else
		printf("\n");
#endif
    return;
  }

	printf("------------------------------------------------------------\n");
  for(i=1; i<dr->n; i++){
    r = &dr->rec[i];
    printf("  %s:\n", r->nam);
#if 0
    min = r->imin;
    max = r->imax;
#else
    min = r->amin;
    max = r->amax;
#endif
    p = &r->value[min-r->imin];
    for(j=min; j<max; j++){
      if(dr_is_nothing(*p))
				printf("  %5d) %15s\n", j, "N/U");
      else
				printf("  %5d) %15.7f %8x\n", j, *p, (int)*p);
      p++;
    }
  }
  printf("\n");
}

void dr_event(){
	neval++;
  if(!event())
		histogram();
#if DEBUG_DR_SHOW
  dr_show();
#endif
}

#if DEBUG_EVAL
void show_eval(str,add)
		 char *str;
{
	static int  show_eval_level=0;
	static char show_eval_str[1024];
	show_eval_level += add;
	if(add>0){
		if(show_eval_level==0) strcpy(show_eval_str,str);
		else{
			strcat(show_eval_str,"->");
			strcat(show_eval_str,str);
		}
		fprintf(stderr, "%s\n", show_eval_str);
	}else{
		if(show_eval_level)
			show_eval_str[strlen(show_eval_str)-strlen(str)-2] = 0;
		else
			show_eval_str[0]= 0;
	}
}
#endif

/* evaluate a value */
double eval(v)
		 vrec_p  v;   /* value to evaluate */
{
	double  d, d1, d2;
	dr_rec_t *r;
	int i;

	dr_set_nothing(&d);
	if(!v) return(d);

  /* return if already evaluated */
	if(v->neval >= neval)
		return(v->dval);

	/* check evaluation */
	if(v->eval==EVAL_CONST)
		return(v->dval);
	if(v->eval==EVAL_UNDER){
		if(v->vtype==NAME || v->vtype==NAME2)
			showerr("Cyclic evaluation of %s.\n", v->sval);
		else if(v->vtype==TARRAY)
			showerr("Cyclic evaluation of %s.\n", v->arg1->sval);
		else if(v->vtype==NUM)
			showerr("Cyclic evaluation of NUM %d.\n", v->sval);
		else if(v->vtype==STR) {
			showerr("Cyclic evaluation of STR %s.\n", v->sval);
		}
		else
			showerr("Cyclic evaluation.\n");
		return(d);
	}

	v->eval = EVAL_UNDER;
#if DEBUG_EVAL
	show_eval(str_vtype(v),1);
#endif
	switch(v->vtype){
	case NUM:
		d = v->dval;
		v->eval = EVAL_CONST;
		break;
	case STR:
		d = 0.;
		break;
	case NAME:
	case NAME2:
#if DEBUG_EVAL && 1
		show_eval(v->sval,1);
#endif
		d = dr_get_r(&dr_data, &v->vref, v->sval, 0);
#if DEBUG_EVAL && 1
		show_eval(v->sval,-1);
#endif
		v->eval = EVAL_NONE;
		break;
	case TARRAY:
#if DEBUG_EVAL && 0
		show_eval(v->arg1->sval,1);
#endif
		d1 = eval(v->arg2);
		if(dr_exists(d1))
			d = dr_get_r(&dr_data,&v->arg1->vref,v->arg1->sval,(int)d1);
#if DEBUG_EVAL && 0
		show_eval(v->arg1->sval,-1);
#endif
		v->eval = EVAL_NONE;
		break;
	case '!':
    /* 'not' of nothing is defined as 'true' */
		d1 = eval(v->arg1);
		d = dr_is_false(d1) ?  DR_TRUE : DR_FALSE;
		v->eval = dr_const(v->arg1) ;
		break;
	case '~':
		d1 = eval(v->arg1);
		if(dr_exists(d1))
			d = (double)(~(int)d1);
		v->eval = dr_const(v->arg1) ;
		break;
	case TUMIN:
		d1 = eval(v->arg1);
		if(dr_exists(d1))
			d = -d1;
		v->eval = dr_const(v->arg1) ;
		break;
	case '|':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = (double)((int)d1 | (int)d2);
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '^':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = (double)((int)d1 ^ (int)d2);
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '&':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = (double)((int)d1 & (int)d2);
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '<':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = ((d2-d1)>DR_ZERO) ? DR_TRUE : DR_FALSE;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '>':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = ((d1-d2)>DR_ZERO) ? DR_TRUE : DR_FALSE;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case TSL:
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = d1 * pow(2.,d2);
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case TSR:
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = d1 * pow(2.,-d2);
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '+':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = d1 + d2;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '-':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = d1 - d2;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '*':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = d1 * d2;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '/':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2) && !dr_is_zero(d2))
				d = d1 / d2;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case '%':
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2) && !dr_is_zero(d2)){
				d = d1-d2*(double)((int)(d1/d2));
			 }
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case TLOR:
		d1 = eval(v->arg1);
		if(dr_is_true(d1)){  /* the judgement of nothing is including in dr_is_true() */
			d = DR_TRUE;
			v->eval = dr_const(v->arg1) ;
			break;
		}
		d2 = eval(v->arg2);
		d = dr_is_true(d2) ? DR_TRUE : DR_FALSE;
		v->eval = dr_const2(v->arg1,v->arg2) ;
		break;
	case TLAND:
		d1 = eval(v->arg1);
		if(dr_is_false(d1)){  /* the judgement of nothing is including in dr_is_false() */
			d = DR_FALSE;
			v->eval = dr_const(v->arg1) ;
			break;
		}
		d2 = eval(v->arg2);
		d = dr_is_true(d2) ? DR_TRUE : DR_FALSE;
		v->eval = dr_const2(v->arg1,v->arg2) ;
		break;
	case TEQ:
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = dr_is_zero(d1-d2) ? DR_TRUE : DR_FALSE;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else{
			v->eval = dr_const(v->arg1) ;
		}
		break;
	case TNEQ:
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = dr_is_zero(d1-d2) ? DR_FALSE : DR_TRUE;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else{
			v->eval = dr_const(v->arg1) ;
		}
		break;
	case TLE:
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = ((d1-d2)<DR_ZERO) ? DR_TRUE : DR_FALSE;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case TGE:
		d1 = eval(v->arg1);
		if(dr_exists(d1)){
			d2 = eval(v->arg2);
			if(dr_exists(d2))
				d = ((d2-d1)<DR_ZERO) ? DR_TRUE : DR_FALSE;
			v->eval = dr_const2(v->arg1,v->arg2) ;
		}else
			v->eval = dr_const(v->arg1) ;
		break;
	case TFUNC:
		d = dr_func_r(&dr_data, &v->arg1->vref, v->arg1->sval, v->arg2);
		v->eval = dr_const(v->arg2) ;
		break;
	case TNODE:
	case TUNDEF:
	default:
		showerr("Illegal operator: %s.\n", str_token(v->vtype));
		v->eval = EVAL_NONE;
		break;
	}
#if DEBUG_EVAL
		show_eval(str_vtype(v),-1);
#endif
	v->dval = d;
	v->neval = neval;
	return(d);
}

void def(v0, v1, v2, v3)
		 vrec_p  v0;
		 vrec_p  v1, v2, v3;
{
	double  d;
	vrec_p  v;
#if DEBUG_YACC
	fprintf(stderr, "enter def()\n");
#endif
	if(v0==(vrec_p)NULL){
		showerr("def: null pointer argument\n");
		return;
	}
	d = eval(v0);
	if(dr_is_nothing(d)) return;
	if(check_level(d)) return;

	if(v1==(vrec_p)NULL || v3==(vrec_p)NULL){
		showerr("def: null pointer argument\n");
		return;
	}
	v1->vtype = '=';
	v1->arg1 = v2;
	v1->arg2 = v3;
	dr_set_vrec(&dr_data, &v1->vref, v1->sval, v1);
#if DEBUG_DEFINITION
	fprintf(stderr, "Define variable '%s'.\n", v1->sval);
#endif
}

/* get new histogram record */
hist_p new_hist(){
	hist_p  p;
	int  i;
	for(i=0; i<nhists; i++){
#if 0
		if(hists[i].type == HIST_NONE){
			p = &hists[i];
			break;
		}
#endif
	}
	if(i>=nhists){
		p = (hist_p)realloc(hists,sizeof(hist_t)*(nhists+1));
		if(p==(hist_p)NULL){
			fprintf(stderr, "new_hist: could not allocate memory.\n");
			return((hist_p)NULL);
		}
		hists = p;
		nhists++;
		p = &p[i];
	}
#if 0
	p->id = i+1;
#else
	p->id = histid++;
#endif
	p->type = HIST_NONE;
	p->sname = p->name = (char*)NULL;
	p->xlabel = p->ylabel = (char*)NULL;
	p->v1 = p->min1 = p->max1 = p->nbin1 = (vrec_p)NULL;
	p->v2 = p->min2 = p->max2 = p->nbin2 = (vrec_p)NULL;
	p->gate = (vrec_p)NULL;
	return(p);
}

hist_p search_hist(sname)
		 char *sname;
{
	hist_p  p;
	int     i;
	char    c;
	p = hists;
	c = *sname;
	for(i=0; i<nhists; i++, p++){
		if(hists[i].type == HIST_NONE)
			continue;
		if(*(p->sname)==c && !strcmp(p->sname,sname)){
			return(p);
		}
	}
	return((hist_p)NULL);
}

void free_hist(p)
	hist_p  p;
{
	int     i;
	vrec_p  v;
	switch(p->type){
	case HIST_NONE:
		break;
	case HIST_1D:
		if(p->id){
			if(!cflag){
				fprintf(stderr, "Delete 1D-histogram #%3d '%s'.\n", p->id, p->name);
			}
#if DEBUG_DEFINITION
#endif
			hstdel(p->id);
		}
		if(p->sname)    { free(p->sname); p->sname = (char*)NULL; }
		if(p->name)     { free(p->name); p->name = (char*)NULL; }
		if((v=p->v1))   { p->v1    = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->min1)) { p->min1  = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->max1)) { p->max1  = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->nbin1)){ p->nbin1 = (vrec_p)NULL; free_vrecp(v); }
		if(p->xlabel)   { free(p->xlabel); p->xlabel = (char*)NULL; }
		if(p->ylabel)   { free(p->ylabel); p->ylabel = (char*)NULL; }
		if((v=p->gate)) { p->gate  = (vrec_p)NULL; free_vrecp(v); }
		p->type = HIST_NONE;
		break;
	case HIST_2D:
		if(p->id){
			hstdel(p->id);
			fprintf(stderr, "Delete 2D-histogram #%3d '%s'.\n", p->id, p->name);
#if DEBUG_DEFINITION
#endif
		}
		if(p->sname)    { free(p->sname); p->sname = (char*)NULL; }
		if(p->name)     { free(p->name); p->name = (char*)NULL; }
		if((v=p->v1))   { p->v1    = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->min1)) { p->min1  = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->max1)) { p->max1  = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->nbin1)){ p->nbin1 = (vrec_p)NULL; free_vrecp(v); }
		if(p->xlabel)   { free(p->xlabel); p->xlabel = (char*)NULL; }
		if((v=p->v2))   { p->v2    = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->min2)) { p->min2  = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->max2)) { p->max2  = (vrec_p)NULL; free_vrecp(v); }
		if((v=p->nbin2)){ p->nbin2 = (vrec_p)NULL; free_vrecp(v); }
		if(p->ylabel)   { free(p->ylabel); p->ylabel = (char*)NULL; }
		if((v=p->gate)) { p->gate  = (vrec_p)NULL; free_vrecp(v); }
		p->type = HIST_NONE;
		break;
	default:
		fprintf(stderr, "free_hist: unknown histogram type %d\n", p->type);
		p->type = HIST_NONE;
		break;
	}
}

/* re-use histogram record */
hist_p reuse_hist(p)
	hist_p  p;
{
	free_hist(p);
	/* currently nothing to do here*/
	return(p);
}

void reinit_hist()
{
	hist_p  p;
	int     i;
	vrec_p  v;
#if DEBUG_DEFINITION
	fprintf(stderr, "Clear all the histograms.\n");
#endif
	p = hists;
	for(i=0; i<nhists; i++, p++)
		free_hist(p);
	free(hists);
	hists = (hist_p)NULL;
	nhists = 0;
	histid = 1;
}

int check_level(d)
		 double  d;
{
	int  l;
	l = (int)(d+0.5);
	if(level>=0){
		if(d<0){
			level = d;
			return(1);
		}else{
			return(0);
		}
	}else{
		if(d<0){
			if(d>level){
				level = d;
				return(1);
			}else{
				return(1);
			}
		}else{
			if(d>-level){
				return(1);
			}else{
				level = 0;
				return(0);
			}
		}
	}
}

void com(v0)
		 vrec_p  v0;
{
	double d;
	if(v0==(vrec_p)NULL){
		showerr("com: null pointer argument\n");
		return;
	}
	d = eval(v0);
	if(dr_is_nothing(d)) return;
	if(check_level(d)) return;
	return;
}

/* convert text to the PAW Text Style */
#define append(d,c) {strcpy((d),(c)); (d) = &(d)[strlen(c)];}

char *cvtext(str)
		 char *str;
{
	static char newtext[256];
	char *s, *d, *e;
	char c;

	d = newtext;
	s = str;
	e = &str[strlen(str)];

	if(!(*s)){  
		/* no title text */
		strcpy(newtext,"^!");  /* no text */
		return(newtext);
	}

	/* convert title text */
	while(s<e && (c=*s++)){
		switch(c){
		case '[': append(d,"\"M#"); break;
		case ']': append(d,"\"N#"); break;
		case '!': append(d,"\"E#"); break;
		case '%': append(d,"\"Y#"); break;
		case '<': append(d,"\"L#"); break;
		case '>': append(d,"\"G#"); break;
		case '}': *d++ = '!';	break;
		case '{': /* Greek */
			*d++ = '[';
			while((c=*s++)){
				if(c=='}'){
					*d++ = ']';
					break;
				}else{
					*d++ = c;
				}
			}
			break;
		case '^': /* upper index */
			*d++ = '^';
			if(*s=='{')
				s++;
			else{
				*d++ = *s++;
				*d++ = '!';
			}
			break;
		case '_': /* lower index */
			*d++ = '?';
			if(*s=='{')
				s++;
			else{
				*d++ = *s++;
				*d++ = '!';
			}
			break;
		default:
			*d++ = c;
		}
	}
	*d = 0x00;
	return(newtext);
}

#if USE_PAW
void output_alias(p)
		 hist_p  p;
{
	extern FILE *falias;
  if(!falias){
    open_alias_file();
	}
	if(falias){
		fprintf(falias, "alias/create   %-20s %d\n", p->sname, p->id);
		fprintf(falias, "alias/create   NAM%d '!%s'\n", p->id, p->sname);
		fprintf(falias, "alias/create   TTL%d '%s'\n", p->id, cvtext(p->name));
		fprintf(falias, "alias/create   TAX%d '%s'\n", p->id, cvtext(p->xlabel));
		fprintf(falias, "alias/create   TAY%d '%s'\n", p->id, cvtext(p->ylabel));
		fflush(falias);
	}
}
#endif
#if USE_ROOT
void cvtolower(d,s)
		 char *d, *s;
{
	int i;
	char *d2, *s2;
	do {
		*d = tolower(*s++);
	}	while(*d++);
}

void output_alias(p)
		 hist_p  p;
{
	FILE *falias;
  char str[256];
  char fnam[256];
	cvtolower(str,p->sname);
	sprintf(fnam, "%s/%s", ROOT_ALIASDIR, str);
	falias = fopen(fnam, "w");
	if(falias==(FILE*)NULL){
		fprintf(stderr, "Warning: could not open alias file '%s'.\n", fnam);
		return;
	}
	fprintf(falias, "{\n");
#if 0
	fprintf(falias, "  shm  = TMapFile::Create(\"%s\");\n", ROOT_SHMNAM);
	fprintf(falias, "  hist = shm->Get(\"%s\");\n", p->sname);
	fprintf(falias, "  hist->Draw();\n");
#else
	fprintf(falias, "  plot(\"%s\");\n", p->sname);
#endif
	fprintf(falias, "}\n");
	fclose(falias);
}
#endif

void hist1(v0, sname, name, v, min, max, nbin, xlabel, ylabel, gate)
		 vrec_p  v0;
		 vrec_p  sname, name;
		 vrec_p  v, min, max, nbin, xlabel, ylabel;
		 vrec_p  gate;
{
	double dmin, dmax, dnbin;
	hist_p p;
	double d;
	char title[256];

	if(v0==(vrec_p)NULL){
		showerr("hist1: null pointer argument\n");
		return;
	}
	d = eval(v0);
	if(dr_is_nothing(d)) return;
	if(check_level(d)) return;

	p = search_hist(sname->sval);
	if(p){
		reuse_hist(p);
		showerr("Histogram #%3d '%s' is superseded.\n", p->id, sname->sval);
#if DEBUG_DEFINITION
#endif
	}else{
		p = new_hist();
		if(!p) return;
#if DEBUG_DEFINITION
		showerr("Create 1D-histogram #%3d '%s'.\n", p->id, sname->sval);
#endif
	}

	p->sname = strdup(sname->sval);
	p->name = strdup(name->sval);
	p->v1 = v;
	p->min1 = min;
	p->max1 = max;
	p->nbin1 = nbin;
	p->gate = gate;
	p->xlabel = strdup(xlabel->sval);
	p->ylabel = strdup(ylabel->sval);
	dmin  = eval(min);
	dmax  = eval(max);
	dnbin = eval(nbin);
	p->type = HIST_1D;
	if(dr_exists(dmin) && dr_exists(dmax) && dr_exists(dnbin)){
		hbk1(p->id, p->sname, cvtext(p->name), dmin, dmax, (int)dnbin);
#if USE_ROOT
    hxytitle(p->id, p->xlabel, p->ylabel);
#endif		
		output_alias(p);
	}else{
		showerr("Failed to create 1D histogram #%3d '%s'.\n",
							p->id, sname->sval);
		/* free allocated memories */
		p->id = 0;
		free_hist(p);
	}
}

void hist2(v0, sname, name, v1, min1, max1, nbin1, xlabel,v2, min2, max2, nbin2, ylabel, gate)
		 vrec_p  v0;
		 vrec_p  sname, name;
		 vrec_p  v1, min1, max1, nbin1, xlabel;
		 vrec_p  v2, min2, max2, nbin2, ylabel;
		 vrec_p  gate;
{
	double dmin1, dmax1, dnbin1;
	double dmin2, dmax2, dnbin2;
	hist_p p;
	double d;
	
	if(v0==(vrec_p)NULL){
		showerr("hist1: null pointer argument\n");
		return;
	}
	d = eval(v0);
	if(dr_is_nothing(d)) return;
	if(check_level(d)) return;

	p = search_hist(sname->sval);
	if(p){
		reuse_hist(p);
		showerr("Histogram #%3d '%s' is superseded.\n", p->id, sname->sval);
#if DEBUG_DEFINITION
#endif
	}else{
		p = new_hist();
		if(!p) return;
#if DEBUG_DEFINITION
		showerr("Create 2D-histogram #%3d '%s'.\n", p->id, sname->sval);
#endif
	}

	p->sname  = strdup(sname->sval);
	p->name   = strdup(name->sval);
	p->v1     = v1;
	p->min1   = min1;
	p->max1   = max1;
	p->nbin1  = nbin1;
	p->xlabel = strdup(xlabel->sval);
	p->v2     = v2;
	p->min2   = min2;
	p->max2   = max2;
	p->nbin2  = nbin2;
	p->ylabel = strdup(ylabel->sval);
	p->gate   = gate;
	p->type = HIST_2D;
	dmin1  = eval(min1);
	dmax1  = eval(max1);
	dnbin1 = eval(nbin1);
	dmin2  = eval(min2);
	dmax2  = eval(max2);
	dnbin2 = eval(nbin2);
	if(dr_exists(dmin1) && dr_exists(dmax1) && dr_exists(dnbin1) &&
		 dr_exists(dmin2) && dr_exists(dmax2) && dr_exists(dnbin2)){
		hbk2(p->id, p->sname, cvtext(p->name),
				 dmin1, dmax1, (int)dnbin1,
				 dmin2, dmax2, (int)dnbin2);
		output_alias(p);
#if USE_ROOT
    hxytitle(p->id, p->xlabel, p->ylabel);
#endif		
	}else{
		showerr("Failed to create 2D histogram #%3d '%s'.\n",
						p->id, sname->sval);
 		/* free allocated memories */
		p->id = 0;
		free_hist(p);
	}
}

/* Histogram Output */
int histogram(){
	int      i, j, n;
	double   f;

	hist_p   h;
	double   g, d, d1, d2;
	vrec_p   v, v1, v2;

	h = hists;
	for(i=0; i<nhists; i++, h++){
		switch(h->type){
		case HIST_NONE:
			break;
		case HIST_1D:
			if(h->gate){
				g = eval(h->gate);
				if(dr_is_nothing(g)) break;
				if(dr_is_false(g)) break;
			}
			v = h->v1;
			if(v->vtype==NAME2){
				(void)eval(v);
				j = dr_min_r(&dr_data,&v->vref,v->sval,0);
				n = dr_max_r(&dr_data,&v->vref,v->sval,0);
				for(;j<n;j++){
					d = dr_get_r(&dr_data,&v->vref,v->sval,j);
					if(!dr_is_nothing(d)){
						hfil1(h->id,d);
					}
				}
			}else{
				d = eval(v);
				if(!dr_is_nothing(d))
					hfil1(h->id,d);
			}
			break;
		case HIST_2D:
			if(h->gate){
				g = eval(h->gate);
				if(dr_is_nothing(g)) break;
				if(dr_is_false(g)) break;
			}
			v1 = h->v1;
			v2 = h->v2;
			if(v1->vtype==NAME2){
				if(v2->vtype==NAME2){
           /* v1 = multi data, v2 = multi data */
					(void)eval(v1);
					(void)eval(v2);
					j = max(dr_min_r(&dr_data,&v1->vref,v1->sval,0),
					        dr_min_r(&dr_data,&v2->vref,v2->sval,0));
					n = min(dr_max_r(&dr_data,&v1->vref,v1->sval,0),
					        dr_max_r(&dr_data,&v2->vref,v2->sval,0));
					for(;j<n;j++){
						d1 = dr_get_r(&dr_data,&v1->vref,v1->sval,j);
						d2 = dr_get_r(&dr_data,&v2->vref,v2->sval,j);
						if(!dr_is_nothing(d1) && !dr_is_nothing(d2)){
							hfil2(h->id,d1,d2);
						}
					}

				}else{
           /* v1 = multi data, v2 = single data */
					(void)eval(v1);
					d2 = eval(v2);
					if(!dr_is_nothing(d2)){
						j = dr_min_r(&dr_data,&v1->vref,v1->sval,0);
						n = dr_max_r(&dr_data,&v1->vref,v1->sval,0);
						for(;j<n;j++){
							d1 = dr_get_r(&dr_data,&v1->vref,v1->sval,j);
							if(!dr_is_nothing(d1)){
								hfil2(h->id,d1,d2);
							}
						}
					}
				}
			}else{
				if(v2->vtype==NAME2){
           /* v1 = single data, v2 = multi data */
					d1 = eval(v1);
					if(!dr_is_nothing(d1)){
						(void)eval(v2);
						j = dr_min_r(&dr_data,&v2->vref,v2->sval,0);
						n = dr_max_r(&dr_data,&v2->vref,v2->sval,0);
						for(;j<n;j++){
							d2 = dr_get_r(&dr_data,&v2->vref,v2->sval,j);
							if(!dr_is_nothing(d2)){
								hfil2(h->id,d1,d2);
							}
						}
					}
				}else{
           /* v1 = single data, v2 = single data */
					d1 = eval(v1);
					d2 = eval(v2);
					if(!dr_is_nothing(d1) && !dr_is_nothing(d2))
						hfil2(h->id,d1,d2);
				}
			}
			break;
		default:
			showerr("histogram: unknown histogram type %d\n", h->type);
			break;
		}
	}
	return(0);
}


double fint(d)
		 double d;
{
	if(!dr_exists(d))
		return(d);
	return((double)((int)d));
}

double fanalog(d)
		 double d;
{
	if(!dr_exists(d))
		return(d);
	return((double)(d+drand48()-0.5));
}

double fbool(d)
		 double d;
{
	if(!dr_exists(d) || dr_is_zero(d))
		return(0.);
	return(1.);
}

double pow2(d)
		 double d;
{
	double t;
	if(!dr_exists(d)){
		dr_set_nothing(&t);
		return(t);
	}
	return(d*d);
}

double f_gate(arg1, arg2, arg3)
		 double arg1, arg2, arg3;
{
	int res;
	res = (arg2 <= arg1 && arg1 < arg3);
	return((double)res);
}

double f_bit(arg1, arg2)
		 double arg1, arg2;
{
	int  x, bit, res;
	x   = (int)arg1;
	bit = (int)arg2;
	res = (x & (1<<bit))!=0;
	return((double)res);
}

double f_if(arg1, arg2, arg3)
		 double arg1, arg2, arg3;
{
	double res;
	if(dr_is_false(arg1))
		res = arg3;
	else
		res = arg2;
	return(res);
}

/* f_polynom() ... added on 14-OCT-2003 */
#if !(MaxNArgs-4)
double f_polynom(arg1, arg2, arg3, arg4, v1, v2, v3, v4)
		 double arg1, arg2, arg3, arg4;
		 vrec_p v1, v2, v3, v4;
{
	double d, dp, c, res;
	int    min, max, j;

	d = arg1;

	if(dr_is_nothing(d))
		return(d);

	if(!v2->sval){
		fprintf(stderr, "Error in the expression of arg2 of the polynom()\n");
		return(0.);
	}

	min = dr_min_r(&dr_data,&v2->vref,v2->sval,0);
	max = dr_max_r(&dr_data,&v2->vref,v2->sval,0);
	res = 0.;

	if(min<0){
		if(d==0.){
			dr_set_nothing(&res);
			return(res);
		}
		dp = 1.;
		for(j=-1;max<=j;j--)
			dp /= d;
		for(;min<=j;j--){
			dp /= d;
			c = dr_get_r(&dr_data,&v2->vref,v2->sval,j);
			if(dr_exists(c))
				 res += c*dp;
		}
	}
	if(0<max){
		dp = 1.;
		for(j=0;j<min;j++)
			dp *= d;
		for(;j<max;j++){
			c = dr_get_r(&dr_data,&v2->vref,v2->sval,j);
			if(dr_exists(c))
				res += c*dp;
			dp *= d;
		}
	}
	return(res);
}
#else
Error in the above procedure.
#endif


int dr_init_func(){
	extern double sin(), cos(), tan();
	extern double acos(), asin(), atan();
	extern double sinh(), cosh(), tanh();
	extern double log(), log10(), exp();
  extern double sqrt(), fabs(), pow2();
	extern double fint();
	extern double fanalog();
	extern double fbool();
	dr_add_func("sin",   sin);
	dr_add_func("pow2",  pow2);
	dr_add_func("cos",   cos);
	dr_add_func("tan",   tan);
	dr_add_func("asin",  asin);
	dr_add_func("acos",  acos);
	dr_add_func("atan",  atan);
	dr_add_func("sinh",  sinh);
	dr_add_func("cosh",  cosh);
	dr_add_func("tanh",  tanh);
	dr_add_func("log",   log10);
	dr_add_func("ln",    log);
	dr_add_func("exp",   exp);
	dr_add_func("sqrt",  sqrt);
  dr_add_func("abs",   fabs);
  dr_add_func("int",   fint);
  dr_add_func("analog",fanalog);
  dr_add_func("bool",  fbool);

	dr_add_func("gate",  f_gate);
	dr_add_func("bit",   f_bit);
	dr_add_func("if",    f_if);
	dr_add_func("polynom",  f_polynom);
	srand48(0);
    return 0;
}

int dr_init(){
  dr_data.n = 1;
  dr_data.nmax = 0;
  dr_data.rec = (dr_rec_t*)NULL;
	level = 0;
	dr_init_func();
  return(0);
}

int dr_exit(){
  event_exit();
  return(0);
}

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
