#ifndef MADC32_REG_H
#define MADC32_REG_H

/*
 * Definitions for MADC32
 *
 * Version 0.10  08-MAR-2016  by A. Tamii
 */


/* Size of I/O space */

#define MADC32_MAX_N_MODULES     32          /* Max number of MADC32 Modules */
#define MADC32_BASE_INI   0x32000000         /* VME base address start      */
#define MADC32_BASE_INC   0x00010000         /* VME base address increment  */
#define MADC32_BASE(module_number)  ((MADC32_BASE_INI)+(MADC32_BASE_INC)*(module_number))

#define MADC32_SIZE   0x00010000             /* Memory Mapping Size */

/* VME addressing mode */
#define MADC32_MODE  VME_A32UD       /* 32bit Extended Non-Prvileged Data Accesss  */

#define MADC32_NUM_CHANNELS 32              /* number of channels */


#define MADC_IRQ_THRESHOLD_DEFAULT 6000     /* IRQ Threshold max=8120  */


#define MADC32_MARKING_TYPE_EVENT_COUNTER        0
#define MADC32_MARKING_TYPE_TIME_STAMP           1
#define MADC32_MARKING_TYPE_EXTENDED_TIME_STAMP  3

#define MADC32_DATA_LEN_FORMAT_8BIT              0
#define MADC32_DATA_LEN_FORMAT_16BIT             1
#define MADC32_DATA_LEN_FORMAT_32BIT             2
#define MADC32_DATA_LEN_FORMAT_64BIT             3

#define MADC32_BANK_OPERATION_BANKS_CONNECTED    0
#define MADC32_BANK_OPERATION_BANKS_INDEPENDENT  1
#define MADC32_BANK_OPERATION_TOGGLE_MODE        3

#define MADC32_ADC_RESOLUTION_2K_800NS           0   /*  0.8 micro sec conversion time*/
#define MADC32_ADC_RESOLUTION_4K_1600NS          1   /*  1.6 micro sec */
#define MADC32_ADC_RESOLUTION_4K_HIGH_RES        2   /*  3.2 micro sec */
#define MADC32_ADC_RESOLUTION_8K                 3   /*  6.4 micro sec */
#define MADC32_ADC_RESOLUTION_8K_HIGH_RES        4   /* 12.5 micro sec */

#define MADC32_DELAY0_25NS                       0   /* 25 ns   */
#define MADC32_DELAY1_25NS                       0   /* 25 ns   */
#define MADC32_WIDTH0_2US                       40   /*  2 us   */
#define MADC32_WIDTH1_2US                       40   /*  2 us   */
#define MADC32_USE_GG_GG0                        1   /* use GG0 */

#define MADC32_INPUT_RANGE_4V                    0
#define MADC32_INPUT_RANGE_10V                   1
#define MADC32_INPUT_RANGE_8V                    2

#define MADC32_NIM_GAT1_OSC_GG1                  0  /* gate 1 input     */
#define MADC32_NIM_GAT1_OSC_TIME                 1  /* oscillator input */

#define MADC32_NIM_FC_RESET_FAST_CLEAR           0  /* fast clear input */
#define MADC32_NIM_FC_RESET_TIME_STAMP           1  /* reset time stamp */

#define MADC32_NIM_BUSY_BUSY                     0  /* as busy */
#define MADC32_NIM_BUSY_GG0                      1  /* as gate0 output */
#define MADC32_NIM_BUSY_GG1                      2  /* as gate1 output */
#define MADC32_NIM_BUSY_CBUS                     3  /* as Cbus output  */
#define MADC32_NIM_BUSY_BUFFER_FULL              4  /* buffer full     */
#define MADC32_NIM_BUSY_DATA_THRESHOLD           8  /* data in buffer above thereshold */


/*
 * MADC32 Memory Register
 */

