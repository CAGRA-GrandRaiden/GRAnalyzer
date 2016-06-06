/*
  ray.c ---- raytracing 
  Version 1.00  10-JUL-2000  by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "freadin.h"
#include "histogram.h"
#include "analyzer.h"

#if DIM!=NCHAMB_4P
   *** Error Not supported ***
#endif


/* show matrix */
static void show_matrix(m)
		 double m[DIM][DIM+1];
{
	int  i, j;
	showerr("matrix\n");
	for(i=0; i<DIM; i++){
		for(j=0; j<DIM+1; j++){
			showerr("%10.3lf ", m[i][j]);
		}
		showerr("\n");
	}
}

/* solve matrix */
static int solve_matrix(m)
		 double  m[DIM][DIM+1];
{
	int      i, j, k;
	double   f;
	
	for(i=DIM-1; i>=0; i--){
		if(fabs(m[i][i])<1e-30){
			showerr("solve_matrix: could not solve the matrix\n");
			return(-1);
			if(i<=0) break;
		}
		for(j=i-1; j>=0; j--){
			f = m[j][i]/m[i][i];
			for(k=0; k<DIM+1; k++){
				m[j][k] -= m[i][k]*f;
			}
		}
	}
	for(i=0; i<DIM; i++){
		for(j=0; j<i; j++)
			m[i][DIM] -= m[i][j]*m[j][DIM];
		/* m[i][i] is already checked to be non zero */
		m[i][DIM] /= m[i][i];
	}
	return(0);
}

/* show_vec */
void show_vec(vec)
		 vec_p     vec;
{
	showerr("(%10.3f, %10.3f, %10.3f)",
					 vec->x, vec->y, vec->z);
}
	
/* vec_mag ... calculate magnitude of a vector */
static double vec_mag(vec)
		 vec_p     vec;
{
  return(sqrt(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z));
}
	
/* rotate ray along the x-axis */
int ray_rotate_x(ray, ang)
		 ray_p  ray;
		 double ang;
{
	vec_rec  org, dir;
	double  c, s;

	if(dr_is_nothing(ang))
		return(-1);
	
	c = cos(ang);
	s = sin(ang);
	dir.x = ray->dir.x;
	dir.y = c*ray->dir.y-s*ray->dir.z;
	dir.z = s*ray->dir.y+c*ray->dir.z;
	org.x = ray->org.x;
	org.y = c*ray->org.y-s*ray->org.z;
	org.z = s*ray->org.y+c*ray->org.z;
	if(fabs(dir.z)>1e-100){
		dir.x /= dir.z;
		dir.y /= dir.z;
		dir.z = 1.0;
		org.x -= dir.x*org.z;
		org.y -= dir.y*org.z;
		org.z = 0.0;
	}
	ray->org = org;
	ray->dir = dir;
	return(0);
}

/* rotate ray along the y-axis */
int ray_rotate_y(ray, ang)
		 ray_p  ray;
		 double ang;
{
	vec_rec  org, dir;
	double  c, s;

	if(dr_is_nothing(ang))
		return(-1);
	
	c = cos(ang);
	s = sin(ang);
	dir.y = ray->dir.y;
	dir.z = c*ray->dir.z-s*ray->dir.x;
	dir.x = s*ray->dir.z+c*ray->dir.x;
	org.y = ray->org.y;
	org.z = c*ray->org.z-s*ray->org.x;
	org.x = s*ray->org.z+c*ray->org.x;
	if(fabs(dir.z)>1e-100){
		dir.x /= dir.z;
		dir.y /= dir.z;
		dir.z = 1.0;
		org.x -= dir.x*org.z;
		org.y -= dir.y*org.z;
		org.z = 0.0;
	}
	ray->org = org;
	ray->dir = dir;
	return(0);
}

/* rotate ray along the z-axis */
int ray_rotate_z(ray, ang)
		 ray_p  ray;
		 double ang;
{
	vec_rec  org, dir;
	double  c, s;

	if(dr_is_nothing(ang))
		return(-1);
	
	c = cos(ang);
	s = sin(ang);
	dir.z = ray->dir.z;
	dir.x = c*ray->dir.x-s*ray->dir.y;
	dir.y = s*ray->dir.x+c*ray->dir.y;
	org.z = ray->org.z;
	org.x = c*ray->org.x-s*ray->org.y;
	org.y = s*ray->org.x+c*ray->org.y;
	if(fabs(dir.z)>1e-100){
		dir.x /= dir.z;
		dir.y /= dir.z;
		dir.z = 1.0;
		org.x -= dir.x*org.z;
		org.y -= dir.y*org.z;
		org.z = 0.0;
	}
	ray->org = org;
	ray->dir = dir;
	return(0);
}

