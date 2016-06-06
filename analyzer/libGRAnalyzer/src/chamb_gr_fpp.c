/*
  chamb_gr_fpp.c ---- GR FPP Analysis
  Version 1.00  10-JUL-2000  by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>  /* for drand48() */
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"

//FLAG

struct chamber gr_fpp_in;
struct chamber gr_fpp_out;

extern struct chamber gr_vdc;

/* initialize data before an event (for GR)*/
int evt_init_gr_fpp()
{
	chamb_init_chamb(&gr_fpp_in);
	chamb_init_chamb(&gr_fpp_out);
    return 0;
}

int set_vertex_pos(chamber_p chamb, int ipl1, int ipl2)
{
  ray_p  ray;
	vec_rec   a;
  vec_rec   plane;
  vec_rec   dir;

  ray = &gr_vdc.ray;
	if(ray->rayid!=RAYID_NOERR) return -1;

  // carbon block center plane
	plane.x = dr_get(GF_V_POS[0]);  // z position
  plane.y = dr_get(GF_V_POS[1]);  // tilting angle (theta)
  plane.z = dr_get(GF_V_POS[2]);  // tilting angle (phi)
  if(!dr_exists(plane.x)) return -1;
  if(!dr_exists(plane.y)) return -1;
  if(!dr_exists(plane.z)) return -1;

  if(ray_intersection_pt(ray, &plane, &a)<0) return -1;

	// fprintf(stderr, "intersection (%7.3f,%7.3f,%7.3f)\n", a.x, a.y, a.z);
	

  dir.x = 0.0;
  dir.y = 1.0;
  dir.z = 0.0;
	chamb->m[ipl1][0] =  dir.y;
	chamb->m[ipl1][1] = -dir.x;
	chamb->m[ipl1][2] =  dir.y*a.z;
	chamb->m[ipl1][3] = -dir.x*a.z;
	chamb->m[ipl1][4] =  dir.y*a.x - dir.x*a.y;

  dir.x = 1.0;
  dir.y = 0.0;
  dir.z = 0.0;
	chamb->m[ipl2][0] =  dir.y;
	chamb->m[ipl2][1] = -dir.x;
	chamb->m[ipl2][2] =  dir.y*a.z;
	chamb->m[ipl2][3] = -dir.x*a.z;
	chamb->m[ipl2][4] =  dir.y*a.x - dir.x*a.y;
  return 0;
}

double chi2_func(ray_p ray)
{
	vec_rec    p;
	double     dist;
  double     vd_res;
  double     vx_res;
  double     vy_res;
  double     chi2;
	vec_rec   x1;
	vec_rec   x2;
  vec_rec   plane;
  vec_rec   dir;

  // carbon block center plane
	plane.x = dr_get(GF_V_POS[0]);  // z position
  plane.y = dr_get(GF_V_POS[1]);  // tilting angle (theta)
  plane.z = dr_get(GF_V_POS[2]);  // tilting angle (phi)
  if(!dr_exists(plane.x)) return -1;
  if(!dr_exists(plane.y)) return -1;
  if(!dr_exists(plane.z)) return -1;

	if(gr_vdc.ray.rayid!=RAYID_NOERR) return -1;
  if(ray_intersection_pt(ray, &plane, &x1)<0) return -1;
  if(ray_intersection_pt(&gr_vdc.ray, &plane, &x2)<0) return -1;

	if(ray_vertex(ray, &gr_vdc.ray, &p, &dist)!=0) return -1;
	if(!dr_exists(dist)||dist<0) return -1;

  vd_res = dr_get(GF_VD_RES);
  vx_res = dr_get(GF_VX_RES);
  vy_res = dr_get(GF_VY_RES);

  chi2 = 0.0;
  if(dr_exists(vd_res)) chi2 += dist*dist/(vd_res*vd_res);
  if(dr_exists(vx_res)) chi2 += (x1.x-x2.x)*(x1.x-x2.x)/(vx_res*vx_res);
  if(dr_exists(vy_res)) chi2 += (x1.y-x2.y)*(x1.y-x2.y)/(vy_res*vy_res);

	//fprintf(stderr, "chi2=%7.3f\n", chi2);
  return chi2;
}


