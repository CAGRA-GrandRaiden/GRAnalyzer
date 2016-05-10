/* matrix.c .... matrix manipulation utilities                 */
/* Author: A. Tamii, Department of Physics, Univ. of Tokyo     */
/* version 1.0  01-JAN-1999  by A. Tamii                       */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"

static mat_val mat_temp;

#define SWAP(a,b) {mat_temp=(a); (a)=(b); (b)=mat_temp;}

#if MATRIX_DEBUG
/*#matrix_error ... show error */
static void matrix_error(func, message)
		 char   *func;
		 char   *message;
{
	fprintf(stderr, "Error in %s(): %s\n", func, message);
}
#else
#define matrix_error(f,m) {}
#endif

#if MATRIX_DEBUG
/*#matrix_set ... set a matrix element (for debug)*/
void   matrix_set(m, ir, ic, v)
		 mat_p  m;
		 int    ir;
		 int    ic;
		 double v;
{
	if(!m){
		matrix_error("matrix_set", "null pointer.");
		return;
	}
	if(ir<0 || m->nr<=ir){
		matrix_error("matrix_set", "ir outof range.");
		return;
	}
	if(ic<0 || m->nc<=ic){
		matrix_error("matrix_set", "ic outof range.");
		return;
	}
	m->m[ir][ic] = v;
}

/*#matrix_get ... get a matrix element (for debug)*/
double   matrix_get(m, ir, ic)
		 mat_p  m;
		 int    ir;
		 int    ic;
{
	if(!m){
		matrix_error("matrix_get", "null pointer.");
		return(-1);
	}
	if(ir<0 || m->nr<=ir){
		matrix_error("matrix_get", "ir outof range.");
		return(-1);
	}
	if(ic<0 || m->nc<=ic){
		matrix_error("matrix_get", "ic outof range.");
		return(-1);
	}
	return(m->m[ir][ic]);
}
#endif

/*#matrix_new ... get new matrix structure */
/* return a pointer to the new matrix      */
/* return with NULL, when failier          */
mat_p matrix_new(nr, nc)
		 int    nr;   /* number of rows */
		 int    nc;   /* number of columns */
{
	mat_p   m;
	int     i;
	mat_val **p;
	mat_val *rp;

	m = (mat_p)malloc(sizeof(matrix));
	if(m==(mat_p)NULL){
		matrix_error("matrix_new", "could not allocate memory.");
		return(NULL);
	}
	m->nr = m->nc = 0;
	m->m = (mat_val**)NULL;
	m->rm = (mat_val*)NULL;
	
  p = (mat_val**)malloc(sizeof(mat_val*)*nr);
	if(p==(mat_val**)NULL){
		matrix_error("matrix_new", "could not allocate memory.");
		free(m);
		return(NULL);
	}

  rp = (mat_val*)malloc(sizeof(mat_val)*nr*nc);
	if(rp==(mat_val*)NULL){
		matrix_error("matrix_new", "could not allocate memory.");
		free(p);
		free(m);
		return(NULL);
	}

	m->m = p;
	m->rm = rp;
	for(i=0; i<nr; i++){
		*p++ = rp;
		rp += nc;
	}
	m->nr = nr;
	m->nc = nc;
	return(m);
}

/*#matrix_free ... free a matrix structure  */
/* return a null pointer                    */
mat_p matrix_free(m)
		 mat_p   m;
{
	if(!m){
		matrix_error("matrix_free", "null pointer.");
	}else{
		if(m->m) free(m->m);
		if(m->rm) free(m->rm);
		free(m);
	}
	return((mat_p)NULL);
}

/*#matrix_showf  ... show matrix with format*/
int matrix_showf(m, format)
		 mat_p   m;
		 char    *format;
{
	int  nr, nc;
	int  i, j;

	if(!m){
		matrix_error("matrix_showf", "null pointer.");
		return(-1);
	}
	nr = m->nr;
	nc = m->nc;

	for(i=0; i<nr; i++){
		for(j=0; j<nc; j++){
			printf(format, m->m[i][j]);
		}
		printf("\n");
	}
	return(0);
}

/*#matrix_show  ... show matrix */\
int matrix_show(m)
		 mat_p   m;
{
	return(matrix_showf(m,"%10.5lf"));
}

