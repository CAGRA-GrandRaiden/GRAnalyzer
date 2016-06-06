/* lr3377.h ---- LeCroy 3377 (Drift Chamber TDC) Definitions                     */
/*                                                                               */
/* Copyright (C) 1996, Atsushi Tamii                                             */
/*                                                                               */
/*  Version 1.00        26-MAY-1996     by A. Tamii                              */
/*  Version 2.10 26-APR-1998 by A. TAmii (restored from 13-MAR-1997 and modified)*/

#if !defined(alpha) && !defined(i586) && !defined(i686) && !defined(x86_64)
    /*---- others (big endian) ---- */

/* LR3377 Header */
typedef struct lr3377_header{
  unsigned hdr:   1;      /* Identifies header word  (=1) */
	unsigned word:  1;      /* Double(1)/Single(0) word format */
	unsigned evt:   3;      /* Event serial number */
	unsigned edge:  1;      /* Leading(0) or leading/trailing(1) edge */
  unsigned res:   2;      /* Resolution */
	unsigned id:    8;      /* Module ID */
} lr3377_header;

/* LR3377 Data (Common) */
typedef struct lr3377_data{
	unsigned hdr:   1;      /* Identifies header word (=0) */
	unsigned ch:    5;      /* Channel number */
	unsigned data: 10;      /* Data */
} lr3377_data;

/* Single word format, leading edge only */
typedef struct lr3377_sgl_led{   
	unsigned hdr:   1;      /* Identifies header word (=0) */
	unsigned ch:    5;      /* Channel number */
	unsigned data: 10;      /* Data */
} lr3377_sgl_led;

/* Single word format, leading/trailing edge */
typedef struct lr3377_sgl_trl{   
	unsigned hdr:   1;      /* Identifies header word (=0) */
	unsigned ch:    5;      /* Channel number */
  unsigned edge:  1;      /* leading(0) or trailing(1) */
	unsigned data:  9;      /* Data */
} lr3377_sgl_trl;

/* Double word format */
typedef struct lr3377_dbl{
	unsigned hdr:   1;      /* Identifies header word (=0) */
	unsigned ch:    5;      /* Channel number */
  unsigned edge:  1;      /* leading(0) or trailing(1) */
  unsigned word:   1;     /* First word(0) or second word(1) */
	unsigned data:  8;      /* Data */
} lr3377_dbl;

/* Module ID */
typedef struct lr3377_mid{
	unsigned spe:   1;      /* Spectrometer (0=GR, 1=LAS) */
	unsigned vdc:   1;      /* VDC (Front=0, Rear=1) */
	unsigned cor:   2;      /* X=0, Y=1, U=2, V=3 */
	unsigned id:    4;      /* ID */
} lr3377_mid;


#else  /*----- Alpha or Intel-PC (little endian) ----*/

/* LR3377 Header */
typedef struct lr3377_header{
	unsigned id:    8;      /* Module ID */
  unsigned res:   2;      /* Resolution */
	unsigned edge:  1;      /* Leading(0) or leading/trailing(1) edge */
	unsigned evt:   3;      /* Event serial number */
	unsigned word:  1;      /* Double(1)/Single(0) word format */
  unsigned hdr:   1;      /* Identifies header word  (=1) */
} lr3377_header;

/* LR3377 Data (Common) */
typedef struct lr3377_data{
	unsigned data: 10;      /* Data */
	unsigned ch:    5;      /* Channel number */
	unsigned hdr:   1;      /* Identifies header word (=0) */
} lr3377_data;

/* Single word format, leading edge only */
typedef struct lr3377_sgl_led{   
	unsigned data: 10;      /* Data */
	unsigned ch:    5;      /* Channel number */
	unsigned hdr:   1;      /* Identifies header word (=0) */
} lr3377_sgl_led;

/* Single word format, leading/trailing edge */
typedef struct lr3377_sgl_trl{   
	unsigned data:  9;      /* Data */
  unsigned edge:  1;      /* leading(0) or trailing(1) */
	unsigned ch:    5;      /* Channel number */
	unsigned hdr:   1;      /* Identifies header word (=0) */
} lr3377_sgl_trl;

/* Double word format */
typedef struct lr3377_dbl{
	unsigned data:  8;      /* Data */
  unsigned word:   1;     /* First word(0) or second word(1) */
  unsigned edge:  1;      /* leading(0) or trailing(1) */
	unsigned ch:    5;      /* Channel number */
	unsigned hdr:   1;      /* Identifies header word (=0) */
} lr3377_dbl;

/* Module ID */
typedef struct lr3377_mid{
	unsigned id:    4;      /* ID */
	unsigned cor:   2;      /* X=0, Y=1, U=2, V=3 */
	unsigned vdc:   1;      /* VDC (Front=0, Rear=1) */
	unsigned spe:   1;      /* Spectrometer (0=GR, 1=LAS) */
} lr3377_mid;

#endif

#define MID_DELIMITER1    0x00FF    /* Delimiter Module ID */
#define MID_DELIMITER2    0x00E7    /* Delimiter Module ID */

#if 0
#define vsn_delim(vsn)  (vsn==MID_DELIMITER1 || vsn==MID_DELIMITER2)
#endif

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
