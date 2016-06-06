/*
  chamb.c ---- wire chamber analysis
  Version 1.00  10-JUL-2000  by A. Tamii
  Version 1.10  21-MAY-2004  by A. Tamii (Add offset to chamb_get_dt2d_tab)
*/

#include <stdio.h>
#include <stdlib.h>  /* for drand48() */
#include <string.h>
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"

//FLAG

#if DT2D_TAB

/* drift time to distance conversion (by table)*/
static double dt2dist_tab(tab,tdc)
		 dt2dtab *tab;
		 int  tdc;
{
	double err;
	double d1, d2;
	if(tdc<0 || tab->nitem<=tdc){
		dr_set_nothing(&err);
		return(err);
	}
	d1 = tab->conv[tdc];
#if DT2D_TAB_ANALOG  /* interpolation by a randam number */
	if(dr_exists(d1) && tdc < tab->end){
    d2 = tab->conv[tdc+1];
		if(dr_exists(d2))
			d1 += (d2-d1)*drand48();
	}
#endif
	return(d1);
}

/* Add offset on 21-MAY-2004 */

/* get drift time to distance table (histogram conversion)*/
int chamb_get_dt2d_tab(chamb)
     chamber_p  chamb;
{
#define MaxNChamb 10
	char     *file;
	FILE     *fd;
	double   data[MaxNChamb];
	int      i, n, nitem;
	char     str[256], fmt[256];
	char     *s;
	static   int itmp;
	int      start, end;
	double   dstart, dend;
	dt2dtab  *tab;
	double   s1, s2, sum;
	int      npl;
	double   offset;   /* added 21-MAY-2004 */
	int      iofs;     /* added 21-MAY-2004 */
	
	/* initialize */
	npl = chamb->npl;
	if(npl>MaxNChamb){
		showerr("chamb_get_dt2d_tab: too many planes %d for %s\n",
						npl, chamb->name);
		return(-1);
	}
	for(n=0; n<npl; n++){
		chamb->plane[n].tab.nitem = 0;
	}

	/* get file name */
	s     = spf("%s_WTDC_FILE", chamb->name);
	itmp  = 0; /* must be cleared */
	file  = dr_get_sval_r(&dr_data, &itmp, s, 0);
	if(file==(char*)NULL){
		showerr("chamb_get_dt2d_tab: could not get file name from '%s'.\n", s);
		return(-1);
	}
	fd = fopen(file, "r");
	if(fd==(FILE*)NULL){
		showerr("chamb_get_dt2d_tab: could not open file '%s'.\n", file);
		return(-1);
	}

	/* read file */
	printf("Reading %s WTDC File: '%s' ... ", chamb->name, file);

  fmt[0]=0; for(n=0; n<npl; n++) strcat(fmt, "%lf");
	for(nitem=0; nitem<MaxNConv; nitem++){
		if(!fgets(str, 256, fd))
			break;
		n = sscanf(str, fmt,
							 &data[0], &data[1], &data[2], &data[3], &data[4],
							 &data[5], &data[6], &data[7], &data[8], &data[9]
							 );
		if(n==0){
			showerr(
				 "chamb_get_dt2d_tab: could not read data from the line %d of '%s'.\n",
				nitem+1, file);
			return(-1);
			break;
		}
		if(n!=npl){
			showerr("chamb_get_dt2d_tab: not enough (%d) data in the line %d of '%s'.\n",
							npl, nitem+1, file);
			return(-1);
		}
		for(n=0; n<npl; n++)
			chamb->plane[n].tab.conv[nitem] = (int)data[n];
	}

	printf("Done.\n");

	/* read offset */
	iofs = 0;
	s = spf("%s_WTDC_OFFSET", chamb->name);
	itmp  = 0;
	offset = dr_get_r(&dr_data, &itmp, s, 0);
	if(dr_exists(offset)){
		iofs = (int)(offset);
		printf("%s WTDC Offset = %5d ch\n", chamb->name, iofs);
	}
	

	/* calc correction table */
	for(n=0; n<npl; n++){
		tab = &chamb->plane[n].tab;
		tab->start = 0;
		tab->end   = 0;
		/* getting the gate values (low bound = start, high bound = end) for each VDC plane */
		s     = spf("%s_WTDC_%s", chamb->name, chamb->plane[n].name);
		itmp  = 0; /* must be cleared */
		dstart = dr_get_r(&dr_data, &itmp, s, 0);
		dend   = dr_get_r(&dr_data, &itmp, s, 1);
		if(dr_is_nothing(dstart)||dr_is_nothing(dend)){
			showerr("chamb_get_dt2d_tab: could not get value from '%s'.\n", s);
			continue;
		}
		start = dr_int(dstart);
		end   = dr_int(dend);
		if(start>=end){
			showerr("chamb_get_dt2d_tab: the arg #0 of '%s' must be smaller than the arg #1.\n", s);
			continue;
		}


		sum = 0;
		start = max(start,0);
		end   = min(end,nitem-1);

		for(i=0; i<start; i++)
			dr_set_nothing(&tab->conv[i]);
		for(; i<end; i++)
			sum += tab->conv[i];
		for(; i<nitem; i++)
			dr_set_nothing(&tab->conv[i]);

		s2 = 0.;
		for(i=end-1; start<=i; i--){
			s1 = tab->conv[i];
			tab->conv[i+1] = s2/sum;
			s2 += s1;
		}
		tab->conv[start] = s2/sum; /* = 1.0 */

		tab->start = start;
		tab->end   = end;
		tab->nitem = nitem;
		tab->offset = iofs;
#if DEBUG_TEMP && 0
		printf("plane = %d\n", n);
		for(i=start; i<end; i++){
			printf("  tab[%3.d] = %f\n", i, tab->conv[i]);
		}
#endif
	}
    return 0;

}

