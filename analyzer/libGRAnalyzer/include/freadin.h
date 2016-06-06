/*
  freain.c ---- header for fread input
  Version 1.00  02-JUN-2000  by A. Tamii
*/


#define DEBUG_LEX         0
#define DEBUG_YACC        0
#define DEBUG_EVAL        0
#define DEBUG_CYCLIC      0
#define DEBUG_DR_SHOW     0
#define DEBUG_THINKPAD    0
#define DEBUG_DEFINITION  0
#define DEBUG_HBOOK       0
#define DEBUG_TEMP        1

#define ROOT_SETIDNUMBER  0  /* set an ID number in front of histogram names */
#if 0  /* defined in Makefile */
#define USE_PAW           0  /* Use paw as a histogrammer */
#define USE_ROOT          1  /* Use root as a histogrammer */
#endif

#if USE_ROOT
#define SHMSIZE     (16*1024*1024)  /* Shared Memory size = 16 MBytes */
#endif
#if USE_PAW
#define SHMSIZE     (1*1024*1024)   /* Shared Memory size = 1 MWords (*4 Bytes) */
#define PAWC_SIZE   (16*1024*1024)  /* 16 MWords (*4 Bytes) */
#endif

#define MAX_NCHILD       10  /* max number of child processes */

#define FNAM_CHILD_STDERR   ".child%.2d.stderr"   /* child process stderr output file */

#define NSYMS  200

#define KEYFILE        "./key"

#define SHMNAME        "BOB"
#define HBKNAME        "analyzer.hb"
#define FNAMALIAS      "alias.kumac"    /* alias output file (for paw) */
#define DEFFNAM        "hist.def"       /* default definition file name */

#define ROOT_ALIASDIR  "./aliases"      /* directory for alias macros (for ROOT) */
#define ROOT_SHMNAM    "analyzer.map"   /* directory for alias macros (for ROOT) */

#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#ifdef LITTLE_ENDIAN
#undef LITTLE_ENDIAN
#endif
enum {
	BIG_ENDIAN = 0,
	LITTLE_ENDIAN
};

enum {
  FORMAT_UNDEFINED = -1,    /* undefined (not determined yet) */
  FORMAT_UNKNOWN  = 0,      /* unknown format */
	FORMAT_TAMIDAQ,           /* RCNP tamidaq format */
	FORMAT_MARS,              /* RIKEN mars format */
	FORMAT_DST_1,             /* DST format ver 1.0 */
	FORMAT_BLD1,              /* RCNP Build variable length format version 1 */
};

#if defined (__cplusplus)
extern "C" {
#endif
struct symtab{
	char    *name;
	double  (*funcptr)();
	double  value;
} symtab[NSYMS];

/* value record */
typedef struct vrec{
	int     vtype;        /* value type */
	double  dval;         /* double floating value */
	char    *sval;        /* string value */
	int     vref;         /* dr_rec reference number */
	int     eval;         /* evaluation type (enum eval) */
	int     neval;        /* evaluation level */
	struct vrec *arg1;    /* argument 1 */
	struct vrec *arg2;    /* argument 2 */
} vrec_t, *vrec_p;

enum eval {
  EVAL_NONE=0,       /* not evaluated */
	EVAL_UNDER,        /* under evaluation */
	EVAL_CONST         /* constant (no need to evaluate) */
};

typedef struct fin {
  int  type;     /* type of the input */
	FILE *file;    /* input stream */
  int  istr;     /* index of the string input */
	char *pstr;    /* pointer to the input string */
} fin_t;

typedef struct child_data {
  int    pid;
	int    readfd;
	int    writefd;
	int    n_sent_buf;
} child_data_t, *child_data_p;

typedef struct hist_data { /* histogram data between parent and child processes */
		int   hid;
		float f1;
		float f2;
} hist_data_t, *hist_data_p;

#define HID_SCALER      50000
#define HID_SCALER_MAX  51000

enum {
  FIN_NONE = 0,  /* no input */
	FIN_FILE,      /* input from a file pointed by the fin */
	FIN_STR        /* input is a string pointed by pstr and index by istr */
};

# define MaxMsgLen 1020
typedef struct msg_t{
	int  mtype;
	char mtext[MaxMsgLen];
} mesg_t;

/* message commands */
enum {
	MSG_NONE = 0,
	MSG_CMD        /* command to the analyzer. Should be 1 in order to
                    avoid confliction with child process IDs */
};

extern vrec_p new_vrecp();
extern fin_t fin;

extern int init_hist_rcnp();
extern int init_hist_gr_fpp();
extern int init_hist_las_fpp();
extern int hb_init();
extern int hb_exit();
extern int hb_hist_out(char*);
extern void show_blk_num(int);
extern void message();
extern int event_init();
extern int event_exit();
extern int event();
extern int evt_print_scaler(FILE*);
extern int evt_time();
extern int evt_start();
extern int evt_scaler();
extern int evt_init_rcnp();
extern int evt_ipr();
extern int evt_init_gr();
extern int evt_gr_adc();
extern int evt_gr_tdc();
extern int evt_chamb_gr();
extern int evt_gr_kinema();
extern int evt_init_las();
extern int evt_las_adc();
extern int evt_las_tdc();
extern int evt_chamb_las();
extern int evt_las_kinema();
extern int evt_gl_kinema();
extern int evt_init_las_fpp();
extern int evt_chamb_las_fpp();
extern int evt_init_gr_fpp();
extern int evt_gr_fpp_adc();
extern int evt_gr_fpp_tdc();
extern int evt_chamb_gr_fpp();
extern int evt_start_rcnp();
extern int evt_start_las_fpp();
extern int evt_start_gr_fpp();
extern void show_debug();
extern void add_scaler(int,int);
extern void open_alias_file();

extern void write_child_data(char*,int);
extern void flush_child_data();
extern int read_in(int fd, unsigned char *buf, int *pos, int size);

extern void showerr(char*, ...);

extern int hbk1(int id, char *name, char *title, double min, double max, int nbin);
extern int hbk2(int id, char *name, char *title, double min1, double max1, int nbin1, double min2, double max2, int nbin2);
extern int hstdel(int hid);
extern int hfil1(int id, double d);
extern int hfil2(int hid, double f1, double f2);

extern int dst_init();
extern int dst_exit();
extern int dst_chk_format(unsigned char *buf);
extern int dst_write_data();
extern int dst_read_data(unsigned char *buf, int *bpos, int size);
extern int root_init(int nrun);
extern int root_exit();
extern int root_write_data();
extern int read_blk_rcnp(unsigned short *buf, int size);
extern int read_blk_mars(unsigned short *buf, int size);
extern void swaps(unsigned int *data);
extern void save_scalers();
extern void save_histograms();
extern int evt_blk(int cblk);

extern int histogram();
extern int check_level(double d);
extern char* str_token(int vtype);


#if defined (__cplusplus)
}
#endif

/*
Local Variables:
mode: C
tab-width: 2
End:
*/