/* get intersection point (in the normal coordinate system)*/
int ray_intersection_pt(ray, plane, pt)
		 ray_p    ray;
		 vec_p    plane;
		 vec_p    pt;
{
	vec_rec  x;
	double   z1, cx, cy, cz;
	double   costh, sinth, cosph, sinph;
	double   p;
	
	dr_set_nothing(&pt->x);
	dr_set_nothing(&pt->y);
	dr_set_nothing(&pt->z);
	if(dr_is_nothing(plane->x)||dr_is_nothing(plane->x)||dr_is_nothing(plane->x))
		return(-1);
	if(dr_is_nothing(ray->dir.x)||dr_is_nothing(ray->dir.y)||dr_is_nothing(ray->dir.z))
		return(-1);
	if(dr_is_nothing(ray->org.x)||dr_is_nothing(ray->org.y)||dr_is_nothing(ray->org.z))
		return(-1);

	z1   = plane->x;
	sinth = sin(plane->y);
	costh = cos(plane->y);
	sinph = sin(plane->z);
	cosph = cos(plane->z);
	cx   = sinth*cosph;
	cy   =       sinph;
	cz   = costh*cosph;
	
	p = cx*ray->dir.x - cy*ray->dir.y + cz*ray->dir.z;
	if(fabs(p)<1e-100)
		return(-1);

	p = -(cx*ray->org.x - cy*ray->org.y + cz*(ray->org.z-z1))/p;
	/* Normal Coordinate (intersect position)*/
	pt->x = ray->org.x + ray->dir.x*p;
	pt->y = ray->org.y + ray->dir.y*p;
	pt->z = ray->org.z + ray->dir.z*p;
	return(0);
}

/* get intersection point (in the coordinate system on the specified plane)*/
int ray_intersection(ray, plane, pt)
		 ray_p    ray;
		 vec_p    plane;
		 vec_p    pt;
{
	vec_rec  x;
	double   z1;
	double   costh, sinth, cosph, sinph;
	
	dr_set_nothing(&pt->x);
	dr_set_nothing(&pt->y);
	dr_set_nothing(&pt->z);
	if(dr_is_nothing(plane->x)||dr_is_nothing(plane->x)||dr_is_nothing(plane->x))
		return(-1);
	if(dr_is_nothing(ray->dir.x)||dr_is_nothing(ray->dir.y)||dr_is_nothing(ray->dir.z))
		return(-1);
	if(dr_is_nothing(ray->org.x)||dr_is_nothing(ray->org.y)||dr_is_nothing(ray->org.z))
		return(-1);

	z1   = plane->x;
	sinth = sin(plane->y);
	costh = cos(plane->y);
	sinph = sin(plane->z);
	cosph = cos(plane->z);
	
	/* Normal Coordinate (intersect position)*/
  if(ray_intersection_pt(ray, plane, &x)<0) return -1;

	/* Coordinate on the plane to which the line intersects */		
	/* projection of (x.x, x.y, x.z-z1) to (cos(th), 0, -sin(th)) */
	pt->x = x.x*costh - (x.z-z1)*sinth;
	/* projection  to (sin(ph)sin(th), cosph, sin(ph)cos(th)) */
	pt->y = x.x*sinph*sinth + x.y*cosph + (x.z-z1)*sinph*costh;
	/* distance */
	pt->z = sqrt(pt->x*pt->x+pt->y*pt->y);
	return(0);
}

/* show_plane */
static void show_wire(wire)
		 wire_p   wire;
{
	showerr("org = ");
	show_vec(&wire->org);
	showerr("\n");
	showerr("dsp = ");
	show_vec(&wire->dsp);
	showerr("\n");
	showerr("dir = ");
	show_vec(&wire->dir);
	showerr("\n");
}

/* ray_4p_get_ncomb ----
	 get number of combinations */
int ray_4p_get_ncomb(chamb)
		 chamber_p  chamb;
{
	plane_p  plane;
	int  j, ncomb;
	int  ic, ipl;
	/* count number of combinations */
	ncomb = 1;
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		j = 0;
		for(ic=0; ic<plane->nc; ic++){
			if(plane->clst[ic].posid==POSID_NOERR
				 && dr_exists(plane->clst[ic].pos))
				j++;
		}
		ncomb *= j;
	}
	return(ncomb);
}
	
/* ray_4p_get_ncomb ----
	 get number of combinations */
int ray_4p_get_max_nclust(chamb)
		 chamber_p  chamb;
{
	int     n=0;
  int     ipl;
  plane_p plane;
	
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
    if(plane->nc>n) n = plane->nc;
	}
	
  return n;
}
	