/*#matrix_set_zero ...  set zero matrix */
int matrix_set_zero(m)
		 mat_p   m;
{
	int     i, n;
	mat_val *p;

	if(!m){
		matrix_error("matrix_set_zero", "null pointer.");
		return(-1);
	}

	n = m->nr * m->nc;
	p = *m->m;
	for(i=0; i<n; i++)
		*p++ = 0;

	return(0);
}

/*#matrix_set_unity ...  set the unit matrix */
int matrix_set_unity(m)
		 mat_p   m;
{
	int   nr, nc;
	int   i, j;

	if(!m){
		matrix_error("matrix_set_unity", "null pointer.");
		return(-1);
	}
	nr = m->nr;
	nc = m->nc;

	for(i=0; i<nr; i++)
		for(j=0; j<nc; j++)
			m->m[i][j] = (i==j);

	if(nr!=nc){
		matrix_error("matrix_set_unity", "not square matrix.");
		return(-1);
	}
	return(0);
}

/*#matrix_copy_new ...  create a new copy of a matirx */
mat_p matrix_copy_new(src)
		 mat_p   src;
{
	int     i, n;
	mat_p   dst;
	mat_val *d, *s;
	if(!src){
		matrix_error("matrix_copy_new", "null pointer.");
		return((mat_p)NULL);
	}
	dst = matrix_new(src->nr, src->nc);
	if(!dst) return((mat_p)NULL);

	n = src->nr * src->nc;
	s = *src->m;
	d = *dst->m;
	for(i=0; i<n; i++)
		*d++ = *s++;

	return(dst);
}

/*#matrix_copy ...  copy a matirx */
int matrix_copy(dst, src)
		 mat_p   dst, src;
{
	int     i, n;
	mat_val *d, *s;
	if(!dst){
		matrix_error("matrix_copy", "null pointer (dst).");
		return(-1);
	}
	if(!src){
		matrix_error("matrix_copy", "null pointer (src).");
		return(-1);
	}
	if(dst->nr!=src->nr || dst->nc!=src->nc){
		matrix_error("matrix_copy", "matrix dimensions are inconsistent.");
		return(-1);
	}

	n = src->nr * src->nc;
	s = *src->m;
	d = *dst->m;
	for(i=0; i<n; i++)
		*d++ = *s++;

	return(0);
}

/*#matrix_mul ...  multiply matrix (y=axb) */
/* y must not be the same as a nor b */
int matrix_mul(y, a, b)
		 mat_p   y, a, b;
{
	int     i, j, k;
	int     nr, nc, nk;
	mat_val v;
	if(!y || !a || !b){
		matrix_error("matrix_mul", "null pointer (y, a, or b).");
		return(-1);
	}
	if(y->nr!=a->nr || y->nc!=b->nc || a->nc != b->nr){
		matrix_error("matrix_mul", "matrix dimensions are inconsistent.");
		return(-1);
	}

	nr = y->nr;
	nc = y->nc;
	nk = a->nc;

	for(i=0; i<nr; i++){
		for(j=0; j<nc; j++){
			v = 0;
			for(k=0; k<nk; k++){
				v += a->m[i][k] * b->m[k][j];
			}
			y->m[i][j] = v;
		}
	}

	return(0);
}

/*#matrix_add ...  add matrix (y=a+b) */
/* y must not be the same as a nor b */
int matrix_add(y, a, b)
		 mat_p   y, a, b;
{
	int     i, j;
	int     nr, nc;
	mat_val v;
	if(!y || !a || !b){
		matrix_error("matrix_add", "null pointer (y, a, or b).");
		return(-1);
	}

	nr = y->nr;
	nc = y->nc;

	if(a->nr!=nr || a->nc!=nc || b->nr!=nr || b->nc!=nc){
		matrix_error("matrix_add", "matrix dimensions are inconsistent.");
		return(-1);
	}

	for(i=0; i<nr; i++){
		for(j=0; j<nc; j++){
				y->m[i][j] = a->m[i][j] + b->m[i][j];
		}
	}

	return(0);
}

