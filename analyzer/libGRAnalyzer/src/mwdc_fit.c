/*
  mwdc_fit.c ---- mwdc rayfit
  Version 1.00  10-JUL-2000  by A. Tamii
*/

#include <stdio.h>
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"
#include "matrix.h"

/*#get_matrix (get the inversion matrix for finding the ray*/
mat_p get_matrix(chamb)
		 chamber_p  chamb;
{
	int      h, hitpat;
	mat_p    m;
	plane_p  plane;
	wire_p   wire;
	int      i, j, ipl;
	double   c,s,cc,ss,sc,z,zz;
	double   a[4][4];

	hitpat = chamb->hitpat;
	if(hitpat<0 || (1<<chamb->npl)<=hitpat){
		showerr("Illegal hit pattern %.4x\n", hitpat);
		return((mat_p)NULL);
	}
	m = ((mat_p*)chamb->matrix)[hitpat];

	/* if already error */
	if(m==(mat_p)-1) return((mat_p)NULL);

	/* if already calculated */
	if(m) return(m);
	 
	/* create the matrix */
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			a[i][j]=0.;
	h = hitpat;
	for(ipl=0; ipl<chamb->npl; ipl++, h>>=1){
		plane = &chamb->plane[ipl];
		wire  = &plane->wire;
		if(!(h&1)) continue;
		c  = wire->dsp.x/wire->disp;
		s  = wire->dsp.y/wire->disp;
		z  = wire->org.z;
		sc = s*c;
		cc = c*c;
		ss = s*s;
		zz = z*z;
		a[0][0] += zz*cc;
		a[0][1] +=  z*cc;
		a[0][2] += zz*sc;
		a[0][3] +=  z*sc;
		a[1][0] +=  z*cc;
		a[1][1] +=    cc;
		a[1][2] +=  z*sc;
		a[1][3] +=    sc;
		a[2][0] += zz*sc;
		a[2][1] +=  z*sc;
		a[2][2] += zz*ss;
		a[2][3] +=  z*ss;
		a[3][0] +=  z*sc;
		a[3][1] +=    sc;
		a[3][2] +=  z*ss;
		a[3][3] +=    ss;
	}
	m = matrix_new(4,4);
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			matrix_set(m,i,j,a[i][j]);

	/* solve the matrix */
	if(matrix_solve_gaussj(m,NULL)<0){
#if MATRIX_DEBUG
		printf("Could not get the inversion matrix for [%.4X]\n", hitpat);
#endif
		matrix_free(m);
		((mat_p*)chamb->matrix)[hitpat] = (mat_p)-1;
		return((mat_p)NULL);
	}

	for(i=0; i<4; i++)
		for(j=0; j<4; j++){
			c = fabs(matrix_get(m,i,j));
#if 1
			if(c>1E+7){
#if MATRIX_DEBUG
				printf("Could not get the inversion matrix for [%.4X]\n", hitpat);
				matrix_showf(m,"%15.7e");
#endif
				((mat_p*)chamb->matrix)[hitpat] = (mat_p)-1;
				return((mat_p)NULL);
			}
#endif
		}

	((mat_p*)chamb->matrix)[hitpat] = m;
	return(m);
}

/*#get_chi2*/
double get_chi2(chamb,ray)
		 ray_p      ray;
		 chamber_p  chamb;
{
	int     i, n, ipl;
	vec_rec pos;
	vec_rec v;
	double  p, d, chi2;
	plane_p plane;
	clst_p  clst;
	wire_p  wire;

	chi2 = 0.0;
	n = 0;
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		clst  = &plane->clst[plane->ic];
		if(!plane->fit||dr_is_nothing(clst->pos)) continue;
		wire  = &plane->wire;
		/* find hit point */
		pos.x = ray->org.x + ray->dir.x/ray->dir.z*(wire->org.z-ray->org.z);
		pos.y = ray->org.y + ray->dir.y/ray->dir.z*(wire->org.z-ray->org.z);
		/* calc. wire position */
		v.x = pos.x - wire->org.x;
		v.y = pos.y - wire->org.y;
		/* calc. deviation */
		p = (v.x*wire->dsp.x+v.y*wire->dsp.y)/wire->disp;
		d = p - clst->pos;
		/* calc. chi2 */
		d /= plane->pres;
		chi2 += d*d;
		n++;
	}
	/* chi2 per plane */
	chi2 = n<=4 ? 1E10 : chi2/(n-4);
	return(chi2);
}

/* ray_mwdc_fit --- MWDC ray fit */
int ray_mwdc_fit(chamb)
		 chamber_p  chamb;
{
	int      ipl;
	plane_p  plane;
	clst_p   clst;
	wire_p   wire;
	int      i, n;
	double   b[4];
	double   c,s,z,p;
	ray_t    ray;
	double   chi2;
	mat_p    ma;
	
	for(i=0; i<4; i++)
		b[i] = 0.;

	ma = get_matrix(chamb);
	if(ma==(mat_p)NULL)
		return(RAYID_NOERR);  /* return with 'no error' is correct */

	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		clst  = &plane->clst[plane->ic];
		if(dr_is_nothing(clst->pos)) continue;
		wire = &plane->wire;
		c  = wire->dsp.x / wire->disp;
		s  = wire->dsp.y / wire->disp;
		z  = wire->org.z;
		p  = clst->pos + wire->org.x * c + wire->org.y * s;
		b[0]    +=  z*c*p;
		b[1]    +=    c*p;
		b[2]    +=  z*s*p;
		b[3]    +=    s*p;
		n++;
	}
	for(i=0; i<4; i++)
		matrix_set(chamb->mb,i,0,b[i]);
	matrix_mul(chamb->mc,ma,chamb->mb);

	ray.dir.x = matrix_get(chamb->mc,0,0);
	ray.org.x = matrix_get(chamb->mc,1,0);
	ray.dir.y = matrix_get(chamb->mc,2,0);
	ray.org.y = matrix_get(chamb->mc,3,0);
	ray.dir.z = 1.;
	ray.org.z = 0.;

	chi2 = get_chi2(chamb,&ray);
#if 0
	printf("chi2 = %10.3f\n", chi2);
#endif
	if(chi2<chamb->chi2){
		chamb->ray       = ray;
		chamb->ray.rayid = RAYID_NOERR;
		chamb->chi2      = chi2;
	}
	return(RAYID_NOERR);
}

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
