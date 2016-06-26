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
#include "histogram.h"
#include "analyzer.h"
#include "mtformat.h"
#include "lr3377.h"
#include "fera.h"
#include "builder.h"
#include "v1190.h"

#define VAR_END      "END VARIABLES"

static char title[] = "Analyzer ver 1.00 31-JUL-2000";

#if 0
static char title[] = "FRead ver 1.22 24-JUN-1997";
static char title[] = "FRead ver 1.22 04-JUN-1997";
static char title[] = "FRead ver 1.21 17-JUN-1997  (Stop at the End of Run)";
static char title[] = "FRead ver 1.10 06-MAY-1997";
static char title[] = "FRead ver 1.00 04-MAY-1997";
static char title[] = "FRead ver 2.00 03-JUN-2000";
#endif

extern int neval;
extern int level;

int msgid = -1;
int msgkey = -1;
int parse_error;
int format=FORMAT_UNDEFINED;  /* data format */
extern int pflag;             /* parent process flag */
extern int cflag;             /* child process flag */
int nchild = 0;               /* number of child processes */
extern int childn;            /* child number for child process */
child_data_t cd[MAX_NCHILD];  /* child data */

#define MAX_NFDEF 10
int    nfdef = 0;
char   *deffnam[MAX_NFDEF];
extern FILE   *findef;

char   *filename;
char   *finnam;
int    findat = 0;
FILE   *fout = (FILE*)NULL;
#if USE_PAW
extern FILE   *falias;
#endif
extern int    byte_order;
extern int    swap;

#if 1
int    blksize = 0x100000;  /* Default = 256KByte */
#else
int    blksize = 0x10000;  /* Default = 64KByte */
#endif

extern int    nrun;
extern int    nblk;
unsigned char   *dbuf = (unsigned char*)NULL;
unsigned char   *sbuf = (unsigned char*)NULL;
extern char   *ofile;
int    line;               /* for lex */

extern int    shmflag;
extern int    rootflag;
char   *hbfnam = (char*)NULL;

extern void yyparse();

extern int CheckSignal(); // terminate signal from GRUTinizer


/* show_info --- show information */
static void show_info()
{
	fprintf(stderr, "%s\n", title);
}

/* usage */
void usage(){
	show_info();
	fprintf(stderr, "Analyzer\n");
	fprintf(stderr,
		"  Usage:  %s [-h] [-d def_file] [-o file] [-b blk_size] [file_name]\n", filename);
  fprintf(stderr,
				"  file_name  ... file to read (default=stdin)\n");
  fprintf(stderr,
				"  Option: -h ... show this help\n");
  fprintf(stderr,
				"  Option: -o ... output histograms to the specified file instead of shared memory\n");
  fprintf(stderr,
				"  Option: -d ... read a definition file (default = hist.def) \n");
  fprintf(stderr,
				"          -b ... specify block size in KBytes (default=%d)\n",
					blksize/1024);
}

int init_msg(){
	FILE *fd;

	fd = fopen(KEYFILE, "w");
	if(fd==(FILE*)NULL){
		fprintf(stderr, "init_msg: cound not open key file: %s.\n",
						strerror(errno));
		return(-1);
	}
	fprintf(fd, "%d\n", getpid());
	fclose(fd);
  msgkey = ftok(KEYFILE,'A');
	msgid = msgget(msgkey,0666 | IPC_CREAT);
	return(0);
}

void exit_msg(){
	if(msgid>=0){
		msgctl(msgid, IPC_RMID, NULL);
		msgid = -1;
	}
	remove(KEYFILE);
}

void domsg(){
	mesg_t msgp;
	int  res;
	int  nc;
  while(1){
		res = msgrcv(msgid, &msgp, MaxMsgLen,
								 cflag ? getpid():MSG_CMD, IPC_NOWAIT);
		if(res==0)
			break;
		if(res<0){
			if(errno==EAGAIN)
				return;
			if(errno==ENOMSG)
				return;
			fprintf(stderr, "domsg: error in msgrcv: %s\n", strerror(errno));
			return;
		}
		/* do message */
		if((cflag && msgp.mtype==getpid()) ||
			 (!cflag && msgp.mtype==MSG_CMD)){
			line = 1;
			level = 0;
			fin.type = FIN_STR;
			fin.pstr = &msgp.mtext[0];
			fin.istr = 0;
#if 1
			fprintf(stderr, "\n%s", fin.pstr);
#endif
			neval++;     /* for evaluations in yyparse() */
			yyparse();
			if(pflag){
				/* send the smae message to child processes */
				for(nc=0; nc<nchild; nc++){
					msgp.mtype = cd[nc].pid;
					res = msgsnd(msgid, &msgp, strlen(msgp.mtext)+1, 0);
					if(res<0){
						fprintf(stderr, "task: error in msgsd: %s\n", strerror(errno));
						break;
					}
				}
			}
		}else{
			fprintf(stderr, "domsg: unknown message type %d.\n", msgp.mtype);
			break;
		}
	}
}


	static char *cd_buf = (char*)NULL;
	static int  cd_pos  = 0;


