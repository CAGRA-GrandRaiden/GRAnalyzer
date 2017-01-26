/*
  analyzer.c ---- analyzer program
  Version 1.00  02-JUN-2000  by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <errno.h>
#include "freadin.h"
#include "histogram.h"
#include "lr3377.h"
#include "analyzer.h"

static char *filename;
extern char *ofile;      /* output file name */

//FLAG

#define MaxArgs 20

int nscalers = 16;
int nmodes   = 4;
int *scaler = (int*)NULL;
int ablk = 0;  /* number of analyzed blocks */ 
int cblk = 0;  /* number of comment blocks */ 

extern int nrun;
extern int nblk;
extern int shmflag;
#if USE_GRUTINIZER // added on 2017.1.25 by A. Tamii
extern int rootflag;
extern char *hbfnam;
#else
char *hbfnam;
#endif
extern char *shmnam;
extern FILE *falias;

extern int pflag;    /* parent flag */
extern int cflag;    /* child flag */
extern int childn;   /* child process number */

extern int ShowStatus();  // print status flag from GRUTinizer


/* initialize */
static void init_hist(){
	init_hist_rcnp();
	init_hist_gr_fpp();
#if LAS_FPP_MWDC
	init_hist_las_fpp();
#endif
#if NP_MWDC
	init_hist_np();
#endif
#if F2_MWDC
	init_hist_f2();
#endif
}

/* initialize */
int event_init(){
	int i, k;
	hb_init(ofile,SHMSIZE);
  init_hist();
	evt_time();

	/* clear scalers */
	k = nscalers*nmodes;
	scaler = (int*)malloc(sizeof(int)*k);
	for(i=0; i<k; i++)
		scaler[i] = 0;

	/* clear analyzed blocks */
	ablk = 0;
	cblk = 0;
    return 0;
}

/* save histograms */
void save_histograms()
{
	char name[256];
	char *f;

	f = dr_get_sval(HB_FILE);
 	if(f!=(char*)NULL && strlen(f)>0){
		strcpy(name, f);
	}else if(nrun){
		sprintf(name, "hb/run%.4d.hb", nrun);
	}else{
		strcpy(name, "hb/analyzer.hb");
	}

	hb_hist_out(name);
}

/* save scaler data */
void save_scalers()
{
	char name[256];
	FILE *fd;
	char *f;
	evt_print_scaler(stderr);
	if(!cflag){
   	f = dr_get_sval(SCA_FILE);
   	if(f!=(char*)NULL && strlen(f)>0){
			strcpy(name, f);
		}else	if(nrun){
			sprintf(name, "sca/run%.4d.sca", nrun);
		}else{
			strcpy(name, "sca/analyzer.sca");
		}
		fd = fopen(name, "w");
		if(fd){
			evt_print_scaler(fd);
			fprintf(stderr, "Scaler data have been saved in '%s'.\n", name);
			fclose(fd);
		}
	}
}

/* open the alias file */
void open_alias_file()
{
	char *f;
  char fnamalias[256];
 	f = dr_get_sval(ALIAS_FILE);
 	if(f!=(char*)NULL && strlen(f)>0){
    strcpy(fnamalias, f);
	}else{
    strcpy(fnamalias, FNAMALIAS);
	}
	falias = fopen(fnamalias, "w");
 	if(falias==(FILE*)NULL){
 		fprintf(stderr,	"Warning: could not open the alias file '%s' (%s).\n", 
			fnamalias, strerror(errno));
	}
}


/* end of tasks */
int event_exit(){
	/* show block number and the analyzing ratio */
	fprintf(stderr,"\n");
	show_blk_num(1);

	save_scalers();
	
	/* DST exit tasks */
	dst_exit();

#if USE_GRUTINIZER // added on 2017.1.25 by A. Tamii
	if(rootflag == 1)
		root_exit();
#endif

	save_histograms();
	hb_exit(shmflag);
	return 0;
}

