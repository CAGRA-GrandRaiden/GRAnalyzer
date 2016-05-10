/*
  chamb_f2.c ---- MWDC F2 data analysis
  Version 1.00  10-JUL-2000  by A. Tamii
*/

#include <stdio.h>
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"
#include "matrix.h"

#if F2_MWDC

#define iswap(a,b) {i_swap=(a); (a)=(b); (b)=i_swap;}

//FLAG

/* Variable reference pointers */

static int h_mwire[N_MWDC_PL];    /* xxx_WIRE_xxx */
static int h_mwirer[N_MWDC_PL];   /* xxx_WIRER_xxx */
static int h_mtdc[N_MWDC_PL];     /* xxx_TDC_xxx */
static int h_mtdcr[N_MWDC_PL];    /* xxx_TDCR_xxx */
static int h_mnhit[N_MWDC_PL];    /* xxx_NHIT_xxx */

static char *mplnam[]= {
  "X1", "X2", "X3", "X4", "Y1", "Y2", "Y3", "Y4"
};

struct chamber f2_mwdc;

/* initialize data before an event (common)*/
int evt_init_f2()
{
	chamb_init_chamb(&f2_mwdc);
	/* nothing to do */
}

/* F2-3377 data analysis */
int evt_chamb_f2(){
	int  i;
	chamber_p  chamb;
	plane_p    plane;
	ray_p    ray;

	chamb = &f2_mwdc;
	ray   = &chamb->ray;
	for(i=0; i<chamb->npl; i++){
		plane = &chamb->plane[i];
    chamb_clust(plane);
    chamb_drift(plane);
    chamb_pos(plane);
	}
	chamb_ray(chamb);	
	dr_set(F2_RAYID,ray->rayid);
	if(ray->rayid==RAYID_NOERR){
    /* rotation of axes as z=central-ray */
		ray_rotate_z(ray,dr_get(F2_MWDC_TILT[2]));
		ray_rotate_y(ray,dr_get(F2_MWDC_TILT[1]));
		ray_rotate_x(ray,dr_get(F2_MWDC_TILT[0]));
    /* output of ray information */
		dr_set(F2_RAY_X[0], ray->org.x);
		dr_set(F2_RAY_Y[0], ray->org.y);
		dr_set(F2_TH[0],atan(ray->dir.x));
		dr_set(F2_PH[0],atan(ray->dir.y));
    /* projection to virtual planes */
		chamb_intersection(chamb);
	}
	return(0);
}

/* initialization of data before data analysis */
int evt_start_f2(){
	chamber_p chamb;
	plane_p   plane;
	int       i, ipl;
	double    d;

	chamb = &f2_mwdc;
#if DT2D_TAB
	chamb_get_dt2d_tab(chamb);
#endif
  chamb_get_config(chamb);
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		plane->pres = 0.3;  /* 0.3 mm resolution */
		plane->fit = dr_is_true(dr_get(F2_FIT_PLANE[ipl]));
	}
	if(dr_exists(d=dr_get(F2_MAX_NCOMB))) chamb->max_ncomb = (int)d;
	chamb->allow_wireskip = dr_is_true(dr_get(F2_ALLOW_WIRESKIP));
	d = dr_get(F2_N_VPLANE);
	chamb->nproj = dr_exists(d) ? min((int)d, MaxNIntPl) : 0;
	for(i=0; i<chamb->nproj; i++)
			dr_ref_n(spf("F2_PLANE_%d",i), &chamb->h_proj[i]);
	dr_ref_n("F2_X", &chamb->h_x);
	dr_ref_n("F2_Y", &chamb->h_y);
	return(0);
}
	
/* initialize variables */
int init_hist_f2(){
	int i,n,ipl;
	chamber_p  chamb;

	chamb = &f2_mwdc;
	chamb->name = "F2";
	chamb->type = CHAMB_MWDC;
	chamb->npl  = 8;
	chamb->plane = (plane_p)malloc(sizeof(plane_t)*chamb->npl);
	chamb->plane[0].name = "X1";
	chamb->plane[1].name = "X2";
	chamb->plane[2].name = "Y1";
	chamb->plane[3].name = "Y2";
	chamb->plane[4].name = "X3";
	chamb->plane[5].name = "X4";
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

	return(0);
}

#endif /* if F2_MWDC */

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