/* reply_parent_buf --- reply to a parent at the end of a buffer */
void reply_parent_buf()
{
	hist_data_t  hd;

	if(!cflag) return;

	// fprintf(stderr, "reply\n");
	hd.hid = -1;
	hd.f1  = 0.;
	hd.f2  = 0.;
	write_child_data((char*)&hd,sizeof(hist_data_t));
	flush_child_data();
}

/* read_child_buf --- read a buffer from child processes*/
int read_child_buf()
{
#define MAX_HIST_DATA 10000
	fd_set readfds;
	struct timeval timeout;
	int nc;
	static hist_data_t hd[MAX_HIST_DATA];
	static int cd_read_prev=0;
	hist_data_p h;
	int   i, k, n, size, tsize;

	if(!pflag) return 0;

	while(1){ /* no loop happens when the final 'break' is active */
  	for(i=0; i<nchild; i++){
  		nc = (i+cd_read_prev+1)%nchild;
  		FD_ZERO(&readfds);
  		FD_SET(cd[nc].readfd, &readfds);
  		n=cd[nc].readfd+1;
  		timeout.tv_sec  = 0;
  		timeout.tv_usec = 0;
  		if(select(n, &readfds, NULL, NULL, &timeout)>0)
  			break;
  	}
  	if(i>=nchild)
			break;

  	cd_read_prev = nc;
		tsize = read(cd[nc].readfd, hd, sizeof(hist_data_t)*MAX_HIST_DATA);
		if(tsize<=0){
			if(errno==EINTR || errno==EAGAIN)
				continue;
		}
		n = tsize/sizeof(hist_data_t);
		if(n*sizeof(hist_data_t)!=tsize){
			n++;
			read_in(cd[nc].readfd, (unsigned char*)hd, &tsize, n*sizeof(hist_data_t));
		}
		h = hd;
		for(i=0; i<n; i++){
			if(h->hid==-1){
				cd[nc].n_sent_buf--;
			}else if(HID_SCALER <= h->hid && h->hid < HID_SCALER_MAX){
				if(dr_exists(h->f1))
					add_scaler(h->hid-HID_SCALER, dr_int(h->f1));
			}else{
				hfil2(h->hid, h->f1, h->f2);
			}
			h++;
  	}
		break; /* no loop (this is the default usage) */
	}

	n = 0;
	for(nc=0; nc<nchild; nc++)
		n += cd[nc].n_sent_buf;

	return(n);
}

// According to the typical event size, you have to reduce the following value,
// otherwize, the analysis in parallel mode, will stop.
#define MAX_N_SENT_BUF  50

/* send_child_buf --- send a buffer to one of child processes to analyze */
void send_child_buf(buf, size)
	unsigned char  *buf;
	int            size;
{
	int i, nc;
	static int cd_send_prev=0;


	if(!pflag) return;
	//		fprintf(stderr, "send\n");

	while(1){
		for(i=0; i<nchild; i++){
			nc = (i+cd_send_prev+1)%nchild;
			if((format==FORMAT_DST_1 && cd[nc].n_sent_buf<MAX_N_SENT_BUF)
				 || cd[nc].n_sent_buf<=0){
				write(cd[nc].writefd,&size,sizeof(int));
				write(cd[nc].writefd,buf,size);
				cd[nc].n_sent_buf++;
				// fprintf(stderr, "NC=%1d, Count=%5d ++\n", nc, cd[nc].n_sent_buf);
				cd_send_prev = nc;
				return;
			}
		}
		// fprintf(stderr, "read_child_buf()\n");

		read_child_buf();
	}
}


