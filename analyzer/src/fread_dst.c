/*
  fread_dst.c --- DST read/write program for analyzer (DST format)
  Author:   A. Tamii
  Facility: Department of Physics, University of Tokyo
  Created:  04-MAY-1997 by A. Tamii
  Version 1.00 04-MAY-1997 by A. Tamii (fread)
  Version 1.22 06-MAY-1997 by A. Tamii (fread)
  Version 2.00 03-JUN-2000 by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <math.h>
#include <errno.h>

#include "freadin.h"
#include "histogram.h"
#include "mtformat.h"
#include "lr3377.h"
#include "fera.h"

#define ENDRUNBLK   0     /* End of Fread at the end of run block */

//FLAG

extern int neval;
extern int level;
extern int swap;
extern int byte_order;
int parse_error;

extern int nrun;
extern int nblk;
extern int nevt;

extern int pflag;
extern int cflag;

static FILE *fd_write = (FILE*)NULL;
static FILE *fd_read  = (FILE*)NULL;

static int ndstvar=0;
static char *dstvar[MaxNDSTVar];
static int dstvarref[MaxNDSTVar];
static  int ndstinp=0;
static int dstinpref[MaxNDSTVar];

static int n_dst = 0;
static size_t n_write = 0;
#define MAX_DST_SIZE  FFFC0000

#define DST_SIG      "DST FILE 1.0"
#define VAR_BEGIN    "BEGIN VARIABLES"
#define VAR_END      "END VARIABLES"
#define IND_END      0x7fffffff
#define EVT_END      0x7ffffffe
#define FILE_END     0x7ffffffd

/*
	open output file
	return with 0 if success, otherwise return with -1
*/
static int dst_open_file()
{
	FILE *fd;
	char *f;
	char fnam[256];

	f  = dr_get_sval(DST_FILE);

	if(f==(char*)NULL || strlen(f)==0){
		/* no output file is specified */
		return(0);
	}

	if(n_dst==0){
		 sprintf(fnam, "%s", f);
	}else{
		 sprintf(fnam, "%s_%.3d", f, n_dst);
	}

	n_dst++;

#if 0
	fprintf(stderr, "Open DST File: %s\n", fnam);
#endif
	fd = fopen(fnam, "w");
	if(fd==NULL){
		printf("Could not open output DST file '%s': %s\n",fnam, strerror(errno));
		return(-1);
	}else{
		printf("Output data to DST file: '%s'\n", fnam);
	}
	fd_write = fd;
	
	return(0);
}

static int dst_close_file()
{
	if(!fd_write)
		return(0);
	fclose(fd_write);
	fd_write = (FILE*)NULL;
}


/* write header */
static int dst_write_header(char const *comment)
{
	int  i, min, max;
	char *name, *p, *d, c;
	char str[256];
	if(!fd_write)
		return(0);
	
	fprintf(fd_write, "%s\n", DST_SIG);
	if(comment){
		fprintf(fd_write, "%s", comment);
		fprintf(fd_write, "\n");
	}
	fprintf(fd_write, "%s\n", VAR_BEGIN);
	
	/* variables to write */
	name = dr_get_sval(DST_VAR);
	ndstvar = 0;
	if(name){
		d = p = name;
		c = 1;  /* any number other than 0 is OK */
		for(i=0;c;i++){
			c = *d++;
			if((!c && i) || c==':'){
				if(i>255) i=255;
				strncpy(str, p, i);
				str[i] = 0x00;
				p = d;
				i = -1;
				fprintf(fd_write, "%s\n", str);
				if(ndstvar>=MaxNDSTVar){
					printf("Too many DST variables (>%d)\n", MaxNDSTVar);
					break;
				}
				dstvar[ndstvar] = strdup(str);
				dr_ref_n(str, &dstvarref[ndstvar++]);
			}
			if(!c) break;
		}
	}
	fprintf(fd_write, "%s\n", VAR_END);
	
	return(0);
	}
	
/* write data */
int dst_write_data()
{
	int    i, iv, ref, min, max, iend;
	double d;
	int    error;
	if(!fd_write)
		return(0);
	if(dr_is_false(dr_get(DST_CONDITION))) 
		 return(0);
	iend = IND_END;
	for(iv=0; iv<ndstvar; iv++){
#if 0
		fprintf(stderr, "output var '%s'\n", dstvar[iv]);
#endif
		ref = dstvarref[iv];
		min = dr_min_ref(ref);
		max = dr_max_ref(ref);
		for(i=min; i<max; i++){
			if(dr_exists(d=dr_get_ref(ref,i))){
#if 0
 				if(nblk==5587)
  				printf("%s[%d] = %f\n", dstvar[iv], i, d);
#endif
				/* write index and data (double floating) */
				n_write += fwrite(&i, sizeof(int), 1, fd_write)*sizeof(int);
				n_write += fwrite(&d, sizeof(double), 1, fd_write)*sizeof(double);
			}
		}
		/* write end of a variable */
		n_write += fwrite(&iend, sizeof(int), 1, fd_write)*sizeof(int);
	}
			
	/* write end of an event */
	i = EVT_END;
	n_write += fwrite(&i, sizeof(int), 1, fd_write)*sizeof(int);
#if 0
	if(n_write >= MAX_DST_SIZE){
		dst_close_file();
		n_write = 0;
		if(error = dst_open_file())
			return(error);
	}
#endif
	return(0);
}
	
