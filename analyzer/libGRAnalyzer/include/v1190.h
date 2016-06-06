#ifndef V1190_REG_H
#define V1190_REG_H

/*
 * Definitions for V1190
 *
 * Version 0.10  23-DEC-2012  by A. Tamii
 */


/* Size of I/O space */

#define V1190_MAX_N_MODULES     32          /* Max number of V1190 Modules */
#define V1190_N_CH_PER_MODULE  128          /* number of channels per module */
#define V1190_BASE_INI   0xEE000000         /* VME base address start      */
#define V1190_BASE_INC   0x00010000         /* VME base address increment  */
#define V1190_BASE(module_number)  ((V1190_BASE_INI)+(V1190_BASE_INC)*(module_number))

#define V1190_SIZE   0x1206

/* VME addressing mode */
#if 1
#define V1190_MODE  VME_A32UD       /* 32bit Extended Non-Prvileged Data Accesss  */
#else
#define V1190_MODE  VME_A32SD       /* 32bit Extended Prvileged Data Accesss  */
#endif

/*
 * V1190 Memory Register
 */

typedef struct V1190{
  unsigned int output_buffer[0x1000/4];
  unsigned short control_register;
  unsigned short status_register;
  unsigned char  blank1[6];
  unsigned short interrupt_level;
  unsigned short interrupt_vector;
  unsigned short geo_address_register;
  unsigned short mcst_base_address;
  unsigned short mcst_control;
  unsigned short module_reset;
  unsigned short software_clear;
  unsigned short software_event_reset;
  unsigned short software_trigger;
  unsigned int  event_counter;
  unsigned short event_stored;
  unsigned short almost_full_level;
  unsigned short blt_event_number;
  unsigned short firmware_revision;
  unsigned int  testreg;
  unsigned short output_prog_control;
  unsigned short micro;
  unsigned short micro_handshake;
  unsigned short select_flash;
  unsigned short flash_memory;
  unsigned short sram_page;
  unsigned int  event_fifo;
  unsigned short event_fifo_stored;
  unsigned short event_fifo_status;
  unsigned char  blank2[0x01c0];
  unsigned int  dummy32;
  unsigned short dummy16;
} V1190_t, *V1190_p;

#define V1190_CR    		 	0x1000   /* control register */
#define V1190_SR			0x1002   /* status register */
#define V1190_InterruptLevel		0x100A   /* interrupt level */
#define V1190_InterruptVector		0x100C   /* interrupt vector */
#define V1190_GeoAddressRegister	0x100A   /* Geo Address_Register */
#define V1190_MCSTBaseAddress    	0x1010   /* MCST Base Address */
#define V1190_MCSTControl    		0x1012   /* MCST control */
#define V1190_ModuleReset    		0x1014   /* Module Reset */
#define V1190_SoftwareClear    		0x1016   /* Software Clear */
#define V1190_SoftwareEventReset    	0x1018   /* Software Event Reset */
#define V1190_SoftwareTrigger    	0x101A   /* Software Trigger */
#define V1190_EventCounter    		0x101C   /* Event Counter */
#define V1190_EventStored    		0x1020   /* Event Stored */
#define V1190_AlmostFullLevel    	0x1022   /* Almost Full Level */
#define V1190_BLTEventNumber    	0x1024   /* BLT Event Number */
#define V1190_FirmwareRevision    	0x1026   /* Almost Full Level */
#define V1190_Testreg    		0x1028   /* Test Register */
#define V1190_OutputProgControl    	0x102C   /* Output Prog Control */
#define V1190_MicroRegister  		0x102E   /* micro register */
#define V1190_MicroHandshake 		0x1030   /* micro handshake */
#define V1190_SelectFlash  		0x1032   /* Select Flash */
#define V1190_FlashMemory 		0x1034   /* Flash Memory */
#define V1190_SramePage  		0x1036   /* Srame Page */
#define V1190_EventFIFO 		0x1038   /* Event FIFO */
#define V1190_EventFIFOStored 		0x103C   /* Event FIFO Stored */
#define V1190_EventFIFOStatus 		0x103E   /* Event FIFO Status */
#define V1190_Dummy32 			0x1200   /* Dummy 32 */
#define V1190_Dummy16 			0x1204   /* Dummy 16 */


#define V1190_MicroHandshakeRegister_ReadOK 0x0002 /* micro handshake read OK */ 
#define V1190_MicroHandshakeRegister_WriteOK 0x0001/*  micro handshake register write OK */



/*
 * V1190 microcode Opecode, see page 47 of the V1190 manual
 */
/* ACQUISITION MODE */
#define V1190_OPECODE_TRIG_MATCH          0x0000
#define V1190_OPECODE_CONTINUOUS_STORAGE  0x0100
#define V1190_OPECODE_ACQUISITION	  0x0200
#define V1190_OPECODE_SET_KEEP_TOKEN	  0x0300
#define V1190_OPECODE_CLEAR_KEEP_TOKEN	  0x0400
#define V1190_OPECODE_LOAD_DEF_CONFIG     0x0500