/* check_format */
int check_format(buf)
		 unsigned char *buf;
{
	int  i, l;
	l = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
	switch(l){
	case 0xffff0006:
	case 0xffff0600:
	case 0xffff0008:
	case 0xffff0800:
		return(FORMAT_TAMIDAQ);
	case 0x424c4431:
	case 0x4c423144:
		return(FORMAT_BLD1);
	case 0x00010000:
	case 0x01000000:
	case 0x00000001:
	case 0x00000100:
	case 0xffff0000:
		return(FORMAT_MARS);
	}

  if(dst_chk_format(buf)){
		return(FORMAT_DST_1);
	}

	showerr("Unknown format: ");
	for(i=0; i<16; i++){
		showerr("%.2x ", buf[i]);
	}
	showerr("\n");
	return(FORMAT_UNKNOWN);
}

/* read_in */
int read_in(fd, buf, pos, size)
		 int           fd;
		 unsigned char *buf;
		 int           *pos;
		 int            size;
{
	int  tsize;
	while(*pos<size){
		tsize = read(fd, &buf[*pos], size-*pos);
		if(tsize<=0){
			if(errno==EINTR || errno==EAGAIN)
				continue;
			if(*pos==0) return(-1);
      return(*pos);
		}
		*pos += tsize;
	}
	return(*pos);
}