/* Event Reduction */
int event(){
  int   i, j, gl_flag;
  double f;
  static int m=0;

  if(!m){
		evt_start();
		m=1;
	}
	message();
#if 0
  if(evt_blk())
		return(-1);
#endif

	dr_set(PROC_NUM[0], cflag ? childn:-1);  /* child process number */
	evt_time();
	evt_scaler();
	dr_set(BLOCK_ANA[0], ablk);
	if(dr_is_true(dr_get(EVENT_RCNP))){
    /* Analyzer RCNP Data */
		if(dr_is_true(dr_get(EVENT_BEND))){
			/* Block-end Event */
		}else{
			evt_init_rcnp();
			evt_ipr();
			gl_flag = 0;
			/* for GR */
      if(dr_is_true(dr_get(ANALYZE_GR))){
				evt_init_gr();
				evt_gr_adc();
				evt_gr_tdc();
				if(dr_is_true(dr_get(ANALYZE_GR_VDC))){
					evt_chamb_gr();
					evt_gr_kinema();
					gl_flag++;
				}
			}
			/* for LAS */
      if(dr_is_true(dr_get(ANALYZE_LAS))){
				evt_init_las();
				evt_las_adc();
				evt_las_tdc();
				if(dr_is_true(dr_get(ANALYZE_LAS_VDC))){
					evt_chamb_las();
					evt_las_kinema();
					gl_flag++;
				}
			}
			if(gl_flag==2){
				evt_gl_kinema();
			}
#if LAS_FPP_MWDC
			/* for LAS_FPP */
      if(dr_is_true(dr_get(ANALYZE_LAS_FPP))){
				evt_init_las_fpp();
				evt_chamb_las_fpp();
			}
#endif
#if NP_MWDC
			/* for NP */
      if(dr_is_true(dr_get(ANALYZE_NP))){
				evt_init_np();
				evt_chamb_np();
			}
#endif
			/* for GR FPP */
      if(dr_is_true(dr_get(ANALYZE_GR_FPP))){
				evt_init_gr_fpp();
				evt_gr_fpp_adc();
				evt_gr_fpp_tdc();
				evt_chamb_gr_fpp();
			}
		}
	}
#if F2_MWDC
	else if(dr_is_true(dr_get(EVENT_F2))){
    /* Analyzer SMART F2 Data */
		if(dr_is_true(dr_get(ANALYZE_F2))){
			evt_init_f2();
			evt_f2_adc();
			evt_f2_tdc();
			if(dr_is_true(dr_get(ANALYZE_F2_MWDC))){
				evt_chamb_f2();
				evt_f2_kinema();
			}
#if 0
			evt_f2_3377();
#endif
		}
	}
#endif
	show_debug();
	dst_write_data();

#if USE_GRUTINIZER // added on 2017.1.25 by A. Tamii
	if(rootflag == 1)
		root_write_data();
#endif

	dr_set(ANALYZED,1);
#if 0
sleep(1);
#endif
	return(0);
}

/* make integer data to analog */
double analog(i)
		 int  i;
{
	return((double)i+drand48()-0.5);
}

void show_debug(){
	double  f;
	int     i, min, max;

  /* dump */
  f = dr_get(DEBUG_DUMP_VARIABLES);
  if(dr_is_true(f)){
    dr_show();
	}
  /* show variable 'DEBUG' */
	f = dr_get(DEBUG_DUMP_DEBUG);
	if(dr_is_true(f)){
		f = dr_get(DEBUG);
		if(dr_exists(f)){
#if 0
			fprintf(stderr, "               DEBUG = %15.7e\n", f);
#endif
			min = dr_min(DEBUG);
			max = dr_max(DEBUG);
			printf("  %s:\n", "DEBUG");
			for(i=min; i<max; i++){
				f = dr_get(DEBUG[i]);
				if(dr_is_nothing(f))
					printf("  %5d) %15s\n", i, "N/U");
				else
					printf("  %5d) %15.7f %8x\n", i, f, (int)f);
			}
		}else{
      f = dr_get(DEBUG_DUMP_SHOW_NONE);
			if(dr_is_true(f))
				fprintf(stderr, "  %s: NONE\n", "DEBUG");
		}
	}
}