#else  /* if DT2D_TAB  -  GR_TDCR */

/* 2 dimensional polynomial fit   */
/* with no division-by-zero check */
static int fit_2dim(x,y,c,s)
		 double  x[3];  /* 3 x points */
		 double  y[3];  /* 3 y points */
		 double  c[3];  /* fit parameters */
		 double  *s;    /* integral of the fit function */
{
	double x1, x2, y1, y2;
	y1 = y[0]-y[1];
	y2 = y[0]-y[2];
	x1 = x[0]-x[1];
	x2 = x[0]-x[2];
	c[2] = (-y1/x1+y2/x2)/(x1-x2);
	c[1] = -y1/x1-c[2]*x1;
	c[0] = y[0];
	*s = ((c[2]/3*x2+c[1]/2)*x2+c[0])*x2;
#if 0
  printf("\n");
	printf("  x  = %15.7f %15.7f %15.7f\n", x[0], x[1], x[2]);
	printf("  y  = %15.7f %15.7f %15.7f\n", y[0], y[1], y[2]);
	printf("  c  = %15.7f %15.7f %15.7f\n", c[2], c[1], c[0]);
  printf("  y[1] = %15.7f\n", c[2]*x1*x1+c[1]*x1+c[0]);
  printf("  y[2] = %15.7f\n", c[2]*x2*x2+c[1]*x2+c[0]);
	printf("  s  = %15.7f\n", *s);
#endif
}

/* VDC drift time to distance conversion (by parameter fit)*/
static double dt2dist_prm(d,tdc)
		 dt2dprm *d;
		 int  tdc;
{
	double dist;
	double t;
	if(tdc > d->t[0]){
		dr_set_nothing(&dist);
	}else if(tdc > d->t[1]){
		t = analog(d->t[0]-tdc);
		dist = ((d->p1[3]*t + d->p1[2])*t + d->p1[1])*t + d->p1[0];
	}else if(tdc > d->t[2]){
		t = analog(d->t[1]-tdc);
		dist = ((d->p2[3]*t + d->p2[2])*t + d->p2[1])*t + d->p2[0];
	}else{
		dr_set_nothing(&dist);
	}
	return(dist);
}