/* file_read */
int file_read()
{
	int       size, tsize, asize, s, h, res;
	int       bsize, tmp;
	int       pos;
	int       nc;
	unsigned char  *p;
	int       endflag=0;

	nblk = 0;
	size = 0;

	if(format==FORMAT_UNDEFINED){
		if(cflag){
			tmp=0;
			read_in(findat, (unsigned char*)&bsize, &tmp, sizeof(int));
			read_in(findat, dbuf, &size, bsize);
		}else{
			read_in(findat, dbuf, &size, 64);
		}
		format = check_format(dbuf);
		/* showerr("format = %d\n", format); */
		switch(format){
		case FORMAT_TAMIDAQ:
			fprintf(stderr, "Data format: RCNP\n");
			break;
		case FORMAT_BLD1:
			fprintf(stderr, "Data format: RCNP BLD1\n");
				break;
		case FORMAT_MARS:
			fprintf(stderr, "Data format: MARS\n");
			swap = !swap;
			break;
		case FORMAT_DST_1:
			showerr("Data format: DST ver 1.0\n");
			break;
		case FORMAT_UNKNOWN:
		default:
			return(-1);
		}
	}
	switch(format){
	case FORMAT_BLD1:
		while(1){
			if(cflag){
				if(size<bsize){
					tmp=0;
					if(read_in(findat, (unsigned char*)&bsize, &tmp, sizeof(int))<0) break;
					if(read_in(findat, dbuf, &size, bsize-size)<0) break;
				}
				s = ntohl(((bld1_header_t*)dbuf)->bsize);
				h = ntohl(((bld1_header_t*)dbuf)->hsize);
			}else{
				if(read_in(findat, dbuf, &size, sizeof(bld1_header_t))<0) break;
				s = ntohl(((bld1_header_t*)dbuf)->bsize);
				h = ntohl(((bld1_header_t*)dbuf)->hsize);
				if(s+h>blksize){
					showerr("Buffer size is too small. Please enlarge the block size"
									"to larger than %d Bytes.\n",	s+h);
					endflag = 1;
					return(-1);
				}
				read_in(findat, dbuf, &size, s+h);
			}
			if(size<s+h){
				showerr("Incomplete data. Ignore %d byte data.\n", size);
				endflag = 1;
				break;
			}
			p = &dbuf[h];
			if(swap){
				swab(p, sbuf, s);
				if(read_blk_rcnp((unsigned short*)sbuf, s)) endflag = 1;
			}else{
				if(read_blk_rcnp((unsigned short*)p, s)) endflag = 1;
			}
			send_child_buf(dbuf, s+h);
			reply_parent_buf();
#if USE_ROOT
			hb_update();
#endif
			size = 0;
			if(shmflag) domsg();
			if (CheckSignal() != 0) { break; } // checks if GRUTinizer is shutting down
		}
		break;
	case FORMAT_TAMIDAQ:
		bsize = blksize;
		while(1){
			if(size<bsize){
				if(cflag){
					tmp=0;
					if(read_in(findat, (unsigned char*)&bsize, &tmp, sizeof(int))<0) break;
					if(read_in(findat, dbuf, &size, bsize)<0) break;
				}else{
					if(read_in(findat, dbuf, &size, blksize)<0) break;
				}
			}
			if(size<bsize){
				showerr("Avoid the last %d bytes (b)\n", size);
				endflag = 1;
				break;
			}
			if(swap){
				swab(dbuf, sbuf, size);
				if(read_blk_rcnp((unsigned short*)sbuf, size)) endflag = 1;
			}else{
				if(read_blk_rcnp((unsigned short*)dbuf, size)) endflag = 1;
			}
			send_child_buf(dbuf, size);
			reply_parent_buf();
#if USE_ROOT
			hb_update();
#endif
			size = 0;
			//if(shmflag) domsg();
			if (CheckSignal() != 0) { break; } // checks if GRUTinizer is shutting down
			domsg();
		}
		break;
	case FORMAT_MARS:
		while(read_in(findat, dbuf, &size, blksize)>=0){
			if(size<blksize){
				showerr("Avoid the last %d bytes (c)\n", size);
				endflag = 1;
				break;
			}
			if(swap){
				swab(dbuf, sbuf, size);
				if(read_blk_mars((unsigned short*)sbuf, size)) endflag = 1;
			}else{
				if(read_blk_mars((unsigned short*)dbuf, size)) endflag = 1;
			}
			send_child_buf(dbuf, size);
			reply_parent_buf();
#if USE_ROOT
			hb_update();
#endif
			size = 0;
			if(shmflag) domsg();
		}
		break;
	case FORMAT_DST_1:
		if(!cflag)
			if(read_in(findat, dbuf, &size, blksize)<0) break;
		if(pflag){
			/* read and send header */
			pos = 0;
			if(dst_read_data(dbuf, &pos, size)!=1){
				fprintf(stderr, "Could not find the position of data start\n");
				exit(-1);
			}

      for(nc=0; nc<nchild; nc++){
        send_child_buf(dbuf, pos);  // send header to each child
      }
			memmove(dbuf, &dbuf[pos], size-pos);
			size -= pos;

			/* read and send data */
			while(read_in(findat, dbuf, &size, blksize)>=0){
				pos = 0;
				while(pos<size*3/4){
					s = pos;
					res = dst_read_data(dbuf, &pos, size);
					if(res<0)	break;
					send_child_buf(&dbuf[s], pos-s);
				}
				if(res<0) break;
				memmove(dbuf, &dbuf[pos], size-pos);
				size -= pos;
#if USE_ROOT
				hb_update();
#endif
				if(shmflag) domsg();
			}
		}else{
			pos = 0;
			if(dst_read_data(dbuf, &pos, size)!=1){   // read header
				fprintf(stderr, "Could not find the position of data start\n");
				exit(-1);
			}
			write(2,dbuf,pos);  // for debug, output header data
			reply_parent_buf();

			memmove(dbuf, &dbuf[pos], size-pos);
			size -= pos;
			bsize = blksize;
			while(1){
				if(cflag){
					tmp=0;  // position (in the memory of the variable 'bsize')
					if(read_in(findat, (unsigned char*)&bsize, &tmp, sizeof(int))<0) break;
					if(read_in(findat, dbuf, &size, bsize)<0) break;
  				if(size<bsize){
  					showerr("Avoid the last %d bytes (d)\n", size);
  					break;
  				}
				}else{
					if(read_in(findat, dbuf, &size, blksize)<0) break;
				}
				res = 0;
				pos = 0;
				while(pos<size*3/4){
					s = pos;
					res=dst_read_data(dbuf, &pos, size);
					if(res<0){
						showerr("Avoid the last %d bytes (e)\n", size-s);
						break;
					}
				}
				if(res<0)	break;
				memmove(dbuf, &dbuf[pos], size-pos);
				size -= pos;
				reply_parent_buf();
				if(shmflag) domsg();
			}
		}
		break;
	default:
		return(-1);
	}
	if(!ofile) domsg();
	if(pflag){
#if 0
		for(nc=0; nc<nchild; nc++){
			close(cd[nc].writefd);
			cd[nc].writefd = -1;
		}
#endif
		while((res=read_child_buf())){
			//fprintf(stderr, "remaining bufs = %d\n", res);
			usleep(100000);
		}

	}

	fprintf(stderr, "---- End of fread. The last block number is %d  ----\n", nblk);
	return(0);
}

void cleanup(){
	if(dbuf){
		free(dbuf);
		dbuf = (unsigned char*)NULL;
	}
	if(sbuf){
		free(sbuf);
		sbuf = (unsigned char*)NULL;
	}
	if(!ofile)
		exit_msg();
	hb_exit();
}

