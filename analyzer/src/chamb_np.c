/*
  mwdc_np.c ---- (n,p) wire chamber data analysis
  Version 1.00  26-SEP-2000  by A. Tamii
*/

#include <stdio.h>
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"
#include "matrix.h"

#if NP_MWDC

//FLAG

struct chamber tgc_mwdc;
struct chamber fec_mwdc;

/* initialize data before an event (for np)*/
int evt_init_np()
{
	chamb_init_chamb(&tgc_mwdc);
	chamb_init_chamb(&fec_mwdc);
}

/* NP-3377 data analysis */
int evt_chamb_np(){
	chamber_p chamb;
	plane_p   plane;
	int       ipl;
	ray_p     ray;
	double      d;

	chamb = &tgc_mwdc;
	ray   = &chamb->ray;
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
    chamb_clust(plane);
		chamb_drift(plane);
		chamb_pos(plane);
	}
	if(dr_exists(d=dr_get(LAS_RAYID)) && dr_int(d)==0){
		chamb_ray(chamb);
		dr_set(TGC_RAYID,ray->rayid);
		dr_set(TGC_CHI2[0],chamb->chi2);
		if(ray->rayid==RAYID_NOERR){
			/* rotation of axes as z=central-ray */
			ray_rotate_z(ray,dr_get(TGC_TILT[2]));
			ray_rotate_y(ray,dr_get(TGC_TILT[1]));
			ray_rotate_x(ray,dr_get(TGC_TILT[0]));
			/* output of ray information */
			dr_set(TGC_RAY_X[0], ray->org.x);
			dr_set(TGC_RAY_Y[0], ray->org.y);
			dr_set(TGC_TH[0],atan(ray->dir.x));
			dr_set(TGC_PH[0],atan(ray->dir.y));
			/* projection to virtual planes */
			chamb_intersection(chamb);
		}
	}

	chamb = &fec_mwdc;
	ray   = &chamb->ray;
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
    chamb_clust(plane);
		chamb_drift(plane);
		chamb_pos(plane);
	}
	if(dr_exists(d=dr_get(LAS_RAYID)) && dr_int(d)==0){
		chamb_ray(chamb);
		dr_set(FEC_RAYID,ray->rayid);
		dr_set(FEC_CHI2[0],chamb->chi2);
		if(ray->rayid==RAYID_NOERR){
			/* rotation of axes as z=central-ray */
			ray_rotate_z(ray,dr_get(FEC_TILT[2]));
			ray_rotate_y(ray,dr_get(FEC_TILT[1]));
			ray_rotate_x(ray,dr_get(FEC_TILT[0]));
			/* output of ray information */
			dr_set(FEC_RAY_X[0], ray->org.x);
			dr_set(FEC_RAY_Y[0], ray->org.y);
			dr_set(FEC_TH[0],atan(ray->dir.x));
			dr_set(FEC_PH[0],atan(ray->dir.y));
			/* projection to virtual planes */
			chamb_intersection(chamb);
		}
	}
	
	return(0);
}

/* initialization of data before data analysis */
int evt_start_np(){
	chamber_p chamb;
	plane_p   plane;
	int       i, ipl;
	double    d;

	/* for TGC */

	chamb = &tgc_mwdc;
#if DT2D_TAB
	chamb_get_dt2d_tab(chamb);
#endif
  chamb_get_config(chamb);
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		plane->pres = 0.5;  /* 0.5 mm resolution */
		plane->fit = dr_is_true(dr_get(TGC_FIT_PLANE[ipl]));
	}
	if(dr_exists(d=dr_get(TGC_MAX_NCOMB))) chamb->max_ncomb = (int)d;
	chamb->allow_wireskip = dr_is_true(dr_get(TGC_ALLOW_WIRESKIP));
	d = dr_get(TGC_N_VPLANE);
	chamb->nproj = dr_exists(d) ? min((int)d, MaxNIntPl) : 0;
	for(i=0; i<chamb->nproj; i++)
			dr_ref_n(spf("TGC_PLANE_%d",i), &chamb->h_proj[i]);
	dr_ref_n("TGC_X", &chamb->h_x);
	dr_ref_n("TGC_Y", &chamb->h_y);

	/* for FEC */

	chamb = &fec_mwdc;