/* ray_4p ---- calculate ray from 4 chamber positions */
int ray_4p(chamb)
		 chamber_p chamb;
{
	vec_rec   a;
	double    pos;
	int       ipl;
	int       errid;
	plane_p   plane;
	ray_p     ray;
	wire_p    wire;

#if 0
	fprintf(stderr, "enter ray_4p()...%s\n",chamb->name);
#endif
	ray = &chamb->ray;
	ray->rayid = RAYID_UNDEF;
	errid = RAYID_NOERR;

	/* chi2 is not calculated in this procedure. For making setup of
	   histogram definitions easier, the default chi2 is set as 0.*/
	chamb->chi2 = 0.0;

	if(chamb->npl!=NCHAMB_4P){
		showerr("ray_4p: unsupported number of planes (%d)\n",
						chamb->npl);
		return(-1);
	}
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		wire  = &plane->wire;
		if(!wire->ok){
			errid = RAYID_CONFG;
			break;
		}
		if(plane->nc>=2){
      /* multi cluster events are not analyzed in this procedure*/
			errid = RAYID_MCLUST;
			/* count number of multi cluster planes */
      for(ipl=0; ipl<NCHAMB_4P; ipl++){
				if(chamb->plane[ipl].nc>=2){
#if 1
					errid++;
#else
					errid += 1<<ipl;
#endif
				}
			}
			chamb->ncomb = ray_4p_get_ncomb(chamb);
			break;
		}
 		pos = plane->clst[0].pos;
		if(plane->clst[0].posid!=POSID_NOERR || dr_is_nothing(pos)){
			errid += 1<<ipl;
			continue;
		}
		a.x = (wire->org.x+wire->dsp.x*pos);
		a.y = (wire->org.y+wire->dsp.y*pos);
		a.z = (wire->org.z+wire->dsp.z*pos);
		chamb->m[ipl][0] =  wire->dir.y;
		chamb->m[ipl][1] = -wire->dir.x;
		chamb->m[ipl][2] =  wire->dir.y*a.z;
		chamb->m[ipl][3] = -wire->dir.x*a.z;
		chamb->m[ipl][4] =  wire->dir.y*a.x - wire->dir.x*a.y;
	}
	if(errid==RAYID_NOERR){
		/* show_matrix(m); */
		if(solve_matrix(chamb->m)<0){
			errid = RAYID_TRACE;
		}else{
			/* show_matrix(m); */
			/* set origin/direction */
			ray->org.x = chamb->m[0][NCHAMB_4P];   /* origin    x */
			ray->org.y = chamb->m[1][NCHAMB_4P];   /*           y */
			ray->org.z = 0.0;                      /*           z */
			ray->dir.x = chamb->m[2][NCHAMB_4P];   /* direction x */
			ray->dir.y = chamb->m[3][NCHAMB_4P];   /*           y */
			ray->dir.z = 1.0;                      /*           z */
		}
	}
	ray->rayid = errid;
#if 0
	fprintf(stderr, "exit chamb_4p()\n");
#endif
	return(errid);
}

/* ray_4p_fpp_v ---- calculate ray from 4 chamber positions (with VDC information)*/
int ray_4p_fpp_v(chamb, set_vertex_pos)
		 chamber_p chamb;
     int (*set_vertex_pos)();
{
	vec_rec   a;
	double    pos;
	int       ipl;
	int       errid;
	plane_p   plane;
	ray_p     ray;
	wire_p    wire;
  int       nhpat;  // no-hit plane pattern
  int       mcpat;  // multi-cluster plane pattern
	int       ic;
	

	ray = &chamb->ray;
	ray->rayid = RAYID_UNDEF;
	errid = RAYID_NOERR;

	/* chi2 is not calculated in this procedure. For making setup of
	   histogram definitions easier, the default chi2 is set as 0.*/
	chamb->chi2 = 0.0;

	if(chamb->npl!=NCHAMB_4P){
		showerr("ray_4p: unsupported number of planes (%d)\n",
						chamb->npl);
		return(-1);
	}

  nhpat = mcpat=0;
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		wire  = &plane->wire;
		if(!wire->ok){
			errid = RAYID_CONFG;
			break;
		}
		if(plane->nc>=2){
			mcpat += 1<< ipl;
			continue;
		}
 		pos = plane->clst[0].pos;
		if(plane->clst[0].posid!=POSID_NOERR || dr_is_nothing(pos)){
			nhpat += 1<< ipl;
			continue;
		}
		a.x = (wire->org.x+wire->dsp.x*pos);
		a.y = (wire->org.y+wire->dsp.y*pos);
		a.z = (wire->org.z+wire->dsp.z*pos);
		chamb->m[ipl][0] =  wire->dir.y;
		chamb->m[ipl][1] = -wire->dir.x;
		chamb->m[ipl][2] =  wire->dir.y*a.z;
		chamb->m[ipl][3] = -wire->dir.x*a.z;
		chamb->m[ipl][4] =  wire->dir.y*a.x - wire->dir.x*a.y;
	}
  chamb->anapat = mcpat==0 ? nhpat : RAYID_MCLUST+mcpat;
  switch(mcpat|nhpat){
  case 0x00:  // good event
		break;
	case 0x01:
  case 0x02:
  case 0x03:
    if((*set_vertex_pos)(chamb, 0, 1)<0)
			errid = RAYID_VTPOS;
    break;
  case 0x04:
  case 0x08:
  case 0x0C:
#if 0
  	for(ipl=0; ipl<chamb->npl; ipl++){
      fprintf(stderr, "ipl=%d", ipl);
  		plane = &chamb->plane[ipl];
  		for(ic=0; ic<plane->nc; ic++){
        fprintf(stderr, " %7.3f", plane->clst[ic].pos);
			}
      fprintf(stderr, "\n");
		}
#endif
    if((*set_vertex_pos)(chamb, 2, 3)<0)
			errid = RAYID_VTPOS;
    break;
	default:
  	errid = chamb->anapat;
	}
	
	if(errid==RAYID_NOERR){
		/* show_matrix(m); */
		if(solve_matrix(chamb->m)<0){
			errid = RAYID_TRACE;
		}else{
			/* show_matrix(m); */
			/* set origin/direction */
			ray->org.x = chamb->m[0][NCHAMB_4P];   /* origin    x */
			ray->org.y = chamb->m[1][NCHAMB_4P];   /*           y */
			ray->org.z = 0.0;                      /*           z */
			ray->dir.x = chamb->m[2][NCHAMB_4P];   /* direction x */
			ray->dir.y = chamb->m[3][NCHAMB_4P];   /*           y */
			ray->dir.z = 1.0;                      /*           z */
		}
	}
	ray->rayid = errid;
