/*
  chamb_rcnp.c ... rcnp chamber analysis
  Version 1.00  10-JUL-2000  by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>  /* for drand48() */
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"

int gr_n_outputray = 0;
FILE *fd_gr_outputray = (FILE*)NULL;
int las_n_outputray = 0;
FILE *fd_las_outputray = (FILE*)NULL;

static int dr_data_index[61];

struct chamber gr_vdc;
struct chamber las_vdc;

tbmat_p gr_xmat = (tbmat_p)NULL;
tbmat_p gr_amat = (tbmat_p)NULL;
tbmat_p gr_bmat = (tbmat_p)NULL;
tbmat_p gr_ymat = (tbmat_p)NULL;
tbmat_p gr_phmat = (tbmat_p)NULL;
int     n_gr_xmat = 0;
int     n_gr_amat = 0;
int     n_gr_bmat = 0;
int     n_gr_ymat = 0;
int     n_gr_phmat = 0;

tbmat_p las_xmat = (tbmat_p)NULL;
tbmat_p las_amat = (tbmat_p)NULL;
int     n_las_xmat = 0;
int     n_las_amat = 0;

/* initialize data before an event (common)*/
int evt_init_rcnp()
{
	/* nothing to do */
    return 0;
}

/* initialize data before an event (for GR)*/
int evt_init_gr()
{
	chamb_init_chamb(&gr_vdc);
    return 0;
}

/* initialize data before an event (for LAS)*/
int evt_init_las()
{
	chamb_init_chamb(&las_vdc);
    return 0;
}

/* GR-3377 data analysis */
int evt_chamb_gr(){
	int  i;
	chamber_p  chamb;
	plane_p    plane;
	ray_p    ray;
	ray_t    ray2;
	double   d;
	double   shift;

	chamb = &gr_vdc;
	ray   = &chamb->ray;
	if(!dr_exists(dr_get_r(&dr_data,&dr_data_index[0],"GR_RAYID",0))){
		for(i=0; i<chamb->npl; i++){
			plane = &chamb->plane[i];
			chamb_clust(plane);
			chamb_drift(plane);
			chamb_pos(plane);
		}
		chamb_ray(chamb);	
		dr_set_r(&dr_data,&dr_data_index[1],"GR_RAYID",0,ray->rayid);
		dr_set_r(&dr_data,&dr_data_index[2],"GR_CHI2",0,chamb->chi2);
		dr_set_r(&dr_data,&dr_data_index[3],"GR_NCOMB",0,chamb->ncomb);
		if(ray->rayid==RAYID_NOERR){
			//			fprintf(stderr, "GRAD_X = %g\n", ray->dir.x);
			dr_set_r(&dr_data,&dr_data_index[4],"GR_GRAD_X",0,ray->dir.x);
			dr_set_r(&dr_data,&dr_data_index[5],"GR_GRAD_Y",0,ray->dir.y);
			/* rotation of axes as z=central-ray */
			ray_rotate_z(ray,dr_get_r(&dr_data,&dr_data_index[6],"GR_VDC_TILT",2));
			ray_rotate_y(ray,dr_get_r(&dr_data,&dr_data_index[7],"GR_VDC_TILT",1));
			ray_rotate_x(ray,dr_get_r(&dr_data,&dr_data_index[8],"GR_VDC_TILT",0));
			/* output of tracking information  */
			if(gr_n_outputray)
				chamb_outputray(chamb,&fd_gr_outputray,FNAMGRRAY,&gr_n_outputray);
			/* output of ray information */
			dr_set_r(&dr_data,&dr_data_index[9],"GR_RAY_X",0, ray->org.x);
			dr_set_r(&dr_data,&dr_data_index[10],"GR_RAY_Y",0, ray->org.y);
			dr_set_r(&dr_data,&dr_data_index[11],"GR_RAY_TH",0,atan(ray->dir.x));
			dr_set_r(&dr_data,&dr_data_index[12],"GR_RAY_PH",0,atan(ray->dir.y));
		}
	}else{
		ray->org.x = dr_get_r(&dr_data,&dr_data_index[13],"GR_RAY_X",0);
		ray->org.y = dr_get_r(&dr_data,&dr_data_index[14],"GR_RAY_Y",0);
		ray->org.z = 0.0;
		ray->dir.x = tan(dr_get_r(&dr_data,&dr_data_index[15],"GR_RAY_TH",0));
		ray->dir.y = tan(dr_get_r(&dr_data,&dr_data_index[16],"GR_RAY_PH",0));
		ray->dir.z = 1.0;
		ray->rayid = dr_int(dr_get_r(&dr_data,&dr_data_index[17],"GR_RAYID",0));
	}
	if(ray->rayid==RAYID_NOERR){
    /* projection to virtual planes */
		chamb_intersection(chamb);
		if(dr_is_true(dr_get_r(&dr_data,&dr_data_index[18],"ANALYZE_GR_MATRIX",0))){
			/* trace-back by matrix
				 x  and  y are in mm  in the VDC cordinate system
				    (GR_X[0] and GR_Y[0])
				 th and ph are in rad in the central ray cordinate system
				    (GR_TH[0] and GR_PH[0])
			*/
			ray2.org.x = dr_get_r(&dr_data,&dr_data_index[19],"GR_X",0);
			ray2.org.y = dr_get_r(&dr_data,&dr_data_index[20],"GR_Y",0);
			ray2.dir.x = dr_get_r(&dr_data,&dr_data_index[21],"GR_TH",0);
			ray2.dir.y = dr_get_r(&dr_data,&dr_data_index[22],"GR_PH",0);
			dr_set_r(&dr_data,&dr_data_index[23],"GR_XC",0, trace_back(&ray2, gr_xmat,  n_gr_xmat,  1));
			dr_set_r(&dr_data,&dr_data_index[24],"GR_AI",0, trace_back(&ray2, gr_amat,  n_gr_amat,  1));
			dr_set_r(&dr_data,&dr_data_index[25],"GR_BI",0, trace_back(&ray2, gr_bmat,  n_gr_bmat,  1));
			dr_set_r(&dr_data,&dr_data_index[26],"GR_YC",0, trace_back(&ray2, gr_ymat,  n_gr_ymat,  1));
			dr_set_r(&dr_data,&dr_data_index[27],"GR_PHC",0,trace_back(&ray2, gr_phmat, n_gr_phmat, 1));
		}
	}
	return(0);
}

