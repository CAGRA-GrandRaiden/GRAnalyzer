/* matrix.h .... header for matrix manipulation utilities      */
/* Author: A. Tamii, Department of Physics, Univ. of Tokyo     */
/* version 1.0  01-JAN-1999  by A. Tamii                       */

#ifndef MATRIX_DEBUG
#define MATRIX_DEBUG  0
#endif

#if defined (__cplusplus)
extern "C" {
#endif
typedef double  mat_val;

typedef struct matrix{
    int       nr;    /* number of rows                  */
	int       nc;    /* number of columns               */
	mat_val   **m;   /* pointers to the rows            */
	mat_val   *rm;   /* pointers to the matrix elements */
} matrix, *mat_p;

#if MATRIX_DEBUG
void   matrix_set(mat_p m, int ir, int ic, double v);
double matrix_get(mat_p m, int ir, int ic);
#else
#define matrix_set(mat,ir,ic,v) {((mat_p)mat)->m[(ir)][(ic)]=(v);}
#define matrix_get(mat,ir,ic)   (((mat_p)mat)->m[(ir)][(ic)])
#endif

#define matrix_get_nr(mat) ((mat)->nr)
#define matrix_get_nc(mat) ((mat)->nc)

mat_p   matrix_new(int nr, int nc);
mat_p   matrix_free(mat_p m);
int     matrix_showf(mat_p m, char *format);
int     matrix_show(mat_p m);
int     matrix_set_unity(mat_p m);
int     matrix_set_zero(mat_p m);
int     matrix_copy(mat_p dst, mat_p src);
mat_p   matrix_copy_new(mat_p src);
int     matrix_mul(mat_p y, mat_p a, mat_p b);
int     matrix_add(mat_p y, mat_p a, mat_p b);
int     matrix_sub(mat_p y, mat_p a, mat_p b);
int     matrix_solve_gaussj(mat_p a, mat_p b);

#if defined (__cplusplus)
}
#endif
