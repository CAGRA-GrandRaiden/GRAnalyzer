/*
  ask.c --- ask a command to the analyzer
  Author:   A. Tamii
  Facility: Department of Physics, University of Tokyo
  Created:  04-MAY-1997 by A. Tamii
  Version 1.00 15-JUN-2000 by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#ifdef OSF1
#include <strings.h>
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>

#include "freadin.h"
#include "histogram.h"

static char title[] = "Ask ver 1.00 15-JUN-2000";

#if 0
#endif

int msgid = -1;
int msgkey = -1;
int pid;

char const *filename;
char const *finnam;

/* usage */
void usage(){
	fprintf(stderr, "%s\n", title);
	fprintf(stderr, "Ask a command to the analyzer\n");
	fprintf(stderr,
		"  Usage:  %s [-h] file_name\n", filename);
  fprintf(stderr,
				"  file_name  ... command file (default=stdin)\n");
  fprintf(stderr,
				"  Option: -h ... show this help\n");
}

int init_msg(){
	FILE *fd;

	fd = fopen(KEYFILE, "r");
	if(fd==(FILE*)NULL){
		fprintf(stderr, "init_msg: cound not open key file: %s.\n",
						strerror(errno));
		return(-1);
	}
	fscanf(fd, "%d\n", &pid);
	fclose(fd);
  msgkey = ftok(KEYFILE,'A');
	msgid = msgget(msgkey,0666 | IPC_CREAT);
	return(0);
	
  msgkey = ftok(KEYFILE,'A');
	if(msgkey<0){
		fprintf(stderr, "init_msg: error in ftok: %s\n", strerror(errno));
		return(-1);
	}
	msgid = msgget(msgkey,0);
	if(msgid<0){
		fprintf(stderr, "init_msg: error in msgget: %s\n", strerror(errno));
		return(-1);
	}
	return(0);
}

int exit_msg(){
	return(0);
}

int task(FILE *fin)
{
  mesg_t  msgp;
	int    res;
	char   str[MaxMsgLen];
	int    i;

	msgp.mtype = MSG_CMD;
	while(1){
		fscanf(fin, "%[\n]", str);
		i=fscanf(fin, "%[^\n]", str);
		if(i<0) return(-1);
		if(i==0) break;
		sprintf(msgp.mtext,"%s\n",str);
#if 1
    fprintf(stderr, "%s", msgp.mtext);
#endif
		res = msgsnd(msgid, &msgp, strlen(msgp.mtext)+1, 0);
		if(res<0){
			fprintf(stderr, "task: error in msgsd: %s\n", strerror(errno));
			return(-1);
		}
#if 1
		kill(pid, SIGHUP); /* send a signal to read the message */
#endif
	}
	return(0);
}

/* main */
int main(int argc,char const *argv[])
{
	FILE    *fin;
	filename = *argv++;
	argc--;
	
	/* check the arguments */
	while(argc>0){
		if(argv[0][0]=='-'){
			if(strlen(argv[0])!=2){
				fprintf(stderr, "Unknown flag '%s'.\n",argv[0]);
				usage();
				exit(1);
			}
			switch(argv[0][1]){
			case 'h':
			case 'H':
				usage();
				exit(0);
				break;
			default:
				fprintf(stderr, "Unknown flag '%s'.\n",argv[0]);
				usage();
				exit(1);
			}
		}else{
			break;
		}
		argc--;
		argv++;
	}

	if(argc>1){
		usage();
		exit(1);
	}
	if(argc==1){
		finnam = *argv;
		fin = fopen(finnam, "r");
		if(fin==(FILE*)NULL){
			fprintf(stderr, "Could not open file '%s'.", finnam);
			exit(1);
		}
		argc--;
		argv++;
	}else{
		fin = stdin;  /* standard input */
		finnam = "stdin";
	}
			
	/* initialize message queue */
	if(init_msg()<0)
		exit(1);

  /* main tasks */
	task(fin);

	exit_msg();
	exit(0);
}

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