/* LAS-3377 data analysis */
int evt_chamb_las(){
	double  f1, f2;
	int  i;
	chamber_p  chamb;
	plane_p    plane;
	ray_p      ray;

	chamb = &las_vdc;
	ray   = &chamb->ray;
	for(i=0; i<chamb->npl; i++){
		plane = &chamb->plane[i];
		chamb_clust(plane);
    chamb_drift(plane);
    chamb_pos(plane);
	}

	/* Temporary for E154 03-MAY-2006 */ 
	i = chamb->npl;
	chamb->npl = 4;
	chamb_ray(chamb);	
	chamb->npl = i;
	
	dr_set_r(&dr_data,&dr_data_index[28],"LAS_RAYID",0,ray->rayid);
	dr_set_r(&dr_data,&dr_data_index[29],"LAS_CHI2",0,chamb->chi2);
	dr_set_r(&dr_data,&dr_data_index[30],"LAS_NCOMB",0,chamb->ncomb);
	if(ray->rayid==RAYID_NOERR){
    /* trace-back by matrix (with the VDC coordingate space) */
		dr_set_r(&dr_data,&dr_data_index[31],"LAS_DP",0,trace_back(ray, las_xmat, n_las_xmat, 0));
		dr_set_r(&dr_data,&dr_data_index[32],"LAS_AI",0,trace_back(ray, las_amat, n_las_amat, 0)/1000.);
		dr_set_r(&dr_data,&dr_data_index[33],"LAS_GRAD_X",0, ray->dir.x/ray->dir.z);
		dr_set_r(&dr_data,&dr_data_index[34],"LAS_GRAD_Y",0, ray->dir.y/ray->dir.z);
    /* rotation of axes as z=central-ray */
		ray_rotate_z(ray,dr_get_r(&dr_data,&dr_data_index[35],"LAS_VDC_TILT",2));
		ray_rotate_y(ray,dr_get_r(&dr_data,&dr_data_index[36],"LAS_VDC_TILT",1));
		ray_rotate_x(ray,dr_get_r(&dr_data,&dr_data_index[37],"LAS_VDC_TILT",0));
    /* output of tracking information  */
		if(las_n_outputray)
			chamb_outputray(chamb,&fd_las_outputray,FNAMLASRAY,&las_n_outputray);
    /* output of ray information */
		dr_set_r(&dr_data,&dr_data_index[38],"LAS_RAY_X",0, ray->org.x);
		dr_set_r(&dr_data,&dr_data_index[39],"LAS_RAY_Y",0, ray->org.y);
		dr_set_r(&dr_data,&dr_data_index[40],"LAS_RAY_TH",0,atan(ray->dir.x));
		dr_set_r(&dr_data,&dr_data_index[41],"LAS_RAY_PH",0,atan(ray->dir.y));
    /* projection to virtual planes */
		chamb_intersection(chamb);
	}
	return(0);
}

