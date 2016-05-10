/*
  histogram.c ---- histogram
  Version 1.00  02-JUN-2000  by A. Tamii
*/

#ifndef DBL_MAX
#define DBL_MAX     1.0E+37
#endif
#define DR_NOTHING  DBL_MAX
#define DR_ZERO     (1.0e-20)

#define DR_MAX_DATA_N  10000
#define DR_MAX_ARRAY_INDEX 100000
#define DR_MIN_ARRAY_INDEX -100000

#define MaxNDSTVar    1000    /* Max Number of DST output variables */

#if defined (__cplusplus)
extern "C" {
#endif

enum DR_TYPE {
  DR_UNDEF=0,   /* undefined */
  DR_CONST,     /* constant */
  DR_CALC,      /* calculation */
  DR_USER,      /* user calculation */
  DR_NTYPE      /* number of types */
};

/* data (variable) record */
typedef struct {
  char   *nam;         /* variable name */
  int    type;         /* type */
  int    imin;         /* index minimum */
  int    imax;         /* index maximum +1*/
  int    amin;         /* index minimum accessed */
  int    amax;         /* index maximum accessed +1*/
  int    nvalue;       /* number of values (=imax-imin) */
  double *value;       /* values */
	int    eval;         /* evaluation type (enum eval) */
	int    neval;        /* evaluation level */
  int    nreg;         /* number of references */
  int    **reg;        /* reference pointers */
	int    nvrec;        /* number of definitions */
	vrec_p *vrec;        /* definitions */
} dr_rec_t;

/* data record handle */
typedef struct {
  int      n;
  int      nmax;
  dr_rec_t *rec;
} dr_t;

#define DR_TRUE  1.0
#define DR_FALSE 0.0

#define dr_ref(nam,h) \
  (dr_ref_r(&dr_data,h,#nam))

#define dr_ref_n(nam,h) \
  (dr_ref_r(&dr_data,h,nam))

#define dr_set_ref(h,i,val)    (dr_set_r(&dr_data,&(h),"",(i),val))
#define dr_append_ref(h,val)   (dr_append_r(&dr_data,&(h),"",0,val))
#define dr_get_ref(h,i)        (dr_get_r(&dr_data,&(h),"",(i)))
#define dr_max_ref(h)          (dr_max_r(&dr_data,&(h),"",0))
#define dr_min_ref(h)          (dr_min_r(&dr_data,&(h),"",0))

#define dr_set_nothing(val) {*((double *)val) = (DR_NOTHING);}
#define dr_is_nothing(val)  ((val) >= (DR_NOTHING))
#define dr_exists(val)      ((val) < (DR_NOTHING))
#define dr_int(val)         (dr_exists(val) ? (int)((val)+0.5) : -1)
#define dr_is_zero(val)     (fabs((double)(val)) < DR_ZERO)
#define dr_is_false(val)    (dr_is_nothing(val) || dr_is_zero(val))
#define dr_is_true(val)     (!dr_is_false(val))
#define dr_const(v)         (((v)==(vrec_p)NULL || (v)->eval==EVAL_CONST) ? \
                             EVAL_CONST : EVAL_NONE);
#define dr_const2(v1,v2)    (((v1)==(vrec_p)NULL || (v1)->eval==EVAL_CONST) &&\
														 ((v2)==(vrec_p)NULL || (v2)->eval==EVAL_CONST) ? \
                             EVAL_CONST : EVAL_NONE);
#define dr_show()   dr_show_r(&dr_data)
#define dr_clear()  dr_clear_r(&dr_data)
#define dr_reinit() dr_reinit_r(&dr_data)

#define min(a,b) ((a)<=(b) ? (a) : (b))
#define max(a,b) ((a)>=(b) ? (a) : (b))

extern dr_t dr_data;

int    dr_init(void);
void   dr_event();
int    dr_set_r(dr_t*, int *, char const *, int, double);
double dr_get_r(dr_t*, int *, char const *, int);
vrec_p dr_get_vrec_r(dr_t*, int *, char const *, int);
char  *dr_get_sval_r(dr_t*, int *, char const *, int);
int    dr_min_r(dr_t*, int *, char const *, int);
int    dr_max_r(dr_t*, int *, char const *, int);
int    dr_nval_r(dr_t*, int *, char const *, int);
int    dr_append_r(dr_t*, int *, char const *, int, double);
int    dr_exit(void);
int    dr_ref_r(dr_t*, int *, char const *);

void   dr_show_r(dr_t*);
int    dr_clear_r(dr_t*);
int    dr_reinit_r(dr_t*);
int    dr_set_vrec(dr_t*, int *, char const *, vrec_p);

void free_vrecp(vrec_p v);

void hist1(vrec_p v0, vrec_p sname, vrec_p name, vrec_p v, vrec_p min, vrec_p max, vrec_p nbin,
           vrec_p xlabel, vrec_p ylabel, vrec_p gate);
void hist2(vrec_p v0, vrec_p sname, vrec_p name, vrec_p v1, vrec_p min1, vrec_p max1, vrec_p nbin1,
           vrec_p xlabel, vrec_p v2, vrec_p min2, vrec_p max2, vrec_p nbin2, vrec_p ylabel, vrec_p gate);
void def(vrec_p v0, vrec_p v1, vrec_p v2, vrec_p v3);
void com(vrec_p v0);
int yyerror(char*);
int yylex(void);

double eval(vrec_p);
char *spf(char*,...);

#if defined (__cplusplus)
}
#endif

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