/* check format */
int dst_chk_format(unsigned char *buf)
{
	if(!strncmp((char*)buf, DST_SIG, strlen(DST_SIG)))
		return(1);
	return(0);
}

/* read header */
int dst_read_header(unsigned char *buf,int  *bpos,int  size)
{
	char str[256];
	unsigned char *ptrini;
	unsigned char *ptr;
	int  n;
	ptr = ptrini = &buf[*bpos];
	for(n=0;n<3;){
		sscanf((char*)ptr, "%[^\n]", str);
		ptr = &ptr[strlen(str)+1];
#if 0
		fprintf(stderr, "%s\n", str);
#endif
		switch(n){
		case 0:
			if(strcmp(str, DST_SIG))
				return(-1);
			n++;
			break;
		case 1:
			if(!strcmp(str, VAR_BEGIN)){
				n++;
			}else{
				printf("%s\n", str); /* Output Comments */
			}
			break;
		case 2:
			if(!strcmp(str, VAR_END)){
				n++;
				break;
			}
			if(ndstinp>=MaxNDSTVar)
				break;
			dr_ref_n(str, &dstinpref[ndstinp++]);
			break;
		}
#if 0
		fprintf(stderr, "n=%d\n", n);
#endif
	}
	*bpos = (long)ptr-(long)ptrini;
	return(1);
}
	
/* read data (one event)*/
int dst_read_data(unsigned char *buf, int  *bpos, int  size)
{
#define SWAP 0
	static int first_read=1;
	static int ana=-1;
	int i, n, iv, blk;
	int pos;
	double d;
	unsigned char   *src;
	unsigned char   *dst;
	if(first_read){
		first_read = 0;
		return(dst_read_header(buf, bpos, size));
	}

	dr_clear();
	neval++;
	dr_set(EVENT_DST[0], 1);
	pos = *bpos;
	for(iv=0;;){
		/* read index */
		if(pos>=size){
			fprintf(stderr, "dst_read_data: run out of buffer (a)\n");
			return(-1);
		}
		if(SWAP){
			src = (unsigned char*)&buf[pos];
			dst = (unsigned char*)&((&n)[1]);
			*(--dst) = *src++;
			*(--dst) = *src++;
			*(--dst) = *src++;
			*(--dst) = *src++;
		}else{
			n = *(int*)&buf[pos];
		}
		
#if 0
		if(nblk==5587)
			printf("iv=%5d n=%5d\n", iv, n);
#endif
		pos += sizeof(int);
		
		if(n==FILE_END)
			return(-1);
		if(n==EVT_END)
			break;
		if(n==IND_END){
			iv++;
			continue;
		}
		/* read data (double floating) */
		if(pos>=size){
			fprintf(stderr, "dst_read_data: run out of buffer (b)\n");
			return(-1);
		}
		if(SWAP){
			src = (unsigned char*)&buf[pos];
			dst = (unsigned char*)&(&d)[1];
			*(--dst) = *src++;
			*(--dst) = *src++;
			*(--dst) = *src++;
			*(--dst) = *src++;
			*(--dst) = *src++;
			*(--dst) = *src++;
			*(--dst) = *src++;
			*(--dst) = *src++;
		}else{
			d = *(double*)&buf[pos];
		}
		pos += sizeof(double);
		/* set data */
		if(iv<ndstinp){
			if(n == 1082955776){
				for(i=0; i<16; i++){
					fprintf(stderr, "%4x\n", buf[pos-16+i]);
				}
			}
			
			dr_set_ref(dstinpref[iv], n, d);
#if 0
			fprintf(stderr, "%s[%d] = %f\n", dstvar[iv], iv, d);
#endif
		}
	}
	*bpos = pos;
	if(dr_exists(d=dr_get(RUN_N[0])))
		nrun = (int)d;
	if(dr_exists(d=dr_get(BLOCK_N[0])))
		blk = (int)d;
	else
		blk = 0;
	if(ana<0 || nblk!=blk){
		nblk = blk;
		ana = !evt_blk(blk);
#if 0
		fprintf(stderr, "%d %d %f %f\n", blk, ana,
						dr_get(ANA_BLK_START), dr_get(ANA_BLK_END));
#endif
	}
	if(ana){
		if(pflag){
		}else{
			dr_event();
		}
		
#if 0
		/* show scalers (if requested) */
		if(dr_is_true(dr_get(ANALYZE_SHOW_SCALER)))
			evt_print_scaler(stdout);
#endif
	}

	return(0);
}

/* initialyze */
int dst_init(){
	int   res;
	if((res=dst_open_file())) return(res);
	if((res=dst_write_header((char*)NULL))) return(res);
	return(0);
}

/* exit */
int dst_exit(){
	int   res;
	dst_close_file();
	return(0);
}

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