/* output messages */
void message(){
	int  i, j;
	double  f;
}

/* starting job */
int evt_start(){
	printf("CALL: evt_start_rcnp");
  evt_start_rcnp();
#if LAS_FPP_MWDC
  evt_start_las_fpp();
#endif
#if NP_MWDC
  evt_start_np();
#endif
  evt_start_gr_fpp();
#if F2_MWDC
  evt_start_f2();
#endif
	dst_init();

#if USE_GRUTINIZER // added on 2017.1.25 by A. Tamii
	if(rootflag == 1)
		root_init(nrun);
#endif

    return 0;

}

void show_blk_num(flag)
		 int flag;
{
#if USE_GRUTINIZER // added on 2017.1.25 by A. Tamii
	if(!ShowStatus()) return;
#endif
	if(flag || cflag || nblk/10*10==nblk){
		if(nblk>1){
#if 1
			fprintf(stderr, "%5d - blocks  A/R = %5.2f%%   \n%c%c",
							nblk, (double)ablk/(double)(nblk-cblk+1)*100.,
							27, 'M');
#else
			fprintf(stderr, "%5d - blocks  A/R = %5.2f%%   \n",
							nblk, (double)ablk/(double)(nblk-cblk+1)*100.);
#endif
		}else{
			fprintf(stderr, "%5d - blocks\n%c%c", nblk, 27, 'M');
		}
	}
}

/* check block number (return with 1 ... no analysis, 0 ... analysis) */
int evt_blk(cblk)
	int cblk;
{
	double d;
	int    blk;
	static int mflag1=0, mflag2=0;
	
	/* check analysis start block */
	d = dr_get(ANA_BLK_START);
	if(dr_exists(d)){
		blk = (int)(d+0.5);
		if(!mflag1){
			mflag1 = 1;
			fprintf(stderr,
			"Warning: analyze blocks from ANA_BLK_START (>=%d).\n", blk);
		}
		if(cblk<blk){
			show_blk_num(0);
			return(1);
		}
	}
	
	/* check analysis end block */
	d = dr_get(ANA_BLK_END);
	if(dr_exists(d)){
		blk = (int)(d+0.5);
		if(!mflag2){
			mflag2 = 1;
			fprintf(stderr,
			"Warning: analyze blocks to ANA_BLK_END (<%d).\n", blk);
		}
		if(blk<cblk){
			show_blk_num(0);
			return(1);
		}
	}

	ablk++;
	if(!cflag){
		if(dr_is_true(dr_get(PRINT_SCALER))
			 || ((pflag && nblk%100==0) && 0)){
			evt_print_scaler(stderr);
		}
		if(!shmflag && hbfnam){
			if((nblk%10000)==0){
				//				hb_hist_out(hbfnam);
			}
		}
	}
	show_blk_num(0);
	return(0);
}
	
/* input register */
#define MAXNIPR 16
int evt_ipr(){
  double f;
  int  i, j, m, n, ipr;
  int  su, sd;

	/* set BIT patterns */
  i = 0;                      	      /* get IPR minimum index (=0) */
  n = min(dr_max(IPR), MAXNIPR);      /* get IPR maximum index (<=MAXNIPR) */
  m = 0;
  for(; i<n; i++, m+=16){
		f = dr_get(IPR[i]);
    if(dr_exists(f)){
      ipr = (int)f;
      for(j=1; j<=16; j++, ipr>>=1)
				if(ipr & 1) dr_set(IPRBIT[m+j],m+j);
    }
  }
    return 0;
}

static int nblk_scaler = 0;
/* Scaler */
void add_scaler(k, n)
		 int k,n;
{
	hist_data_t hd;
	if(0<=k && k<nscalers*nmodes){
		scaler[k] += n;
		dr_set(SCALER_SUM[k],scaler[k]);
		if(cflag){
			hd.hid = HID_SCALER+k;
			hd.f1  = (double)n;
			hd.f2  = 0.;
			if(hd.hid<HID_SCALER_MAX)
				write_child_data((char*)&hd, sizeof(hist_data_t));
		}
	}
}