/* get GR trace-back matrix */
int get_gr_matrix(){
	int  n;
	int i;

	/* get matrix for x */
	n = get_tb_matrix("GR_XMAT", &gr_xmat);
	if(n>0) n_gr_xmat = n;

	/* get matrix for a */
	n = get_tb_matrix("GR_AMAT", &gr_amat);
	if(n>0) n_gr_amat = n;
	//for(i=0; i<20; i++)
	//	fprintf(stderr, "GR_AMAT_%02d = %15.3E\n", i+1, gr_amat[i].c);

	/* get matrix for b */
	n = get_tb_matrix("GR_BMAT", &gr_bmat);
	if(n>0) n_gr_bmat = n;
	//for(i=0; i<20; i++)
	//fprintf(stderr, "GR_BMAT_%02d = %15.3E\n", i+1, gr_bmat[i].c);

	/* get matrix for y */
	n = get_tb_matrix("GR_YMAT", &gr_ymat);
	if(n>0) n_gr_ymat = n;

	/* get matrix for ph */
	n = get_tb_matrix("GR_PHMAT", &gr_phmat);
	if(n>0) n_gr_phmat = n;

	return(0);
}

/* get LAS trace-back matrix */
int get_las_matrix(){
	int  n;

	/* get matrix for x */
	n = get_tb_matrix("LAS_XMAT", &las_xmat);
	if(n>0) n_las_xmat = n;

	/* get matrix for a */
	n = get_tb_matrix("LAS_AMAT", &las_amat);
	if(n>0) n_las_amat = n;

	return(0);
}


/* initialization of data before data analysis */
int evt_start_rcnp(){
	double    d;
	int       i, cmin, cmax, ipl;
	chamber_p chamb;
	plane_p   plane;

	/* common */

#if !DT2D_TAB
	get_dt2d_prm();
#endif

	/* for GR */

	if(dr_is_true(dr_get_r(&dr_data,&dr_data_index[42],"ANALYZE_GR",0))){
		cmax = dr_exists(d=dr_get_r(&dr_data,&dr_data_index[43],"GR_VDC_MAX_CLUST_SIZE",0)) ?
			min(dr_int(d),Clst_MaxClstSize) :  Clst_DefMaxClstSize;
		cmin = dr_exists(d=dr_get_r(&dr_data,&dr_data_index[44],"GR_VDC_MIN_CLUST_SIZE",0)) ?
			dr_int(d) : Clst_DefMinClstSize;

		chamb = &gr_vdc;
#if DT2D_TAB
		chamb_get_dt2d_tab(chamb);
#endif
		chamb_get_config(chamb);
		d = dr_get_r(&dr_data,&dr_data_index[45],"DEBUG_GR_N_OUTPUTRAY",0);
		gr_n_outputray  = dr_exists(d) ? (int)d : 0;
		if(dr_is_true(dr_get_r(&dr_data,&dr_data_index[46],"ANALYZE_GR_MATRIX",0))){
			get_gr_matrix();
		}
		for(ipl=0; ipl<chamb->npl; ipl++){
			plane = &chamb->plane[ipl];
			plane->min_clst_size = cmin;
			plane->max_clst_size = cmax;
			plane->ana_2hit_clst = dr_is_true(dr_get_r(&dr_data,&dr_data_index[47],"GR_VDC_ANA_2HIT_CLUST",0));
			/* angular resolution of a plane */
			plane->ares = dr_get_r(&dr_data,&dr_data_index[48],"GR_ARES",ipl)*d2r;
			if(dr_is_nothing(plane->ares)) plane->ares = 1.05*d2r; /* default */
		}
		chamb->ana_mult_clst = dr_is_true(dr_get_r(&dr_data,&dr_data_index[49],"GR_VDC_ANA_MULTI_CLUST",0));
		chamb->max_chi2 = dr_get_r(&dr_data,&dr_data_index[50],"GR_VDC_MAX_CHI2",0);
		if(dr_is_nothing(chamb->max_chi2))
			chamb->max_chi2 = 0.;
		d = dr_get_r(&dr_data,&dr_data_index[51],"GR_N_VPLANE",0);
		chamb->nproj = dr_exists(d) ? min((int)d, MaxNIntPl) : 0;
		for(i=0; i<chamb->nproj; i++)
			dr_ref_n(spf("GR_PLANE_%d",i), &chamb->h_proj[i]);
		dr_ref_n("GR_XP", &chamb->h_x); /* projection points */
		dr_ref_n("GR_YP", &chamb->h_y); /* projection points */

		fprintf(stderr, "GR VDC multi-cluster analysis = %s\n",
						chamb->ana_mult_clst ? "ON" : "OFF");
		fprintf(stderr, "GR VDC 2-hit cluster analysis = %s\n",
						chamb->plane[0].ana_2hit_clst ? "ON" : "OFF");
		fprintf(stderr, "GR VDC cluster size = %d <= nhit <= %d\n",
						cmin, cmax);
	}

	/* for LAS */

	if(dr_is_true(dr_get_r(&dr_data,&dr_data_index[52],"ANALYZE_LAS",0))){
		cmax = dr_exists(d=dr_get_r(&dr_data,&dr_data_index[53],"LAS_VDC_MAX_CLUST_SIZE",0)) ?
			min(dr_int(d),Clst_MaxClstSize) :  Clst_DefMaxClstSize;
		cmin = dr_exists(d=dr_get_r(&dr_data,&dr_data_index[54],"LAS_VDC_MIN_CLUST_SIZE",0)) ?
			dr_int(d) : Clst_DefMinClstSize;

		chamb = &las_vdc;
#if DT2D_TAB
		chamb_get_dt2d_tab(chamb);
#endif
		chamb_get_config(chamb);
		d = dr_get_r(&dr_data,&dr_data_index[55],"DEBUG_LAS_N_OUTPUTRAY",0);
		las_n_outputray  = dr_exists(d) ? (int)d : 0;
		get_las_matrix();
		for(ipl=0; ipl<chamb->npl; ipl++){
			plane = &chamb->plane[ipl];
			plane->min_clst_size = cmin;
			plane->max_clst_size = cmax;
			plane->ana_2hit_clst = dr_is_true(dr_get_r(&dr_data,&dr_data_index[56],"LAS_VDC_ANA_2HIT_CLUST",0));
			/* angular resolution of a plane */
			plane->ares = dr_get_r(&dr_data,&dr_data_index[57],"LAS_ARES",ipl)*d2r;
			if(dr_is_nothing(plane->ares)) plane->ares = 1.05*d2r; /* default */
		}
		d = dr_get_r(&dr_data,&dr_data_index[58],"LAS_N_VPLANE",0);
		chamb->nproj = dr_exists(d) ? min((int)d, MaxNIntPl) : 0;
		for(i=0; i<chamb->nproj; i++)
			dr_ref_n(spf("LAS_PLANE_%d",i), &chamb->h_proj[i]);
		dr_ref_n("LAS_XP", &chamb->h_x); /* projection points */
		dr_ref_n("LAS_YP", &chamb->h_y); /* projection points */
		chamb->ana_mult_clst = dr_is_true(dr_get_r(&dr_data,&dr_data_index[59],"LAS_VDC_ANA_MULTI_CLUST",0));
		chamb->max_chi2 = dr_get_r(&dr_data,&dr_data_index[60],"LAS_VDC_MAX_CHI2",0);
		if(dr_is_nothing(chamb->max_chi2))
			chamb->max_chi2 = 0.;

		fprintf(stderr, "LAS VDC multi-cluster analysis = %s\n",
						chamb->ana_mult_clst ? "ON" : "OFF");
		fprintf(stderr, "LAS VDC cluster size = %d <= nhit <= %d\n",
						cmin, cmax);
	}
	return(0);
}
	
