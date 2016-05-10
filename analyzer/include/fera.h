/* fera.h ---- LeCroy 4300B (FERA/FERET) Definitions                    */
/*                                                                      */
/* Copyright (C) 1996, Atsushi Tamii                                    */
/*                                                                      */
/*  Version 1.00 15-JUN-1996 by A. Tamii                                */
/*  Version 2.00 05-AUG-1997 by A. Tamii (vsn_delim() for FERA-GSO)     */

#if defined (__cplusplus)
extern "C" {
#endif

#if !defined(alpha) && !defined(i586) && !defined(i686) && !defined(x86_64)
    /*---- others (big endian) ---- */
/* FERA Header */
typedef struct fera_header{
  unsigned hdr:   1;      /* Identifies header word  (=1) */
  unsigned cnt:   4;      /* Word Count */
  unsigned res:   3;      /* Not Used */
  unsigned vsn:   8;      /* Virtual Station Number */
} fera_header;

/* FERA Data (Compressed Mode) */
typedef struct fera_data{
  unsigned hdr:   1;      /* Identifies header word (=0) */
  unsigned ch:    4;      /* Channel number */
  unsigned data: 11;      /* Data */
} fera_data;

/* Virtual Station Number ID */
typedef struct fera_vsn{
  unsigned type:  1;      /* FERA_TYPE_ADC or FERA_TYPE_TDC */
  unsigned res:   2;      /* Reserved */
  unsigned spe:   1;      /* Spectrometer (0=GR, 1=LAS) */
  unsigned id:    4;      /* ID */
} fera_vsn;

#else  /*----- Alpha or Intel-PC (little endian) ----*/

/* FERA Header */
typedef struct fera_header{
  unsigned vsn:   8;      /* Virtual Station Number */
  unsigned res:   3;      /* Not Used */
  unsigned cnt:   4;      /* Word Count */
  unsigned hdr:   1;      /* Identifies header word  (=1) */
} fera_header;

/* FERA Data (Compressed Mode) */
typedef struct fera_data{
  unsigned data: 11;      /* Data */
  unsigned ch:    4;      /* Channel number */
  unsigned hdr:   1;      /* Identifies header word (=0) */
} fera_data;

/* Virtual Station Number ID */
typedef struct fera_vsn{
  unsigned id:    4;      /* ID */
  unsigned spe:   1;      /* Spectrometer (0=GR, 1=LAS) */
  unsigned res:   2;      /* Reserved */
  unsigned type:  1;      /* FERA_TYPE_ADC or FERA_TYPE_TDC */
} fera_vsn;

#endif   /*----- end of OSF1 ----*/

#if defined (__cplusplus)
}
#endif

# define VSN_TDC           0x80  /* VSN TDC Mask */

# define VSN_DELIMITER1    0xF0  /* Delimiter VSN */
# define VSN_DELIMITER2    0x11  /* Delimiter VSN */

#if 0
#define vsn_delim(vsn)  (vsn==VSN_DELIMITER1 || vsn==VSN_DELIMITER2)
#define vsn_delim(vsn)  (vsn==0xF0)  /* for E07 Jul96 */
#else
#define vsn_delim(vsn)  ((vsn&0x8F)==0x01)
#endif