int evt_scaler(){
  int  i, j, k;
	double d;
	k = 0;
	for(i=0; i<nmodes; i++){
		for(j=0; j<nscalers; j++){
			d = dr_get(SCALER[k]);
			if(dr_exists(d))
				add_scaler(k, dr_int(d));
			k++;
		}
	}
	nblk_scaler = (int)dr_get(BLOCK_N);
    return 0;
}

/* Print Scalers */
int evt_print_scaler(fd)
		 FILE *fd;
{
  int  i, j, k, sum1, sum2;
	if(!scaler)
		return 0;
	if(nmodes==4){
		if(nblk_scaler>0)
			fprintf(fd, "Scalers (at block #%d)\n", nblk_scaler);
		fprintf(fd, "%3s%12s%12s%12s%12s%12s\n",
						"","Mode #1","Mode #2","Mode #1","Mode #2","Sum");
		for(j=0; j<nscalers; j++){
			sum1 = scaler[0*nscalers+j]
				+ scaler[1*nscalers+j];
			sum2 = scaler[2*nscalers+j]
				+ scaler[3*nscalers+j];
			fprintf(fd, "%2d)", j);
			for(i=0; i<nmodes; i++){
				k = i*nscalers+j;
				fprintf(fd, "%12d", scaler[k]);
			}
			if(sum1==sum2)
				fprintf(fd, "%12d\n", sum1);
			else
				fprintf(fd, "    Mismatch!\n");
		}
	}else{
		for(j=0; j<nscalers; j++){
			fprintf(fd, "%2d)", j);
			sum1 = 0;
			for(i=0; i<nmodes; i++){
				k = i*nscalers+j;
				fprintf(fd, "%12d", scaler[k]);
				sum1 += scaler[k];
                        }
			fprintf(fd, "%12d\n", sum1);
		}
	}
    return 0;
}

/* GR-ADC */
int evt_gr_adc(){
  /* nothing to do */
    return 0;
}

/* GR-TDC */
int evt_gr_tdc(){
  /* nothing to do */
    return 0;
}

/* LAS-ADC */
int evt_las_adc(){
  /* nothing to do */
    return 0;
}

/* LAS-TDC */
int evt_las_tdc(){
  /* nothing to do */
    return 0;
}

/* GR-FPP Hodo-ADC */
int evt_gr_fpp_adc(){
  /* nothing to do */
    return 0;
}

/* GR-FPP Hodo-TDC */
int evt_gr_fpp_tdc(){
  /* nothing to do */
    return 0;
}

#if F2_MWDC
/* F2-ADC */
int evt_f2_adc(){
  /* nothing to do */
    return 0;
}

/* F2-TDC */
int evt_f2_tdc(){
  /* nothing to do */
    return 0;
}
#endif

/* Date and Time */
int evt_time(){
	time_t    t;
	struct tm *tm;
	double f;
	time(&t);
	tm = localtime(&t);
	f = (((double)tm->tm_sec/60.)+tm->tm_min)/60.+tm->tm_hour;
	dr_set(TIME[0], f);
	dr_set(TIME[1], tm->tm_year);
	dr_set(TIME[2], tm->tm_mon+1);
	dr_set(TIME[3], tm->tm_mday);
	dr_set(TIME[4], tm->tm_hour);
	dr_set(TIME[5], tm->tm_min);
	dr_set(TIME[6], tm->tm_sec);
    return 0;
}
	
