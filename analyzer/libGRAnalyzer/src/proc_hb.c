/*
  proc_hb.c --- drex HBOOK manipulation procedures
  Copyright (C) 1997  A. Tamii
  Author:   A. Tamii
  Facility: Department of Physics, Kyoto University 
            & Research Center for Nuclear Physics
  Created:  09-MAY-1997 by A. Tamii
  Version 1.00 09-MAY-1997 by A. Tamii
  Version 2.00 03-JUN-2000 by A. Tamii
*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdarg.h>

#include "freadin.h"
#include "histogram.h"

extern int shmflag;  /* shared memory flag */
extern int pflag;    /* parent process  flag */
extern int cflag;    /* child process flag */

#if 0
float pawc_[8000000];
#else
float pawc_[16000000];
#endif
extern void hstini_(char*, int*);
extern void hstshm_(char*, int*);
extern void hstend_(char*, int*);
extern void hstbk1_(int*, char*, float*, float*, int*);
extern void hstfl1_(int*, float *);
extern void hstbk2_(int*, char*, float*, float*, int*, float*, float*, int*);
extern void hstfl2_(int*, float*, float*);
extern void hstdel_(int*);
extern void hstout_(char *name, int *namelen);

#define NAME_LEN  80

/* strctof */
void strctof(f, c, n)
		 char   *f, *c;
		 int    n;
{
	int  i;
	if(c){
		strncpy(f, c, n);
		i = strlen(c);
	}else{
		i = 0;
	}
	for(; i<n; i++)
		f[i] = ' ';
}

/* initialize */
int hb_init(){
	char      name[NAME_LEN];
  int       len;
	if(!cflag){
		strctof(name, SHMNAME, NAME_LEN);
		len = strlen(SHMNAME);
		if(shmflag){
			hstshm_(name, &len);
		}else{
			hstini_(name, &len);
		}
	}
	return(0);
}

/* exit */
int hb_exit(){
	char      name[NAME_LEN];
  int       len;
	if(!cflag){
		strctof(name, SHMNAME, NAME_LEN);
		len = strlen(SHMNAME);
		hstend_(name, &len);
	}
	return(0);
}

/* histgram output */
int hb_hist_out(name)
		 char    *name;
{
	int   len;
	char  nam[NAME_LEN];
	if(!cflag){
		strctof(nam, name, NAME_LEN);
		len = strlen(name);
		hstout_(nam, &len);
		fprintf(stderr, "Histogram data have been saved in '%s'.\n", name);
	}
	return(0);
}

/* hbk1 */
int hbk1(id, name, title, min, max, nbin)
		 int   id;
		 char  *name, *title;
		 double min, max;
		 int  nbin;
{
	float  fmin, fmax;
	char  str[256];
	char  nam[256];
	if(!cflag){
		fmin = (float)min;
		fmax = (float)max;
		sprintf(str, "%-10s %s", name, title);
		strctof(nam, str, 256);
		hstbk1_(&id, nam, &fmin, &fmax, &nbin);
	}
	return(0);
}

/* hbk2 */
int hbk2(id, name, title, min1, max1, nbin1, min2, max2, nbin2)
		 int    id;
		 char   *name, *title;
		 double min1, max1, min2, max2;
		 int    nbin1, nbin2;
{
	float  fmin1, fmax1, fmin2, fmax2;
	char  str[256];
	char  nam[256];
	if(!cflag){
		fmin1 = (float)min1;
		fmax1 = (float)max1;
		fmin2 = (float)min2;
		fmax2 = (float)max2;
		sprintf(str, "%-10s %s", name, title);
		strctof(nam, str, 256);
		hstbk2_(&id, nam, &fmin1, &fmax1, &nbin1, &fmin2, &fmax2, &nbin2);
	}
	return(0);
}

/* hfil1 */
int hfil1(id,d)
		 int    id;
		 double d;
{
	float  f;
	hist_data_t  hd;
	if(dr_exists(d)){
		f = (float)d;
		if(cflag){
			/* child process */
			hd.hid = id;
			hd.f1  = f;
			hd.f2  = 0.;
			write_child_data((char*)&hd, sizeof(hist_data_t));
		}else{
			if(d<-1.0E+50 || 1.0E+50<d){
				//	fprintf(stderr, "hfil1: out of data range: id=%3d data=%15.7e\n", id, d);
			}else{
				hstfl1_(&id, &f);
			}
		}
	}
	return(0);
}

/* hfil2 */
int hfil2(id,d1,d2)
		 int     id;
		 double  d1, d2;
{
	float  f1, f2;
	hist_data_t  hd;
	if(dr_exists(d1) && dr_exists(d2)){
		f1 = (float)d1;
		f2 = (float)d2;
		if(cflag){
			/* child process */
			hd.hid = id;
			hd.f1  = f1;
			hd.f2  = f2;
			write_child_data((char*)&hd, sizeof(hist_data_t));
		}else{
			if(d1<-1.0E+50 || 1.0E+50<d1 || d2<-1.0E+50 || 1.0E+50<d2){
				//	fprintf(stderr, "hfil1: out of data range: id=%3d data=%15.7e,%15.7e\n", id, d1, d2);
			}else{
				hstfl2_(&id, &f1, &f2);
			}
		}
	}
	return(0);
}

/* hstdel */
int hstdel(id)
		 int     id;
{
	if(!cflag){
		hstdel_(&id);
	}
	return(0);
}

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