#if 0
	fprintf(stderr, "exit chamb_4p()\n");
#endif
	return(errid);
}

/* ray_4p_mult_1p---- 
	 calculate ray from 4 chamber positions (subroutine for recursive call) */
static int ray_4p_mult_1p(chamb, nc)
		 chamber_p   chamb;
		 int         nc;
{
	plane_p   plane;
	wire_p    wire;
	ray_p     ray;
	vec_rec   a;
	double    s[NCHAMB_4P][NCHAMB_4P+1];    /* solution of ray finding matrix */
	double    pos;
	int       i, j;
	int       ic, ipl;
	int       errid;
	double    d, dx, dy, r, chi2;
	struct clst *clst;

	ray   = &chamb->ray;
	
	if(nc<NCHAMB_4P){
		plane = &chamb->plane[nc];
		wire  = &plane->wire;
		if(!wire->ok) return(ray->rayid=RAYID_CONFG);
		plane->ic = -1;
		for(ic=0; ic<plane->nc; ic++){
			clst = &plane->clst[ic];
			pos = clst->pos;
			if(clst->posid!=POSID_NOERR || dr_is_nothing(pos))
				continue;
			a.x = (wire->org.x+wire->dsp.x*pos);
			a.y = (wire->org.y+wire->dsp.y*pos);
			a.z = (wire->org.z+wire->dsp.z*pos);
			chamb->m[nc][0] =  wire->dir.y;
			chamb->m[nc][1] = -wire->dir.x;
			chamb->m[nc][2] =  wire->dir.y*a.z;
			chamb->m[nc][3] = -wire->dir.x*a.z;
			chamb->m[nc][4] =  wire->dir.y*a.x - wire->dir.x*a.y;
			plane->ic = ic;
			errid = ray_4p_mult_1p(chamb, nc+1);
			if(errid) return(errid);
		}
		if(plane->ic<0){ /* no legal hit */
			/* check error planes */
			errid = 0;
			for(ipl=0; ipl<chamb->npl; ipl++){
				plane = &chamb->plane[ipl];
				for(ic=0; ic<plane->nc; ic++){
					if(plane->clst[ic].posid==POSID_NOERR &&
						 dr_exists(plane->clst[ic].pos))
						break;
				}
				if(ic>=plane->nc)
					errid += 1<<ipl;
			}
			return(errid);
		}
		return(RAYID_NOERR);
	}

	/* show_matrix(m); */
	/* 
		 use s instead of chamb->m in order no to destroy chamb->m.
		 This bug was fixed on 23-AUG-2005.
	*/
	for(i=0; i<NCHAMB_4P; i++){
		for(j=0; j<NCHAMB_4P+1; j++){
			s[i][j] = chamb->m[i][j];
		}
	}
	if(solve_matrix(s)<0)
		return(RAYID_NOERR); /* return with RAYID_NOERR is correct */
	
	/* calculate chi square */
	chi2 = 0.;
	dx = s[2][NCHAMB_4P];
	dy = s[3][NCHAMB_4P];
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		r = chamb->plane[ipl].wire.rot;
		d = atan(dx*cos(r)-dy*sin(r))
			- atan(chamb->plane[ipl].clst[chamb->plane[ipl].ic].grad);
		d /= chamb->plane[ipl].ares;
		chi2 += d*d;
	}
	chi2 /= chamb->npl;

#if 0 // Mistake? found on 26-JUNE-2009 (for counting chamb->ncomb)
	if(chamb->chi2<0 || chi2 < chamb->chi2){ /* take the minimum chi2 */
		chamb->chi2 = chi2;
		if(0 <= chi2 && chi2 < chamb->max_chi2){
			/* only if ch2 is less than max_chi2 */
			chamb->ncomb++;
			/* set origin/direction */
			ray->org.x = s[0][NCHAMB_4P];          /* origin    x */
			ray->org.y = s[1][NCHAMB_4P];          /*           y */
			ray->org.z = 0.0;                      /*           z */
			ray->dir.x = dx;                       /* direction x */
			ray->dir.y = dy;                       /*           y */
			ray->dir.z = 1.0;                      /*           z */
		}
	}