/* get drift time to distance parameters (5-point 2-dimensional fit)*/
static int get_dt2d_prm(){
	char *str, *nsp, *npl;
	double  t[5];
	double  f[5];
	double  c1[3];
	double  c2[3];
	double  s1, s2, sa;
	static int h_dt2dprm[N_VDC_PL][10];
	dt2dprm *prm;
	int  i, pl;
	for(pl=0; pl<N_VDC_PL; pl++){
		prm = &vdc_cluster[pl].prm;
		nsp = spnam[(pl>>2)&0x01];
		npl = vplnam[pl&0x03];
		str = spf("%s_VDC_%s_T", nsp, npl);
		for(i=0; i<5; i++){
			t[i] = dr_get_r(&dr_data, &h_dt2dprm[pl][i*2],   str, i*2);
			f[i] = dr_get_r(&dr_data, &h_dt2dprm[pl][i*2+1], str, i*2+1);
			if(dr_is_nothing(t[i])||dr_is_nothing(f[i]))
				break;
			if(f[i]<0){
				showerr("get_dt2d_prm: %s[%d](%f) must be greater than 0\n",
								str, i*2+1, f[i]);
				break;
			}
			if(0<i && t[i-1]<=t[i]){
				showerr("get_dt2d_prm: %s[%d](%f) must be greater than %s[%d](%f)\n",
								str, (i-1)*2, t[i-1], str, i*2, t[i]);
				break;
			}
		}
		if(i<5) continue;
		
		fit_2dim(&t[0],&f[0],c1,&s1);
		fit_2dim(&t[2],&f[2],c2,&s2);

		prm->t[0] = dr_int(t[0]);
		prm->t[1] = dr_int(t[2]);
		prm->t[2] = dr_int(t[4]);
    sa = 1/(s1+s2);
		prm->p1[0] =  0.;
		prm->p1[1] =  sa*c1[0];
		prm->p1[2] =  sa*c1[1]/2;
    prm->p1[3] =  sa*c1[2]/3;
		prm->p2[0] =  sa*s1;
		prm->p2[1] =  sa*c2[0];
		prm->p2[2] =  sa*c2[1]/2;
    prm->p2[3] =  sa*c2[2]/3;
#if 0
		printf("pl=%1d prm = %d %d %d\n", pl, prm->t[0], prm->t[1], prm->t[2]);
		printf("       c1 = %f %f %f\n", c1[2], c1[1], c1[0]);
		printf("       c2 = %f %f %f\n", c2[2], c2[1], c2[0]);
#endif
	}
}

#endif /* if DT2D_TAB */


/* Clusterize */
int chamb_clust(plane)
		 plane_p plane;
{
	switch(plane->chamb->type){
	case CHAMB_VDC:
		return(chamb_vdc_clust(plane));
	case CHAMB_MWDC:
		return(chamb_mwdc_clust(plane));
	case CHAMB_MWPC:
		return(chamb_mwpc_clust(plane));
	}
	showerr("Unknown chamber type %d for %s of %s\n",
					plane->chamb->type, plane->name, plane->chamb->name);
	return(-1);
}

/* drift time to distance calculation */
int chamb_drift(plane)
		 plane_p plane;
{
	int i, ic, size;
	struct clst *clst;
	double d;

	if(plane==(plane_p)NULL){
		fprintf(stderr, "evt_drift: null plane pointer\n");
		return(-1);
	}

	for(ic=0; ic<plane->nc; ic++)
	{
		clst = &plane->clst[ic];
		size = clst->size;
		for(i=0; i<size; i++){
#if DT2D_TAB
			clst->dist[i] = d = dt2dist_tab(&plane->tab,clst->tdc[i]);
#else
			clst->dist[i] = d = dt2dist_prm(&plane->prm,clst->tdc[i]);
#endif
			if(dr_exists(d))
				dr_append_ref(plane->h_drift,d);
		}
		if(dr_is_true(dr_get(VDC_ANA_MULTI_CLUST))){
			/* calculate drift length only for the cluster #1 */
			if(plane->chamb->type==CHAMB_VDC)
				break;
		}
	}
	return(0);
}