typedef struct MADC32 {
  /* Data FIFO */
  unsigned int fifo_read;        /* read FIFO */
  unsigned char  res00[0x3FFC];

  /* Threshold memory */
  unsigned short threshold[32];  /* threshold */
  unsigned char  res01[0x1FC0];

  /* Registers */
  unsigned short address_source;
  unsigned short address_reg;
  unsigned short module_id;
  unsigned short res02;
  unsigned short soft_reset;
  unsigned short res03[2];
  unsigned short firmware_revision;

  /* IRQ(ROACK) */
  unsigned short irq_level;
  unsigned short irq_vector;
  unsigned short irq_test;
  unsigned short irq_reset;
  unsigned short irq_threshold;
  unsigned short max_transfer_data;
  unsigned short withdraw_irq;
  unsigned char  res04[0x0002];

  /* MCST CBLT */
  unsigned short cblt_mcst_control;
  unsigned short cblt_address;
  unsigned short mcst_address;
  unsigned char  res05[0x000A];

  /* FIFO handling */
  unsigned short buffer_data_length;
  unsigned short data_len_format;
  unsigned short readout_reset;
  unsigned short multi_event;
  unsigned short marking_type;
  unsigned short start_acq;
  unsigned short fifo_reset;
  unsigned short data_ready;

  /* operation mode */
  unsigned short bank_operation;
  unsigned short adc_resolution;
  unsigned short output_format;
  unsigned short override;
  unsigned short slc_off;
  unsigned short skip_oorange;
  unsigned short ignore_threshold;
  unsigned char  res06[0x0002];

  /* gate generator */
  unsigned short hold_delay0;
  unsigned short hold_delay1;
  unsigned short hold_width0;
  unsigned short hold_width1;
  unsigned short use_gg;
  unsigned char  res07[0x0006];

  /* Inputs, outputs */
  unsigned short input_range;
  unsigned short ecl_term;
  unsigned short ecl_gate1_osc;
  unsigned short ecl_fc_res;
  unsigned short ecl_busy;
  unsigned short nim_gate1_osc;
  unsigned short nim_fc_reset;
  unsigned short nim_busy;

  /* Test pulser */
  unsigned short pulser_status;
  unsigned char  res08[0x000E];

  /* Mesytec control bus */
  unsigned short rc_busno;
  unsigned short rc_modnum;
  unsigned short rc_opcode;
  unsigned short rc_adr;
  unsigned short rc_dat;
  unsigned short send_return_status;
  unsigned char  res09[0x0004];

  /* CTRA */
  unsigned short reset_ctr_ab;
  unsigned short evctr_lo;
  unsigned short evctr_hi;
  unsigned short ts_sources;
  unsigned short ts_divisor;
  unsigned char  res10[0x0002];
  unsigned short ts_counter_lo;
  unsigned short ts_counter_hi;

  /* CTRB */
  unsigned short adc_busy_time_lo;
  unsigned short adc_busy_time_hi;
  unsigned short gate1_time_lo;
  unsigned short gate1_time_hi;
  unsigned short time_0;
  unsigned short time_1;
  unsigned short time_2;
  unsigned short stop_ctr;
} MADC32_t, *MADC32_p;

#define MADC32_MULTI_EVENT_NO             0 
#define MADC32_MULTI_EVENT_YES_UNLIMITED  1 
#define MADC32_MULTI_EVENT_YES_LIMITED    3


/*
 *  TDC data format @Output Buffer
 */

#if 1  /* Little Endian Definition for Intel CPU */

/* MADC32 Data */
#define MADC32_HEADER_SIGNATURE_DATA         0
#define MADC32_HEADER_SIGNATURE_HEADER       1
#define MADC32_HEADER_SIGNATURE_END_OF_EVENT 3
#define MADC32_SUBHEADER_EXTENDED_TIME_STAMP 0x0024
#define MADC32_SUBHEADER_EVENT               0x0020
#define MADC32_SUBHEADER_FILL                0x0000


typedef struct MADC32_HEADER_SIGNATURE {  /* Header Signature bits*/
  unsigned res:             21;    /* reserved */
  unsigned subheader:        9;    /* subheader */
  unsigned header:           2;   /* Header Signature */
} MADC32_HEADER_SIGNATURE_t, *MADC32_HEADER_SIGNATURE_p;

typedef struct MADC32_DATA_HEADER {  /* Header Word*/
  unsigned n_data_words:   12;    /* number of data words */
  unsigned adc_resolution:  3;    /* ADC resolution */
  unsigned output_format:   1;    /* Output Format */
  unsigned module_id:       8;    /* Module ID */
  unsigned subheader:       6;    /* Subheader=b00000 */
  unsigned header_signature: 2;   /* Header Signature = b01 */
} MADC32_DATA_HEADER_t, *MADC32_DATA_HEADER_p;

typedef struct MADC32_DATA_EVENT {  /* Data Event Word*/
  unsigned adc_amplitude:   14;   /* ADC amplitude */
  unsigned out_of_range:     1;   /* Out of range */
  unsigned res0:             1;   /* Reserved */
  unsigned channel_number:   5;   /* Channel Number */
  unsigned subheader:        9;   /* Subheader=b000100000 */
  unsigned header_signature: 2;   /* Header Signature = b00 */
} MADC32_DATA_EVENT_t, *MADC32_DATA_EVENT_p;

typedef struct MADC32_EXTENDED_TIME_STAMP {  /* Extended Time Stamp */
  unsigned time_stamp_msb:  16;   /* Most Significant 16bit Time Stamp*/
  unsigned res0:             5;   /* Reserved */
  unsigned subheader:        9;   /* Subheader=b000100100 */
  unsigned header_signature: 2;   /* Header Signature = b00 */
} MADC32_EXTENDED_TIME_STAMP_t, *MADC32_EXTENDED_TIME_STAMP_p;

typedef struct MADC32_FILL {  /* FILL DUMMY (PADDING) */
  unsigned res0:            21;   /* Reserved =0 */
  unsigned subheader:        9;   /* Subheader=b000000000 */
  unsigned header_signature: 2;   /* Header Signature = b00 */
} MADC32_FILL_t, *MADC32_FILL_p;

typedef struct MADC32_END_OF_EVENT {  /* End of Event */
  unsigned event_counter:   30;   /* Event Counter or Time Stamp */
  unsigned header_signature: 2;   /* Header Signature = b11 */
} MADC32_END_OF_EVENT_t, *MADC32_END_OF_EVENT_p;

#endif

extern char *max_number_hits_s[];

/*
 * prototype definitions of library functions
 */
int madc32_open(void);
MADC32_p madc32_map(int module_number);
int madc32_unmap(int module_number);
int madc32_close(void);


#endif  /* for ifndef MADC32_REG_H */