#else
  if(0 <= chi2 && chi2 < chamb->max_chi2){
		/* only if ch2 is less than max_chi2 */
		chamb->ncomb++;
 	  if(chamb->chi2<0 || chi2 < chamb->chi2){ /* take the minimum chi2 */
  		chamb->chi2 = chi2;
			/* set origin/direction */
			ray->org.x = s[0][NCHAMB_4P];          /* origin    x */
			ray->org.y = s[1][NCHAMB_4P];          /*           y */
			ray->org.z = 0.0;                      /*           z */
			ray->dir.x = dx;                       /* direction x */
			ray->dir.y = dy;                       /*           y */
			ray->dir.z = 1.0;                      /*           z */
		}
	}
#endif
	return(RAYID_NOERR);
}

/* ray_4p_mult ---- calculate ray from 4 chamber positions
	                  (support multi clusters)*/
int ray_4p_mult(chamb)
		 chamber_p chamb;
{
	chamb->chi2 = -1.0;
	chamb->ray.rayid = RAYID_UNDEF;
	if(chamb->npl!=NCHAMB_4P){
		showerr("ray_4p_mult: unsupported number of planes (%s)\n",
						chamb->npl);
		return(-1);
	}
	chamb->ncomb = 0;  /* for counting number of combinations with chi2
												less than max_chi2 */
	chamb->ray.rayid = ray_4p_mult_1p(chamb, 0);
	if(chamb->ray.rayid==RAYID_NOERR){
		if(chamb->chi2<0){
			chamb->ray.rayid = RAYID_TRACE; /* matrix solve error */
		}else{
			switch(chamb->ncomb){
			case 1:
				break;
			case 0:
			default:
				/* RAYID_MCLUST + number of combinations with chi2 less than
					 max_chi2 */
				chamb->ray.rayid = RAYID_MCLUST + chamb->ncomb;
				break;
			}
		}
	}else{
		dr_set_nothing(&chamb->chi2);
	}
	return(chamb->ray.rayid);
}

/* ray_4p_fpp_mult_1p---- 
	 calculate ray from 4 MWPC positions with checking chi^2 (subroutine for recursive call) */
static int ray_4p_fpp_mult_1p(chamb, nc, chi2_func)
		 chamber_p   chamb;
		 int         nc;
     double (*chi2_func)(ray_p);
{
	plane_p   plane;
	wire_p    wire;
	ray_p     ray;
	ray_t     rayt;
	vec_rec   a;
	double    s[NCHAMB_4P][NCHAMB_4P+1];    /* solution of ray finding matrix */
	double    pos;
	int       i, j;
	int       ic, ipl;
	int       errid;
	double    d, dx, dy, r, chi2;
	struct clst *clst;

	ray   = &chamb->ray;
	
	if(nc<NCHAMB_4P){
		plane = &chamb->plane[nc];
		wire  = &plane->wire;
		if(!wire->ok) return(ray->rayid=RAYID_CONFG);
		plane->ic = -1;
		for(ic=0; ic<plane->nc; ic++){
			clst = &plane->clst[ic];
			pos = clst->pos;
			// fprintf(stderr, "nc=%d pos=%7.3f\n", nc, pos);
			if(clst->posid!=POSID_NOERR || dr_is_nothing(pos))
				continue;
			a.x = (wire->org.x+wire->dsp.x*pos);
			a.y = (wire->org.y+wire->dsp.y*pos);
			a.z = (wire->org.z+wire->dsp.z*pos);
			chamb->m[nc][0] =  wire->dir.y;
			chamb->m[nc][1] = -wire->dir.x;
			chamb->m[nc][2] =  wire->dir.y*a.z;
			chamb->m[nc][3] = -wire->dir.x*a.z;
			chamb->m[nc][4] =  wire->dir.y*a.x - wire->dir.x*a.y;
			plane->ic = ic;
			errid = ray_4p_fpp_mult_1p(chamb, nc+1, chi2_func);
			if(errid) return(errid);
		}
		if(plane->ic<0){ /* no legal hit */
			/* check error planes */
			errid = 0;
			for(ipl=0; ipl<chamb->npl; ipl++){
				plane = &chamb->plane[ipl];
				for(ic=0; ic<plane->nc; ic++){
					if(plane->clst[ic].posid==POSID_NOERR &&
						 dr_exists(plane->clst[ic].pos))
						break;
				}
				if(ic>=plane->nc)
					errid += 1<<ipl;
			}
			return(errid);
		}
		return(RAYID_NOERR);
	}

	/* show_matrix(m); */
	/* 
		 use s instead of chamb->m in order no to destroy chamb->m.
		 This bug was fixed on 23-AUG-2005.
	*/
	for(i=0; i<NCHAMB_4P; i++){
		for(j=0; j<NCHAMB_4P+1; j++){
			s[i][j] = chamb->m[i][j];
		}
	}
	if(solve_matrix(s)<0)
		return(RAYID_NOERR); /* return with RAYID_NOERR is correct */
	
	/* calculate chi square */
  rayt.rayid = RAYID_NOERR;
  rayt.org.x = s[0][NCHAMB_4P];          /* origin    x */
	rayt.org.y = s[1][NCHAMB_4P];          /*           y */
	rayt.org.z = 0.0;                      /*           z */
	rayt.dir.x = s[2][NCHAMB_4P];          /* direction x */
	rayt.dir.y = s[3][NCHAMB_4P];          /*           y */
	rayt.dir.z = 1.0;                      /*           z */

	chi2 = (*chi2_func)(&rayt);
  //chi2 = -1;
	

  if(0 <= chi2 && chi2 < chamb->max_chi2){
		/* only if ch2 is less than max_chi2 */
		chamb->ncomb++;
 	  if(chamb->chi2<0 || chi2 < chamb->chi2){ /* take the minimum chi2 */
  		chamb->chi2 = chi2;
			/* set origin/direction */
      ray->rayid = rayt.rayid;
      ray->org.x = rayt.org.x;
      ray->org.y = rayt.org.y;
      ray->org.z = rayt.org.z;
      ray->dir.x = rayt.dir.x;
      ray->dir.y = rayt.dir.y;
      ray->dir.z = rayt.dir.z;
		}
	}
	return(RAYID_NOERR);
}