void sig_handler(int signal){
	fprintf(stderr, "Caught signal %d.\n", signal);
	event_exit();
	cleanup();
	//exit(1);
}

/* do initialization task */
int fread_init(bsize)
		 int bsize;
{
	if(bsize)
		blksize = bsize;
	swap = byte_order = htons(0x0102)==0x0102 ? BIG_ENDIAN:LITTLE_ENDIAN;

	dbuf = malloc(blksize+65536);
	if(dbuf==(unsigned char*)NULL){
		fprintf(stderr, "Could not allocate memory(%s)\n", strerror(errno));
		exit(1);
	}

	sbuf = malloc(blksize+65536);
	if(sbuf==(unsigned char*)NULL){
		fprintf(stderr, "Could not allocate memory(%s)\n", strerror(errno));
		exit(1);
	}

  /* initialize sub-modules */
  if(dr_init()) exit(1);
  event_init();
	if(!ofile && init_msg()<0) exit(1);
	return 0;
}

/* do exit task */
int fread_exit(){
  if(dr_exit()) exit(1);

#if USE_PAW
	if(falias){
		fclose(falias);
		falias = (FILE*)NULL;
	}
#endif

	cleanup();
	return(0);
}


/* read definition file */
int fread_readdef()
{
	int  i;
	char *fnam;

	if(nfdef==0){
		/* set the default definition file name */
		deffnam[0] = DEFFNAM;
		nfdef++;
	}

	for(i=0; i<nfdef; i++){
		/* open the definition file */
		fnam = deffnam[i];
		showerr("Read the definition file '%s'.\n", fnam);
		findef = fopen(fnam, "r");
		if(findef==(FILE*)NULL){
			showerr("Could not open the definition file '%s'.\n", fnam);
			exit(1);
		}

		/* read definition file */
		fin.type = FIN_FILE;
		fin.file = findef;
    line = 1;
		parse_error = 0;
		neval++;     /* for evaluations in yyparse() */
		yyparse();

		/* close the definition file */
		fclose(findef);
		findef = (FILE*)NULL;

		/* check the error */
		if(parse_error){
			showerr("Errors are detected in '%s'.\n", fnam);
			exit(1);
		}
	}

#if DEBUG_THINKPAD
  neval++;
	histogram();
#if 0
	dr_show();
	exit(0);
#endif
#endif

	return 0;
}

/* read data ana analize them */
int fread_ana(fd)
		 int fd;
{
	int  i;
	findat = fd;
	finnam = "";

	/* message */
	showerr("Analyze '%s' with blksize=%dkB.\n", finnam, blksize/1024);

  /* main tasks */
#if 0
  for(i=1; i<16; i++)
    signal(i, sig_handler);
#endif

	/* signal(SIGINT,   sig_handler); */
	/* signal(SIGQUIT,  sig_handler); */
	/* signal(SIGPIPE,  sig_handler); */

	/* // signal(SIGHUP,domsg); */
	/* //signal(SIGHUP, sig_handler); */
	/* signal(SIGHUP,SIG_IGN); */

	file_read();

	return 0;
}

#define MAX_NARGS 20

/* main */
int start_analyzer(const char* cmd)
{
	int bsize=0;
  int fin=0; /* standard input */

	clock_t t1,t2;
	t1=clock();

	showerr("ROOT flag on, YEAH! You rock dude! \n");
	rootflag = 1;
	//fin = open("./datatest/run6106.bld", O_RDONLY);
	//fin = open(filename, O_RDONLY);

	FILE* fPipe = popen(cmd,"r");
	fin = fileno(fPipe);

	if(fin<0){
		fprintf(stderr, "Could not open file.\n");
		exit(1);
	}

	if(cflag){
		fout = fopen("/dev/null","w");
	}else{
		fout = stdout;
	}
	fprintf(fout, "%s\n", title);


	/* initialize	 */
  fread_init(bsize);
  fread_readdef();

	/* do analysis */
  fread_ana(fin);

  /* do end tasks */
  fread_exit();

	pclose(fPipe);

	t2 = clock() - t1;
	float seconds = (float)t2 / CLOCKS_PER_SEC;
	showerr("Total running time was %f s. \n", seconds);
}


/*
Local Variables:
mode: C
tab-width: 2
End:
*/
