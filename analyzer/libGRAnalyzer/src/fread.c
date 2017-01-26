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
#include "builder.h"
#include "histogram.h"

static char title[] = "Analyzer ver 1.00 31-JUL-2000";

#if 0
static char title[] = "FRead ver 1.22 24-JUN-1997";
static char title[] = "FRead ver 1.22 04-JUN-1997";
static char title[] = "FRead ver 1.21 17-JUN-1997  (Stop at the End of Run)";
static char title[] = "FRead ver 1.10 06-MAY-1997";
static char title[] = "FRead ver 1.00 04-MAY-1997";
static char title[] = "FRead ver 2.00 03-JUN-2000";
#endif


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


#if USE_GRUTINIZER // added on 2017.1.25 by A. Tamii
#else
extern int neval;
extern int level;

int msgid = -1;
int msgkey = -1;
int parse_error;
int format=FORMAT_UNDEFINED;      /* data format */
int nchild = 0;               /* number of child processes */
child_data_t cd[MAX_NCHILD];  /* child data */

#define MAX_NFDEF 10
int    nfdef = 0;
char   *deffnam[MAX_NFDEF];

char   *filename;
char   *finnam;
int    findat = 0;
FILE   *fout = (FILE*)NULL;

#if 1
int    blksize = 0x100000;  /* Default = 256KByte */
#else
int    blksize = 0x10000;  /* Default = 64KByte */
#endif

unsigned char   *dbuf = (unsigned char*)NULL;
unsigned char   *sbuf = (unsigned char*)NULL;

char   *hbfnam = (char*)NULL;

extern void yyparse();
#endif


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


#if USE_GRUTINIZER // added on 2017.1.25 by A. Tamii
#else

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
				//if(read_blk_mars((unsigned short*)sbuf, size)) endflag = 1;
			}else{
				//if(read_blk_mars((unsigned short*)dbuf, size)) endflag = 1;
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
	exit(1);
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
	
	signal(SIGINT,   sig_handler);
	signal(SIGQUIT,  sig_handler);
	signal(SIGPIPE,  sig_handler);

	// signal(SIGHUP,domsg);
	//signal(SIGHUP, sig_handler);
	signal(SIGHUP,SIG_IGN);

	file_read();

	return 0;
}

#define MAX_NARGS 20