/* ray_4p_fpp_mult ---- calculate ray from 4 MWPC positions with checking chi2
	                  (support multi clusters)*/
int ray_4p_fpp_mult(chamb, chi2_func)
		 chamber_p chamb;
     double (*chi2_func)(ray_p);
{
	dr_set_nothing(&chamb->chi2);
	chamb->ray.rayid = RAYID_UNDEF;
	if(chamb->npl!=NCHAMB_4P){
		showerr("ray_4p_fpp_mult: unsupported number of planes (%s)\n",
						chamb->npl);
		return(-1);
	}
  chamb->ncomb = ray_4p_get_ncomb(chamb);
  if(chamb->ncomb>chamb->max_ncomb){
		/* RAYID_MCLUST + number of combinations with chi2 less than max_chi2 */
		chamb->ray.rayid = RAYID_MCLUST + chamb->ncomb;
  	return(chamb->ray.rayid);
	}
  if(dr_exists(chamb->max_nclust)){
    if(ray_4p_get_max_nclust(chamb)>chamb->max_nclust){
		chamb->ray.rayid = RAYID_TRACE;
  	return(chamb->ray.rayid);
		}
	}
 	chamb->ray.rayid = ray_4p_fpp_mult_1p(chamb, 0, chi2_func);
	if(chamb->ray.rayid==RAYID_NOERR){
		if(chamb->chi2<0){
			chamb->ray.rayid = RAYID_TRACE;
		}
	}else{
		dr_set_nothing(&chamb->chi2);
	}
	return(chamb->ray.rayid);
}

/* count number of combinations */
static int get_ncomb(chamb)
		 chamber_p  chamb;
{
	int      ipl;
	plane_p  plane;
	clst_p   clst;
	int      ic, j, n, k;
	k=0; n=1;
	for(ipl=0; ipl<chamb->npl; ipl++){
		plane = &chamb->plane[ipl];
		if(plane->fit){
			j = chamb->allow_wireskip ? 1:0;
			for(ic=0; ic<plane->nc && ic<Clst_MaxNClst; ic++){
				clst = &plane->clst[ic];
				if(clst->posid == POSID_NOERR)
					j+=2;
			}
			n*=j;
			if(j>1) k++;
		}
	}
	if(k<MWDC_MIN_PLHITS) return(-1);  /* too few plane hits */
	return(n);
}

/* ray_mwdc_1p calculate ray from 4 chamber positions
	 (subroutine for recursive call) */
static int ray_mwdc_1p(chamb, ipl)
		 chamber_p   chamb;
		 int         ipl;
{
	plane_p   plane;
	wire_p    wire;
	ray_p     ray;
	clst_p    clst;
  int       ic, n;
	int       errid;

	ray   = &chamb->ray;
	
	if(ipl<chamb->npl){
		plane = &chamb->plane[ipl];
		wire  = &plane->wire;
		if(!wire->ok) return(ray->rayid=RAYID_CONFG);
		/* hit loop */
		if(plane->fit){
			chamb->hitpat |= 1<<ipl;
			n=0;
			for(ic=0; ic<plane->nc && ic<1; ic++){
				plane->ic = ic;
				clst = &plane->clst[ic];
				if(clst->posid) continue;
				/* one of the left-right ambiguity */
				clst->pos = clst->pos1;
				if((errid=ray_mwdc_1p(chamb, ipl+1))) return(errid);
				/* the other of the left-right ambiguity */
				clst->pos = clst->pos2;
				if((errid=ray_mwdc_1p(chamb, ipl+1))) return(errid);
				n++;
			}
			chamb->hitpat &= ~(1<<ipl);
		}
		if(!plane->fit || chamb->allow_wireskip || n==0){
			plane->ic = 0;
			clst = &plane->clst[0];
			dr_set_nothing(&clst->pos);
			if((errid=ray_mwdc_1p(chamb, ipl+1))) return(errid);
		}
		return(0);
	}
	return(ray_mwdc_fit(chamb));
}

