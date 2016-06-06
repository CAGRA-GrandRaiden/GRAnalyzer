/*
  analyzer.h ---- histogram
  Version 1.00  10-JUL-2000  by A. Tamii
*/

#define PI     M_PI
#define r2d    (180./PI)
#define d2r    (PI/180.)

#define Clst_MaxNData    128
#define Clst_MaxNClst     16
#define Clst_MaxClstSize 100	

#define Clst_DefMaxClstSize  10
#define Clst_DefMinClstSize   2

#define Clst_DefMaxNComb   1000 /* default max number of combinations */

#define ENDRUNBLK   0     /* End of Fread at the run end block */

#define LAS_FPP_MWDC    1
#define NP_MWDC         0
#define F2_MWDC         0

#if F2_MWDC
#define N_MWDC_PL       8      /* number of MWDC planes */
#endif

#define MWDC_MIN_PLHITS 5    /* MWDC minimum number of plane hits for ray
																tracing */

#define DT2D_TAB        1    /* use a table for drift time to distance (1)
	                              conversion, otherwise (0) use polynomial fit */
#define DT2D_TAB_ANALOG 1    /* make tdc digital data to analog values by
																adding a random number */

#define FNAMGRRAY      "grray.dat"   /* output file of rays */
#define FNAMLASRAY     "lasray.dat"  /* output file of rays */

#define VDC_ALLOW_HITSKIP  1   /* Allow a wire hit incontinuity in clusterization */

/* Position Calculation ID */
#define POSID_CONFG  -10         /* Plane configuration error */
#define POSID_UNDEF   -2         /* Not analyzed yet */
#define POSID_SIMPLE  -1         /* Simplified fitting */
#define POSID_NOERR    0         /* Good position determination */
#define POSID_DRIFT    1         /* Drift time order inconsistency */
#define POSID_MONOTONE 2         /* Monotone dec/increase drift time */
#define POSID_LFIT     3         /* Error in the fitting to get the position */
#define POSID_NOHIT   10         /* No legal hit in a cluster */
#define POSID_1HIT    11         /* 1 hit in a cluster */
#define POSID_2HIT    12         /* 2 hits in a cluster */
#define POSID_MHIT    14         /* too many hits in a cluster */
#define POSID_NOCLUST 20         /* No Cluster */
#define POSID_MCLUST  20         /* Multi Cluster (+ number of clusters)*/

/* Ray Calculation ID */
#define RAYID_NOERR    0
/*
   1 - 15                        No Chamber Hit
     bit#0  chamber-1
     bit#1  chamber-2
     bit#2  chamber-3
     bit#3  chamber-4
*/
#define RAYID_MCLUST  20         /* Multi Cluster (>=21)*/
                                 /* 20+number of multi-cluster planes */
#define RAYID_UNDEF   -1         /* not analyzed yet */
#define RAYID_TRACE   -2         /* Tracing error (matrix inversion) */
#define RAYID_FEWHIT  -4         /* Too few plane hits */
#define RAYID_NCOMB   -6         /* Too many combinations */
#define RAYID_VTPOS   -8         /* Error in setting vertex position only in ray_4p_fpp_v() */
#define RAYID_CONFG  -10         /* Plane configuration error */

/****** --- Ray analysis --- ******/



#define NCHAMB_4P     4
#define NCHAMB_2P     2
#define DIM           4
#define MaxNConv   1030    /* max number of items in conversion table */
#define TB_MAX_POWER  4    /* max power of trace-back matrix */
#define MaxNIntPl 10       /* max number of intersection planes for each
															spectrometer */