/* Position Calculation */
int chamb_pos(plane)
		 plane_p plane;
{
	switch(plane->chamb->type){
	case CHAMB_VDC:
		return(chamb_vdc_pos(plane));
	case CHAMB_MWDC:
		return(chamb_mwdc_pos(plane));
	case CHAMB_MWPC:
		return(chamb_mwpc_pos(plane));
	}
	showerr("Unknown chamber type %d for %s of %s\n",
					plane->chamb->type, plane->name, plane->chamb->name);
	return(-1);
}

/* Position Calculation */
int chamb_ray(chamb)
		 chamber_p chamb;
{
	switch(chamb->type){
	case CHAMB_VDC:
		if(chamb->ana_mult_clst){
			return(ray_4p_mult(chamb));
		}else{
			return(ray_4p(chamb));
		}
	case CHAMB_MWDC:
		return(ray_mwdc(chamb));
	case CHAMB_MWPC:
		return(ray_4p(chamb));
	}
	showerr("Unknown chamber type %d for %s\n",
					chamb->type, chamb->name);
	return(-1);
}

/* output rays into a file */
int chamb_outputray(chamb, fd, fnam, nrays)
		 chamber_p chamb;
		 FILE      **fd;
		 char      *fnam;
		 int       *nrays;
{
	ray_p   ray;
	ray = &chamb->ray;
	if(!*fd){
		*fd = fopen(fnam, "w");
		if(!*fd){
			showerr("outputray: could not open file '%s'\n.", fnam);
			*nrays = 0;
			return(-1);
		}
		showerr("output %d rays in '%s'.\n", *nrays, fnam);
	}
	fprintf(*fd, "set arrow from %f,%f to %f, %f nohead \n",
					-1000., ray->org.x-ray->dir.x*1000,
					1000.,	ray->org.x+ray->dir.x*1000);
	if(--(*nrays)==0){
		fclose(*fd);
		*fd = (FILE*)NULL;
	}
	return(-1);
}

/* get intersection points */
int chamb_intersection(chamb)
		 chamber_p chamb;
{
	vec_rec  plane;
	vec_rec  pt;
	int      i;
	
	for(i=0; i<chamb->nproj; i++){
		plane.x = dr_get_ref(chamb->h_proj[i], 0);
		plane.y = dr_get_ref(chamb->h_proj[i], 1);
		plane.z = dr_get_ref(chamb->h_proj[i], 2);
		ray_intersection(&chamb->ray, &plane, &pt);
		dr_set_ref(chamb->h_x, i, pt.x);
		dr_set_ref(chamb->h_y, i, pt.y);
	}
    return 0;
}

/* initialize data before an event (for a chamber)*/
int chamb_init_chamb(chamb)
		 chamber_p  chamb;
{
	double  f;
	int     i;
	ray_p   ray;
	ray = &chamb->ray;
	ray->rayid = RAYID_UNDEF;
	dr_set_nothing(&ray->org.x);
	dr_set_nothing(&ray->org.y);
	dr_set_nothing(&ray->org.z);
	dr_set_nothing(&ray->dir.x);
	dr_set_nothing(&ray->dir.y);
	dr_set_nothing(&ray->dir.z);

	for(i=0; i<chamb->npl; i++){
		chamb->plane[i].nc = 0;
		chamb->plane[i].clst[0].posid = POSID_UNDEF;
		dr_set_nothing(&chamb->plane[i].clst[0].pos);
	}

	chamb->ncomb = -1;
	chamb->chi2  = -1;
	return(0);
}
	