/* Kinematical calculation for GR */
int evt_gr_kinema(){
	double  m1, m2, m3, m4;
	double  e1, e2, e3, e4;
	double  p1, p3, p4;
	double  t1, th3, th4;
	double  p4s, sth3, cth3, ds, dc;
	double  dE;
	double  ex;

	m1  = dr_get(M1);
	m2  = dr_get(M2);
	m3  = dr_get(GR_M3);
	m4  = dr_get(GR_M4);
	if(dr_is_nothing(m1)||dr_is_nothing(m2)||
		 dr_is_nothing(m3)||dr_is_nothing(m4))
		return(-1);

  t1  = dr_get(T1);
  p3  = dr_get(GR_P);
	th3 = dr_get(GR_TH3);
	if(dr_is_nothing(t1)||dr_is_nothing(p3)||
		 dr_is_nothing(th3))
		return(-1);
	
	dE  = dr_get(GR_DE);
	if(dr_is_nothing(dE)) dE = 0.0;
	
	sth3  = sin(th3);
  cth3  = cos(th3);

	e1  = m1+t1;
	e2  = m2;
	e3  = sqrt(m3*m3+p3*p3)+dE;
	p3  = sqrt(e3*e3-m3*m3);
	e4  = e1+e2-e3;
	
  p1  = sqrt(e1*e1-m1*m1);
	ds  = p3*sth3;
	dc  = p1-p3*cth3;

	p4s = ds*ds+dc*dc;
	p4  = sqrt(p4s);
	ex  = sqrt(e4*e4-p4s)-m4;
  th4 = atan2(ds,dc);

	dr_set(GR_EX, ex);
	dr_set(GR_P4, p4);
	dr_set(GR_TH4, th4);
	return(0);
}

/* Kinematical calculation for LAS */
int evt_las_kinema(){
	double  m1, m2, m3, m4;
	double  e1, e2, e3, e4;
	double  p1, p3, p4;
	double  t1, th3, th4;
	double  p4s, sth3, cth3, ds, dc;
	double  dE;
	double  ex;

	m1  = dr_get(M1);
	m2  = dr_get(M2);
	m3  = dr_get(LAS_M3);
	m4  = dr_get(LAS_M4);
	if(dr_is_nothing(m1)||dr_is_nothing(m2)||
		 dr_is_nothing(m3)||dr_is_nothing(m4))
		return(-1);

  t1  = dr_get(T1);
  p3  = dr_get(LAS_P);
	th3 = dr_get(LAS_TH3);
	if(dr_is_nothing(t1)||dr_is_nothing(p3)||
		 dr_is_nothing(th3))
		return(-1);
	
	dE  = dr_get(LAS_DE);
	if(dr_is_nothing(dE)) dE = 0.0;

	sth3  = sin(th3);
  cth3  = cos(th3);

	e1  = m1+t1;
	e2  = m2;
	e3  = sqrt(m3*m3+p3*p3)+dE;
	p3  = sqrt(e3*e3-m3*m3);
	e4  = e1+e2-e3;
	
  p1  = sqrt(e1*e1-m1*m1);
	ds  = p3*sth3;
	dc  = p1-p3*cth3;

	p4s = ds*ds+dc*dc;
	p4  = sqrt(p4s);
	ex  = sqrt(e4*e4-p4s)-m4;
  th4 = atan2(ds,dc);

	dr_set(LAS_EX, ex);
	dr_set(LAS_P4, p4);
	dr_set(LAS_TH4, th4);
	return(0);
}