#if defined (__cplusplus)
extern "C" {
#endif
/* vector record */
typedef struct vec_rec{
	double   x;
	double   y;
	double   z;
} vec_rec, *vec_p;

/* wire plane information */
typedef struct wire {
  int      ok;       /* the following configurations are ok or not */
	double   disp;     /* sense wire displacement */
	double   rot;      /* wire rotation from vertical-axis along z-axis */
	double   dl;       /* maximum drift length */
	vec_rec  org;      /* coordinate of the origin */
	vec_rec  dsp;      /* displacement of a channel */
  vec_rec  dir;      /* direction of the wire */
	vec_rec  drf;      /* drift length */
  double   sc;       /* coefficient to calculate gradient = |dsp|/|drf| */
} wire_t, *wire_p;

/* ray information */
typedef struct ray {
	int      rayid;    /* ray definition ID */
  vec_rec  org;      /* coordinate of a point in the ray */
	vec_rec  dir;      /* direction of the ray */
} ray_t, *ray_p;

/* trace-back matrix */
typedef struct tbmat {
	double   c;        /* coefficient */
	int      x;        /* order of x */
	int      a;        /* order of a */
	int      y;        /* order of y */
	int      b;        /* order of b */
} tbmat_t, *tbmat_p;
			
#if DT2D_TAB
/* drift time to distance table */
typedef struct dt2dtab{
  int    start;          /* legal region start TDC */
  int    end;            /* legal region end   TDC */
	int    nitem;          /* number of conversion items */
	int    offset;         /* offset */
	double conv[MaxNConv]; /* conversion values for start<=tdc<end */
} dt2dtab;
#else
/* drift time to distance parameters */
typedef struct dt2dprm{
	int   t[3];   /* conversion tdc points */
	double p1[4]; /* conversion parameters for t[0]<t<t[1] */
	double p2[4]; /* conversion parameters for t[1]<t<t[2] */
} dt2dprm;

#endif

/* cluster information */

typedef struct clst {
	int     size;                   /* cluster size */
	int     wire[Clst_MaxClstSize]; /* hit wire */
	int     tdc[Clst_MaxClstSize];  /* tdc */
	double  dist[Clst_MaxClstSize]; /* distance */
	double  grad;                   /* slope */
	double  pos;                    /* position */
	double  pos1;                   /* position for LR-ambiguity (1) */
	double  pos2;                   /* position for LR-ambiguity (2) */
	int     posid;                  /* position definition ID */
} clst_t, *clst_p;

typedef struct plane {
	char      *name;            /* name of the plane */
	struct chamber *chamb;      /* pointer to the parent chamber */
	int       nc;               /* number of clusters */
  int       ic;               /* selected cluster */
	int       fit;              /* used for ray fitting */
#if DT2D_TAB
	dt2dtab   tab;              /* drift time to distance table */
#else
	dt2dprm   prm;              /* drift time to distance parameters */
#endif
	clst_t    clst[Clst_MaxNClst]; /* cluster information        */
	wire_t    wire;             /* anode plane information       */
	double    pres;             /* position resolution of a plane */
	double    ares;             /* angular resolution of a plane */
	int       min_clst_size;    /* minimum cluster size          */
	int       max_clst_size;    /* maximum cluster size          */
	int       ana_2hit_clst;    /* analyze 2hit cluster          */
	int       h_wire;           /* reference to variable WIRE    */
	int       h_wirer;          /* reference to variable WIRER   */
	int       h_tdc;            /* reference to variable TDC     */
	int       h_tdcr;           /* reference to variable TDCR    */
	int       h_clstsz;         /* reference to variable CLUSTSZ */
	int       h_nhit;           /* reference to variable NHIT    */
	int       h_nclst;          /* reference to variable NCLUST  */
	int       h_drift;          /* reference to variable DRIFT   */
	int       h_posid;          /* reference to variable POSID   */
	int       h_pos;            /* reference to variable POS     */
	int       h_grad;           /* reference to variable GRAD    */
} plane_t, *plane_p;

typedef struct chamber {
  int       type;             /* type of the chamber */
	char      *name;            /* name of the chamber */
	int       npl;              /* number of planes */
  plane_p   plane;            /* plane information */
	int       ana_mult_clst;    /* multi cluster analysis is on(1) or off(0) */
	double    max_ncomb;        /* max number of combinations allowed */
	double    max_nclust;       /* max number of clusters in a plane */
	int       ncomb;            /* number of combinations */
	double    m[DIM][DIM+1];    /* ray finding matrix */
	double    max_chi2;         /* max allowed chi quare */
	double    chi2;             /* chi square of the ray fit */
	int       hitpat;           /* hit pattern */
	int       anapat;           /* analysis pattern */
	void      *matrix;          /* solved matrices for hit patterns*/
	void      *mb;              /* for solving matrix */
	void      *mc;              /* for solving matrix */
	int       allow_wireskip;   /* allow wire skip for ray fitting */
	ray_t     ray;              /* reconstructed ray */
	int       nproj;            /* number of projection planes */
	int       h_proj[MaxNIntPl];/* reference to projection planes */
	int       h_x;              /* reference to projection point x */
	int       h_y;              /* reference to projection point y */
} chamber_t, *chamber_p;

enum chamb_type {
  CHANB_UNKOWN = 0,           /* unknown chamber type */
	CHAMB_MWDC,                 /* MWDC */
  CHAMB_VDC,                  /* VDC */
	CHAMB_MWPC                  /* MWPC */
};


/****** --- VDC cluster analysis --- ******/

static char const *vplnam[4] = {
  "X1", "U1", "X2", "U2"
};

static char const *spnam[2] = {
  "GR", "LAS"
};


/****** --- function prototype definitions --- ******/

double analog(int);
int    init_plane_config(plane_p);
int    ray_4p(chamber_p);
int    ray_4p_mult(chamber_p);
int    init_hist_vdc();
int    event_vdc_init();

/* chamb.c */
int chamb_get_dt2d_tab(chamber_p chamb);
int chamb_clust(plane_p plane);
int chamb_drift(plane_p plane);
int chamb_pos(plane_p plane);
int chamb_outputray(chamber_p chamb, FILE **fd, char *fnam, int *nrays);
int chamb_intersection(chamber_p chamb);
int chamb_init_chamb(chamber_p chamb);
int chamb_get_config(chamber_p chamb);
int chamb_init_hist(chamber_p chamb);
int chamb_ray(chamber_p);

/* vdc.c */
int chamb_vdc_clust(plane_p plane);
int chamb_vdc_pos(plane_p plane);

/* mwdc.c */
int chamb_mwdc_clust(plane_p plane);
int chamb_mwdc_pos(plane_p plane);

/* mwpc.c */
int chamb_mwpc_clust(plane_p plane);
int chamb_mwpc_pos(plane_p plane);

/* ray.c */
void show_vec(vec_p vec);
int ray_rotate_x(ray_p ray, double ang);
int ray_rotate_y(ray_p ray, double ang);
int ray_rotate_z(ray_p ray, double ang);
int ray_intersection(ray_p ray, vec_p plane, vec_p pt);
int ray_intersection_pt(ray_p ray, vec_p plane, vec_p pt);
int ray_4p(chamber_p chamb);
int ray_4p_mult(chamber_p chamb);
int    get_tb_matrix(char *, tbmat_p *mat_h);
double trace_back(ray_p ray, tbmat_p mat, int nmat, int flag);
int ray_mwdc(chamber_p chamb);
int ray_vertex(ray_p r1, ray_p r2, vec_p p, double *dist);
int ray_4p_fpp_mult(chamber_p chamb, double (*chi2_func)(ray_p));
int ray_scat_ang(ray_p r1, ray_p r2, double *ath, double *aph, double *hor, double *ver);
int ray_mwdc_fit(chamber_p chamb);

#if defined (__cplusplus)
}
#endif

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
