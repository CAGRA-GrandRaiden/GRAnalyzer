/* lr3351.h ---- LeCroy 3351 Peak Sensing ADC Definitions               */
/*                                                                      */
/* Copyright (C) 1996, Atsushi Tamii                                    */
/*                                                                      */
/*  Version 1.00 26-APR-1998 by A. Tamii (from fera.h)                  */ 
/*  Version 1.10 22-MAY-1998 by A. Tamii (bug fix)                      */ 

#if defined(OSF1) || defined(Linux) /*----- OSF1 and Linux ----*/

/* 3351 Header */
typedef struct lr3351_header{
	unsigned vsn:   8;      /* Virtual Station Number */
	unsigned cnt:   4;      /* Valid Data Counter */
	unsigned res:   3;      /* Not Used */
  unsigned hdr:   1;      /* Identifies header word  (=1) */
} lr3351_header;

/* 3351 Pattern Word (Compressed Mode)*/
typedef struct lr3351_pat{
	unsigned pat:   8;      /* Pattern Word (LSB = ch. 0) */
  unsigned res:   7;      /* Not Used */
	unsigned hdr:   1;      /* Identifies header word (=0) */
} lr3351_pat;

/* 3351 Data (Compressed Mode) */
typedef struct lr3351_data{
	unsigned data: 12;      /* Data */
	unsigned ch:    3;      /* Channel number */
	unsigned ovf:   1;      /* Overflow bit */
} lr3351_data;

/* Virtual Station Number ID */
typedef struct lr3351_vsn{
	unsigned id:    4;      /* ID */
	unsigned det:   2;      /* Detector Type (0=GR, 1=LAS, 2=GSO, 3=SSD) */
  unsigned t3351: 1;      /* Type 3351 (=1 for 3351)*/
	unsigned type:  1;      /* FERA_TYPE_ADC or FERA_TYPE_TDC */
} lr3351_vsn;


#else /*----- Others ----*/

/* 3351 Header */
typedef struct lr3351_header{
  unsigned hdr:   1;      /* Identifies header word  (=1) */
	unsigned res:   3;      /* Not Used */
	unsigned cnt:   4;      /* Valid Data Counter */
	unsigned vsn:   8;      /* Virtual Station Number */
} lr3351_header;

/* 3351 Pattern Word (Compressed Mode)*/
typedef struct lr3351_pat{
	unsigned hdr:   1;      /* Identifies header word (=0) */
  unsigned res:   7;      /* Not Used */
	unsigned pat:   8;      /* Pattern Word (LSB = ch. 0) */
} lr3351_pat;

/* 3351 Data (Compressed Mode) */
typedef struct lr3351_data{
	unsigned ovf:   1;      /* Overflow bit */
	unsigned ch:    3;      /* Channel number */
	unsigned data: 12;      /* Data */
} lr3351_data;

/* Virtual Station Number ID */
typedef struct lr3351_vsn{
	unsigned type:  1;      /* FERA_TYPE_ADC or FERA_TYPE_TDC */
  unsigned t3351: 1;      /* Type 3351 (=1 for 3351)*/
	unsigned det:   2;      /* Detector Type (0=GR, 1=LAS, 2=GSO, 3=SSD) */
	unsigned id:    4;      /* ID */
} lr3351_vsn;

#endif /*----- end of if OSF1 and Linux ----*/

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