/* main */
int main(argc, argv)
		 int   argc;
		 char  *argv[];
{
	int i;
	int ac;
	char **av;
	int bsize=0;
  int fin;
	pid_t pid;
	int   pfd, pfd1[2], pfd2[2];
	char *newargv[MAX_NARGS+3];
	char  str[256];
	char  cnum[256];

	//	for(i=0; i<argc; i++){
	//		fprintf(stderr, "arg %2d = '%s'\n",i,argv[i]);
	//	}
	//	exit(0);

	ac = argc;
	av = argv;

	filename = *av++;
	ac--;

	/* check the arguments */
	while(ac>0){
		if(av[0][0]=='-'){
			if(strlen(av[0])!=2){
				fprintf(stderr, "Unknown flag '%s'.\n",av[0]);
				usage();
				exit(1);
			}
			switch(av[0][1]){
			case 'b':
				ac--;
				av++;
				if(ac<=0){
					fprintf(stderr, "no argument to '-b'.\n");
					usage();
					exit(1);
				}
				bsize = atoi(*av)*1024;
				break;
#if 0
			case 's':
				swap = !swap;
				break;
#endif
			case 'd':
				ac--;
				av++;
				if(ac<=0){
					fprintf(stderr, "no argument to '-d'.\n");
					usage();
					exit(1);
				}
				deffnam[nfdef] = *av;
				nfdef++;
				break;
			case 'o':
				ac--;
				av++;
				if(ac<=0){
					fprintf(stderr, "no argument to '-o'.\n");
					usage();
					exit(1);
				}
				ofile = *av;
				break;
			case 'f':
				ac--;
				av++;
				if(ac<=0){
					fprintf(stderr, "no argument to '-f'.\n"
									);
					usage();
					exit(1);
				}
				shmflag = 0;
				hbfnam = *av;
				break;
			case 'p':
				ac--;
				av++;
				if(ac<=0){
					fprintf(stderr, "no argument to '-p'.\n"
									);
					usage();
					exit(1);
				}
				nchild = atoi(*av);
				if(nchild<1){
					fprintf(stderr, "Argument of p (number of child processes) "
									"must be larger than 0.\n");
					usage();
					exit(1);
				}
				if(MAX_NCHILD < nchild){
					fprintf(stderr, "Argument of p (number of child processes) "
									"must be <= %d (=MAX_NCHILD).\n", MAX_NCHILD);
					usage();
					exit(1);
				}
				if(!cflag){  /* cflag has higher priority than pflag */
					pflag = 1;
				}
				break;
			case 'c':
				ac--;
				av++;
				if(ac<=0){
					fprintf(stderr, "no argument to '-c'.\n"
									);
					usage();
					exit(1);
				}
				cflag = 1;
				childn = atoi(*av);
				fprintf(stderr, "Child process number = %d\n", childn);
				fprintf(stderr, "Process ID = %d\n", getpid());
				if(pflag){
					pflag = 0;
					nchild = 0;
				}
				break;
			case 'h':
			case 'H':
				usage();
				exit(0);
				break;
			default:
				fprintf(stderr, "Unknown flag '%s'.\n",av[0]);
				usage();
				exit(1);
			}
		}else{
			break;
		}
		ac--;
		av++;
	}

	if(cflag){
		fout = fopen("/dev/null","w");
	}else{
		fout = stdout;
	}
	fprintf(fout, "%s\n", title);

	if(ac>1){
		usage();
		exit(1);
	}

	
	fin = 0;  /* standard input */
	if(!cflag && ac==1){
		if(*av){
			/* open data file */
			fin = open(*av, O_RDONLY);
			if(fin<0){
				fprintf(stderr, "Could not open file '%s'.", *av);
				exit(1);
			}
		}
	}

	/* create child processes */
	if(pflag){
		/* create arguments */
		if(argc>MAX_NARGS){
			fprintf(stderr, "main: too many arguments to fork\n");
			exit(-1);
		}
		newargv[0] = argv[0];
		newargv[1] = "-c";   /* append -c flag */
		newargv[2] = cnum;   /* child number */
		for(i=1; i<argc; i++){
			newargv[i+2] = argv[i];
		}
		newargv[i+2] = (char*)NULL;

		/* fork child processes */
		for(i=0; i<nchild; i++){
			if(pipe(pfd1)<0 || pipe(pfd2)<0){
				fprintf(stderr, "main: Error in pipe();\n");
				exit(-1);
			}
			pid = fork();
			if(pid==-1){
				fprintf(stderr, "main: Error in fork();\n");
				exit(-1);
			}
			if(pid==0){
				/* child process */
 				close(0); dup(pfd1[0]);
				close(1); dup(pfd2[1]);
				close(pfd1[0]);
				close(pfd1[1]);
				close(pfd2[0]);
				close(pfd2[1]);
				sprintf(str, FNAM_CHILD_STDERR, i);
				pfd = open(str,O_WRONLY|O_CREAT|O_TRUNC,0644);
				if(pfd==-1){
					fprintf(stderr, "Error in opening file '%s'.\n", str);
					perror("open");
					exit(-1);
				}
				close(2); dup(pfd);
				sprintf(cnum, "%d", i);
				/* exec*/
				execvp(argv[0], newargv);
				/* Never come here */
			}
			/* parent process */
			cd[i].pid = pid;
			close(pfd1[0]);
			close(pfd2[1]);
			cd[i].readfd = pfd2[0];
			cd[i].writefd = pfd1[1];
		}
	}

	/* initialize	 */
  fread_init(bsize);
  fread_readdef();

	/* do analysis */

  fread_ana(fin);

  /* do end tasks */

  fread_exit();
}

#if defined (f2cFortran)
void * MAIN__ = main;
#endif

#endif

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
