/*
  fread_mars.c --- rawdata readout program for analyzer (mars format)
  Author:   A. Tamii
  Facility: Department of Physics, University of Tokyo
  Created:  04-MAY-1997 by A. Tamii
  Version 1.00 04-MAY-1997 by A. Tamii (fread)
  Version 1.22 06-MAY-1997 by A. Tamii (fread)
  Version 2.00 03-JUN-2000 by A. Tamii
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#include "freadin.h"
#include "histogram.h"
#include "lr3377.h"
#include "fera.h"
#include "lr3351.h"

#define ENDRUNBLK   0     /* End of Fread at the end of run block */

//FLAG

extern int neval;
extern int level;
extern int swap;
extern int byte_order;
int parse_error;

extern int nblk;
static int nevt = 0;

/* definitions about mars */
#define StartBlockHeader  0x0001
#define EndBlockHeader    0xffff
#define DataBlockHeader   0x0000

/* read one event */
static unsigned short *read_one_event(buf, te)
		 unsigned short  *buf;
		 unsigned short  *te;
{
	unsigned short *ptr;
	fera_header    fera;
	fera_data      dfera;
	lr3377_data    d3377;
	lr3351_data    d3351;
	lr3351_pat     p3351;
  int id, ch, len, pat, c;
  int i;

	ptr = buf;
	while(ptr < te){
		/* get the next region header */
		if(!(*ptr&0x8000)){
      showerr("Illegal data without header (%.4x).\n", *ptr);
			while(ptr<te && !(*ptr&0x8000)) ptr++;
			continue;
		}
		id = *ptr & 0x00ff;
		if(id==0x00 && ptr!=buf)  /* end of event */
			break;
		/* check the region header */
		if(id&0x80){  /* 3377 */
			ptr++;
			for(i=0; ptr<te; i++){
				*(short*)&d3377 = *ptr++;
				if(d3377.hdr){
					ptr--;
					break;
				}
				ch = d3377.ch;
				switch(id){
				case 0x80:  /* X1 */
					dr_append(F2_WIRE_X1, (double)d3377.ch);
					dr_append(F2_TDC_X1, (double)d3377.data);
					break;
				case 0x81:  /* X2 */
					dr_append(F2_WIRE_X2, (double)d3377.ch);
					dr_append(F2_TDC_X2, (double)d3377.data);
					break;
				case 0x82:  /* X3 */
					dr_append(F2_WIRE_X3, (double)d3377.ch);
					dr_append(F2_TDC_X3, (double)d3377.data);
					break;
				case 0x83:  /* X4 */
					dr_append(F2_WIRE_X4, (double)d3377.ch);
					dr_append(F2_TDC_X4, (double)d3377.data);
					break;
				case 0x84:  /* Y1, Y2 */
					if(d3377.ch<16){
						dr_append(F2_WIRE_Y1, (double)(d3377.ch&0x0f));
						dr_append(F2_TDC_Y1, (double)d3377.data);
					}else{
						dr_append(F2_WIRE_Y2, (double)(d3377.ch&0x0f));
						dr_append(F2_TDC_Y2, (double)d3377.data);
					}
					break;
				case 0x85:  /* Y3, Y4 */
					if(d3377.ch<16){
						dr_append(F2_WIRE_Y3, (double)(d3377.ch&0x0f));
						dr_append(F2_TDC_Y3, (double)d3377.data);
					}else{
						dr_append(F2_WIRE_Y4, (double)(d3377.ch&0x0f));
						dr_append(F2_TDC_Y4, (double)d3377.data);
					}
					break;
				}
			}
		}else if(64<=id && id<=69){  /* 3351 SSD */
			ptr++;
			id=(id-64)<<3;
			for(i=0; ptr<te; i++){
				*(short*)&p3351 = *ptr++;
				if(p3351.hdr){
					ptr--;
					break;
				}
				if(ptr>=te) break;
				pat = p3351.pat;
				for(ch=0; ch<8; ch++){
					if(pat & (1<<ch)){
						*(short*)&d3351 = *ptr++;
						if(d3351.ch != ch){
							showerr("Inconsistency in the 3351 pattern(%xH,ch=%d) and channel data(%d).\n",
											pat, ch, d3351.ch);
						}else{
							c = id+ch;
							dr_set(F2_3351[c],d3351.data);
							if(d3351.data>500)
								dr_append(F2_3351_HIT,c);
							switch(c/10){
							case 0:
								dr_append(SSD_LX_CH, c%10);
								dr_append(SSD_LX_ADC, d3351.data);
								break;
							case 1:
								dr_append(SSD_LY_CH, c%10);
								dr_append(SSD_LY_ADC, d3351.data);
								break;
							case 2:
								dr_append(SSD_RX_CH, c%10);
								dr_append(SSD_RX_ADC, d3351.data);
								break;
							case 3:
								dr_append(SSD_RY_CH, c%10);
								dr_append(SSD_RY_ADC, d3351.data);
								break;
							default:
								break;
							}
						}
					}
					if(ptr>=te) break;
				}
			}
		}else{        /* FERA */
			*(short*)&fera = *ptr++;
			len = ((fera.cnt+15) & 0x0f)+1;
      /*
			  	The upper line is equivalent to:
      			len = fera.cnt==0 ? 16 : fera_cnt;
      */
			for(i=0; i<len && ptr<te; i++){
				*(short*)&dfera = *ptr++;
				if(dfera.hdr){
					ptr--;
					break;
				}else{
					ch = (id<<4)+dfera.ch;
					dr_set(F2_FERA[ch], dfera.data);
				}
			}
		}
	}
	return(ptr);
}