/* TRIGGER */
#define V1190_OPECODE_SET_WIN_WIDTH	  0x1000
#define V1190_OPECODE_SET_WIN_OFFS	  0x1100
#define V1190_OPECODE_SET_SW_MARGIN	  0x1200
#define V1190_OPECODE_SET_REJ_MARGIN	  0x1300
#define V1190_OPECODE_EN_SUB_TRG	  0x1400
#define V1190_OPECODE_DIS_SUB_TRG	  0x1500
#define V1190_OPECODE_READ_TRG_CONF	  0x1600

/* TDC EDGE DETECTION & RESOLUTION */
#define V1190_OPECODE_SET_DETECTION	  0x2200
#define V1190_OPECODE_READ_DETECTION	  0x2300
#define V1190_OPECODE_READ_RES		  0x2600
#define V1190_OPECODE_SET_DEAD_TIME	  0x2800
#define V1190_OPECODE_READ_DEAD_TIME	  0x2900

/* TDC READOUT */
#define V1190_OPECODE_EN_HEAD_TRAILER	  0x3000
#define V1190_OPECODE_DIS_HEAD_TRAILER	  0x3100
#define V1190_OPECODE_READ_HEAD_TRAILER	  0x3200
#define V1190_OPECODE_SET_EVENT_SIZE	  0x3300
#define V1190_OPECODE_READ_EVENT_SIZE	  0x3400
#define V1190_OPECODE_EN_ERROR_MARK	  0x3500
#define V1190_OPECODE_DIS_ERROR_MARK	  0x3600
#define V1190_OPECODE_EN_ERROR_BYPASS	  0x3700
#define V1190_OPECODE_DIS_ERROR_BYPASS	  0x3800
#define V1190_OPECODE_SET_ERROR_TYPES	  0x3900
#define V1190_OPECODE_READ_ERROR_TYPES	  0x3A00
#define V1190_OPECODE_SET_FIFO_SIZE	  0x3B00
#define V1190_OPECODE_READ_FIFO_SIZE	  0x3C00

/* CHANNEL ENABLE */
#define V1190_OPECODE_EN_ALL_CH		  0x4200
#define V1190_OPECODE_DIS_ALL_CH	  0x4300

/* ADJUST */
#define V1190_OPECODE_SET_GLOB_OFFS	  0x5000
#define V1190_OPECODE_READ_GLOB_OFFS	  0x5100




/*
 *  TDC data format @Output Buffer
 */

#if 1  /* Little Endian Definition for Intel CPU */
typedef struct V1190_GH {  /* Global Header */
  unsigned geo:          5;    /* Geographical Address */
  unsigned event_count: 22;    /* Event Count */
  unsigned id:           5;    /* Identifier (=8) */
} V1190_GH_t, *V1190_GH_p;

typedef struct V1190_TH {  /* TDC Header */
  unsigned bunch_id:    12;    /* Bunch ID */
  unsigned event_id:    12;    /* Event ID */
  unsigned tdc:          2;    /* TDC */
  unsigned reserved1:    1;    /* not used */
  unsigned id:           5;    /* Identifier (=1) */
} V1190_TH_t, *V1190_TH_p;

typedef struct V1190_TM {  /* TDC Measurement */
  unsigned measurement: 19;    /* Measurement (TDC data) */
  unsigned channel:      7;    /* Channel */
  unsigned trailing:     1;    /* trailing(=1), leading (=0) edge */
  unsigned id:           5;    /* Identifier (=0) */
} V1190_TM_t, *V1190_TM_p;

typedef struct V1190_TT {  /* TDC Trailer */
  unsigned word_count:  12;    /* Word count */
  unsigned event_id:    12;    /* Event ID */
  unsigned tdc:          2;    /* TDC */
  unsigned reserved1:    1;    /* not used */
  unsigned id:           5;    /* Identifier (=3) */
} V1190_TT_t, *V1190_TT_p;

typedef struct V1190_TE {  /* TDC Error */
  unsigned error_flags: 15;    /* Error flag */
  unsigned reserved1:    9;    /* not used */
  unsigned tdc:          2;    /* TDC */
  unsigned reserved2:    1;    /* not used */
  unsigned id:           5;    /* Identifier (=4) */
} V1190_TE_t, *V1190_TE_p;

typedef struct V1190_ETTT {  /* Extended Trigger Time Tag */
  unsigned time_tag:    27;    /* Extended trigger time tag */
  unsigned id:           5;    /* Identifier (=17) */
} V1190_ETTT_t, *V1190_ETTT_p;

typedef struct V1190_GT {  /* Global trailer (the trailer) */
  unsigned geo:          5;    /* Geographical Address */
  unsigned word_count:  16;    /* Word count */
  unsigned reserved1:    3;    /* not used */
  unsigned status:       3;    /* Status */
  unsigned id:           5;    /* Identifier (=16) */
} V1190_GT_t, *V1190_GT_p;

