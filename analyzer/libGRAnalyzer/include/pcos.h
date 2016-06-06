/* pcos.h ---- PCOSIII (MWPC) Definitions                       */
/*                                                              */
/* Copyright (C) 1996, Atsushi Tamii                            */
/*                                                              */
/*  Version 1.00        12-MAY-1997     by A. Tamii             */

#if !defined(alpha) && !defined(i586) && !defined(i686) && !defined(x86_64)
    /*---- others (big endian) ---- */

#if defined (__cplusplus)
extern "C" {
#endif
/* PCOS Delimiter */
typedef struct pcos_delim{
  unsigned hdr:   1;      /* Identifies header word  (=1) */
	unsigned delim: 1;      /* delimiter bit (=1) */
  unsigned pcos:  4;      /* PCOS number */
	unsigned res:  10;      /* Reserved */
} pcos_delim;

/* PCOS Cluster Width */
typedef struct pcos_width{
  unsigned hdr:   1;      /* Identifies header word  (=1) */
  unsigned res:   9;      /* Reserved */
	unsigned width: 4;      /* Cluster width */
} pcos_width;

/* PCOS Data */
typedef struct pcos_data{
	unsigned hdr:   1;      /* Identifies header word (=0) */
	unsigned wire: 15;      /* wire */
} pcos_data;

/* PCOS Module */
typedef struct pcos_mod{
	unsigned hdr:   1;      /* Identifies header word (=0) */
	unsigned addr:  9;      /* Logical Address */
	unsigned ch:    5;      /* Channel */
	unsigned h:     1;      /* Half bit */
} pcos_mod;

/* PCOS Wire */
typedef struct pcos_chamb{
	unsigned hdr:   1;      /* Identifies header word (=0) */
	unsigned chamb: 4;      /* Logical Address (Chamber ID)*/
                          /* (X,U,V,-)/(1,2,3,4) */
	unsigned wire: 11;      /* wire id (with half bit) */ 
} pcos_chamb;

#else  /*----- Alpha or Intel-PC (little endian) ----*/

/* PCOS Delimiter */
typedef struct pcos_delim{
	unsigned res:  10;      /* Reserved */
  unsigned pcos:  4;      /* PCOS number */
	unsigned delim: 1;      /* delimiter bit (=1) */
  unsigned hdr:   1;      /* Identifies header word  (=1) */
} pcos_delim;

/* PCOS Cluster Width */
typedef struct pcos_width{
	unsigned width: 4;      /* Cluster width */
  unsigned res:   9;      /* Reserved */
  unsigned hdr:   1;      /* Identifies header word  (=1) */
} pcos_width;

/* PCOS Data */
typedef struct pcos_data{
	unsigned wire: 15;      /* wire */
	unsigned hdr:   1;      /* Identifies header word (=0) */
} pcos_data;

/* PCOS Module */
typedef struct pcos_mod{
	unsigned h:     1;      /* Half bit */
	unsigned ch:    5;      /* Channel */
	unsigned addr:  9;      /* Logical Address */
	unsigned hdr:   1;      /* Identifies header word (=0) */
} pcos_mod;

/* PCOS Wire */
typedef struct pcos_chamb{
	unsigned wire: 11;      /* wire id (with half bit) */ 
	unsigned chamb: 4;      /* Logical Address (Chamber ID)*/
                          /* (X,U,V,-)/(1,2,3,4) */
	unsigned hdr:   1;      /* Identifies header word (=0) */
} pcos_chamb;

#if defined (__cplusplus)
}
#endif
#endif

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