/* initialize variables */
int init_hist_rcnp()
{
	chamber_p  chamb;
	int        ipl;
	/* initialyze for GR VDC */
	chamb = &gr_vdc;
	chamb->name = "GR";
	chamb->type = CHAMB_VDC;
	chamb->npl  = 4;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "X1";
	chamb->plane[1].name = "U1";
	chamb->plane[2].name = "X2";
	chamb->plane[3].name = "U2";
	for(ipl=0; ipl<chamb->npl; ipl++){
		chamb->plane[ipl].chamb = chamb;
	}
	chamb_init_hist(chamb);

	/* initialyze for LAS VDC */
	chamb = &las_vdc;
	chamb->name = "LAS";
	chamb->type = CHAMB_VDC;
#if 0
	chamb->npl  = 4;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "X1";
	chamb->plane[1].name = "U1";
	chamb->plane[2].name = "X2";
	chamb->plane[3].name = "U2";
#endif
#if 1 /* 04-JUL-2005 use UV planes */	
	chamb->npl  = 4;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "U1";
	chamb->plane[1].name = "V1";
	chamb->plane[2].name = "U2";
	chamb->plane[3].name = "V2";
#endif
#if 0 /* 04-JUL-2005 use XUV planes (tracking does not work)*/	
	chamb->npl  = 6;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "U1";
	chamb->plane[1].name = "V1";
	chamb->plane[2].name = "U2";
	chamb->plane[3].name = "V2";
	chamb->plane[4].name = "X1";
	chamb->plane[5].name = "X2";
#endif
	for(ipl=0; ipl<chamb->npl; ipl++){
		chamb->plane[ipl].chamb = chamb;
	}
	chamb_init_hist(chamb);
    return 0;
}

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
