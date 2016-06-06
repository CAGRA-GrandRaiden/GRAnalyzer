/*
	builder.h ... Definitions for Event Builder
  Copyright (C) 1995  A. Tamii
  Author:   A. Tamii
	Facility: Department of Physics, Kyoto University 
	          & Research Center for Nuclear Physics
  Created:  30-JUn-1995 by A. Tamii
*/

/*** constant definitions ***/
#ifndef TRUE
#define FALSE 0
#define false 0
#define TRUE  1
#define true  1
#endif

/*** for DEBUG ***/
#define DBALL     0xFFFF
#define DBBLOCK   0x0001
#define DBEVENT   0x0002
#define DBFIELD   0x0004
#define DBDATA    0x0008
#define DBBLKEND  0x0010
#define DBRFM     0x0200
#define DBBASE    0x1000

#define DEBUG     (DBBASE)

/*** Procedures ***/
enum {
  ProcJ11=0,               /* 0 = Data From J11                         */
  ProcRDTM,                /* 1 = Data From RDTM (PCOS or 4298)         */
  Proc3377,                /* 2 = Data From 3377 (without RDTM)         */
  ProcFERA,                /* 3 = Data From 3377 (without RDTM)         */
  ProcFCET_3377,           /* 4 = Data From FCET (for 3377)             */
  ProcFCET_FERA,           /* 5 = Data From FCET (for FERA)             */
  ProcFCET_3351,           /* 6 = Data From FCET (for peak sensing ADC) */
  ProcFCET_1612,           /* 7 = Data From FCET (for Pantek 1612F(QDC)/812F(TDC) */
  NumProcs
};

/*** Buffer Types ***/
#define BufData      0x0001
#define BufBlockEnd  0x0002
#define BufTrans     0x0004

/*** Block Types ***/
#define BLK_START    0x0010    /* run start block */
#define BLK_END      0x0020    /* run end block */
#define BLK_MIDDLE   0x0040    /* middle block */
#define BLK_DATA     0x0001    /* contain data */
#define BLK_BUILD    0x0002    /* start building */
#define BLK_TERM     0x0100    /* end of event building */

/*** Buffer Error Code ***/
#define BF_ERR_FATAL     0x80000000
#define BF_ERR_CNT_MASK  0x7FFFFFFF
#define BF_ERR_MAX    100          /* maxmimum error count */
	
/*** base buffer size */
#define BufSize 0x4000
#define MaxBufSize  0x100000

/* Number of events for FCET nad RDTM scaler check */
#define MinNumEvents     0
#define MaxNumEvents  3000

/*** type definitions ***/
typedef struct BufInfo{
  int            buf_type;
  unsigned short *buf;
  unsigned short *end;
  unsigned short *ptr;
  int            size;
  int            proc_type;
  int            proc_subtype;
  int            blk_num;
  int            evt_num;
  int            error;
  int            shm_pos;
  int            work[16];
}BufInfo, *BufInfoPtr;

typedef struct ProcInfo{
  void           (*proc_init)(BufInfoPtr);
  int            (*proc_assign)(BufInfoPtr);
  unsigned short *(*proc_event)(unsigned short*, BufInfoPtr);
  unsigned short *(*proc_blockend)(unsigned short*, BufInfoPtr);
} ProcInfo, *ProcInfoPtr;

typedef struct bld1_header{  /* all components are defined by the network byte order */
  int            id;        /* identification word ('BLD1') */
  int            seq_num;   /* sequential number */
  int            bsize;     /* block size in bytes */
  int            hsize;     /* header size in byte */
  int            prev_pos1; /* absolute prev buffer position in byte (upper)*/
  int            prev_pos2; /*                                       (lower)*/
  int            next_pos1; /* absolute prev buffer position in byte (upper)*/
  int            next_pos2; /*                                       (lower)*/
} bld1_header_t;

#define BLD1_ID   0x424c4431 /*. 'BLD1' 1 means the version 1.0 */

/* router_builder.c/event_builder.c */
int get_output_buf(int, BufInfoPtr, BufInfoPtr);
int release_output_buf(BufInfoPtr);
int output_buf(BufInfoPtr);
int get_buf(BufInfoPtr);
int release_buf(BufInfoPtr);

void proc_null_init(BufInfoPtr);
int proc_null_assign(BufInfoPtr);
unsigned short *proc_null_event(unsigned short*, BufInfoPtr);
unsigned short *proc_null_blockend(unsigned short*, BufInfoPtr);

void proc_j11_init(BufInfoPtr);
int proc_j11_assign(BufInfoPtr);
unsigned short *proc_j11_event(unsigned short*, BufInfoPtr);
unsigned short *proc_j11_blockend(unsigned short*, BufInfoPtr);

void proc_rdtm_init(BufInfoPtr);
int proc_rdtm_assign(BufInfoPtr);
unsigned short *proc_rdtm_event(unsigned short*, BufInfoPtr);

void proc_3377_init(BufInfoPtr);
int proc_3377_assign(BufInfoPtr);
unsigned short *proc_3377_event(unsigned short*, BufInfoPtr);

void proc_fera_init(BufInfoPtr);
int proc_fera_assign(BufInfoPtr);
unsigned short *proc_fera_event(unsigned short*, BufInfoPtr);

void proc_fcet_init(BufInfoPtr);
int proc_fcet_assign(BufInfoPtr);
unsigned short *proc_fcet_event(unsigned short*, BufInfoPtr);

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