/* GR-FPP data analysis */
int evt_chamb_gr_fpp(){
	int  i;
	chamber_p  chamb;
	plane_p    plane;
	ray_p      ray;
	vec_rec    p;
	double     dist;
	double     ath, aph, hor, ver;

#if 0
	/* incoming ray X1,X2 */
	chamb = &gr_fpp_in;
	ray   = &chamb->ray;
	for(i=0; i<chamb->npl; i++){
		plane = &chamb->plane[i];
    chamb_clust(plane);
    chamb_drift(plane);
    chamb_pos(plane);
	}
#endif
#if 0
	chamb_ray(chamb);	
	dr_set(GF_RAYID,ray->rayid);
	if(ray->rayid==RAYID_NOERR){
    /* rotation of axes as z=central-ray */
		ray_rotate_z(ray,dr_get(GF_IN_TILT[2]));
		ray_rotate_y(ray,dr_get(GF_IN_TILT[1]));
		ray_rotate_x(ray,dr_get(GF_IN_TILT[0]));
    /* output of tracking information  */
    /* output of ray information */
		dr_set(GF_RAY_X[0], ray->org.x);
		dr_set(GF_RAY_Y[0], ray->org.y);
		dr_set(GF_TH[0],atan(ray->dir.x));
		dr_set(GF_PH[0],atan(ray->dir.y));
    /* projection to virtual planes */
		chamb_intersection(chamb);
	}
#endif

	/* outgoing ray U3,V3,U4,V4 */
	chamb = &gr_fpp_out;
	ray   = &chamb->ray;
  dr_set_nothing(&chamb->anapat);
	if(!dr_exists(dr_get(GF_RAYID[0]))){
  	for(i=0; i<chamb->npl; i++){
  		plane = &chamb->plane[i];
      chamb_clust(plane);
      chamb_drift(plane);
      chamb_pos(plane);
  	}
    if(chamb->ana_mult_clst){
#if 0
		  ray_4p_fpp_v(chamb, set_vertex_pos);	
#else
			ray_4p_fpp_mult(chamb, chi2_func);	
#endif
		}else{
	    chamb_ray(chamb);	
		}
  	dr_set(GF_RAYID,ray->rayid);
    dr_set(GF_ANA_PAT,chamb->anapat);
    dr_set(GF_CHI2,chamb->chi2);
  	if(ray->rayid==RAYID_NOERR){
      /* rotation of axes as z=central-ray */
			ray_rotate_z(ray,dr_get(GF_OUT_TILT[2]));
			ray_rotate_y(ray,dr_get(GF_OUT_TILT[1]));
			ray_rotate_x(ray,dr_get(GF_OUT_TILT[0]));
      /* output of ray information */
  		dr_set(GF_OUT_RAY_X[0], ray->org.x);
  		dr_set(GF_OUT_RAY_Y[0], ray->org.y);
  		dr_set(GF_OUT_TH[0],atan(ray->dir.x));
  		dr_set(GF_OUT_PH[0],atan(ray->dir.y));
	  }
	}else{
		ray->org.x = dr_get(GF_OUT_RAY_X[0]);
		ray->org.y = dr_get(GF_OUT_RAY_Y[0]);
		ray->org.z = 0.0;
		ray->dir.x = tan(dr_get(GF_OUT_TH[0]));
		ray->dir.y = tan(dr_get(GF_OUT_PH[0]));
		ray->dir.z = 1.0;
		ray->rayid = dr_int(dr_get(GF_RAYID[0]));
	}

	if(ray->rayid==RAYID_NOERR){
    /* projection to virtual planes */
		chamb_intersection(chamb);

		/* vetex point and the scattering angle */
		if(gr_vdc.ray.rayid==RAYID_NOERR){
			if(ray_vertex(ray,&gr_vdc.ray,&p,&dist)==0){
				dr_set(GF_VERTEX_X[0], p.x);
				dr_set(GF_VERTEX_Y[0], p.y);
				dr_set(GF_VERTEX_Z[0], p.z);
			}
			dr_set(GF_DIST[0], dist);  /* if error, error code is saved in 'dist' */
      /* The argument order of ray_vertex has been corrected on 08-AUG-2006 */
      if(ray_scat_ang(&gr_vdc.ray,ray,&ath,&aph,&hor,&ver)==0){
				/* scattering angle (all in radian) */
				dr_set(GF_SCAT_TH[0], ath);  /* scattering angle (polar angle) */
				dr_set(GF_SCAT_PH[0], aph);  /* scattering angle (azimuthal angle) */
				dr_set(GF_SCAT_DX[0], hor);  /* scattering angle (horizontal) */
				dr_set(GF_SCAT_DY[0], ver);  /* scattering angle (vertical) */
			}
		}
	}

	return(0);
}