/* read_evt */
static int read_evt(buf, size)
		 unsigned short  *buf;
		 int             size;
{
	unsigned short *ptr, *te;

	ptr = buf;
	te = &buf[size];
	nevt = 0;
	while(ptr<te){
		nevt++;
		dr_clear();
		dr_set(BLOCK_N[0], nblk);
		dr_set(EVENT_N[0], nevt);
    dr_set(EVENT_F2[0], 1);
#if 0
		fprintf(stderr, "blk = %5d, evt = %5d\n", nblk, nevt);
#endif
		/* read one event */
		ptr = read_one_event(ptr, te);
		dr_event();
	}
	return nevt;
}


/* read comment block */
static int read_com(buf)
		 char  *buf;
{
	char com1[30], com2[30], com3[30], com4[30], com5[30];
	char com6[256];
	strncpy(com1, &buf[0x14], 10);  com1[10]=0;
	strncpy(com2, &buf[0x1e], 19);  com2[19]=0;
	strncpy(com3, &buf[0x31], 19);  com3[19]=0;
	strncpy(com4, &buf[0x44], 19);  com4[19]=0;
	strncpy(com5, &buf[0x57], 9);   com5[9]=0;
	strncpy(com6, &buf[0x64], 160); com6[160]=0;
	showerr("%s\n", com1);
	showerr("%s\n", com2);
	showerr("%s\n", com3);
	showerr("%s\n", com4);
	showerr("%s\n", com5);
	showerr("%s\n", com6);
    return 0;
}
	
/* read block */
int read_blk_mars(buf, size)
		 unsigned short  *buf;
		 int             size;
{
	unsigned short *ptr;
	int header;
  int evtsize;
	int *scalers;

	/* check the block header */
	header = buf[0];
	switch(header){
	case StartBlockHeader:
		showerr("Run Start Block\n");
		read_com((char*)buf);
		return(0);
	case EndBlockHeader:
		showerr("Run End Block\n");
		read_com((char*)buf);
		return(0);
	case DataBlockHeader:
		break;
	default:
		showerr("Unknown block Header. Header = %xH\n", header);
		return(-1);
		break;
	}
  nblk = buf[1];
  evtsize = min(buf[2], size)-3;
  scalers = (int*)&buf[buf[3]];
	ptr = &buf[4];
#if 0
	fprintf(stderr, "Block = %d size = %x\n", nblk, evtsize<<1);
#endif
	
	/* read events */
  if(!evt_blk(nblk)){ /* chack analyze block range */
		(void)read_evt(ptr, evtsize);
	}
	
	/* read scaler  */
  /* not coded yet */
	return(0);
}

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