typedef union V1190_DATA {  /* Output buffer data (union, 32 bit)*/
  unsigned int idata;           /* 32 bit integer data */
  V1190_GH_t   global_header;   /* global header */
  V1190_TH_t   tdc_header;      /* TDC header */
  V1190_TM_t   tdc_measurement; /* TDC measurement */
  V1190_TT_t   tdc_trailer;     /* TDC trailer */
  V1190_TE_t   tdc_error;       /* TDC error */
  V1190_ETTT_t time_tag;        /* Extended trigger time tag */
  V1190_GT_t   global_trailer;  /* Global Trailer */
} V1190_DATA_t, *V1190_DATA_p;


/* Control Register */
typedef struct V1190_CR_T {  /* Status Register */
  unsigned berr_en:                          1; /* Bus Error Enable */
  unsigned term:                             1; /* software termination */
  unsigned term_sw:                          1; /* termination mode */
  unsigned empty_event:                      1; /* allow empty event */
  unsigned align64:                          1; /* allows to add a 32 bit dummy word for alignment */
  unsigned compensation_enable:              1; /* compensation enable */
  unsigned test_fifo_enable:                 1; /* test FIFO enable */
  unsigned read_compensation_sram_enable:    1; /* read compensation SRAM enable */
  unsigned event_fifo_enable:                1; /* event FIFO enable */
  unsigned extended_trigger_time_tag_enable: 1; /* extended trigger time tag enable */
  unsigned reserved1:                        2; /* not used */
  unsigned addr_range_meb_access_enable:     1; /* 16MB address range MEB access enable */
  unsigned reserved2:                        3; /* not used */
} V1190_CR_t, *V1190_CR_p;

/* Status Register */
typedef struct V1190_SR_T {  /* Status Register */
  unsigned data_ready:    1;  /* data ready */
  unsigned alm_full:      1;  /* almost full */
  unsigned full:          1;  /* full */
  unsigned trig_match:    1;  /* trigger match mode */
  unsigned header_en:     1;  /* header enable */
  unsigned term_on:       1;  /* termination on */
  unsigned error0:        1;  /* error in TDC 0 */
  unsigned error1:        1;  /* error in TDC 1 */
  unsigned error2:        1;  /* error in TDC 2 */
  unsigned error3:        1;  /* error in TDC 3 */
  unsigned berr_flag:     1;  /* bus error */
  unsigned purg:          1;  /* purged */
  unsigned res_0:         1;  /* resolution 00=800ps, 01=200ps, 10=10ps*/
  unsigned res_1:         1;  /* (resolution) */
  unsigned pair:          1;  /* pair mode */
  unsigned trigger_lost:  1;  /* trigger lost */
} V1190_SR_t, *V1190_SR_p;


/* TDC header ID (5 bit)*/
#define V1190_HEADER_ID_GH     0x08
#define V1190_HEADER_ID_TH     0x01
#define V1190_HEADER_ID_TM     0x00
#define V1190_HEADER_ID_TT     0x03
#define V1190_HEADER_ID_TE     0x04
#define V1190_HEADER_ID_ETTT   0x11
#define V1190_HEADER_ID_GT     0x10
#define V1190_HEADER_ID_FILLER 0x18
#endif


//extern int v1190_wire_map_data[]; // For DAQ code version
// write a PERL script to get this table from /home/tamidaq/daq/v1190/v1190_util.c
static int v1190_wire_map_data[] = {
  0, 2, 1, 3, 4, 6, -2, -1, 5, 7, 8, 10, 9, 11, -1, -1,  // X1 plane
  1, 3, 2, 4, 5, 7,  0, -1, 6, 8, 9, 11,10, 12, -1, -1,  // U1 plane
  0, 2, 1, 3, 4, 6, -1, -1, 5, 7, 8, 10, 9, 11, -1, -1,  // X2 plane
  1, 3, 2, 4, 5, 7,  0, -1, 6, 8, 9, 11,10, 12, -1, -1   // U2 plane
};

#define v1190_wire_map(geo, ch) ((v1190_wire_map_data[(((geo)&7)<<3)|(((ch)&0x70)>>4)]<<4)|((ch)&0x0f))
#define v1190_plane_map(geo, ch) (((geo)&6)>>1)


/*
 * prototype definitions of library functions
 */
int v1190_open(void);
V1190_p v1190_map(int module_number);
int v1190_unmap(int module_number);
int v1190_close(void);
int v1190_micro_handshake_write(V1190_p ptr, int opecode);
int v1190_micro_handshake_write_1w(V1190_p ptr, int opecode, unsigned short w);
int v1190_micro_handshake_read(V1190_p ptr, int opecode, unsigned short *data);
int v1190_micro_handshake_read_cont(V1190_p ptr, unsigned short *data);


extern int v1190_n_tdc_error_string;
extern char *(v1190_tdc_error_string[]);

#endif  /* for ifndef V1190_REG_H */