/* Kinematical calculation for GR and LAS */
int evt_gl_kinema(){
	double  m1, m2, m3, m4, m5;
	double  e1, e2, e3, e4, e5;
	double  p1, p3, p4, p5;
	double  t1, th3, th4, th5;
	double  sth3, cth3, sth4, cth4, sth5, cth5;
	double  ds, dc, p5s;
	double  ex;
	double  m34, m35, m45;

	m1  = dr_get(M1);
	m2  = dr_get(M2);
	m3  = dr_get(GR_M3);
	m4  = dr_get(LAS_M3);
	m5  = dr_get(M5);
	if(dr_is_nothing(m1)||dr_is_nothing(m2)||
		 dr_is_nothing(m3)||dr_is_nothing(m4)||
		 dr_is_nothing(m5))
		return(-1);


  t1  = dr_get(T1);
  p3  = dr_get(GR_P);
	th3 = dr_get(GR_TH3);
  p4  = dr_get(LAS_P);
	th4 = dr_get(LAS_TH3);
	if(dr_is_nothing(t1)||
		 dr_is_nothing(p3)||dr_is_nothing(th3)||
		 dr_is_nothing(p4)||dr_is_nothing(th4))
		return(-1);
	
	/*
		change the sign of th4 to make it consistent with the
		definition of th3 (left scattering)
	*/
	th4 = -th4;  
	sth3  = sin(th3); cth3  = cos(th3);
	sth4  = sin(th4); cth4  = cos(th4);

	e1  = m1+t1;
	e2  = m2;
	e3  = sqrt(m3*m3+p3*p3);
	e4  = sqrt(m4*m4+p4*p4);
	e5  = e1+e2-e3-e4;
	
  p1  = sqrt(e1*e1-m1*m1);
	ds  = -p3*sth3-p4*sth4;
	dc  = p1-p3*cth3-p4*cth4;

	p5s = ds*ds+dc*dc;
	p5  = sqrt(p5s);
	ex  = sqrt(e5*e5-p5s)-m5;
  th5 = atan2(ds,dc);
  sth5 = sin(th5);
  cth5 = cos(th5);

	dr_set(GL_EX, ex);
	dr_set(GL_P5, p5);
	dr_set(GL_TH5, th5);

	/* calculate invariant masses */

  dc = p3*cth3+p4*cth4;
  ds = p3*sth3+p4*sth4;
  m34 = sqrt((e3+e4)*(e3+e4)-(dc*dc+ds*ds));
  dc = p3*cth3+p5*cth5;
  ds = p3*sth3+p5*sth5;
  m35 = sqrt((e3+e5)*(e3+e5)-(dc*dc+ds*ds));
  dc = p5*cth5+p4*cth4;
  ds = p5*sth5+p4*sth4;
  m45 = sqrt((e4+e5)*(e4+e5)-(dc*dc+ds*ds));
	dr_set(GL_M34, m34);
	dr_set(GL_M35, m35);
	dr_set(GL_M45, m45);
	dr_set(GL_M34_EX, m34-m3-m4);
	dr_set(GL_M35_EX, m35-m3-m5);
	dr_set(GL_M45_EX, m45-m4-m5);

	return(0);
}

/* Kinematical calculation for F2 */
int evt_f2_kinema(){
#if 0
	double  m1, m2, m3, m4;
	double  e1, e2, e3, e4;
	double  p1, p3, p4;
	double  t1, th3, th4;
	double  p4s, sth3, cth3, ds, dc;
	double  dE;
	double  ex;

	m1  = dr_get(M1);
	m2  = dr_get(M2);
	m3  = dr_get(F2_M3);
	m4  = dr_get(F2_M4);
	if(dr_is_nothing(m1)||dr_is_nothing(m2)||
		 dr_is_nothing(m3)||dr_is_nothing(m4))
		return(-1);

  t1  = dr_get(T1);
  p3  = dr_get(F2_P);
	th3 = dr_get(F2_TH3);
	if(dr_is_nothing(t1)||dr_is_nothing(p3)||
		 dr_is_nothing(th3))
		return(-1);
	
	dE  = dr_get(F2_DE);
	if(dr_is_nothing(dE)) dE = 0.0;
	
	sth3  = sin(th3);
  cth3  = cos(th3);

	e1  = m1+t1;
	e2  = m2;
	e3  = sqrt(m3*m3+p3*p3)+dE;
	p3  = sqrt(e3*e3-m3*m3);
	e4  = e1+e2-e3;
	
  p1  = sqrt(e1*e1-m1*m1);
	ds  = p3*sth3;
	dc  = p1-p3*cth3;

	p4s = ds*ds+dc*dc;
	p4  = sqrt(p4s);
	ex  = sqrt(e4*e4-p4s)-m4;
  th4 = atan2(ds,dc);

	dr_set(F2_EX, ex);
	dr_set(F2_P4, p4);
	dr_set(F2_TH4, th4);
#endif
	return(0);
}

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