/* init_plane_config */
int init_plane_config(plane)
		 plane_p   plane;
{
	int   i, j, nc;
	double   f;
	double   rot;
  double   disp;
  double   dl;
	int      h;
	wire_p   wire;
	h = 0;
	dr_ref_n(spf("%s_PL_%s" , plane->chamb->name, plane->name), &h);
	wire = &plane->wire;
	wire->ok = 0;
	wire->org.x = dr_get_ref(h,0);
	wire->org.y = dr_get_ref(h,1);
	wire->org.z = dr_get_ref(h,2);
	wire->disp = disp = dr_get_ref(h,3);
	wire->rot  = rot  = dr_get_ref(h,4)*d2r;
	wire->dl   = dl   = dr_get_ref(h,5);
	wire->dsp.x = disp*cos(rot);
	wire->dsp.y = disp*sin(rot);
	wire->dsp.z = 0.;
	wire->dir.x = -sin(rot);
	wire->dir.y = cos(rot);
	wire->dir.z = 0.;
	wire->drf.x = 0.;
	wire->drf.y = 0.;
	wire->drf.z = dl;

	for(i=0; i<6; i++){
		if(dr_is_nothing(dr_get_ref(h,i)))
			return(-1);
	}
	if(fabs(wire->dir.z) > 1e-10){
		showerr(
			 "init_plane_config: wire direction with z component is not supported\n");
		show_vec(&wire->dir);
		showerr("\n");
		return(-1);
	}
	if(fabs(wire->dsp.z) > 1e-10){
		showerr(
			 "init_plane_config: wire displacement with z component is not supported\n");
		show_vec(&wire->dsp);
		showerr("\n");
		return(-1);
	}
  if(fabs(dl) < 1e-10){
		showerr("init_plane_config: no drift length.\n");
		show_vec(&wire->drf);
		showerr("\n");
		return(-1);
	}
  wire->sc = disp/dl;
	wire->ok = 1;
	return(0);
}

/* get chamber configuration */
int chamb_get_config(chamb)
		 chamber_p chamb;
{
  int  i;
	plane_p  plane;
	for(i=0; i<chamb->npl; i++){
		plane = &chamb->plane[i];
		init_plane_config(plane);
	}
	chamb->max_ncomb = Clst_DefMaxNComb;
	return(0);
}

/* initialize variables for a chamber*/
int chamb_init_hist(chamb)
		 chamber_p  chamb;
{
	int       i;
	plane_p   plane;
	char      *nsp, *npl;
  nsp = chamb->name;
	for(i=0; i<chamb->npl; i++){
    plane = &chamb->plane[i];
		npl = plane->name;
		dr_ref_n(spf("%s_WIRE_%s",   nsp, npl), &plane->h_wire);
		dr_ref_n(spf("%s_WIRER_%s",  nsp, npl), &plane->h_wirer);
		dr_ref_n(spf("%s_TDC_%s",    nsp, npl), &plane->h_tdc);
		dr_ref_n(spf("%s_TDCR_%s",   nsp, npl), &plane->h_tdcr);
		dr_ref_n(spf("%s_NHIT_%s",   nsp, npl), &plane->h_nhit);
		dr_ref_n(spf("%s_NCLUST_%s", nsp, npl), &plane->h_nclst);
		dr_ref_n(spf("%s_CLUSTSZ_%s",nsp, npl), &plane->h_clstsz);
		dr_ref_n(spf("%s_DRIFT_%s",  nsp, npl), &plane->h_drift);
		dr_ref_n(spf("%s_POSID_%s",  nsp, npl), &plane->h_posid);
		dr_ref_n(spf("%s_POS_%s",    nsp, npl), &plane->h_pos);
		dr_ref_n(spf("%s_GRAD_%s",   nsp, npl), &plane->h_grad);
	}
	return(0);
}

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
