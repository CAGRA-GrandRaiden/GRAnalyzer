/*
  chamb_las_fpp.c ---- LAS FPP MWDC data analysis
  Version 1.00  04-DEC-2005  by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"
#include "matrix.h"

#if LAS_FPP_MWDC

//FLAG

struct chamber las_fpp_mwdc;

extern struct chamber las_vdc;

/* initialize data before an event (for np)*/
int evt_init_las_fpp()
{
	chamb_init_chamb(&las_fpp_mwdc);
    return 0;
}

/* LAS-FPP-3377 data analysis */
int evt_chamb_las_fpp(){
	chamber_p chamb;
	plane_p   plane;
	int       ipl;
	ray_p     ray, lray;
	ray_t     r;
	vec_rec   p;
	double    d;
	double    dist;
	double    ath, aph, hor, ver;

	chamb = &las_fpp_mwdc;
	ray   = &chamb->ray;
	
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
    chamb_clust(plane);
		chamb_drift(plane);
		chamb_pos(plane);
	}

	//	if(dr_exists(d=dr_get(LAS_RAYID)) && dr_int(d)==0){
	chamb_ray(chamb);
	dr_set(LF_RAYID,ray->rayid);
	dr_set(LF_CHI2[0],chamb->chi2);
	dr_set(LF_NCOMB,chamb->ncomb);
	if(ray->rayid==RAYID_NOERR){
		/* rotation of axes as z=central-ray */
		ray_rotate_z(ray,dr_get(LF_TILT[2]));
		ray_rotate_y(ray,dr_get(LF_TILT[1]));
		ray_rotate_x(ray,dr_get(LF_TILT[0]));
		/* output of ray information */
		dr_set(LF_RAY_X[0], ray->org.x);
		dr_set(LF_RAY_Y[0], ray->org.y);
		dr_set(LF_TH[0],atan(ray->dir.x));
		dr_set(LF_PH[0],atan(ray->dir.y));
		/* projection to virtual planes */
		chamb_intersection(chamb);
	}

	/* vetex point and the scattering angle */
#if 0 /* for special use 19-DEC-2005 E154*/
	r.org.x =  4.1;
	r.org.y = 13.3;
	r.org.z = 0.;
	r.dir.x = tan(+0.15*d2r);
	r.dir.y = tan(-0.24*d2r);
	r.dir.z = 1.;
	r.rayid = RAYID_NOERR;
	lray = &r;
#else
	lray = &las_vdc.ray;
#endif
	if(lray->rayid==RAYID_NOERR){
		if(ray_vertex(ray,lray,&p,&dist)==0){
			dr_set(LF_VERTEX_X[0], p.x);
			dr_set(LF_VERTEX_Y[0], p.y);
			dr_set(LF_VERTEX_Z[0], p.z);
		}
		dr_set(LF_DIST[0], dist);  /* if error, error code is saved in 'dist' */
		if(ray_scat_ang(lray,ray,&ath,&aph,&hor,&ver)==0){
			/* scattering angle (all in radian) */
			dr_set(LF_SCAT_TH[0], ath);  /* scattering angld (polar angle) */
			dr_set(LF_SCAT_PH[0], aph);  /* scattering angld (azimuthal angle) */
			dr_set(LF_SCAT_DX[0], hor);  /* scattering angld (horizontal) */
			dr_set(LF_SCAT_DY[0], ver);  /* scattering angld (vertical) */
		}
	}
	return(0);
}

/* initialization of data before data analysis */
int evt_start_las_fpp(){
	chamber_p chamb;
	plane_p   plane;
	int       i, ipl;
	double    d;

	chamb = &las_fpp_mwdc;
#if DT2D_TAB
	chamb_get_dt2d_tab(chamb);
#endif
  chamb_get_config(chamb);
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		plane->pres = 0.5;  /* 0.5 mm resolution */
		plane->fit = dr_is_true(dr_get(LF_FIT_PLANE[ipl]));
	}
	if(dr_exists(d=dr_get(LF_MAX_NCOMB))) chamb->max_ncomb = (int)d;
	chamb->allow_wireskip = dr_is_true(dr_get(LF_ALLOW_WIRESKIP));
	d = dr_get(LF_N_VPLANE);
	chamb->nproj = dr_exists(d) ? min((int)d, MaxNIntPl) : 0;
	for(i=0; i<chamb->nproj; i++)
			dr_ref_n(spf("LF_PLANE_%d",i), &chamb->h_proj[i]);
	dr_ref_n("LF_XP", &chamb->h_x);
	dr_ref_n("LF_YP", &chamb->h_y);
    return 0;
}
	
/* initialize variables */
int init_hist_las_fpp()
{
	chamber_p  chamb;
	int        ipl;
	double     d;
	int        i, n;

	/* initialyze for LAS-FPP MWDC */
	chamb = &las_fpp_mwdc;
	chamb->name = "LF";
	chamb->type = CHAMB_MWDC;
	chamb->npl  = 8;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "X1";
	chamb->plane[1].name = "X2";
	chamb->plane[2].name = "X3";
	chamb->plane[3].name = "X4";
	chamb->plane[4].name = "Y1";
	chamb->plane[5].name = "Y2";
	chamb->plane[6].name = "Y3";
	chamb->plane[7].name = "Y4";
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
    return 0;
}


#endif /* if LAS_FPP_MWDC */

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
