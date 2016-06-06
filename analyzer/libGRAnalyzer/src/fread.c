/*
  fread.c --- analyzer program Author: A. Tamii Facility: Department
  of Physics, University of Tokyo Created: 04-MAY-1997 by A. Tamii
  Version 1.00 04-MAY-1997 by A. Tamii (fread) Version 1.22
  06-MAY-1997 by A. Tamii (fread) Version 2.00 03-JUN-2000 by A. Tamii
  (for analyzer) Version 3.00 31-JUL-2000 by A. Tamii (analyzer.c) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef OSF1
#include <strings.h>
#endif
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include <string.h>


#include "freadin.h"

int pflag = 0;                /* parent process flag */
int cflag = 0;                /* child process flag */
int childn = 0;               /* child number for child process */

#define MAX_NFDEF 10
FILE   *findef = (FILE*)NULL;

#if USE_PAW
FILE   *falias = (FILE*)NULL;
#endif
int    byte_order=BIG_ENDIAN;
int    swap=0;

int    nrun = 0;
int    nblk = 0;
char   *ofile = (char*)NULL;
int    line;               /* for lex */

int    shmflag = 1;
int    rootflag = 0;

/* swaps --- swap the short words in an integer */
void swaps(data)
		 unsigned int  *data;
{
	union val{
		int      vint;
		short    vshort[2];
	} v, *vp;
	v.vint = *data;
	vp = (union val*)data;
	vp->vshort[0] = v.vshort[1];
	vp->vshort[1] = v.vshort[0];
}
	
	static char *cd_buf = (char*)NULL;
	static int  cd_pos  = 0;

/* write_child_data --- buffer child data and write */
void write_child_data(buf, size)
		 char *buf;
		 int  size;
{
#define CD_BUF_SIZE  65536*10
	
	if(cd_buf==(char*)NULL){
		cd_buf = (char*)malloc(CD_BUF_SIZE);
		if(cd_buf==(char*)NULL){
			fprintf(stderr, "write_child_data: could not allocate memory.\n");
			exit(-1);
		}
	}
	
	memmove(&cd_buf[cd_pos], buf, size);
	cd_pos+=size;
	if(cd_pos>(CD_BUF_SIZE*4/5)){
		flush_child_data();
	}
}

/* flush_child_data --- flush child data */
void flush_child_data()
{
	write(1, cd_buf, cd_pos);
	//fprintf(stderr, "flush size = %d\n", cd_pos);
	cd_pos = 0;
}

/* show error */
void showerr(char *format, ...)
{
				va_list  args;
				va_start(args, format);
				vfprintf(stderr, format, args);
				va_end(args);
}

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
