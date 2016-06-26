/*
	mtformat.h ... MT format definitions
  Copyright (C) 1993, 1994, 1995  A. Tamii
  Author:   A. Tamii
	Facility: Department of Physics, Kyoto University 
	          & Research Center for Nuclear Physics
	Created:  05-SEP-1993 by A. Tamii
  Modified: 07-JAN-1995 by A. Tamii (Solaris 2.3)
  Modified: 06-MAY-1997 by A. Tamii (Degital Unix)
  Modified: 19-OCT-2014 by A. Tamii (extended the BlkHeader to add a block size of long word)
  Modified: 20-AUG-2015 by A. Tamii (DAQ Data ver 4.0)
  Modified: 27-MAR-2016 by A. Tamii (vmedaq version)
*/

typedef struct BlkHeader{ // from Ver. 4.0
  unsigned short		headerID;         /* = 0xFFFF */
  unsigned short		headerSize;
  unsigned short		blockID;
  unsigned short		blockSize;        /* Disposed. Fixed = 0xFFF0 */
  unsigned short		blockNumber;
  unsigned short		numEvents;
  unsigned short                blockSize32_l;    /* 32 bit block size , added on 19-OCT-2014 lower bit*/
  unsigned short                blockSize32_u;    /* 32 bit block size , added on 19-OCT-2014 upper bit*/
} BlkHeader, *BlkHeaderPtr, **BlkHeaderHdl;

typedef struct BlkHeaderV3{ // older block header until 19-OCT-2014 (Ver 3.0 or earlier)
  unsigned short		headerID;         /* = 0xFFFF */
  unsigned short		headerSize;
  unsigned short		blockID;
  unsigned short		blockSize;
  unsigned short		blockNumber;
  unsigned short		numEvents;
} BlkHeaderV3, *BlkHeaderV3Ptr, **BlkHeaderV3Hdl;

typedef struct BlkTrailer{
  unsigned short		trailerID;	  /* = 0xFFEF */
  unsigned short		trailerSize;
} BlkTrailer, *BlkTrailerPtr, **BlkTrailerHdl;

typedef struct EvtHeader{
  unsigned short		headerID;	  /* = 0xFFDF */
  unsigned short		headerSize;
  unsigned short		eventID;
  unsigned short		eventSize;
  unsigned short		eventNumber;
  unsigned short		numFields;
#if false
  unsigned short		fieldFlag;
#endif
} EvtHeader, *EvtHeaderPtr, **EvtHeaderHdl;

typedef struct FldHeader{
  unsigned short		headerID;	 /* = 0xFFCF */
  unsigned short		headerSize;
  unsigned short		fieldID;
  unsigned short		fieldSize;
} FldHeader, *FldHeaderPtr, **FldHeaderHdl;

#define ComVar4_0  (4<<8)  /* version 4.0 */
#define MaxComLen  124


#if 0  // before Ver 4.0 
typedef struct RunComment{
  unsigned int   version;
  unsigned int   byte;
  unsigned int   time;
  unsigned short run;
  unsigned char  comment[MaxComLen];
} RunComment, *RunCommentPtr;
#endif

// revised on 11-NOV-2014  (Ver4.0)
typedef struct RunComment{
  unsigned short version;      /* = 4.0 */
  unsigned short res1;         /* reserved word 1*/
  unsigned int   byte;
  unsigned int   time;
  unsigned short run;
  unsigned short res2;         /* reserved word 2*/
  unsigned char  comment[MaxComLen];
} RunComment, *RunCommentPtr;

/*** Module IDs ***/
# define	ID_Reserved    (0x00<<12)
# define	ID_V1190       (0x01<<12)
# define ID_NimIn       (0x02<<12)
# define ID_MADC32      (0x03<<12)
# define	ID_MYRIAD      (0x04<<12)
# define ID_3351        (0x05<<12)
# define	ID_Scaler      (0x06<<12)
# define	ID_3377        (0x07<<12)
# define	ID_UNIX_TIME   (0x08<<12) // spared ID
# define	ID_V830        (0x09<<12)
# define	ID_4299_MWPC   (0x0a<<12)
# define	ID_ADC_LAS     (0x0b<<12)
# define	ID_TDC_LAS     (0x0c<<12)
# define	ID_FERA_ADC    (0x0d<<12)
# define	ID_FERA_TDC    (0x0e<<12)
# define	ID_CHKSUM      (0x0f<<12)

# define	ModuleIDMask	0xF000L
# define	DataLengthMask	0x0FFFL

/* Header/Trailer ID */
# define	BlkHeaderID	  0xFFFFL
# define	BlkTrailerID	  0xFFEFL
# define	EvtHeaderID	  0xFFDFL
# define	FldHeaderID	  0xFFCFL

/* Block ID */
# define  DataBlockID     0x0000  /* lower 3 bits is crate number */
# define  ScalerBlockID   0x0E00
# define  StartBlockID    0x0F01
# define  EndBlockID      0x0F02
# define  MiddleBlockID   0x0F03
# define  BuiltBlockID    0x0100

/* Event ID */
# define	DataEvent	0
# define	BlockEndEvent	1

/* Field ID */
# define	FieldID	0

/*** Minimum/Maximum size ***/
#define S_BH_MIN  sizeof(BlkHeaderV3)/2
#define S_BH_MAX  sizeof(BlkHeader)/2
#define S_EH_MIN  sizeof(EvtHeader)/2
#define S_EH_MAX  sizeof(EvtHeader)/2
#define S_FH_MIN  sizeof(FldHeader)/2
#define S_FH_MAX  sizeof(FldHeader)/2
#define S_BT_MIN  sizeof(BlkTrailer)/2
#define S_BT_MAX  sizeof(BlkTrailer)/2