/* ray_mwdc --- calculate ray for MWDC */
int ray_mwdc(chamb)
		 chamber_p chamb;
{
	int  n;
	/* check number of combinations */
	n = get_ncomb(chamb);
	if(n<0){
		chamb->ray.rayid = RAYID_FEWHIT;
		return(chamb->ray.rayid);
	}
	if(n>chamb->max_ncomb){
		chamb->ray.rayid = RAYID_NCOMB;
		return(chamb->ray.rayid);
	}

	/* ray trace */
	chamb->chi2 = 1E10;
	chamb->hitpat = 0;
	chamb->ray.rayid = ray_mwdc_1p(chamb, 0);
	if(!chamb->ray.rayid && chamb->chi2>=1E10){
		chamb->ray.rayid = RAYID_TRACE;
		chamb->chi2 = -1.;
	}
#if 0
	if(!chamb->ray.rayid)
		showerr("chi2 = %f\n", chamb->chi2);
#endif
	return(chamb->ray.rayid);
}

/*
	get trace back matrix
  return the number of paramters and the pointer to the tb_mat_t array
	return value:
	  -1:  error
	   0:  no matrix data
	  >0:  number of parameters
*/

int get_tb_matrix(mat_nam, mat_h)
		 char  *mat_nam;   /* like "LAS_XMAT" */
		 tbmat_p *mat_h;
{
	static int itmp;
	int  i, k, n, min, max;
	double c;
	char   nam[256];
	char   *str;
	tbmat_p mat;

	*mat_h = (tbmat_p)NULL;

	/* get matrix */
	for(n=0;;n++){
		sprintf(nam, "%s_%s", mat_nam, "%.2d");
		str = spf(nam,n+1);
		itmp = 0; /* must be cleared */
		min = dr_min_r(&dr_data, &itmp, str, 0);
		max = dr_max_r(&dr_data, &itmp, str, 0);
		if(min>=max) break;
	}

	if(n<=0) return(0);

  mat = (tbmat_p)malloc(n*sizeof(tbmat_t));
	if(mat == (tbmat_p)NULL){
		showerr("get_tb_matrix: no enough memory space.\n");
		return(-1);
	}
	for(i=0; i<n; i++){
		str = spf(nam,i+1);
		itmp = 0; /* must be cleared */
		k = dr_int(dr_get_r(&dr_data, &itmp, str, 0));
		c = dr_get_r(&dr_data, &itmp, str, 1);
		mat[i].c = c;
		mat[i].x = (k/1000) % 10;
		mat[i].a = (k/100) % 10;
			mat[i].y = (k/10) % 10;
			mat[i].b = k % 10;
			if(mat[i].x > TB_MAX_POWER ||
			   mat[i].a > TB_MAX_POWER ||
			   mat[i].y > TB_MAX_POWER ||
			   mat[i].b > TB_MAX_POWER){
				showerr("get_tb_matrix: trace-back matrix has a power > %d"
								"in %s_%.2d.\n",
								TB_MAX_POWER, mat_nam, i+1);
				return(-1);
			}
	}

	*mat_h = mat;
	return(n);
}

/* trace-back by matrix */
double trace_back(ray, mat, nmat, flag)
		 ray_p   ray;   /* rad->dir is in grad */
		 tbmat_p mat;
		 int     nmat;
		 int     flag;
		 /*
			 0 to use grad (=ray->dir)
			 1 to use atan (=atan(ray->dir))
		 */
{
	int      i;
	double   c;
	double   th, ph;
	double x[TB_MAX_POWER+1];
	double a[TB_MAX_POWER+1];
	double y[TB_MAX_POWER+1];
	double b[TB_MAX_POWER+1];

	if(nmat==0 || mat==(tbmat_p)NULL){
		dr_set_nothing(&c);
		return(c);
	}

	/* calculate power series */
	x[0] = a[0] = y[0] = b[0] = 1.;
	th = atan(ray->dir.x);
	ph = atan(ray->dir.y);
	for(i=1; i<=TB_MAX_POWER; i++){
		/* changed to use atan(dir) on 14-DEC-2004 */
		if(flag){
			x[i] = x[i-1] * ray->org.x;
			a[i] = a[i-1] * th;
			y[i] = y[i-1] * ray->org.y;
			b[i] = b[i-1] * ph;
		}else{
			x[i] = x[i-1] * ray->org.x;
			a[i] = a[i-1] * ray->dir.x;
		  y[i] = y[i-1] * ray->org.y;
  		b[i] = b[i-1] * ray->dir.y;
		}
	}

	/* calculate trace-back matrix */
	c = 0.;
	for(i=0; i<nmat; i++){
#if 0 /* already checked */
		if(mat->x > TB_MAX_POWER ||
			 mat->a > TB_MAX_POWER ||
			 mat->y > TB_MAX_POWER ||
			 mat->b > TB_MAX_POWER){
			showerr("trace_back: too large power series.\n");
			break;
		}
#endif
		c += mat->c * x[mat->x] * a[mat->a]* y[mat->y]* b[mat->b];
		mat++;
	}
	return(c);
}