#if DT2D_TAB
	chamb_get_dt2d_tab(chamb);
#endif
  chamb_get_config(chamb);
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		plane->pres = 0.5;  /* 0.5 mm resolution */
		plane->fit = dr_is_true(dr_get(FEC_FIT_PLANE[ipl]));
	}
	if(dr_exists(d=dr_get(FEC_MAX_NCOMB))) chamb->max_ncomb = (int)d;
	chamb->allow_wireskip = dr_is_true(dr_get(FEC_ALLOW_WIRESKIP));
	d = dr_get(FEC_N_VPLANE);
	chamb->nproj = dr_exists(d) ? min((int)d, MaxNIntPl) : 0;
	for(i=0; i<chamb->nproj; i++)
			dr_ref_n(spf("FEC_PLANE_%d",i), &chamb->h_proj[i]);
	dr_ref_n("FEC_X", &chamb->h_x);
	dr_ref_n("FEC_Y", &chamb->h_y);

	return(0);
}
	
/* initialize variables */
int init_hist_np()
{
	chamber_p  chamb;
	int        ipl;
	double     d;
	int        i, n;

	/* initialyze for Target Chamber MWDC */
	chamb = &tgc_mwdc;
	chamb->name = "TGC";
	chamb->type = CHAMB_MWDC;
	chamb->npl  = 10;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "VT1";
	chamb->plane[1].name = "VT2";
	chamb->plane[2].name = "VT3";
	chamb->plane[3].name = "VT4";
	chamb->plane[4].name = "X1";
	chamb->plane[5].name = "X2";
	chamb->plane[6].name = "U1";
	chamb->plane[7].name = "U2";
	chamb->plane[8].name = "V1";
	chamb->plane[9].name = "V2";
	for(ipl=0; ipl<chamb->npl; ipl++){
		chamb->plane[ipl].chamb = chamb;
	}
	chamb_init_hist(chamb);
	n = 1<<chamb->npl;
	chamb->matrix = (void*)malloc(sizeof(mat_p)*n);
	for(i=0; i<n; i++) ((mat_p*)chamb->matrix)[i] = (mat_p)NULL;
	chamb->mb = matrix_new(4,1);
	chamb->mc = matrix_new(4,1);
	if(chamb->matrix==NULL||chamb->mb==NULL||chamb->mc==NULL){
		showerr("init_hist_np: No enough memory available\n");
		exit(1);
	}

	/* initialyze for Front-end Chamber MWDC */
	chamb = &fec_mwdc;
	chamb->name = "FEC";
	chamb->type = CHAMB_MWDC;
	chamb->npl  = 6;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "Y1";
	chamb->plane[1].name = "Y2";
	chamb->plane[2].name = "V1";
	chamb->plane[3].name = "V2";
	chamb->plane[4].name = "U1";
	chamb->plane[5].name = "U2";
	for(ipl=0; ipl<chamb->npl; ipl++){
		chamb->plane[ipl].chamb = chamb;
	}
	chamb_init_hist(chamb);
	n = 1<<chamb->npl;
	chamb->matrix = (void*)malloc(sizeof(mat_p)*n);
	for(i=0; i<n; i++) ((mat_p*)chamb->matrix)[i] = (mat_p)NULL;
	chamb->mb = matrix_new(4,1);
	chamb->mc = matrix_new(4,1);
	if(chamb->matrix==NULL||chamb->mb==NULL||chamb->mc==NULL){
		showerr("init_hist_np: No enough memory available\n");
		exit(1);
	}
}

#endif /* if NP_MWDC */

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