/*#matrix_sub ...  sub matrix (y=a-b) */
/* y must not be the same as a nor b */
int matrix_sub(y, a, b)
		 mat_p   y, a, b;
{
	int     i, j;
	int     nr, nc;
	mat_val v;
	if(!y || !a || !b){
		matrix_error("matrix_sub", "null pointer (y, a, or b).");
		return(-1);
	}

	nr = y->nr;
	nc = y->nc;

	if(a->nr!=nr || a->nc!=nc || b->nr!=nr || b->nc!=nc){
		matrix_error("matrix_sub", "matrix dimensions are inconsistent.");
		return(-1);
	}

	for(i=0; i<nr; i++){
		for(j=0; j<nc; j++){
				y->m[i][j] = a->m[i][j] - b->m[i][j];
		}
	}

	return(0);
}

/*#matrix_solve_gaussj                                                */
/*  ... solve the equation b=ax by Gauss-Jordan ellimination method   */
/* The inverse matrix is returned in the matrix a.                    */
/* The solution is return in the matrix b.                            */
/* The result of the matrix m becomes unity if successfully solved.   */
/* if b is a null pointer, this function returns only inversion of a. */
/* return value:  0 ... successfully solved                           */
/*               -1 ... failed                                        */
/* reference: Numerical Recipes in C 2nd Edition, pp.39.              */
int matrix_solve_gaussj(a, b)
		 mat_p   a;
		 mat_p   b;
{
	int   m, n;
	int   i, j, k, l, ll;
	int   ir, ic;
	int   *idr, *idc, *ipiv;
	mat_val big, f;

	if(!a){
		matrix_error("matrix_solve_gaussj", "null pointer.");
		return(-1);
	}
	n = a->nr;
	m = b ? b->nc : 0;
	if(a->nc!=n){
		matrix_error("matrix_solve_gaussj", "the matrix a is not square matrix.");
		return(-1);
	}
	if(b && b->nr!=n){
		matrix_error("matrix_solve_gaussj",
								 "matrix dimensions of a and b are inconsistent.");
		return(-1);
	}

	idr  = (int*)malloc(sizeof(int)*n);
	idc  = (int*)malloc(sizeof(int)*n);
	ipiv = (int*)malloc(sizeof(int)*n);

	for(i=0; i<n; i++)
		ipiv[i] = 0;

	for(i=0; i<n; i++){
		big = 0.0;
		for(j=0; j<n; j++){
			if(!ipiv[j]){
				for(k=0; k<n; k++){
					if(!ipiv[k]){
						if(fabs(a->m[j][k])>=big){
							big=fabs(a->m[j][k]);
							ir = j;
							ic = k;
						}
					}else if(ipiv[k]>1){
						matrix_error("matrix_solve_gaussj", "singular matrix-1.");
						free(idr);
						free(idc);
						free(ipiv);
						return(-1);
					}
				}
			}
		}
		ipiv[ic]++;
		if(ir!=ic){
			for(l=0; l<n; l++) SWAP(a->m[ir][l],a->m[ic][l])
			for(l=0; l<m; l++) SWAP(b->m[ir][l],b->m[ic][l])
		}
		idr[i] = ir;
		idc[i] = ic;
		if(fabs(a->m[ic][ic])<=1E-30){
			matrix_error("matrix_solve_gaussj", "singular matrix-2.");
			free(idr);
			free(idc);
			free(ipiv);
			return(-1);
		}
		f = 1.0/a->m[ic][ic];
		a->m[ic][ic] = 1.0;
		for(l=0; l<n; l++) a->m[ic][l] *= f;
		for(l=0; l<m; l++) b->m[ic][l] *= f;
		for(ll=0; ll<n; ll++){
			if(ll == ic) continue;
			f = a->m[ll][ic];
			a->m[ll][ic] = 0.0;
			for(l=0; l<n; l++) a->m[ll][l] -= a->m[ic][l]*f;
			for(l=0; l<m; l++) b->m[ll][l] -= b->m[ic][l]*f;
		}
	}
	for(l=n-1; l>=0; l--){
		if(idr[l] != idc[l]){
			for(k=0; k<n; k++){
				SWAP(a->m[k][idr[l]],a->m[k][idc[l]]);
			}
		}
	}
	free(idr);
	free(idc);
	free(ipiv);
	return(0);
}

/*
  Local Variables:
  mode: C
  tab-width: 2
  End:
*/