/* initialization of data before data analysis */
int evt_start_gr_fpp(){
	double    d;
	int       i, cmin, cmax, ipl;
	chamber_p chamb;
	plane_p   plane;

	cmin = 1;
	cmax = 10;

#if 0
	if(dr_is_true(dr_get(ANALYZE_GR_FPP))){
#else
	if(1){
#endif
		chamb = &gr_fpp_in;
		chamb_get_config(chamb);
		for(ipl=0; ipl<chamb->npl; ipl++){
			plane = &chamb->plane[ipl];
			plane->min_clst_size = cmin;
			plane->max_clst_size = cmax;
		}
		d = dr_get(GF_IN_N_VPLANE);
		chamb->nproj = dr_exists(d) ? min((int)d, MaxNIntPl) : 0;
		for(i=0; i<chamb->nproj; i++)
			dr_ref_n(spf("GF_IN_PLANE_%d",i), &chamb->h_proj[i]);
		dr_ref_n("GF_IN_X", &chamb->h_x);
		dr_ref_n("GF_IN_Y", &chamb->h_y);

		chamb = &gr_fpp_out;
		chamb_get_config(chamb);
		for(ipl=0; ipl<chamb->npl; ipl++){
			plane = &chamb->plane[ipl];
			plane->min_clst_size = cmin;
			plane->max_clst_size = cmax;
		}
		chamb->ana_mult_clst = dr_is_true(dr_get(GF_ANA_MULTI_CLUST));
		chamb->max_chi2 = dr_get(GF_MAX_CHI2);
		chamb->max_ncomb = dr_get(GF_MAX_NCOMB);
    chamb->max_nclust = dr_get(GF_MAX_NCLUST);
		if(dr_is_nothing(chamb->max_chi2))
			chamb->max_chi2 = 0.;
		d = dr_get(GF_OUT_N_VPLANE);
		chamb->nproj = dr_exists(d) ? min((int)d, MaxNIntPl) : 0;
		for(i=0; i<chamb->nproj; i++)
			dr_ref_n(spf("GF_OUT_PLANE_%d",i), &chamb->h_proj[i]);
		dr_ref_n("GF_OUT_X", &chamb->h_x);
		dr_ref_n("GF_OUT_Y", &chamb->h_y);
	}
        return 0;
}
	
/* initialize variables */
int init_hist_gr_fpp()
{
	chamber_p  chamb;
	int        ipl;

	chamb = &gr_fpp_in;
	chamb->name = "GF";
	chamb->type = CHAMB_MWPC;
	chamb->npl  = 2;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "X1";
	chamb->plane[1].name = "X2";
	for(ipl=0; ipl<chamb->npl; ipl++){
		chamb->plane[ipl].chamb = chamb;
	}
	chamb_init_hist(chamb);

	chamb = &gr_fpp_out;
	chamb->name = "GF";
	chamb->type = CHAMB_MWPC;
	chamb->npl  = 4;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "U3";
	chamb->plane[1].name = "V3";
	chamb->plane[2].name = "U4";
	chamb->plane[3].name = "V4";
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