/*
	ray_vertex()
	Calculate the vertex point from 2 rays.
	The vertex distance is defined as the minimum distance between the two rays.
	The vertex point is defined as the central position of the line of minimum
	distance between the two rays.
*/
int ray_vertex(r1, r2, p, dist)
		 ray_p    r1, r2;
		 vec_p    p;
		 double   *dist;
{
	vec_rec  b, x, x1, x2;
	double   det;
	double   m11, m12, m21, m22;
	double   s1, s2, p1, p2;
	
	/* set the default value */
	p->x = p->y = p->z = 0.0;
	*dist = -10.0;

	det = r1->dir.x*r2->dir.y - r2->dir.x*r1->dir.y;
	if(fabs(det)<1e-10){
    /*  2 rays are parallel */
		*dist = -1.0;
		return(-1);
	}
	b.x = -( r2->dir.y*r1->dir.z - r1->dir.y*r2->dir.z)/det;
	b.y = -(-r2->dir.x*r1->dir.z + r1->dir.x*r2->dir.z)/det;
	b.z = 1.0;

	m11 =  r1->dir.x - r1->dir.z*b.x;
	m12 = -r2->dir.x + r2->dir.z*b.x;
	m21 =  r1->dir.y - r1->dir.z*b.y;
	m22 = -r2->dir.y + r2->dir.z*b.y;

	det = m11*m22 - m12*m21;
	if(fabs(det)<1e-10){
    /*  This also means 2 rays are parallel (never come here)*/
		*dist = -2.0;
		return(-1);
	}
	s1 = r1->org.x - r2->org.x - (r1->org.z-r2->org.z)*b.x;
	s2 = r1->org.y - r2->org.y - (r1->org.z-r2->org.z)*b.y;
	p1 = -( m22*s1 - m12*s2)/det;
	p2 = -(-m21*s1 + m11*s2)/det;
	
	x1.x = r1->org.x + r1->dir.x*p1;
	x1.y = r1->org.y + r1->dir.y*p1;
	x1.z = r1->org.z + r1->dir.z*p1;
	x2.x = r2->org.x + r2->dir.x*p2;
	x2.y = r2->org.y + r2->dir.y*p2;
	x2.z = r2->org.z + r2->dir.z*p2;
	x.x = x1.x - x2.x;
	x.y = x1.y - x2.y;
	x.z = x1.z - x2.z;
	*dist = sqrt(x.x*x.x + x.y*x.y + x.z*x.z);
	p->x = (x1.x + x2.x)/2;
	p->y = (x1.y + x2.y)/2;
	p->z = (x1.z + x2.z)/2;
	return(0);
}

/*
	ray_scat_ang()
	Calculate scattering angle from two rays.
	ath ... polar angle
	aph ... azimuthal angle
	hor ... horizontal angle
	ver ... vertical angle
*/
int ray_scat_ang(r1, r2, ath, aph, hor, ver)
		 ray_p    r1, r2;
		 double *ath, *aph, *hor, *ver;
{
	vec_rec  c1, c2, d1, d2;
	double   th, ph, c, s;
	
	if(fabs(r1->dir.z)<1e-100)
		return(-1);
	th = atan(r1->dir.x/r1->dir.z);
	s  = sin(th);
	c  = cos(th);
	c1.x = -s*r1->dir.z + c*r1->dir.x;
	c1.y =  r1->dir.y;
	c1.z =  c*r1->dir.z + s*r1->dir.x;
	c2.x = -s*r2->dir.z + c*r2->dir.x;
	c2.y =  r2->dir.y;
	c2.z =  c*r2->dir.z + s*r2->dir.x;
	if(fabs(c1.z)<1e-100)
		return(-1);
	ph = atan(c1.y/c1.z);
	s  = sin(ph);
	c  = cos(ph);
	d1.x =  c1.x;
	d1.y =  c*c1.y - s*c1.z;
	d1.z =  s*c1.y + c*c1.z;
	d2.x =  c2.x;
	d2.y =  c*c2.y - s*c2.z;
	d2.z =  s*c2.y + c*c2.z;
	if(fabs(d2.z)<1e-100)
		return(-1);
	th = atan(sqrt(d2.x*d2.x+d2.y*d2.y)/d2.z);
	ph = atan2(d2.y, d2.x);
	*ath = th;
	*aph = ph;
	*hor = atan(d2.x/d2.z);
	*ver = atan(d2.y/d2.z);
	return(0);
}


/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
