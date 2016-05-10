/*    Eurosupernova Online analysis routine
   *************************************
   
   Version     : 2.10
   Last Update : Oct 27, 1999 (VH)

   Written by

   Jan Heyse (JH)
   Universiteit Gent
   Vakgroep Subatomaire en Stralingsfysica
   Proeftuinstraat 86
   9000 Gent
   Belgium

   +32(0)9 264.65.35
   jan.heyse@rug.ac.be

   and

   Volker Hannen (VH)
   KVI Groningen
   Zernikelaan 25 
   9747 AA Groningen
   The Netherlands
   
   +31(0)50 363.36.32
   hannen@kvi.nl

*/
   
#define  VERSION "2.10"
#define  UPDATE  "Oct 27 1999"

#include <math.h>
#include <cfortran/cfortran.h>
#include <cfortran/packlib.h>

#if defined(GRAPHS)
#include <g2.h>
#include <g2_X11.h>
#endif

#include "UI.h"
#include "EventTypes.h"
#include "utils.h"
#include "infobase.h"

#include "pcos.h"
#include "tdc3377.h"
#include "myutils.h"
#include "adc4300b.h"

#include "Online_ana.h"

/* VESNA VDC reconstruction routine */
extern float VESNA(int, int, int, int, int, Event_t*);
extern void  VESNA_set_T0(int T0_X1, int T0_U1, int T0_X2, int T0_U2);

/*******************************************************
 * definitions for HBOOK routines and PAW shared memory
 *******************************************************/
#define PAWC_SIZE 500000
typedef struct { float PAW[PAWC_SIZE]; } PAWC_DEF;
#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;

/* LRECL is the bufferlength used in the HROPEN function */
#define LRECL 1024

/* Now comes an extremly strange hack to get HCDIR of the Linux
   cernlib running the way it is supposed to be, V. Hannen */
#if defined(Linux86)
void myHCDIR(char* CHPATH, char* CHOPT)
{
  char dummy[50];
  strcpy(dummy, CHPATH);
  HCDIR (dummy,CHOPT);
}
#  ifdef HCDIR
#    undef HCDIR
#  endif
#define HCDIR myHCDIR
#endif

/***********************************
 * Prototypes of internal routines 
 ***********************************/
static void Redef_histo (Boolean * Running);
static void Set_debug (Boolean * Running);
static void TOF_gate(Boolean * Running);
static void Variance_gate(Boolean * Running);
static void ClusterSize(Boolean * Running);
static void Theta_gate(Boolean * Running);
static void cos_theta_carb_gate(Boolean * Running);
static void X1_gate(Boolean * Running);
static void vertz_gate(Boolean * Running);
static void define_offsets(Boolean * Running);
static void Change_zero(Boolean * Running);
static void pol_gate (Boolean * Running);
static void Save_oas (void);
static void Load_oas (void);
static void QDC (Boolean * Running);

static void BBS_gate(Boolean * Running);
static void Scat_gate_1(Boolean * Running);
static void Scat_gate_2(Boolean * Running);

static void compose_ntuple (Boolean * Running);

int  Report (uint16 * Buffer, int Size);
int  define_histograms(void);
static void reset_histograms(void);
void open_ntuple(char * nt_name);

int load_analysis_setup (char * filename);
static int save_analysis_setup (char * filename);

int  analysis_getpar(Ana_par_t *);
int  vdc_getpar();
int  mwpc_getpar();
static int  histo_getpar(Histopar_t *);

static int Unpack_event(uint16 * Buffer, Event_t * Event);
int Interpret_event (Event_t * Pevent);
static int Interpret_scaler(uint16* Buffer);

static void calc_fpds(float WX1, float WU1, float WX2, float WU2,
		      float * X1_xb, float * X1_yb,
		      float * Theta, float * Phi);

static int calc_vfp(float X1, float Theta, float * Xf);

static double calc_ana(double kinetic_energy, double sin_vivo);

static void calc_fpp(float X1, float Y1, float Theta, float Phi,
		     float D1x_raw, float D1y_raw, float D2x_raw, 
		     float D2y_raw, float D3x_raw, float D3y_raw,
		     float D4x_raw, float D4y_raw, int polarization);

static void calc_target(float Xfocal, float Yfocal, float Tfocal,
		       float * pTtarget, float * pPtarget, float * pmomentum);

static float Straight_line (Event_t * Pevent, int Wic, 
			    int Miss, int First, int Index,
			    float *PVariance, int *PCentroid);

static void second_test(int D1x, int D1y, int D2x, int D2y, int D4x, int D4y);





/********************************************
 * definitions for the VDC straight line fit 
 ********************************************/
#define NOF_PASSES 3 /* number of passes, i. e. max number of datapoints
			kicked out of the fit */
#define REPAIR_VAR 3 /* variance beneath which the fit is considered good
			and no further repair takes place */


/***********************************************************
 * Variables and Parameters global to the analysis software
 ***********************************************************/

/* parameters needed to calculate the analyzing power */
static double a0 =      5.4771;
static double a1 =     -4.2906;
static double a2 =    -25.3790;
static double a3 =    121.1500;
static double a4 =   -108.3700;
static double b0 =    -10.4750;
static double b1 =    -40.1700;
static double b2 =    525.8400;
static double b3 =   -899.2900;
static double b4 =   1616.6000;
static double c0 =   1052.3000;
static double c1 =    628.5100;
static double c2 = -13215.0000;
static double c3 =  19083.0000;
static double c4 =  -5485.5000;

static double proton_restmass = 0.93828;

char Mem_id[6];

static Boolean Ntuple               = FALSEB;

static Boolean Report_Buferr        = FALSEB;
static Boolean File_FPP             = FALSEB;

static Boolean Correct_missing      = FALSEB;
static Boolean Correct_ringing      = FALSEB;
static Boolean Correct_multiple     = FALSEB;
static Boolean Correct_unchecked    = FALSEB;

static Boolean Correct_D4           = FALSEB;
static Boolean QDC_in               = TRUEB;
static Boolean Select_pol[4]        = {TRUEB,TRUEB,TRUEB,TRUEB};
static Boolean Select_type[20]      = {TRUEB,TRUEB,TRUEB,TRUEB,TRUEB,
				       TRUEB,TRUEB,TRUEB,TRUEB,TRUEB,
				       TRUEB,TRUEB,TRUEB,TRUEB,TRUEB,
				       TRUEB,TRUEB,TRUEB,TRUEB,TRUEB};
static Boolean Gate_TOF             = FALSEB;
static Boolean Gate_variance        = FALSEB;
static Boolean Gate_X1              = FALSEB;
static Boolean Gate_Theta           = FALSEB;
static Boolean Correct_DSP          = FALSEB;
static Boolean Gate_cos_theta_carb  = FALSEB;
static Boolean Gate_vertz           = FALSEB;
static Boolean Gate_target          = FALSEB;



static Boolean Gate_BBS             = FALSEB;
static Boolean Gate_scat_1          = FALSEB;
static Boolean Gate_scat_2          = FALSEB;

static Boolean Ntup_Block[4]        = {TRUEB,TRUEB,TRUEB,TRUEB};

static MWPar_t MWPC_Par[2][512][2];
static int     VDC_Par[4];
static int     MWPC_mid[8];

static Boolean Use_VESNA            = FALSEB;
static int     VDC_T0[4];

/* virtual focal plane distance and tilt angle */
static float vfp_L = 0;
static float vfp_psi = 0;

static char nt_name[256]="fpp_ntuple.hbook";
static FPDS_ntuple_t  FPDS_ntup;
static FPP_ntuple_t   FPP_ntup;
static PID_ntuple_t   PID_ntup;
static TAR_ntuple_t   TAR_ntup;

static FILE * statfile;

static FILE * FPP_file=NULL;

static Ana_par_t Ana_par;
static Boolean NewRun = TRUEB;
/* static Boolean Data_valid = FALSEB; */

static uint32 Counter = 0;

static int   TOF_limit[2];
static float Variance_limit[2][4];
static int   MaxClusterSize;
static float X1_limit[2];
static float Theta_limit[2];
static float cos_theta_carb_limit[2];
static float vertz_limit[2];
static float Theta_target_limit[2];
static float Phi_target_limit[2];


/* Set gates on two different scattering angles within the
acceptance of the BBS */
static float BBS_angle;
static float Scat_limit_1[2];
static float Scat_limit_2[2];
static float Scat_angle; /* real scattering angle in the lab */

/* definition of all the offset variables */
static float VDC0x,VDC0y,VDC0z;
static float CAz_offset;
static float D2x_offset,D2y_offset,D2z_offset;
static float D3x_offset,D3y_offset,D3z_offset;
static float D4x_offset,D4y_offset,D4z_offset;
static float Phi_offset,cr_angle_offset;

/* some global variables to calulate polarization stuff */
static float estim_cos[250];
static float estim_sin[250];
static float estim_den[250];

/* all relevant information is passed from the Online_ana routine */
/* via the Pevent pointer; */
/* see Online_ana.h for the definition of Event_type */
/* updated on April 18, 1999   J. Heyse */


int Interpret_event (Event_t * Pevent)
{

  /* definition of variables */
  /* ----------------------- */

  static  int scale_counter = 0; /* used when downscaling part of VDC-1-X */
  /* static  int missing_counter = 0;
     static  int multiple_counter = 0; */

  int     i, j, k, m, n, PlaneID;

  int     Polarization_info;

  int     TOT[20], T1, T2, TOF, RF;
  int     Sum_TOT[2], NOF_PMs[2];

  int     QDC[10], Sum_QDC;

  int     Gap, Previous_wire, First_wire;
  int     Missing_in_cluster, Wires_in_cluster;
  int     NOF_Clusters[4], Clustersize[10][4];
  int     NOF_Missing[4], Missing_wires[10][4];
  int     NOF_VDC_Pos[4], Centroid[10][4];
  float   Variance[10][4], VDC_Position[10][4];

  int     NOF_MWPC_Pos[8], MWPC_Position[50][8], MWPC_Width[50][8]; 

  Boolean FPDS_OK;
  float   X1_x, X1_y, Theta, Phi, Xf;
  float   Theta_target, Phi_target, momentum;

  double  VDCx, VDCy, VDCz;
  double  VDCx_2, VDCy_2, VDCz_2;
  double  cr_angle;
  double  D1_extra[2];

  Boolean FPP_OK;
  int     D1_x, D1_y, D2_x, D2_y, D3_x, D3_y, D4_x, D4_y;

  int     TOT_Paddle, QDC_Paddle;

  int     centr, width;
  double  wirenum;

  Boolean gate1, gate2, gate3, gate4, gate5, gate6, gate7, gate8, gate9;


  /* Initialization of variables */
  /* --------------------------- */

  TOF = -999;
  RF  = -999;
  for(i=0;i<20;i++) TOT[i] = -999;
  Sum_TOT[0] = Sum_TOT[1] = -999;
  NOF_PMs[0] = NOF_PMs[1] = 0;

  for(i=0;i<10;i++) QDC[i] = 0;
  Sum_QDC = 0;

  First_wire = 0;
  PlaneID = 0;
  for (i=0;i<4;i++)
    {
      NOF_Missing[i]   = 0;
      NOF_VDC_Pos[i]   = 0;
      NOF_Clusters[i]  = 0;
      for (j=0;j<10;j++)
	{	
	  Clustersize[j][i]   = 0;
	  VDC_Position[j][i]  = 0.;
	  Variance[j][i]      = 0.;
	  Centroid[j][i]      = 0.;
	  Missing_wires[j][i] = -1;
	}
    }

  for(i=0;i<8;i++)
    {
      NOF_MWPC_Pos[i] = 0;
      for(j=0;j<50;j++)
	{
	  MWPC_Position[j][i] = 0;
	  MWPC_Width[j][i]    = 0;
	}
    }

  FPDS_OK = FALSEB;
  X1_x = X1_y = Theta = Phi = -999.;
  Theta_target = Phi_target = momentum = -999.;

  FPP_OK = FALSEB;
  D1_x = D1_y = D2_x = D2_y = D3_x = D3_y = D4_x = D4_y = -999;


  if (Ntuple)
    {
      if (Ntup_Block[0])
	{
	 /*	FPDS_ntup.x1_int      = -999.;
	  	FPDS_ntup.u1_int      = -999.;
	  	FPDS_ntup.x2_int      = -999.;
	  	FPDS_ntup.u2_int      = -999.;
	  
	  	FPDS_ntup.x1_var      = -999.;
	  	FPDS_ntup.u1_var      = -999.;
	  	FPDS_ntup.x2_var      = -999.;
	  	FPDS_ntup.u2_var      = -999.; */
	  
	  FPDS_ntup.x1          = -999.;
	  FPDS_ntup.y1          = -999.;
	  FPDS_ntup.theta       = -999.;
	  FPDS_ntup.phi         = -999.;
	}

      if (Ntup_Block[1])
	{
	  FPP_ntup.d1x          = -999;
	  FPP_ntup.d1y          = -999;
	  FPP_ntup.d2x          = -999;
	  FPP_ntup.d2y          = -999;
	  FPP_ntup.d3x          = -999;
	  FPP_ntup.d3y          = -999;
	  FPP_ntup.d4x          = -999;
	  FPP_ntup.d4y          = -999;
	  
	  FPP_ntup.theta_FPP    = -999.;
	  FPP_ntup.phi_FPP      = -999.;
	 /*	FPP_ntup.theta_DSP    = -999.;
	  	FPP_ntup.phi_DSP      = -999.; */
	  
	  FPP_ntup.x_FPP        = -999.;
	  FPP_ntup.y_FPP        = -999.;
	  /* FPP_ntup.x_DSP        = -999.;
	     FPP_ntup.y_DSP        = -999.; */
	}

      if (Ntup_Block[2])
	{
	  PID_ntup.tof          = -999.;
	  PID_ntup.beam_pol     = -999.;
	  
	  PID_ntup.sum_tot_s1   = -999.; 
	  PID_ntup.sum_tot_s2   = -999.;
	}

      if (Ntup_Block[3])
	{
	  TAR_ntup.theta_target = -999.;
	  TAR_ntup.phi_target   = -999.;
	  TAR_ntup.momentum     = -999.;
	}
    }

  /* calculation of variables */
  /* ------------------------ */

  /* rf of cyclotron */
	
  RF = Pevent->S_time[30][0];
	
  /* polarization */

  Polarization_info = Pevent->Beam_pol;

  /* scintillator variables */

  /* TOF */

  T1 = Pevent->S_time[20][0];
  T2 = Pevent->S_time[21][0];

  if ((T1!=-1)&&(T2!=-1)) 
    TOF = T1 - T2;

  /* TOT */

  for(i=0;i<2;i++)
    {
      for(j=0;j<10;j++)
	{
	  if ((Pevent->STDCM[10*i+j][0]>1)||(Pevent->STDCM[10*i+j][1]>1)||
	      (Pevent->S_time[10*i+j][0]==-1)||(Pevent->S_time[10*i+j][1]==-1))
	    continue;

	  TOT[10*i+j] = Pevent->S_time[10*i+j][0]
	    - Pevent->S_time[10*i+j][1];
	  
	  if (NOF_PMs[i]==0) Sum_TOT[i] = 0;

	  Sum_TOT[i] += TOT[10*i+j];
	   
          NOF_PMs[i]++;
	}
    }

  /* QDC variables */

  if (QDC_in)
    {
      for (i=0;i<10;i++)
	{
	  QDC[i]   = Pevent->QDC_channel[i];
	  Sum_QDC += Pevent->QDC_channel[i];
	}
    }


  /* VDC variables */

  if (Use_VESNA)
    {
      VESNA(0,0,0,0,2,Pevent);
    }
  else
    {
      for(i=0;i<4;i++)  
	{
	  if (Pevent->VDC_hits[i][0] > 0)
	    {
	      NOF_Clusters[i]  = 1;
	      Previous_wire = -1;
	      Missing_in_cluster = 0;
	      Wires_in_cluster = 0;
	      
	      for(k=0;k<WIRES_VDC;k++)
		{
		  if (Pevent->VTDCM[k][i][0] != 1) 
		    continue;
		  
		  if (Previous_wire == -1) 
		    {
		      Previous_wire = k - 1;
		      First_wire = k;
		    }
		  
		  Gap = k - Previous_wire;
		  
		  switch(Gap)
		    {
		    case 1:  /* normal cluster */
		      Wires_in_cluster++;
		      break;
		    case 2:  /* missing wire */
		      if (NOF_Missing[i]<10)
			Missing_wires[NOF_Missing[i]][i]=(k-1);
		      NOF_Missing[i]++;
		      Missing_in_cluster++;
		      Wires_in_cluster++;
		      break;
		    case 3:  /* three missing wires */
		      if (NOF_Missing[i]<10)
			Missing_wires[NOF_Missing[i]][i]=(k-1);
		      NOF_Missing[i]++;
		      Missing_in_cluster++;
		      Wires_in_cluster++;
		      break;
		    default: /* new cluster */
		      if ((Wires_in_cluster > 2) && (NOF_VDC_Pos[i] <10)
			  &&((Wires_in_cluster+Missing_in_cluster) 
			     <= MaxClusterSize)
			  && (TOF != -999) )
			{
			  VDC_Position[NOF_VDC_Pos[i]][i] = 
			    Straight_line(Pevent, Wires_in_cluster, 
					  Missing_in_cluster, First_wire, i,
					  &Variance[NOF_VDC_Pos[i]][i],
					  &Centroid[NOF_VDC_Pos[i]][i]);
			  
			  /* here the variance gate comes in to play */
			  
			  gate3 = TRUEB;
			  
			  if ((Gate_variance)&&
			      ((Variance[NOF_VDC_Pos[i]][i] 
				< Variance_limit[0][i])||
			       (Variance[NOF_VDC_Pos[i]][i] 
				> Variance_limit[1][i])))
			    gate3 = FALSEB;
			  
			  if (gate3)
			    NOF_VDC_Pos[i]++;
			}
		      
		      Clustersize[NOF_Clusters[i]-1][i]=
			Wires_in_cluster+Missing_in_cluster;
		      Wires_in_cluster=1;
		      Missing_in_cluster = 0;
		      First_wire=k;
		      NOF_Clusters[i]++;
		    }
		  
		  Previous_wire = k;
		}
	      
	      /* do necessary calculations for last cluster */
	      if ( (Wires_in_cluster > 2) && (NOF_VDC_Pos[i] <10)
		   && ((Wires_in_cluster+Missing_in_cluster) <= MaxClusterSize)
		   && (TOF != -999) )
		{
		  VDC_Position[NOF_VDC_Pos[i]][i] = 
		    Straight_line(Pevent, Wires_in_cluster, 
				  Missing_in_cluster, First_wire, i,
				  &Variance[NOF_VDC_Pos[i]][i],
				  &Centroid[NOF_VDC_Pos[i]][i]);
		  
		  /* here the variance gate comes in to play */
		  
		  gate3 = TRUEB;
		  
		  if ((Gate_variance)&&
		      ((Variance[NOF_VDC_Pos[i]][i]<Variance_limit[0][i])||
		       (Variance[NOF_VDC_Pos[i]][i]>Variance_limit[1][i])))
		    gate3 = FALSEB;
		  
		  if (gate3)
		    NOF_VDC_Pos[i]++;
		}	  
	      Clustersize[NOF_Clusters[i]-1][i]=
		Wires_in_cluster+Missing_in_cluster;
	    }
	}
    }
  

  /* MWPC variables */
  
  for (i=0;i<8;i++)
    {
      n = 0;

      NOF_MWPC_Pos[i] = Pevent->MWPC_hits[i];
      for(j=0;(j<50)&&(j<NOF_MWPC_Pos[i]);j++)
	{
	  MWPC_Position[j-n][i] = Pevent->MWPC_position[j][i];
	  MWPC_Width[j-n][i]    = Pevent->MWPC_width[j][i];
	  

	  if (Correct_ringing)
	    {
	      /* take care of ringing wires */
	      
	      for (k=0;k<Ana_par.MWPC_no_ring[i];k++)
		if ((NOF_MWPC_Pos[i]>1)&&
		    (fabs((MWPC_Position[j-n][i]/2.+1)-
			  Ana_par.MWPC_ringing[i][k])<1))
		  n++;   
	    }
	}
      NOF_MWPC_Pos[i] -=n;
    }
  
  
  
  /* ************************************************************* */

  /* So now we have :
     
     Polarization_info : 
         value provided by source, telling whether polization is 
	 up, down or undefined

     TOF :
         time of flight; defaults to -999 if undefined

     TOT :
         time over threshold for all photomultipliers (0-19),
	 starting in S1, with the top one on the high momentum side,
	 going down to the bottom one, ... up to the bottom PM of 
	 S2 on the low momentum side; default to -999

     NOF_PMs :
         number of PM's read out for one scintillator plane (0-1)

     Sum_TOT :
         sum of all TOT values for one plane (0-1); defaults to -999
     
     QDC :
         QDC-values (0-9); index corresponds to PM number in S1

     Sum_QDC :
         sum of all QDC values

     NOF_Clusters :
         number of clusters for each VDC-plane (0-3, i.e. VDC1x, 
	 VDC1u, ...)
	 
     Clustersize :
         size of every cluster (maximum 10 clusters per plane)

     NOF_Missing :
         number of missing wires for each VDC-plane

     Missing_wires :
         adresses of missing wires (maximum 10 per plane)

     NOF_VDC_Pos :
         number of positions in VDC which could be calculated 
	 with a straight line fit (maximum 10 per plane)

     VDC_Position :
         calculated position (in wire number units, maximum 10 per plane)

     Centroid :
         centroid wire (maximum 10 per plane)

     Variance :
         variance of the fit for every calculated position

     NOF_MWPC_Pos :
         number of hits in each MWPC plane

     MWPC_Position :
         position in MWPC plane (half wire number units, maximum 50 per plane)

     MWPC_Width :
         number of read-out wires for each hit (maximum 50 per plane)
     


     Now we can play ball !!!! */

  /* ************************************************************* */




  /* DSP checks ... (JH) */
  /* "trick" to recover unchecked missing wires/multiples */

  /*  if ((Correct_unchecked)&&(Pevent->Eventtype==EVENT_REST))
    {
      missing_counter++;
      if (missing_counter>=100)
	{
	  missing_counter=0;
	  Pevent->Eventtype=EVENT_UNCHECKED;
	}
    }

  if ((Correct_unchecked)&&(Pevent->Eventtype==EVENT_MULT))
      {
      if (((NOF_MWPC_Pos[6]+NOF_MWPC_Pos[2]+NOF_MWPC_Pos[1]+NOF_MWPC_Pos[5])>5)
	  ||((NOF_MWPC_Pos[7]+NOF_MWPC_Pos[3]+NOF_MWPC_Pos[0]+NOF_MWPC_Pos[4])>5))
	{
	  multiple_counter++;
	}
      if (multiple_counter>=100)
	{
	  multiple_counter=0;
	  Pevent->Eventtype=EVENT_UNCHECKED;
	}
    }
  */
  /* end DSP checks */



  /* If possible, calculate x, y, theta and phi at x-plane of VDC1 */
  /* At the moment, this requires exactly one good hit in each     */
  /* plane of the VDC's. 
                                          */
  if (Use_VESNA)
    {
      /*            */
      /* ADDON CODE */
      /*            */
      VDC_Position[0][0] = VESNA(1,0,0,0,9,NULL);
      VDC_Position[0][1] = VESNA(2,0,0,0,9,NULL);
      VDC_Position[0][2] = VESNA(3,0,0,0,9,NULL);
      VDC_Position[0][3] = VESNA(4,0,0,0,9,NULL);
      
      if ( (VDC_Position[0][0]<0) || (VDC_Position[0][1]<0) || 
	   (VDC_Position[0][2]<0) || (VDC_Position[0][3]<0) )
	{  
	  FPDS_OK = FALSEB;
	  VDC_Position[0][0] = -999;
	  VDC_Position[0][1] = -999;
	  VDC_Position[0][2] = -999;
	  VDC_Position[0][3] = -999;
	}
      else
	{
	  calc_fpds(VDC_Position[0][0],VDC_Position[0][1],VDC_Position[0][2],
		    VDC_Position[0][3], &X1_x, &X1_y, &Theta, &Phi);
	  FPDS_OK = TRUEB;
	}
    }
  else
    {
      /*               */
      /* ORIGINAL CODE */
      /*               */
      
      if (NOF_VDC_Pos[0]*NOF_VDC_Pos[1]*NOF_VDC_Pos[2]*NOF_VDC_Pos[3] == 1)
	{
	  calc_fpds(VDC_Position[0][0],VDC_Position[0][1],VDC_Position[0][2],
		    VDC_Position[0][3], &X1_x, &X1_y, &Theta, &Phi);
	  FPDS_OK = TRUEB;
	}
    }

	/* calc virtual focal plane position */
  calc_vfp(X1_x, Theta, &Xf);
	
  /* With x, y, theta and phi, calculate the proton momentum and   */
  /* theta and phi at the target position. */
  if (FPDS_OK) 
    {
      calc_target(X1_x, X1_y, Theta, &Theta_target, &Phi_target, &momentum);

      /* do the extrapolation of the D1 position from the VDC position */

      cr_angle = ANGLE_CENTRAL_RAY + cr_angle_offset;
      
      VDCx_2 = X1_x * cos(cr_angle);
      VDCy_2 = X1_y; 
      VDCz_2 = X1_x * sin(cr_angle);
      
      VDCx =  VDCx_2 + VDC0x;
      VDCy = -VDCy_2 + VDC0y;
      VDCz = -VDCz_2 + VDC0z;
      
      D1_extra[0] = VDCx + VDCz*tan(Theta);
      D1_extra[1] = VDCy + (VDCz-20)*tan(Phi);
    }

  /* now we check and set some gates */

  gate1 = TRUEB; /* TOF */

  if ((Gate_TOF)&&((TOF<TOF_limit[0])||(TOF>TOF_limit[1])))
    gate1 = FALSEB;
  
  gate2 = TRUEB; /* X1 and Theta */

  if ((Gate_X1)&&((X1_x<X1_limit[0])||(X1_x>X1_limit[1])))
    gate2 = FALSEB;
  if ((Gate_Theta)&&((Theta<Theta_limit[0])||(Theta>Theta_limit[1])))
    gate2 = FALSEB;

  /* for the variance gate (gate3) : check the VDC variables part */

  gate4 = FALSEB; /* eventtype gate */
  switch(Pevent->Eventtype)
    {
    case EVENT_FPP:
      if (Select_type[EVENT_FPP]) gate4=TRUEB;
      break;
    case EVENT_ANGLECUT:
      if (Select_type[EVENT_ANGLECUT]) gate4=TRUEB;
      break;
    case EVENT_UNCHECKED:
      if (Select_type[EVENT_UNCHECKED]) gate4=TRUEB;
      break;
    case EVENT_PULSER:
      if (Select_type[EVENT_PULSER]) gate4=TRUEB;
      break;
    case EVENT_MULT:
      if (Select_type[EVENT_MULT]) gate4=TRUEB;
      break;
    case EVENT_REST:
      if (Select_type[EVENT_REST]) gate4=TRUEB;
      break;
    case EVENT_MULT_REP:
      if (Select_type[EVENT_MULT_REP]) gate4=TRUEB;
      break;
    case EVENT_REST_REP:
      if (Select_type[EVENT_REST_REP]) gate4=TRUEB;
      break;
    }
  

  gate5 = FALSEB; /* polarization gate */
  switch(Polarization_info)
    {
    case BPOL_DOWN:
      if (Select_pol[0]) gate5=TRUEB;
      break;
    case BPOL_UP:
      if (Select_pol[1]) gate5=TRUEB;
      break;
    case BPOL_OFF:
      if (Select_pol[2]) gate5=TRUEB;
      break;
    default:
      if (Select_pol[3]) gate5=TRUEB;
      break;
    }
  
  gate6 = TRUEB;
  if ((Gate_target)&&((Theta_target<Theta_target_limit[0])||
			    (Theta_target>Theta_target_limit[1])))
    gate6 = FALSEB;
  if ((Gate_target)&&((Phi_target<Phi_target_limit[0])||
			  (Phi_target>Phi_target_limit[1])))
    gate6 = FALSEB;

  
  gate7 = FALSEB;
  if (Gate_BBS) 
	  gate7 = TRUEB;
  
  
  Scat_angle = sqrt((BBS_angle*M_PI/0.18 +
		  Theta_target)*(BBS_angle*M_PI/0.18 + Theta_target)+Phi_target*Phi_target);
  
  gate8 = FALSEB;
  if ( Gate_scat_1 && ((Scat_angle  > (M_PI/0.18)*
		  Scat_limit_1[0]) && (Scat_angle < (M_PI/0.18)* Scat_limit_1[1])) )
		  
	 gate8 = TRUEB;
  
  gate9 = FALSEB;
  if ( Gate_scat_2 && ((Scat_angle  > (M_PI/0.18)* Scat_limit_2[0])
		  && (Scat_angle < (M_PI/0.18)* Scat_limit_2[1])) )  
	  
	  gate9 = TRUEB;

  /* If possible, calculate some polarimeter variables. This       */
  /* requires a value for x, y, theta and phi in the VDC's and     */
  /* exactly one hit in each plane of D1, D2 and D4. It also uses  */
  /* the polarization info provided by the source.                 */
  
  if ((gate1)&&(gate2)&&(gate4)&&(gate5)&&(gate6))
    {
      if ((FPDS_OK)&&(FPP_OK))
	calc_fpp(X1_x,X1_y,Theta,Phi,D1_x,D1_y,D2_x,D2_y,
		 D3_x,D3_y,D4_x,D4_y, Polarization_info);
      if (File_FPP)
	{
	  fprintf(FPP_file,"\n\n");
	  fprintf(FPP_file,"FPDS_OK  %1d  -  FPP_OK  %1d\n",FPDS_OK,FPP_OK);
	  fprintf(FPP_file,"Eventtype : %3d\n",Pevent->Eventtype);
	  for(i=0;i<8;i++)
	    {
	      switch(i)
		{
		case 0:
		  fprintf(FPP_file,"\nD1 X-plane\t");
		  break;
		case 1:
		  fprintf(FPP_file,"\nD1 Y-plane\t");
		  break;
		case 2:
		  fprintf(FPP_file,"\nD2 X-plane\t");
		  break;
		case 3:
		  fprintf(FPP_file,"\nD2 Y-plane\t");
		  break;
		case 4:
		  fprintf(FPP_file,"\nD3 X-plane\t");
		  break;
		case 5:
		  fprintf(FPP_file,"\nD3 Y-plane\t");
		  break;
		case 6:
		  fprintf(FPP_file,"\nD4 X-plane\t");
		  break;
		case 7:
		  fprintf(FPP_file,"\nD4 Y-plane\t");
		  break;
		}
	      
	      fprintf(FPP_file,"Hits  : %3d\t",NOF_MWPC_Pos[i]);
	      if (NOF_MWPC_Pos[i])
		{
		  fprintf(FPP_file,"(");
		  for (j=0;j<NOF_MWPC_Pos[i];j++)
		    fprintf(FPP_file,"\t%3d",MWPC_Position[j][i]-MWPC_mid[i]); 
		  fprintf(FPP_file,")");
		}
	    }
	} 
    }


  /* booking histograms */
  /* ------------------ */
  
  if (!Ntuple)
    {
      /* "scaler" */

      HF1(5,1,1.);   /* add one for every event */
      
      for(i=0;i<4;i++)
	if (Pevent->VDC_hits[i][0] > 0)
	  HF1(5,3+i,1.); /* fill if this VDC-plane has been hit */

      for(i=0;i<8;i++)
	if (NOF_MWPC_Pos[i]>0)
	  HF1(5,8+i,1.); /* fill if this MWPC-plane has been hit */
		  
      HF1(6,1,1.);   /* add one for every event */
      
      for(i=0;i<4;i++)
	if (NOF_VDC_Pos[i] == 1)
	  HF1(6,3+i,1.); /* fill if this VDC-plane has been hit once */

      for(i=0;i<8;i++)
	if (NOF_MWPC_Pos[i] == 1)
	  HF1(6,8+i,1.); /* fill if this MWPC-plane has been hit once */

      if (FPDS_OK) HF1(6,17,1.); /* fill if coincidence all VDC-planes */
      if (FPP_OK) HF1(6,18,1.);  /* fill if coincidence all MWPC-planes */
      if ((FPDS_OK)&&(FPP_OK)) HF1(6,19,1.); /* fill if coincidence ALL planes */

      /* Event type histogram (ungated) */
      
      HF1(7, Pevent->Eventtype,1.);

      if ((gate1)&&(gate4)&&(gate5))  
	/* gate on TOF && eventtype && polarization */
	{

	  /* polarization */
	  
	  HF1(4,Polarization_info,1.);
	  
	  /* time of flight */
	  
	  HF1(11,TOF,1.);
	  HF1(12, RF,1.);
	  HF2(101,Sum_TOT[0],Sum_TOT[1],1.); /* s1 vs s2 */
	  
	  /* Event type histogram (gated) */
	  
	  HF1(8, Pevent->Eventtype,1.);

	  /* scintillators */
	  
	  for(i=0;i<2;i++)
	    {
	      switch(i)
		{
		case 0:
		  HCDIR("S1"," ");
		  PlaneID = 2;   /* D2_x */
		  break;
		case 1:
		  HCDIR("S2"," ");
		  PlaneID = 6;   /* D4_x */
		  break;
		}
	      
	      for(j=0;j<10;j++)  
		{
		  
		  /* book multiplicity */
		  if ((Pevent->STDCM[10*i+j][0]>1) ||
		      (Pevent->STDCM[10*i+j][1]>1))
		    HF1(1,j+1,1.);
		  
		  /* book leading edge missing */
		  if ((Pevent->S_time[10*i+j][0]==-1) &&
		      (Pevent->S_time[10*i+j][1]!=-1))
		    HF1(2,j+1,1.);
		  
		  /* book trailing edge missing */
		  if ((Pevent->S_time[10*i+j][1]==-1) &&
		      (Pevent->S_time[10*i+j][0]!=-1))
		    HF1(3,j+1,1.);
		}
	      
	      
	      HF1(4,Sum_TOT[i],1.);
	      HF1(5,NOF_PMs[i],1.);
	      
	      HCDIR("TOT SPECTRA"," ");
	      
	      for(j=0;j<10;j++)
		{
		  if(TOT[10*i+j]!=-999)
		    {
		      HF1( 1+j, TOT[10*i+j], 1.);
		      HF2(51+j, TOT[10*i+j], TOF, 1.);
		    }
		}
	      
	      HCDIR("\\"," ");
	      
	      /* sum TOT versus x in either D2x (S1) or D4x (S2) */
	      
	      for (k=0;(k<NOF_MWPC_Pos[PlaneID])&&(k<50);k++)
		{
		  wirenum = MWPC_Position[k][PlaneID]/2.+1;
		  HF2(6,wirenum,Sum_TOT[i],1.);
		}
	      
	      HCDIR("\\"," ");
	      
	    }
	  
	  /* QDC */
	  
	  if (QDC_in)
	    {
	      HCDIR("QDC"," ");
	      
	      for(i=0;i<10;i++)
		{
		  HF1(i+  1,QDC[i],1.);
		  HF2(i+101,TOT[i],QDC[i],1.);
		}
	      
	      for(i=0;i<5;i++)
		{
		  QDC_Paddle = QDC[2*i] + QDC[2*i+1];
		  TOT_Paddle = 0;
		  for(j=0;j<2;j++)
		    if (TOT[2*i+j]!=-999) 
		      TOT_Paddle+=TOT[2*i+j];
		  if (TOT_Paddle > 0)
		    HF2(i+121,TOT_Paddle,QDC_Paddle,1.);
		}
	      
	      for (k=0;(k<NOF_MWPC_Pos[2])&&(k<50);k++)
		{
		  wirenum = MWPC_Position[k][2]/2.+1;
		  HF2(21,wirenum,Sum_QDC,1.);
		}
	      
	      HF2(22,TOF,Sum_QDC,1.);
	      
	      HCDIR("\\"," ");
	    }
	  
	  /* VDC's */
	  
	  for(i=0;i<4;i++)
	    {
	      switch(i)
		{
		case 0:
		  HCDIR("VDC1/X-PLANE"," ");
		  break;
		case 1:
		  HCDIR("VDC1/U-PLANE"," ");
		  break;
		case 2:
		  HCDIR("VDC2/X-PLANE"," ");
		  break;
		case 3:
		  HCDIR("VDC2/U-PLANE"," ");
		  break;
		}
	      
	      /* we always add 1 before booking a wire number
		 because the read out start counting from wire 0,
		 we like to start counting from 1 !! */
	      
	      
	      HF1(9,NOF_Clusters[i],1.);
	      for(k=0;(k<NOF_Clusters[i])&&(k<10);k++)
		{
		  HF1(8, Clustersize[k][i], 1.);
		}
	      
	      for(k=0;(k<NOF_Missing[i])&&(k<10);k++)
		{
		  HF1(5, Missing_wires[k][i] + 1, 1.);
		}
	      
	      HF1(400, NOF_VDC_Pos[i], 1.);
	      for(k=0;(k<NOF_VDC_Pos[i])&&(k<10);k++)
		{
		  HF1(100, Centroid[k][i]+1, 1.);
		  HF1(200, VDC_Position[k][i]+1, 1.);
		  HF1(300, Variance[k][i], 1.);
		}
	      
	      for(k=0;k<WIRES_VDC;k++)
		{
		  if (Pevent->VTDCM[k][i][0] == 0)
		    continue;     /* wire not read out */
		  
		  HF1(1,k+1,1.);  /* book wire regardless of multiplicity */
		  
		  switch(Pevent->VTDCM[k][i][0])
		    {
		    case 1:  /* one signal on this wire; good one ! */
		      HF1(2,k+1,1.);  /* book TDCM = 1 */
		      break;
		    case 2:  /* two signals on this wire */
		      HF1(3,k+1,1.); /* book TDCM = 2 and go to next wire */
		      continue;
		    default: /* more than two signals on this wire */
		      HF1(4,k+1,1.); /* book TDCM > 2 and go to next wire */
		      continue;		  
		    }
		}
	      
	      HCDIR("DRIFTTIMES"," ");
	      for(k=0;k<WIRES_VDC;k++)
		{
		  if (Pevent->VTDCM[k][i][0] == 1)
		    {
		      if ((k+1)%10 == 5)
			{
			  HF1(k+1,Pevent->VDC_time[k][i][0],1.);
			  continue;
			}

		      /* just checking a U1 wire problem */
		      if ((i==1)&((k+1)>=130)&&((k+1)<=160))
			HF1(k+1,Pevent->VDC_time[k][i][0],1.);

		    }
		}
	      HCDIR("\\"," ");
	      
	      HCDIR("\\\\"," ");
	    }
	  
	  /* MWPC's */
	  
	  for(i=0;i<4;i++)
	    {
	      switch(i)
		{
		case 0:
		  HCDIR("MWPC1"," ");
		  break;
		case 1:
		  HCDIR("MWPC2"," ");
		  break;
		case 2:
		  HCDIR("MWPC3"," ");
		  break;
		case 3:
		  HCDIR("MWPC4"," ");
		  break;
		}
	      
	      for(j=0;j<2;j++)
		{
		  PlaneID = 2*i + j;
		  
		  HF1(j+5,NOF_MWPC_Pos[PlaneID],1.);
		  
		  for(k=0;(k<NOF_MWPC_Pos[PlaneID])&&(k<50);k++)
		    {
		      centr = MWPC_Position[k][PlaneID]/2;
		      width = MWPC_Width[k][PlaneID];
		      
		      for (m=(centr-(width-1)/2);m<=(centr+width/2);m++)
			{
			  HF1(j+11,m+1,1.);
			}
		      
		      wirenum = MWPC_Position[k][PlaneID]/2. + 1;
		      
		      HF1(j+1,wirenum,1.);
		      HF1(j+3,width,1.);
		    }
		}
	      
	      HCDIR("\\"," ");
	    }
	  
	  if ((gate2)&&(gate6))   /* gate on X1, theta */
	    {
	      /* FPDS variables */
	      
	      HF1(21,Theta,1.);
	      HF1(22,Phi,1.);
	      HF1(23,X1_y,1.);  
	      HF1(24,X1_x,1.);  
	      HF2(25,X1_x,X1_y,1.);
	      HF2(26,X1_x,Theta,1.);
	      HF2(27,X1_y,Phi,1.);
	      HF2(102,X1_x,RF,1.);
	      HF2(103,X1_x,Sum_TOT[1],1.);

	      /* target variables */
	      
	      if (FPDS_OK)
		{
		  HCDIR("TARGET"," ");
		  HF1(1,Theta_target,1.);
		  HF1(2,Phi_target,1.);
		  HF1(3,momentum,1.);
		  
	
		/* gates on scattering angles */	  
	  	  if ((gate7) && (gate8))
		  {
		  HF1(4,momentum,1.);
	  	  }
		  
		  if ((gate7) && (gate9))
		  {  
		  HF1(5,momentum,1.);
	      }
		
		 		    
		  HF2(11,Theta_target,Phi_target,1.);
		  HF2(12,momentum,Theta_target,1.);
		  HF2(13,momentum,Phi_target,1.);
		  switch (Polarization_info)
		  {
		    case BPOL_UP: 
		      HF1(21,momentum,1.);
		      break;
		    case BPOL_DOWN:
		      HF1(22,momentum,1.);
		      break;
		    case BPOL_OFF:
		      HF1(23,momentum,1.);
		      break;
		  }
		  if ((Theta_target < 40.) && (Theta_target > -40.)
		  	&&(Phi_target < 50.) && (Phi_target >-50.))
		    HF1(31,momentum,1.);
		  HCDIR("\\"," ");
		}
	    }
	}
    }
  

  /* booking ntuple */
  /* -------------- */
  
  if (Ntuple)
    {
      /* downscale region in X1_x if requested */

      if (X1_x < Ana_par.x_scale_below)
	{
	  scale_counter++;
	  
	  if (scale_counter == Ana_par.downscale_factor) 
	    scale_counter = 0;
	  else
	    return -2;
	}
      
      if (Ntup_Block[2])
	{
	  /* polarization */
	  PID_ntup.beam_pol = Polarization_info;
	  
	  /* time of flight */
	  PID_ntup.tof = TOF;

	  PID_ntup.eventtype = Pevent->Eventtype;

	  PID_ntup.sum_tot_s1 = Sum_TOT[0];
	  PID_ntup.sum_tot_s2 = Sum_TOT[1];	  
	}

      if (Ntup_Block[0])
	{
	  /* FPDS_ntup.x1_int = VDC_Position[0][0];
	     FPDS_ntup.u1_int = VDC_Position[0][1];
	     FPDS_ntup.x2_int = VDC_Position[0][2];
	     FPDS_ntup.u2_int = VDC_Position[0][3];
	     
	     FPDS_ntup.x1_var = Variance[0][0];
	     FPDS_ntup.u1_var = Variance[0][1];
	     FPDS_ntup.x2_var = Variance[0][2];
	     FPDS_ntup.u2_var = Variance[0][3];
	  */
	  
	  FPDS_ntup.x1     = X1_x;
	  FPDS_ntup.y1     = X1_y;
	  FPDS_ntup.theta  = Theta;
	  FPDS_ntup.phi    = Phi;
	}

      if (Ntup_Block[1])
	{
	  FPP_ntup.d1x = D1_x;
	  FPP_ntup.d1y = D1_y;
	  FPP_ntup.d2x = D2_x;
	  FPP_ntup.d2y = D2_y;
	  FPP_ntup.d3x = D3_x;
	  FPP_ntup.d3y = D3_y;
	  FPP_ntup.d4x = D4_x;
	  FPP_ntup.d4y = D4_y;
	  
	} 

      if (Ntup_Block[3])
	{
	  TAR_ntup.theta_target = Theta_target;
	  TAR_ntup.phi_target   = Phi_target;
	  TAR_ntup.momentum     = momentum;	      
	}
      
      /* fill ntuple */
      
      HFNT(1001); 
    }
  

  return 0;
}


/***********************************************/
/* Calculate X, Y, Theta and Phi from VDC data */
/***********************************************/

static void calc_fpds(float WX1, float WU1, float WX2, float WU2, 
		     float * X1_xb, float * X1_yb,
		     float * Theta, float * Phi)
{
  float X1_xa, X2_xa, U1_xa, U2_xa, U1_xb, U2_xb;
  float U1_ub, U2_ub, U1_yb, U2_yb, Dist_12;
  float cr_angle;

  cr_angle = ANGLE_CENTRAL_RAY + cr_angle_offset;

  X1_xa = FWX1_X_POS - WX1*WIRE_DISTANCE;
  X2_xa = FWX2_X_POS - WX2*WIRE_DISTANCE;
  
  U1_xa = X1_xa +
          (X1_xa - X2_xa)*(FWU1_Y_POS-FWX1_Y_POS)/(FWX1_Y_POS-FWX2_Y_POS);
  U2_xa = X1_xa + 
          (X1_xa - X2_xa)*(FWU2_Y_POS-FWX1_Y_POS)/(FWX1_Y_POS-FWX2_Y_POS);
  
  U1_xb = FWU1_X_POS - U1_xa;
  U2_xb = FWU2_X_POS - U2_xa;
  
  U1_ub = WU1*WIRE_DISTANCE;
  U2_ub = WU2*WIRE_DISTANCE;
  
  U1_yb = (U1_xb * COS_ANGLE_U - U1_ub)/SIN_ANGLE_U;
  U2_yb = (U2_xb * COS_ANGLE_U - U2_ub)/SIN_ANGLE_U;
  
  *X1_xb = WX1*WIRE_DISTANCE;
  *X1_yb = U2_yb - 
           (FWU2_Y_POS-FWX1_Y_POS)*(U2_yb-U1_yb)/(FWU2_Y_POS-FWU1_Y_POS);

  Dist_12 = sqrt((X2_xa-X1_xa)*(X2_xa-X1_xa) +
		 (FWX2_Y_POS-FWX1_Y_POS)*(FWX2_Y_POS-FWX1_Y_POS));
  
  *Theta = atan((X2_xa - X1_xa)/(FWX2_Y_POS-FWX1_Y_POS))
           - cr_angle;
  
  *Phi   = atan((U2_yb - U1_yb)/(cos(*Theta)*Dist_12)) - Phi_offset;

  return;
}



/********************************************************/
/* Calculate virtual focal plane coordinates            */
/********************************************************/
static int calc_vfp(float X1, float Theta, float * Xf)
{
  float tanpsi, a, b;

  if (X1 == -999. || Theta == -999.) return -1;

  tanpsi = tan(vfp_psi * M_PI/180.0);
  
  a = tanpsi * (X1_MIDDLE  - vfp_L * COS_ALPHA) - vfp_L * SIN_ALPHA;
  b = tan(ALPHA - Theta) * X1;

  *Xf = (a + b) / (b/X1 + tanpsi);

  return 0;
}

/********************************************************/
/* Calculate analyzing power for given energy and angle */
/********************************************************/

static double calc_ana(double Ecenter, double sin_theta_carb)
{
  double a,b,c;
  double pi,pi2,pi3,pi4,ksi,ksi2,ksi4;
  double analyzing_power;
  double pcenter;

  pcenter = sqrt(Ecenter*(2*proton_restmass+Ecenter));

  pi  = pcenter - 0.700;
  pi2 = pi*pi;
  pi3 = pi*pi2;
  pi4 = pi3*pi;

  a = a0 + a1*pi + a2*pi2 + a3*pi3 + a4*pi4;
  b = b0 + b1*pi + b2*pi2 + b3*pi3 + b4*pi4;
  c = c0 + c1*pi + c2*pi2 + c3*pi3 + c4*pi4;

  ksi  = pcenter * sin_theta_carb;
  ksi2 = ksi*ksi;
  ksi4 = ksi2*ksi2;

  analyzing_power = a*ksi/(1 + b*ksi2 + c*ksi4);

  return analyzing_power;
}


/*****************************************************/
/* calculate target variables from fitted parameters */
/*****************************************************/

static void calc_target(float Xd, float Yd, float Td,
			float *Tt, float *Pt, float *moment)
{
  /* float c1, c2, c3, c4; */
  
  /* Momentum fit: p = f(Xd, Yd, Td) [MeV/c] (third oder fit)
     --------------------------------------------------------
     C          +6.824467e+02
     C_Xd       -1.232002e-01
     C_Yd       +5.181561e-04
     C_Td       +9.821224e+00
     C_Xd*Yd    -1.825561e-06
     C_Xd*Td    -2.270458e-02
     C_Yd*Td    -2.184000e-02
     C_Xd**2    -4.111105e-05
     C_Yd**2    +9.079623e-05
     C_Td**2    -7.388398e+01
     C_Xd*Yd*Td +4.307513e-05
     C_Xd**2*Yd +3.059155e-10
     C_Xd**2*Td +1.233548e-05
     C_Yd**2*Xd -1.515269e-07
     C_Yd**2*Td -2.456049e-04
     C_Td**2*Xd +2.097084e-01
     C_Td**2*Yd +9.297566e-02
     C_Xd**3    +8.530886e-09
     C_Yd**3    +8.000667e-09
     C_Td**3    +3.865610e+02
  */
  
  *moment = +6.824467e+02
    -1.232002e-01*Xd         +5.181561e-04*Yd        +9.821224e+00*Td
    -1.825561e-06*Xd*Yd      -2.270458e-02*Xd*Td     -2.184000e-02*Yd*Td
    -4.111105e-05*Xd*Xd      +9.079623e-05*Yd*Yd     -7.388398e+01*Td*Td
    +4.307513e-05*Xd*Yd*Td
    +3.059155e-10*Xd*Xd *Yd  +1.233548e-05*Xd*Xd *Td
    -1.515269e-07*Yd*Yd *Xd  -2.456049e-04*Yd*Yd *Td
    +2.097084e-01*Td*Td *Xd  +9.297566e-02*Td*Td *Yd
    +8.530886e-09*Xd*Xd*Xd   +8.000667e-09*Yd*Yd*Yd  +3.865610e+02*Td*Td*Td;
  
  /* Theta_target fit: Tt = f(Xd, Yd, Td) [MeV/c] (third oder fit)
     -------------------------------------------------------------
     C          -2.945145e+01
     C_Xd       +6.717314e-02
     C_Yd       +2.754338e-02
     C_Td       +4.492754e+02
     C_Xd*Yd    -6.646432e-05
     C_Xd*Td    -1.081587e-01
     C_Yd*Td    +2.433274e-02
     C_Xd**2    -1.040832e-05
     C_Yd**2    +5.014310e-04
     C_Td**2    +1.202321e+02
     C_Xd*Yd*Td -8.003344e-05
     C_Xd**2*Yd +4.015931e-08
     C_Xd**2*Td +1.698658e-04
     C_Yd**2*Xd -8.159989e-07
     C_Yd**2*Td -1.222418e-03
     C_Td**2*Xd +2.120180e-01
     C_Td**2*Yd -1.164198e-01
     C_Xd**3    +3.047526e-08
     C_Yd**3    +2.017390e-08
     C_Td**3    +6.310898e+01
  */

  *Tt = -2.945145e+01
    +6.717314e-02*Xd         +2.754338e-02*Yd        +4.492754e+02*Td
    -6.646432e-05*Xd*Yd      -1.081587e-01*Xd*Td     +2.433274e-02*Yd*Td
    -1.040832e-05*Xd*Xd      +5.014310e-04*Yd*Yd     +1.202321e+02*Td*Td
    -8.003344e-05*Xd*Yd*Td
    +4.015931e-08*Xd*Xd *Yd  +1.698658e-04*Xd*Xd *Td
    -8.159989e-07*Yd*Yd *Xd  -1.222418e-03*Yd*Yd *Td
    +2.120180e-01*Td*Td *Xd  -1.164198e-01*Td*Td *Yd
    +3.047526e-08*Xd*Xd*Xd   +2.017390e-08*Yd*Yd*Yd  +6.310898e+01*Td*Td*Td;

  /* Phi_target fit: p = f(Xd, Yd)
     -----------------------------
     because producing a third order fit with MINUIT did not work
     out for Phi_target, I produced a (rather crude, but working)
     fit using the approach described by R. Zegers in his internal 
     report on 'Ray tracing at the Big Bite Spectrometer'.
  
     c1 = -3.03115;
     c2 = -0.18714;
     c3 = -0.0117104;
     c4 = +0.00252921;
 
     *Pt = ( Yd - c1 - c3*Xd ) / ( c2 + c4*Xd ); */
     
  /* Using a similar approach with MINUIT again i got to the 
     following fit. This is as good as it gets at the moment.

     C          +1.952730e+00
     C_Xd       +2.818747e-02
     C_Yd       +1.075646e+00
     C_Xd*Yd    +1.375872e-04
     C_Xd**2    -4.159629e-05
     C_Yd**2    -2.529846e-03
     C_Xd**2*Yd -5.918242e-07
     C_Yd**2*Xd +5.661931e-06
     C_Xd**3    +2.045242e-08
     C_Yd**3    +1.059039e-04
     D          -2.288684e-01
     D_Xd       +2.423389e-03
     D_Yd       -3.217867e-04
     D_Yd**2    +1.622719e-04
  */
  
  *Pt = ( +1.952730e+00 + +2.818747e-02*Xd + +1.075646e+00*Yd +
	  +1.375872e-04*Xd*Yd + -4.159629e-05*Xd*Xd + -2.529846e-03*Yd*Yd +
	  -5.918242e-07*Xd*Xd *Yd + +5.661931e-06*Yd*Yd *Xd +
	  +2.045242e-08*Xd*Xd*Xd + +1.059039e-04*Yd*Yd*Yd )
    / ( -2.288684e-01 + +2.423389e-03*Xd +
	-3.217867e-04*Yd + +1.622719e-04*Yd*Yd ); 
  
  return;
}



/*****************************************************/
/* Calculate polarimeter info from VDC and MWPC data */
/*****************************************************/

static void calc_fpp(float x1, float y1, float Theta, float Phi,
		     float D1x_raw, float D1y_raw, float D2x_raw, 
		     float D2y_raw, float D3x_raw, float D3y_raw,
		     float D4x_raw, float D4y_raw, int polarization)
{
  Boolean gate;

  float Corry,Corrz;
  float VDCx_2,VDCy_2,VDCz_2;
  float VDCx,VDCy,VDCz;

  float D1x,D1y,D1z;
  float D2x,D2y,D2z;
  float D3x,D3y,D3z;
  float D4x,D4y,D4z;

  float D1x_dsp,D1y_dsp,D1z_dsp;
  float D2x_dsp,D2y_dsp,D2z_dsp;
  float D3x_dsp,D3y_dsp,D3z_dsp;
  float D4x_dsp,D4y_dsp,D4z_dsp;
  float CAx,CAy,CAz;

  float vinmod,vinx,viny,vinz;
  float voutmod,voutx,vouty,voutz;
  float cos_theta_carb,sin_theta_carb,tan_theta_carb;
  float cos_phi_carb,sin_phi_carb;

  float vinmod_dsp,vinx_dsp,viny_dsp,vinz_dsp;
  float voutmod_dsp,voutx_dsp,vouty_dsp,voutz_dsp;
  float sin_theta_carb_dsp,cos_theta_carb_dsp;
  float cos_phi_carb_dsp,sin_phi_carb_dsp;

  float D12x,D12y,D12z;
  float dx,dy,dz,dmod;
  float vertx,verty,vertz;
  float nx,ny,nz,nmod;
  float nx_dsp,ny_dsp,nz_dsp,nmod_dsp;
  float Phi_carb,Phi_carb_dsp,Phi_carb_deg,Phi_carb_deg_dsp;
  float Theta_carb,Theta_carb_dsp,Theta_carb_deg,Theta_carb_deg_dsp;
  float conex_sphere,coney_sphere;
  float conex_sphere_dsp,coney_sphere_dsp;
  float conex_plane,coney_plane;

  float cr_angle;
  float s,t;

  float Theta2,Phi2; 

  float D1x_diff,D1y_diff;
  float D2x_diff,D2y_diff;
  float D3x_diff,D3y_diff;
  float D4x_diff,D4y_diff;

  float D2x_int,D2y_int;
  float D3x_int,D3y_int;

  double Ay,Ay_2,Ay_cos,Ay_sin;
  int    channel;

  cr_angle = ANGLE_CENTRAL_RAY + cr_angle_offset;

  VDCx_2 = x1 * cos(cr_angle);
  VDCy_2 = y1; 
  VDCz_2 = x1 * sin(cr_angle);
  
  VDCx =  VDCx_2 + VDC0x;
  VDCy = -VDCy_2 + VDC0y;
  VDCz = -VDCz_2 + VDC0z;
  
  D1x = D1x_raw * 1.25;
  D2x = D2x_raw * 1.25 + D2x_offset;
  D3x = D3x_raw * 1.25 + D3x_offset;
  D4x = D4x_raw * 1.25 + D4x_offset;
  
  Corrz     = 20;
  Corry     = (float)(D4y_raw-D2y_raw)/D2_D4 * Corrz;
  
  D1y  = D1y_raw * 1.25 + tan(Phi) * Corrz;
  D2y  = D2y_raw * 1.25 - Corry + D2y_offset;
  D3y  = D3y_raw * 1.25 - Corry + D3y_offset;
  D4y  = D4y_raw * 1.25 - Corry + D4y_offset;
  
  D1z  = 0.;
  D2z  = 850. + D2z_offset;
  D3z  = 1120. + D3z_offset;
  D4z  = 1390. + D4z_offset;

  if (Correct_DSP)
    {
      CAz  = 700. + CAz_offset;

      D1x_dsp = D1x;
      D1y_dsp = D1y;
      D1z_dsp = D1z;

      D2x_dsp = D2x;
      D2y_dsp = D2y;
      D2z_dsp = D2z;

      D4x_dsp = D4x;
      D4y_dsp = D4y;
      D4z_dsp = D4z;
    }
  else
    {
      CAz  = 700.;

      D1x_dsp = D1x_raw * 1.25;
      D1y_dsp = D1y_raw * 1.25;
      D1z_dsp = 0.;

      D2x_dsp = D2x_raw * 1.25;
      D2y_dsp = D2y_raw * 1.25;
      D2z_dsp = 850.;

      D3x_dsp = D3x_raw * 1.25;
      D3y_dsp = D3y_raw * 1.25;
      D3z_dsp = 1120.;

      D4x_dsp = D4x_raw * 1.25;
      D4y_dsp = D4y_raw * 1.25;
      D4z_dsp = 1390.;
    }

  CAx  = D4x_dsp - (D4x_dsp-D2x_dsp)*(D4z_dsp-CAz)/(D4z_dsp-D2z_dsp);
  CAy  = D4y_dsp - (D4y_dsp-D2y_dsp)*(D4z_dsp-CAz)/(D4z_dsp-D2z_dsp);

  vinx    = D1x - VDCx;
  viny    = D1y - VDCy;
  vinz    = D1z - VDCz;
  vinmod  = sqrt(vinx*vinx + viny*viny + vinz*vinz);
  if (vinmod!=0)
    {
      vinx    = vinx/vinmod;
      viny    = viny/vinmod;
      vinz    = vinz/vinmod;
    }
  else 
    vinx = viny = vinz = 0;

  vinx_dsp    = CAx - D1x_dsp;
  viny_dsp    = CAy - D1y_dsp;
  vinz_dsp    = CAz - D1z_dsp;
  vinmod_dsp  = sqrt(vinx_dsp*vinx_dsp 
		     + viny_dsp*viny_dsp + vinz_dsp*vinz_dsp);
  if (vinmod_dsp!=0)
    {
      vinx_dsp    = vinx_dsp/vinmod_dsp;
      viny_dsp    = viny_dsp/vinmod_dsp;
      vinz_dsp    = vinz_dsp/vinmod_dsp;
    }
  else
    vinx_dsp = viny_dsp = vinz_dsp = 0;

  Theta2   = -atan(vinx/vinz);
  Phi2    = -atan(viny/vinz);
  
  voutx   = D4x - D2x;
  vouty   = D4y - D2y;
  voutz   = D4z - D2z;
  voutmod = sqrt(voutx*voutx + vouty*vouty + voutz*voutz);
  if (voutmod !=0)
    {
      voutx   = voutx/voutmod;
      vouty   = vouty/voutmod;
      voutz   = voutz/voutmod;
    }
  else
    voutx = vouty = voutz =0;

  voutx_dsp   = D4x_dsp - D2x_dsp;
  vouty_dsp   = D4y_dsp - D2y_dsp;
  voutz_dsp   = D4z_dsp - D2z_dsp;
  voutmod_dsp = sqrt(voutx_dsp*voutx_dsp
		     + vouty_dsp*vouty_dsp + voutz_dsp*voutz_dsp);
  if (voutmod_dsp!=0)
    {
      voutx_dsp   = voutx_dsp/voutmod_dsp;
      vouty_dsp   = vouty_dsp/voutmod_dsp;
      voutz_dsp   = voutz_dsp/voutmod_dsp;
    }
  else
    voutx_dsp = vouty_dsp = voutz_dsp = 0;

  nx    = viny*voutz - vouty*vinz;
  ny    = vinz*voutx - voutz*vinx;
  nz    = vinx*vouty - voutx*viny;
  nmod  = sqrt(nx*nx + ny*ny + nz*nz);
  if (nmod != 0)
    {
      nx    = nx/nmod;
      ny    = ny/nmod;
      nz    = nz/nmod;
    }
  else
    nx = ny = nz = 0;

  nx_dsp    = viny_dsp*voutz_dsp - vouty_dsp*vinz_dsp;
  ny_dsp    = vinz_dsp*voutx_dsp - voutz_dsp*vinx_dsp;
  nz_dsp    = vinx_dsp*vouty_dsp - voutx_dsp*viny_dsp;
  nmod_dsp  = sqrt(nx_dsp*nx_dsp + ny_dsp*ny_dsp + nz_dsp*nz_dsp);
  if (nmod_dsp != 0)
    {
      nx_dsp    = nx_dsp/nmod_dsp;
      ny_dsp    = ny_dsp/nmod_dsp;
      nz_dsp    = nz_dsp/nmod_dsp;
    }
  else
    nx_dsp = ny_dsp = nz_dsp = 0;

  if (nx > 0) 
    Phi_carb = acos(ny); 
  else 
    Phi_carb = -acos(ny);

  Phi_carb_deg   = Phi_carb/M_PI*180;

  if (nx_dsp > 0) 
    Phi_carb_dsp = acos(ny_dsp); 
  else 
    Phi_carb_dsp = -acos(ny_dsp);

  Phi_carb_deg_dsp   = Phi_carb_dsp/M_PI*180;

  cos_theta_carb = vinx*voutx + viny*vouty + vinz*voutz;
  Theta_carb     = acos(cos_theta_carb);
  Theta_carb_deg = Theta_carb/M_PI*180;
  sin_theta_carb = sin(Theta_carb);
  tan_theta_carb = tan(Theta_carb);

  cos_theta_carb = vinx*voutx + viny*vouty + vinz*voutz;
  cos_theta_carb_dsp = vinx_dsp*voutx_dsp 
    + viny_dsp*vouty_dsp + vinz_dsp*voutz_dsp;
  Theta_carb_dsp     = acos(cos_theta_carb_dsp);
  Theta_carb_deg_dsp = Theta_carb_dsp/M_PI*180;
  sin_theta_carb_dsp = sin(Theta_carb_dsp);
  
  cos_phi_carb  = cos(Phi_carb);
  sin_phi_carb  = sin(Phi_carb);

  cos_phi_carb_dsp  = cos(Phi_carb_dsp);
  sin_phi_carb_dsp  = sin(Phi_carb_dsp);

  conex_sphere  = sin_theta_carb*cos_phi_carb;
  coney_sphere  = sin_theta_carb*sin_phi_carb;
  conex_plane   = tan_theta_carb*cos_phi_carb;
  coney_plane   = tan_theta_carb*sin_phi_carb;

  conex_sphere_dsp  = sin_theta_carb_dsp*cos_phi_carb_dsp;
  coney_sphere_dsp  = sin_theta_carb_dsp*sin_phi_carb_dsp;

  D12x   = D2x - D1x;
  D12y   = D2y - D1y;
  D12z   = D2z - D1z;
  
  if (cos_theta_carb != 1)
    {
      s       = ((D12x*vinx + D12y*viny + D12z*vinz) - 
		 (D12x*voutx + D12y*vouty + D12z*voutz)*cos_theta_carb) /
	(1 - cos_theta_carb*cos_theta_carb);
      
      t       = ((D12x*voutx + D12y*vouty + D12z*voutz) - 
		 (D12x*vinx + D12y*viny + D12z*vinz)*cos_theta_carb) /
	(1 - cos_theta_carb*cos_theta_carb);

      dx      = D12x - s*vinx - t*voutx;
      dy      = D12y - s*viny - t*vouty;
      dz      = D12z - s*vinz - t*voutz;
      dmod    = sqrt(dx*dx + dy*dy + dz*dz);

      vertx   = D1x + s*vinx + 0.5*dx;
      verty   = D1y + s*viny + 0.5*dy;
      vertz   = D1z + s*vinz + 0.5*dz;
    }
  else
    {
      dx = dy = dz = dmod = 0;
      vertx = verty = vertz = -999.;
    }
  
  Ay      = calc_ana(.170,sin_theta_carb);
  Ay_2    = Ay*Ay;
  Ay_cos  = Ay*cos_phi_carb;
  Ay_sin  = Ay*sin_phi_carb;
  channel =  5*Theta_carb_deg;

  /* check gates */

  gate = TRUEB;
  if ((Gate_cos_theta_carb)&&((cos_theta_carb_dsp<cos_theta_carb_limit[0])
			      ||(cos_theta_carb_dsp>cos_theta_carb_limit[1])))
    gate = FALSEB;
  if ((Gate_vertz)&&((vertz<vertz_limit[0])||(vertz>vertz_limit[1])))
    gate = FALSEB;

  if (gate)
    {
      if (!Ntuple)
	{
	  HCDIR("POLARIMETER"," ");
	  
	  HF1(1,dmod,1.);
	  HF1(2,vertz,1.);
	  HF1(3,Theta_carb_deg_dsp,1.);
	  HF1(4,Theta_carb_deg,1.);
	  HF1(5,cos_theta_carb,1.);
	  HF1(6,Phi_carb_deg,1.);
	  HF1(7,cos_phi_carb,1.);
	  HF2(8,Theta_carb_deg,Phi_carb_deg,1.);
	  HF2(9,conex_sphere,coney_sphere,1.);
	  HF2(10,conex_plane,coney_plane,1.);

	  HF1(11,Theta2,1.);
	  HF1(12,Phi2,1.);
	  HF1(13,Theta-Theta2,1.);
	  HF1(14,Phi-Phi2,1.);
	  HF1(15,dx,1.);
	  HF1(16,dy,1.);
	  HF1(17,dz,1.);
	  HF2(18,dx,dy,1.);

	  /*
	  HF2(204,coney_sphere_dsp,conex_sphere_dsp,1.);
	  HF2(205,Theta_carb_deg_dsp,Phi_carb_deg_dsp,1.);
	  */

	  D1x_diff = D1x - (VDCx - (D1z-VDCz)*tan(Theta));
	  D1y_diff = D1y - (VDCy - (D1z-VDCz)*tan(Phi));
	  D2x_diff = D2x - (VDCx - (D2z-VDCz)*tan(Theta));
	  D2y_diff = D2y - (VDCy - (D2z-VDCz)*tan(Phi));
	  D3x_diff = D3x - (VDCx - (D3z-VDCz)*tan(Theta));
	  D3y_diff = D3y - (VDCy - (D3z-VDCz)*tan(Phi));
	  D4x_diff = D4x - (VDCx - (D4z-VDCz)*tan(Theta));
	  D4y_diff = D4y - (VDCy - (D4z-VDCz)*tan(Phi));
	  
	  HF1(101,D1x_diff,1.);
	  HF1(102,D1y_diff,1.);
	  HF1(103,D2x_diff,1.);
	  HF1(104,D2y_diff,1.);
	  HF1(105,D3x_diff,1.);
	  HF1(106,D3y_diff,1.);
	  HF1(107,D4x_diff,1.);
	  HF1(108,D4y_diff,1.);

	  D2x_int = D2x - (D1x + (D4x - D1x)*(D2z-D1z)/(D4z - D1z));
	  D2y_int = D2y - (D1y + (D4y - D1y)*(D2z-D1z)/(D4z - D1z));
	  D3x_int = D3x - (D1x + (D4x - D1x)*(D3z-D1z)/(D4z - D1z));
	  D3y_int = D3y - (D1y + (D4y - D1y)*(D3z-D1z)/(D4z - D1z));

	  HF1(111,D2x_int,1.);
	  HF1(112,D2y_int,1.);
	  HF1(113,D3x_int,1.);
	  HF1(114,D3y_int,1.);

	  HF1(201, D1x, 1.);
	  HF1(202, D1y, 1.);
	  HF1(203, D2x, 1.);
	  HF1(204, D2y, 1.);
	  HF1(205, D3x, 1.);
	  HF1(206, D3y, 1.);
	  HF1(207, D4x, 1.);
	  HF1(208, D4y, 1.);
	  
	  HF1(301,Theta_carb_deg,Ay_cos);
	  HF1(302,Theta_carb_deg,Ay_sin);
	  HF1(303,Theta_carb_deg,Ay_2);

	  if (cos_phi_carb>0)
	    HF1(304,Theta_carb_deg,1.);
	  else
	    HF1(304,Theta_carb_deg,-1.);
	  
	  if ((channel>=0)&&(channel<250))
	      {
		estim_cos[channel]+=Ay_cos;
		estim_sin[channel]+=Ay_sin;
		estim_den[channel]+=Ay_2;
	      }

	  HCDIR("\\"," ");
	}
      else
	{
	  if (Ntup_Block[1])
	    {
	      FPP_ntup.theta_FPP = Theta_carb_deg;
	      FPP_ntup.phi_FPP   = Phi_carb_deg;
	      /* FPP_ntup.theta_DSP = Theta_carb_deg_dsp;
		 FPP_ntup.phi_DSP   = Phi_carb_deg_dsp; */
	      
	      FPP_ntup.x_FPP = conex_sphere;
	      FPP_ntup.y_FPP = coney_sphere;
	      /* FPP_ntup.x_DSP = conex_sphere_dsp;
		 FPP_ntup.y_DSP = coney_sphere_dsp; */
	    }
	}
    }  
}

/************************************************/
/* Calculate intercept with a straight line fit */
/************************************************/
static float Straight_line (Event_t * Pevent, int Wic, 
			    int Miss, int First, int Index,
			    float * PVariance, int *PCentroid)
{
  float  Sum_x, Sum_y, Sum_xy, Sum_xx;
  int    i, n, Centroid;
  int    Maximum;

  float  a, b;
  float  Difference;
  float  Intercept;
  float  Variance = 0;

  double data[100][2];
  int nofpoints;
  float maxdiff;
  int worst_point = -1;
  int pass;

#if defined (GRAPHS)
  static int g2d = -1;
  static int red;
  static int black;
  static int green;
  double pldata[100][2];
  static int good_events = 0;
  static int bad_events = 0;
  int left, right;
  double dummy;
  char text[5]; 
#endif 

  Sum_x    = 0.;
  Sum_y    = 0.;
  Sum_xy   = 0.;
  Sum_xx   = 0.;
  Centroid = 0;
  Maximum  = 0;
  a        = 0.;
  b        = 0.;

  for(n=First;(n-First)<(Wic+Miss);n++)
    {
      if (Pevent->VDC_time[n][Index][0]>Maximum)
	{
	  Maximum = Pevent->VDC_time[n][Index][0];
	  Centroid = n;
	}  
    }

  nofpoints = 0;

  for(n = First; (n-First) < (Wic+Miss); n++)
    {
      if (Pevent->VDC_time[n][Index][0] < 0) continue;

      if (n < Centroid)
	data[nofpoints][1] = VDC_T0[Index] - Pevent->VDC_time[n][Index][0];
      
      if (n > Centroid)
	data[nofpoints][1] = Pevent->VDC_time[n][Index][0] - VDC_T0[Index];
      
      if (n == Centroid)
	{
	  if ( Pevent->VDC_time[n-1][Index][0] >
	       Pevent->VDC_time[n+1][Index][0] )
	    {
	      data[nofpoints][1] = Pevent->VDC_time[n][Index][0] 
		- VDC_T0[Index];
	    }
	  else
	    {
	      data[nofpoints][1] = VDC_T0[Index] 
		- Pevent->VDC_time[n][Index][0];
	    }
	}
      data[nofpoints][0] = n;
      nofpoints++;
    }

  pass = 0;

  while ( (Variance > REPAIR_VAR && nofpoints > 3 && pass <= NOF_PASSES) ||
	  (pass == 0) )
    {
      if ( pass > 0 )
	{
	  i = 0;
	  for(n = 0; n < nofpoints; n++)
	    {
	      if (n == worst_point) continue;
	      data[i][0] = data[n][0];
	      data[i][1] = data[n][1];
	      i++;
	    }
	  nofpoints--;
	}
      
      pass++;
      
      Sum_x  = 0.;
      Sum_y  = 0.;
      Sum_xy = 0.;
      Sum_xx = 0.;
      
      for (n = 0; n < nofpoints; n++)
	{
	  Sum_x  += data[n][0];
	  Sum_y  += data[n][1];
	  Sum_xy += data[n][0] * data[n][1];
	  Sum_xx += data[n][0] * data[n][0];
	}
      
      a = (nofpoints * Sum_xy - Sum_x * Sum_y) 
	/ (nofpoints * Sum_xx - Sum_x * Sum_x);
      b = (Sum_xx * Sum_y - Sum_x * Sum_xy)
	/ (nofpoints * Sum_xx - Sum_x * Sum_x);
      
      Variance = 0;
      maxdiff = 0;
      worst_point = -1;

      for(n = 0; n < nofpoints; n++)
	{
	  Difference = abs( a * data[n][0] + b - data[n][1] );
	  Variance += Difference*Difference;
	  if (Difference > maxdiff)
	    {
	      maxdiff = Difference;
	      worst_point = n;
	    }
	}
      Variance = sqrt(Variance)/(nofpoints-2);
    }
  
  if (a == 0.) 
    Intercept = 999.;
  else
    Intercept = -b/a;  
  
  *PVariance = Variance;
  *PCentroid = Centroid;

#if defined (GRAPHS)
  if ( Variance <= Variance_limit[1][Index] )
    {
      good_events++;
    }
  else
    {
      bad_events++;

      left = ( (int) (First/5) ) * 5;
      right = ( (int) ((First+Wic+Miss)/5) ) * 5 + 5;

      for (n = 0; n < nofpoints; n++)
	{
	  pldata[n][0] = (data[n][0] - left) * 500/(right-left);
	  pldata[n][1] = data[n][1] + 400;
	} 

      if (g2d < 0) 
	{
	  g2d = g2_open_X11(500, 800);
	  black  = g2_ink(g2d,0,0,0);
	  red    = g2_ink(g2d,1,0,0);
	  green  = g2_ink(g2d,0,1,0);
	}

      g2_clear(g2d);
      g2_line(g2d, 0, 400, 500, 400);

      dummy = 1;
      g2_set_dash(g2d,1,&dummy);
      for (n = 1; n < (right-left); n++)
	g2_line(g2d, n*500/(right-left), 0, n*500/(right-left), 800);
      g2_set_dash(g2d,0,0);

      sprintf(text,"%d",left);
      g2_string(g2d, 0, 400, text);
      sprintf(text,"%d",right);
      g2_string(g2d, 480, 400, text);

      g2_poly_line(g2d, nofpoints, pldata);

      g2_pen(g2d,red);
      for (n = 0; n < nofpoints; n++)
	{
	  sprintf(text,"%d",n+1);
	  g2_string(g2d, pldata[n][0], pldata[n][1], text );
	}

      g2_pen(g2d,green);

      g2_line(g2d, 
	      pldata[0][0],
	      a * data[0][0] + b + 400, 
	      pldata[nofpoints-1][0],
	      a * data[nofpoints-1][0] + b + 400);

      g2_pen(g2d,black);
      sprintf(text,"Variance = %6.3f",Variance);
      g2_string(g2d, 350, 770, text);

      if (good_events > 0)
	{
	  sprintf(text,"bad events = %d", bad_events);
	  g2_string(g2d, 350, 750, text);
	  sprintf(text,"good events = %d",good_events);
	  g2_string(g2d, 350, 730, text);
	  sprintf(text,"bad / total events = %5.2f%%",
		  (float) bad_events/ (float) (bad_events+good_events) * 100.);
	  g2_string(g2d, 350, 710, text);
	}
      getchar(); 
    }
#endif

  return Intercept;
}


/***************************/
/* report on buffer errors */
/***************************/
int Report (uint16 * Buffer, int Size)
{
  int i;

  if (Size==0) UI_Msg("Empty buffer !\n");

  for (i=0;i<Size;i++)
    {
      /* if (i%6 == 0) printf("\n"); */
      UI_Msg("%3d : %4x  ",i,*(Buffer++));
    }

  return 0;
}


/*************************/
/* definition of n-tuple */
/*************************/
void open_ntuple(char * nt_name)
{
  int  isize = LRECL, istat=0;

  char fppblock[1024];

  HROPEN(40,"FPP_NTUPLE",nt_name,"N",isize,istat);
  HCDIR("//FPP_NTUPLE"," ");
  HBNT(1001,"FPP","D");
  
  if (Ntup_Block[0])
    {
      sprintf(fppblock,"%s%s%s",FPDSBLOCK1,FPDSBLOCK2,FPDSBLOCK3);
      HBNAME(1001,"FPDS",FPDSSTART,fppblock);
    }

  if (Ntup_Block[1])
    {
      sprintf(fppblock,"%s%s%s",FPPBLOCK1,FPPBLOCK2,FPPBLOCK3);
      HBNAME(1001,"FPP",FPPSTART,fppblock);
    }

  if (Ntup_Block[2])
    {
      sprintf(fppblock,"%s",PIDBLOCK1);
      HBNAME(1001,"PID",PIDSTART,fppblock);
    }

  if (Ntup_Block[3])
    {
      sprintf(fppblock,"%s",TARBLOCK1);
      HBNAME(1001,"TAR",TARSTART,fppblock);
    }

  UI_Msg("Ntuple created: name %s, record length LRECL=%d\n", nt_name, isize);
}


/*******************************************************/
/* routine that is called at the begin of a run        */
/*******************************************************/
int Online_ana_start()
{
  unsigned int ONE = 1;
  UI_CtxRead CtxRead = NULL;
  Boolean Clear=TRUEB;
  Boolean Dummy=FALSEB;
  const unsigned int FileNameLenMax = 256;
  char statfilename[256];

  /* evaluation of scaler stuff needs to know when a new run is started */
  NewRun = TRUEB;
  /* Data_valid = FALSEB; */ /* for the 1998 data the events before the first
			  scaler event have to be thrown away */
  Counter = 0;

  if (Ntuple)
    {
      if (CurrImpl == UI_ClientImpl)
	{
	  /* Ask for a name for the ntuple */
	  if (UI_StrSetupRead (&CtxRead, "Ntuple filename",
			       nt_name, NULL,
			       &ONE, &FileNameLenMax, NULL) < 0)
	    {
	      FatalError ("UI_SetupRead failed");
	    }
	  
	  switch (UI_Read("Analysis startup", &CtxRead, NULL, NULL))
	    {
	    case 0:
	      break;	 /* Normal */
	    case 1:
	      return -1; /* Cancelled */
	    default:
	      FatalError ("UI_Read failed");
	    }
	}

      strcpy(SysStat->out_file,nt_name);
      open_ntuple(nt_name); /* create ntuple file */

      strcpy (statfilename, nt_name);
      strcat (statfilename, ".stat");
      if ((statfile = fopen(statfilename,"w")) == NULL)
	FatalError("Could not open file to save statistics.");

      UI_Msg("Scaling region below %d mm in VDC-1-X down by a factor of %d",
	     Ana_par.x_scale_below, Ana_par.downscale_factor);

      UI_Msg("Processed events are:\n  - events which survived the event buffer unpacking routine\n  - were not thrown away because of downscaling part of the VDC-1 X spectrum\n  - are not scaler or other type of events");

      fprintf(statfile,
	      "Scaling region below %d mm in VDC-1-X down by a factor of %d\n",
		Ana_par.x_scale_below, Ana_par.downscale_factor);
      fprintf(statfile,"\nProcessed events are:\n");
      fprintf(statfile,"  - events which survived the event buffer ");
      fprintf(statfile,"unpacking routine\n");
      fprintf(statfile,"  - were not thrown away because of downscaling ");
      fprintf(statfile,"part of the VDC-1 X spectrum\n");
      fprintf(statfile,"  - are not scaler or other type of events\n\n\n");

    }
  else
    {
      /* Ask if spectra should be cleared */
      if (CurrImpl == UI_ClientImpl)
	{
	  if (UI_BooSetupRead (&CtxRead, "Clear spectra before run",
			       &Clear, NULL) < 0)
	    FatalError ("UI_SetupRead failed");

	  switch (UI_Read("Analysis startup", &CtxRead, NULL, NULL))
	    {
	    case 0:
	      break;	 /* Normal */
	    case 1:
	      return -1; /* Cancelled */
	    default:
	      FatalError ("UI_Read failed");
	    }
	  if (Clear) Clear_spectra (&Dummy);
	}
    }
  return 0;
}


/*****************************************************/
/* routine that is called at the end of a run        */
/*****************************************************/
int Online_ana_stop()
{
  int icycle;

  if (Use_VESNA)
    VESNA(0,0,0,0,3,NULL);

  if (File_FPP)
    if (FPP_file != NULL) fclose(FPP_file);

  if (Ntuple)
    {
      /* close ntuple on disk */
      HCDIR("//FPP_NTUPLE"," ");      
      HROUT(1001,icycle," ");
      HREND("FPP_NTUPLE");

      /* close statistics file */
      UI_Msg("Total processed events: %lu",Counter);
      if (Ntuple) fprintf(statfile,"Total processed events: %u\n",Counter);
      fclose(statfile);
    }
  
  return 0;
}


/************************************************************************/
/* read parameters for analysis and ntuple production from analysis.par */
/************************************************************************/
int analysis_getpar(Ana_par_t * Ana_par)
{
  FILE * ifp;
  int i, j;
  char vdc_planes[4][8]  = {"VDC-1-X", "VDC-1-U", "VDC-2-X", "VDC-2-U"};
  char mwpc_planes[8][9] = {"MWPC-1-X", "MWPC-1-Y", "MWPC-2-X", "MWPC-2-Y",
			    "MWPC-3-X", "MWPC-3-Y", "MWPC-4-X", "MWPC-4-Y"};

  char fname[256];

  UI_Msg("Getting analysis/ntuple parameters.");
  
  strcpy(fname,BASEDIR);
  strcat(fname,"/stadaq/Ana_parameters/");
  strcat(fname,Ana_Par_File);

  /* open parameter file */
  if ((ifp = fopen(fname,"r")) == NULL) 
    {
      UI_Msg("Could not open file %s. Using default values.",fname);

      strcpy (Ana_par->mwpc_parfile, "mwpc.par"); /* file containing the pcos 
						     logical addresses */
      strcpy (Ana_par->oas_parfile, "default"); /* file containing the online 
						     analysis settings */
      for (i=0; i<4; i++)
	Ana_par->VDC_no_ring[i]=0;   /* number of ringing wires per plane */

      for (i=0; i<8; i++)
	Ana_par->MWPC_no_ring[i]=0;  /* number of ringing wires per plane */
      
      Ana_par->no_scalers = 44;        /* number of scaler channels read out */
      Ana_par->current_scaler = 23;    /* scaler channel counting the current
					  integrator */
      Ana_par->pri_event_scaler = 13;  /* channel counting primary events */
      Ana_par->acq_event_scaler = 15;  /* channel counting acquired events */
      
      Ana_par->nt_max_events = 500000; /* max no. of events before a new 
					  ntuple is started */
      Ana_par->x_scale_below = 0;      /* downscale region of VDC1-X */
      Ana_par->downscale_factor = 1;

      return 0;
    }

  if (get_keyword(ifp,"MWPC_parfile") < 0) return -1;
  if (get_string (ifp, Ana_par->mwpc_parfile) < 0) return -1;

  if (get_keyword(ifp,"oas_parfile") < 0) return -1;
  if (get_string (ifp, Ana_par->oas_parfile) < 0) return -1;

  for (i=0; i<4; i++)
    {
      if (get_keyword(ifp,vdc_planes[i]) < 0) return -1;
      if (get_int(ifp, &Ana_par->VDC_no_ring[i],0,10) < 0) return -1;
  
      for (j=0; j<Ana_par->VDC_no_ring[i]; j++)
	if (get_int(ifp, &Ana_par->VDC_ringing[i][j],0,WIRES_VDC) < 0)
	  return -1;
    }

  for (i=0; i<8; i++)
    {
      if (get_keyword(ifp,mwpc_planes[i]) < 0) return -1;
      if (get_int(ifp, &Ana_par->MWPC_no_ring[i],0,10) < 0) return -1;
  
      for (j=0; j<Ana_par->MWPC_no_ring[i]; j++)
	if (get_int(ifp, &Ana_par->MWPC_ringing[i][j],0,WIRES_D4X) < 0)
	  return -1;
    }

  if (get_keyword(ifp,"No_of_scalers") < 0) return -1;
  if (get_int(ifp, &Ana_par->no_scalers,0,64) < 0) return -1;
  
  if (get_keyword(ifp,"Current_readout") < 0) return -1;
  if (get_int(ifp, &Ana_par->current_scaler,0,64) < 0) return -1;
  
  if (get_keyword(ifp,"Prim_events_readout") < 0) return -1;
  if (get_int(ifp, &Ana_par->pri_event_scaler,0,64) < 0) return -1;
  
  if (get_keyword(ifp,"Acq_event_readout") < 0) return -1;
  if (get_int(ifp, &Ana_par->acq_event_scaler,0,64) < 0) return -1;
  
  if (get_keyword(ifp,"Ntuple_max_events") < 0) return -1;
  if (get_long(ifp, &Ana_par->nt_max_events,0,1000000) < 0) return -1;
  
  if (get_keyword(ifp,"Scale_region_below") < 0) return -1;
  if (get_int(ifp, &Ana_par->x_scale_below,0,1500) < 0) return -1;

  if (get_keyword(ifp,"Downscale_factor") < 0) return -1;
  if (get_int(ifp, &Ana_par->downscale_factor,0,32000) < 0) return -1;
  
  fclose(ifp);

  return 0;
}


/****************************************************************/
/* read parameters for histogram definition from histograms.par */
/****************************************************************/
static int histo_getpar(Histopar_t * PHiPar)
{
  FILE * ifp;
  char fname[256];

  UI_Msg("Getting histogram parameters.");

  strcpy(fname,BASEDIR);
  strcat(fname,"/stadaq/Ana_parameters/histograms.par");
  
  /* open parameter file */
  if ((ifp = fopen(fname,"r")) == NULL) 
    {
      UI_Msg("Could not find histogram parameter file. Using default values.");

      strcpy(Mem_id,"FPP");

      PHiPar->MEM_Size   = 1000000;  /* shared memory size */

      PHiPar->Drift_Min  = 0;        /* mimimum drifttime */
      PHiPar->Drift_Max  = 512;      /* maximum drifttime */
      PHiPar->Drift_Bins = 128;      /* number of bins */

      PHiPar->VTOT_Min   = 0;        /* minimum VTOT */
      PHiPar->VTOT_Max   = 128;      /* maximum VTOT */
      PHiPar->VTOT_Bins  = 128;      /* number of bins */

      PHiPar->STOT_Min   = 0;        /* minimum STOT */
      PHiPar->STOT_Max   = 128;      /* maximum STOT */
      PHiPar->STOT_Bins  = 128;      /* number of bins */

      PHiPar->TOF_Min    = 0;        /* minimum TOF */
      PHiPar->TOF_Max    = 128;      /* maximum TOF */
      PHiPar->TOF_Bins   = 128;      /* number of bins */

      PHiPar->Int_Bins   = 2400;     /* bins in intercept */

      PHiPar->X_Min      = 0;        /* minimum x */
      PHiPar->X_Max      = 1024;     /* maximum x */
      PHiPar->X_Bins     = 1024;     /* number of bins */

      PHiPar->Y_Min      = -192;     /* minimum y */
      PHiPar->Y_Max      = 192;      /* maximum y */
      PHiPar->Y_Bins     = 384;      /* number of bins */

      return 0;
    }

  if (get_keyword(ifp,"MEM_ID") < 0) return -1;
  if (get_string (ifp, Mem_id) < 0) return -1;

  if (get_keyword(ifp,"MEM_Size") < 0) return -1;
  if (get_int(ifp,&PHiPar->MEM_Size,10000,1000000) < 0) return -1;
  
  if (get_keyword(ifp,"Drift_Min") < 0) return -1;
  if (get_int(ifp,&PHiPar->Drift_Min,0,2048) < 0) return -1;
  
  if (get_keyword(ifp,"Drift_Max") < 0) return -1;
  if (get_int(ifp,&PHiPar->Drift_Max,0,2048) < 0) return -1;
  
  if (get_keyword(ifp,"Drift_Bins") < 0) return -1;
  if (get_int(ifp,&PHiPar->Drift_Bins,0,1024) < 0) return -1;
  
  if (get_keyword(ifp,"VTOT_Min") < 0) return -1;
  if (get_int(ifp,&PHiPar->VTOT_Min,-512,512) < 0) return -1;
  
  if (get_keyword(ifp,"VTOT_Max") < 0) return -1;
  if (get_int(ifp,&PHiPar->VTOT_Max,-512,512) < 0) return -1;

  if (get_keyword(ifp,"VTOT_Bins") < 0) return -1;
  if (get_int(ifp,&PHiPar->VTOT_Bins,0,1024) < 0) return -1;
  
  if (get_keyword(ifp,"STOT_Min") < 0) return -1;
  if (get_int(ifp,&PHiPar->STOT_Min,-512,512) < 0) return -1;
  
  if (get_keyword(ifp,"STOT_Max") < 0) return -1;
  if (get_int(ifp,&PHiPar->STOT_Max,-512,512) < 0) return -1;
  
  if (get_keyword(ifp,"STOT_Bins") < 0) return -1;
  if (get_int(ifp,&PHiPar->STOT_Bins,0,1024) < 0) return -1;
  
  if (get_keyword(ifp,"TOF_Min") < 0) return -1;
  if (get_int(ifp,&PHiPar->TOF_Min,-2048,2048) < 0) return -1;
  
  if (get_keyword(ifp,"TOF_Max") < 0) return -1;
  if (get_int(ifp,&PHiPar->TOF_Max,-2048,2048) < 0) return -1;
  
  if (get_keyword(ifp,"TOF_Bins") < 0) return -1;
  if (get_int(ifp,&PHiPar->TOF_Bins,0,1028) < 0) return -1;
  
  if (get_keyword(ifp,"Int_Bins") < 0) return -1;
  if (get_int(ifp,&PHiPar->Int_Bins,0,100000) < 0) return -1;
  
  if (get_keyword(ifp,"X_Min") < 0) return -1;
  if (get_int(ifp,&PHiPar->X_Min,-128,1024) < 0) return -1;
  
  if (get_keyword(ifp,"X_Max") < 0) return -1;
  if (get_int(ifp,&PHiPar->X_Max,-128,1024) < 0) return -1;
  
  if (get_keyword(ifp,"X_Bins") < 0) return -1;
  if (get_int(ifp,&PHiPar->X_Bins,0,2048) < 0) return -1;
  
  if (get_keyword(ifp,"Y_Min") < 0) return -1;
  if (get_int(ifp,&PHiPar->Y_Min,-256,256) < 0) return -1;
  
  if (get_keyword(ifp,"Y_Max") < 0) return -1;
  if (get_int(ifp,&PHiPar->Y_Max,-256,256) < 0) return -1;
  
  if (get_keyword(ifp,"Y_Bins") < 0) return -1;
  if (get_int(ifp,&PHiPar->Y_Bins,0,1024) < 0) return -1;
  
  fclose(ifp);

  return 0;
}


/***************************************************/
/* read parameters for mwpc decoding from mwpc.par */
/***************************************************/
int mwpc_getpar()
{
  FILE * ifp;
  int  i,j,k;
  int  nr_of_mod = 0;
  int  address;
  int  slot;
  char fname[256];

  UI_Msg("Getting MWPC parameters.");

  /* initializing MWPC_Par structure */
  for (i=0;i<2;i++)
    {
      for (j=0;j<512;j++)
	{
	  for (k=0;k<2;k++)
	    {
	      MWPC_Par[i][j][k].mwpc  = -1;
	      MWPC_Par[i][j][k].plane = -1;
	      MWPC_Par[i][j][k].card  = -1;
	    }
	}
    }

  strcpy(fname,BASEDIR);
  strcat(fname,"/stadaq/Ana_parameters/mwpc.par");
  strcat(fname,Ana_par.mwpc_parfile);

  /* open parameter file */
  if ((ifp = fopen(fname,"r")) == NULL) 
    {
      UI_MsgOk("Could not find MWPC parameter file.");
      return -1;
    }

  for(j=0;j<2;j++)
    {
      switch(j)
	{
	case 0:
	  if (get_keyword(ifp,"Branch_1") < 0)
	    {
	      fclose(ifp);
	      return -1;
	    }
	  break;
	case 1:
	  if (get_keyword(ifp,"Branch_2") < 0)
	    {
	      fclose(ifp);
	      return -1;
	    }
	  break;
	}

      if (get_keyword(ifp,"Nr_of_modules")<0)
	{
	  fclose(ifp);
	  return -1;
	}
      
      if (get_int(ifp,&nr_of_mod,0,46) < 0)
	{
	  fclose(ifp);
	  return -1;
	}
      
      if (get_keyword(ifp,"#card")<0) 
	{ 
	  fclose(ifp); 
	  return -1; 
	} 

      for(i=0;i<nr_of_mod;i++)
	{
	  for(k=0;k<2;k++)
	    {
	      if (get_int(ifp,&address,0,511) < 0)
		{
		  fclose(ifp);
		  return -1;
		}

	      if (get_int(ifp,&slot,-1,1) < 0)
		{
		  fclose(ifp);
		  return -1;
		}
	      
	      if (slot==-1) continue;
 	  
	      if (MWPC_Par[j][address][slot].mwpc != -1)
		{
		  fclose(ifp);
		  return -1;
		}
	      
	      if (get_int(ifp,&MWPC_Par[j][address][slot].mwpc,0,3) < 0)
		{
		  fclose(ifp);
		  return -1;
		}
	      
	      if (get_int(ifp,&MWPC_Par[j][address][slot].plane,0,1) < 0)
		{
		  fclose(ifp);
		  return -1;
		}
	      
	      if (get_int(ifp,&MWPC_Par[j][address][slot].card,0,32) < 0)
		{
		  fclose(ifp);
		  return -1;
		}	  
	    }
	}
    }
  
  fclose(ifp);
  
  return 0;
}


/***************************************************/
/* read parameters for vdc decoding from vdc.par  */
/***************************************************/  
int vdc_getpar()
{
  FILE * ifp;
  char fname[256];
  int  i;

  UI_Msg("Getting VDC parameters.");

  for (i=0;i<4;i++) VDC_Par[i]=0;

  strcpy(fname,BASEDIR);
  strcat(fname,"/stadaq/Ana_parameters/vdc.par");

  /* open parameter file */
  if ((ifp = fopen(fname,"r")) == NULL) 
    {
      UI_MsgOk("Could not find VDC parameter file.");
      return -1;
    }

  if (get_keyword(ifp,"x1") < 0)
    {
      fclose(ifp);
      return -1;
    }
  
  if (get_int(ifp,&VDC_Par[0],0,15) < 0)
    {
      fclose(ifp);
      return -1;
    }      
  
  if (get_keyword(ifp,"u1") < 0)
    {
      fclose(ifp);
      return -1;
    }
  
  if (get_int(ifp,&VDC_Par[1],0,15) < 0)
    {
      fclose(ifp);
      return -1;
    }      
  
  if (get_keyword(ifp,"x2") < 0)
    {
      fclose(ifp);
      return -1;
    }
  
  if (get_int(ifp,&VDC_Par[2],0,15) < 0)
    {
      fclose(ifp);
      return -1;
    }      
  
  if (get_keyword(ifp,"u2") < 0)
    {
      fclose(ifp);
      return -1;
    }
  
  if (get_int(ifp,&VDC_Par[3],0,15) < 0)
    {
      fclose(ifp);
      return -1;
    }      
  
  fclose(ifp);
  return 0;
}



/***************************************/
/* input and output of  analysis setup */
/***************************************/

int load_analysis_setup (char * filename)
{
  int  status = 0;
  int  bool;
  int  i,j;
  FILE * ifp;
  char fname[255];

  strcpy(fname,BASEDIR);
  strcat(fname,"/stadaq/Ana_parameters/");
  strcat(fname,filename);
  strcat(fname,".oas");

  if ((ifp=fopen(fname,"r"))==NULL)
    {
      UI_MsgOk("Could not find the file named %s.oas.",filename);
      return -1;
    }
    
  if (get_keyword(ifp,"Correct_D4") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Correct_D4 = FALSEB; else Correct_D4 = TRUEB;

  if (get_keyword(ifp,"Correct_missing") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Correct_missing = FALSEB; else Correct_missing = TRUEB;

  if (get_keyword(ifp,"Correct_ringing") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Correct_ringing = FALSEB; else Correct_ringing = TRUEB;

  if (get_keyword(ifp,"Correct_multiple") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Correct_multiple = FALSEB; else Correct_multiple = TRUEB;

  if (get_keyword(ifp,"QDC_in") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) QDC_in = FALSEB; else QDC_in = TRUEB;

  if (get_keyword(ifp,"Select_pol") < 0) status--;
  for (i=0;i<4;i++) 
    {
      if (get_int(ifp,&bool,0,1) < 0) status--;
      if (bool==0) Select_pol[i] = FALSEB; else Select_pol[i] = TRUEB;
    }

  if (get_keyword(ifp,"Select_type") < 0) status--;
  for (i=0;i<20;i++) 
    {
      if (get_int(ifp,&bool,0,1) < 0) status--;
      if (bool==0) Select_type[i] = FALSEB; else Select_type[i] = TRUEB;
    }

  if (get_keyword(ifp,"Gate_TOF") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_TOF = FALSEB; else Gate_TOF = TRUEB;

  if (get_keyword(ifp,"Gate_variance") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_variance = FALSEB; else Gate_variance = TRUEB;
 
  if (get_keyword(ifp,"Gate_X1") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_X1 = FALSEB; else Gate_X1 = TRUEB;

  if (get_keyword(ifp,"Gate_Theta") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_Theta = FALSEB; else Gate_Theta = TRUEB;

  if (get_keyword(ifp,"Correct_DSP") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Correct_DSP = FALSEB; else Correct_DSP = TRUEB;

  if (get_keyword(ifp,"Gate_cos_theta_carb") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_cos_theta_carb = FALSEB; else Gate_cos_theta_carb = TRUEB;

  if (get_keyword(ifp,"Gate_vertz") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_vertz = FALSEB; else Gate_vertz = TRUEB;

  if (get_keyword(ifp,"VESNA_handles_VDCs") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Use_VESNA = FALSEB; else Use_VESNA = TRUEB;

  if (get_keyword(ifp,"VDC_T0") < 0) status--;
  for(i=0;i<4;i++)
    if (get_int(ifp,&VDC_T0[i],0,1024) < 0) status--;

  if (get_keyword(ifp,"TOF_limit") < 0) status--;
  for(i=0;i<2;i++)
    if (get_int(ifp,&TOF_limit[i],-256,512) < 0) status--;

  if (get_keyword(ifp,"Variance_limit") < 0) status--;
  for(j=0;j<4;j++)
    for(i=0;i<2;i++)
      if (get_float(ifp,&Variance_limit[i][j],0.,100.) < 0) status--;

  if (get_keyword(ifp,"MaxClusterSize") < 0) status--;
  if (get_int(ifp,&MaxClusterSize,0,100) < 0) status--;

  if (get_keyword(ifp,"X1_limit") < 0) status--;
    for(i=0;i<2;i++)
      if (get_float(ifp,&X1_limit[i],0.,1024.) < 0) status--;

  if (get_keyword(ifp,"Theta_limit") < 0) status--;
    for(i=0;i<2;i++)
      if (get_float(ifp,&Theta_limit[i],-2.,2.) < 0) status--;

  if (get_keyword(ifp,"cos_theta_carb_limit") < 0) status--;
    for(i=0;i<2;i++)
      if (get_float(ifp,&cos_theta_carb_limit[i],-1.,1.) < 0) status--;

  if (get_keyword(ifp,"vertz_limit") < 0) status--;
    for(i=0;i<2;i++)
      if (get_float(ifp,&vertz_limit[i],-1000.,2000.) < 0) status--;


  if (get_keyword(ifp,"VDC0x") < 0) status--;
  if (get_float(ifp,&VDC0x,-400.,-200.) < 0) status--;

  if (get_keyword(ifp,"VDC0y") < 0) status--;
  if (get_float(ifp,&VDC0y,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"VDC0z") < 0) status--;
  if (get_float(ifp,&VDC0z,-600.,-400.) < 0) status--;

  if (get_keyword(ifp,"CAz_offset") < 0) status--;
  if (get_float(ifp,&CAz_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D2x_offset") < 0) status--;
  if (get_float(ifp,&D2x_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D2y_offset") < 0) status--;
  if (get_float(ifp,&D2y_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D2z_offset") < 0) status--;
  if (get_float(ifp,&D2z_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D3x_offset") < 0) status--;
  if (get_float(ifp,&D3x_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D3y_offset") < 0) status--;
  if (get_float(ifp,&D3y_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D3z_offset") < 0) status--;
  if (get_float(ifp,&D3z_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D4x_offset") < 0) status--;
  if (get_float(ifp,&D4x_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D4y_offset") < 0) status--;
  if (get_float(ifp,&D4y_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"D4z_offset") < 0) status--;
  if (get_float(ifp,&D4z_offset,-100.,100.) < 0) status--;

  if (get_keyword(ifp,"Phi_offset") < 0) status--;
  if (get_float(ifp,&Phi_offset,-1.,1.) < 0) status--;

  if (get_keyword(ifp,"cr_angle_offset") < 0) status--;
  if (get_float(ifp,&cr_angle_offset,-1.,1.) < 0) status--;


  if (get_keyword(ifp,"Ntup_Block") < 0) status--;
  for (i=0;i<4;i++) 
    {
      if (get_int(ifp,&bool,0,1) < 0) status--;
      if (bool==0) Ntup_Block[i] = FALSEB; else Ntup_Block[i] = TRUEB;
    }

  if (get_keyword(ifp,"VFP_distance") < 0) status--;
  if (get_float(ifp,&vfp_L,-1000.,1000.) < 0) status--;

  if (get_keyword(ifp,"VFP_angle") < 0) status--;
  if (get_float(ifp,&vfp_psi,-180.,180.) < 0) status--;
  
  
	
  if (get_keyword(ifp,"Gate_BBS") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_BBS = FALSEB; else Gate_BBS = TRUEB;

  if (get_keyword(ifp,"BBS_angle") < 0) status--;
  if (get_float(ifp,&BBS_angle,-10.,60.) < 0) status--;

  if (get_keyword(ifp,"Gate_scat_1") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_scat_1 = FALSEB; else Gate_scat_1 = TRUEB;

  if (get_keyword(ifp,"Gate_scat_2") < 0) status--;
  if (get_int(ifp,&bool,0,1) < 0) status--;
  if (bool==0) Gate_scat_2 = FALSEB; else Gate_scat_2 = TRUEB;

  if (get_keyword(ifp,"Scat_limit_1") < 0) status--;
  for(i=0;i<2;i++)
    if (get_float(ifp,&Scat_limit_1[i],-100.,100.) < 0) status--;

  if (get_keyword(ifp,"Scat_limit_2") < 0) status--;
  for(i=0;i<2;i++)
    if (get_float(ifp,&Scat_limit_2[i],-100.,100.) < 0) status--;

		
  fclose(ifp);
  return status;
}


static int save_analysis_setup (char * filename)
{
  int  i,j;
  FILE * ifp;
  char fullname[350];

  strcpy(fullname,BASEDIR);
  strcat(fullname,"/stadaq/Ana_parameters/");
  strcat(fullname,filename);
  strcat(fullname,".oas");

  if ((ifp=fopen(fullname,"w"))==NULL)
    {
      UI_MsgOk("Could not open the file named %s.oas.",filename);
      return -1;
    }
    
  if (fprintf(ifp,"/* Online analysis version %s */\n",VERSION) < 0) return -1;
  if (fprintf(ifp,"/* Last update on %s */\n\n",UPDATE) < 0) return -1;
  
  if (fprintf(ifp,"%-20s","Correct_D4") < 0) return -1;
  if (Correct_D4 == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","Correct_missing") < 0) return -1;
  if (Correct_missing == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","Correct_ringing") < 0) return -1;
  if (Correct_ringing == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","Correct_multiple") < 0) return -1;
  if (Correct_multiple == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","QDC_in") < 0) return -1;
  if (QDC_in == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }


  if (fprintf(ifp,"%-20s","Select_pol") < 0) return -1;
  for(i=0;i<4;i++)
    {
      if (Select_pol[i] == FALSEB) 
	{
	  if (fprintf(ifp,"\t0")<0) return -1;
	}
      else 
	{
	  if (fprintf(ifp,"\t1")<0) return -1;
	}
    }
  if (fprintf(ifp,"\n") < 0) return -1;
 
  if (fprintf(ifp,"%-20s","Select_type") < 0) return -1;
  for(i=0;i<20;i++)
    {
      if (Select_type[i] == FALSEB)  
	{
	  if (fprintf(ifp,"\t0")<0) return -1;
	}
      else 
	{
	  if (fprintf(ifp,"\t1")<0) return -1;
	}
    }
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","Gate_TOF") < 0) return -1;
  if (Gate_TOF == FALSEB) 
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }
 
  if (fprintf(ifp,"%-20s","Gate_variance") < 0) return -1;
  if (Gate_variance == FALSEB) 
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }
 
  if (fprintf(ifp,"%-20s","Gate_X1") < 0) return -1;
  if (Gate_X1 == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","Gate_Theta") < 0) return -1;
  if (Gate_Theta == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","Correct_DSP") < 0) return -1;
  if (Correct_DSP == FALSEB) 
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }
 
  if (fprintf(ifp,"%-20s","Gate_cos_theta_carb") < 0) return -1;
  if (Gate_cos_theta_carb == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","Gate_vertz") < 0) return -1;
  if (Gate_vertz == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","VESNA_handles_VDCs") < 0) return -1;
  if (Use_VESNA == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","VDC_T0") < 0) return -1;
  for(i=0;i<4;i++)
    if (fprintf(ifp,"\t%d",VDC_T0[i]) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","TOF_limit") < 0) return -1;
  for(i=0;i<2;i++)
    if (fprintf(ifp,"\t%d",TOF_limit[i]) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","Variance_limit") < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;
  for(j=0;j<4;j++)
    {
      for(i=0;i<2;i++)
	if (fprintf(ifp,"\t%f",Variance_limit[i][j]) < 0) return -1;
      if (fprintf(ifp,"\n") < 0) return -1;
    }

  if (fprintf(ifp,"%-20s","MaxClusterSize") < 0) return -1;
  if (fprintf(ifp,"\t%d\n",MaxClusterSize) < 0) return -1;

  if (fprintf(ifp,"%-20s","X1_limit") < 0) return -1;
  for(i=0;i<2;i++)
    if (fprintf(ifp,"\t%f",X1_limit[i]) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;
  
  if (fprintf(ifp,"%-20s","Theta_limit") < 0) return -1;
  for(i=0;i<2;i++)
    if (fprintf(ifp,"\t%f",Theta_limit[i]) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","cos_theta_carb_limit") < 0) return -1;
  for(i=0;i<2;i++)
    if (fprintf(ifp,"\t%f",cos_theta_carb_limit[i]) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;
  
  if (fprintf(ifp,"%-20s","vertz_limit") < 0) return -1;
  for(i=0;i<2;i++)
    if (fprintf(ifp,"\t%f",vertz_limit[i]) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;
  


  if (fprintf(ifp,"%-20s","VDC0x") < 0) return -1;
  if (fprintf(ifp,"\t%f",VDC0x) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","VDC0y") < 0) return -1;
  if (fprintf(ifp,"\t%f",VDC0y) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","VDC0z") < 0) return -1;
  if (fprintf(ifp,"\t%f",VDC0z) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","CAz_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",CAz_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D2x_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D2x_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D2y_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D2y_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D2z_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D2z_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D3x_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D3x_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D3y_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D3y_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D3z_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D3z_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D4x_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D4x_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D4y_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D4y_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","D4z_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",D4z_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","Phi_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",Phi_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","cr_angle_offset") < 0) return -1;
  if (fprintf(ifp,"\t%f",cr_angle_offset) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","Ntup_Block") < 0) return -1;
  for(i=0;i<4;i++)
    {
      if (Ntup_Block[i] == FALSEB)  
	{
	  if (fprintf(ifp,"\t0")<0) return -1;
	}
      else 
	{
	  if (fprintf(ifp,"\t1")<0) return -1;
	}
    }
  if (fprintf(ifp,"\n") < 0) return -1;
  
  if (fprintf(ifp,"%-20s","VFP_distance") < 0) return -1;
  if (fprintf(ifp,"\t%f",vfp_L) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","VFP_angle") < 0) return -1;
  if (fprintf(ifp,"\t%f",vfp_psi) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  
  if (fprintf(ifp,"%-20s","Gate_BBS") < 0) return -1;
  if (Gate_BBS == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }

  if (fprintf(ifp,"%-20s","BBS_angle") < 0) return -1;
  if (fprintf(ifp,"\t%f",BBS_angle) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;
  
  if (fprintf(ifp,"%-20s","Gate_scat_1") < 0) return -1;
  if (Gate_scat_1 == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }
  
  if (fprintf(ifp,"%-20s","Gate_scat_2") < 0) return -1;
  if (Gate_scat_2 == FALSEB)  
    {
      if (fprintf(ifp,"\t0\n")<0) return -1;
    }
  else 
    {
      if (fprintf(ifp,"\t1\n")<0) return -1;
    }
  
  if (fprintf(ifp,"%-20s","Scat_limit_1") < 0) return -1;
  for(i=0;i<2;i++)
    if (fprintf(ifp,"\t%f",Scat_limit_1[i]) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;

  if (fprintf(ifp,"%-20s","Scat_limit_2") < 0) return -1;
  for(i=0;i<2;i++)
    if (fprintf(ifp,"\t%f",Scat_limit_2[i]) < 0) return -1;
  if (fprintf(ifp,"\n") < 0) return -1;


  fclose(ifp);
  return 0;
}


/**********************************/
/* Definition of  online commands */
/**********************************/
void Clear_spectra(Boolean * Running)
{
  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (Ntuple)
    {
      UI_MsgOk("Command not applicable to NTUPLEs.");
      return;
    }

  reset_histograms();

  return;
}

static void Redef_histo (Boolean * Running)
{
  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (Ntuple)
    {
      UI_MsgOk("Command not applicable to NTUPLEs.");
      return;
    }

  define_histograms();

  return;
}

static void Set_debug (Boolean * Running)
{
  int status;

  UI_CtxRead CtxRead = NULL;
  
  if (UI_BooSetupRead (&CtxRead, "Dump buffer errors to screen",
		       &Report_Buferr, NULL) < 0 ||
      UI_BooSetupRead (&CtxRead, "Write FPP data to file",
		       &File_FPP, NULL) < 0)
    FatalError ("UI_SetupRead failed");
  

  status = UI_Read("Analysis debug options", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  if (File_FPP)
    if ((FPP_file=fopen("fpp_data.dat","w"))==NULL)
      {
	UI_MsgOk("Couldn't open FPP data file");
	File_FPP = FALSEB;
      }

  return;
}

static void TOF_gate(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  int Default_Min=-20, Default_Max=100;
  int Min=-256, Max=512;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put gate on TOF",
		       &Gate_TOF, NULL) ||
      UI_IntSetupRead (&CtxRead, "Lower limit for TOF",
		       &TOF_limit[0], &Default_Min, &Min, &Max)  ||
      UI_IntSetupRead (&CtxRead, "Upper limit for TOF",
		       &TOF_limit[1], &Default_Max, &Min, &Max) < 0) {
    FatalError ("UI_SetupRead failed");
  }
  
  status = UI_Read("TOF gate adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}


static void Variance_gate(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_Min=0., Default_Max=10.;
  float Min=0., Max=100.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put gate on variance",
		       &Gate_variance, NULL) ||
      UI_FltSetupRead (&CtxRead, "Lower limit for X1-Variance",
		       &Variance_limit[0][0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for X1_Variance",
		       &Variance_limit[1][0], &Default_Max, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Lower limit for U1_Variance",
		       &Variance_limit[0][1], &Default_Max, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for U1_Variance",
		       &Variance_limit[1][1], &Default_Max, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Lower limit for X2_Variance",
		       &Variance_limit[0][2], &Default_Max, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for X2_Variance",
		       &Variance_limit[1][2], &Default_Max, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Lower limit for U2_Variance",
		       &Variance_limit[0][3], &Default_Max, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for U2_Variance",
		       &Variance_limit[1][3], &Default_Max, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Variance gate adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void ClusterSize(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  int Default=30;
  int Min=0, Max=100;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_IntSetupRead (&CtxRead, "Maximum clustersize",
		       &MaxClusterSize, &Default, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Maximum cluster size", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");

  return;
}


static void Theta_gate(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_Min=-1., Default_Max=1.;
  float Min=-2., Max=2.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put gate on theta",
		       &Gate_Theta, NULL) ||
      UI_FltSetupRead (&CtxRead, "Lower limit for theta",
		       &Theta_limit[0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for theta",
		       &Theta_limit[1], &Default_Max, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Theta gate adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void cos_theta_carb_gate(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_Min=-1., Default_Max=1.;
  float Min=-1., Max=1.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put gate on cosine secondary scattering angle",
		       &Gate_cos_theta_carb, NULL) ||
      UI_BooSetupRead (&CtxRead, "Correct DSP coordinates",
		       &Correct_DSP, NULL) ||
      UI_FltSetupRead (&CtxRead, "Lower limit for cosine secondary scattering angle",
		       &cos_theta_carb_limit[0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for cosine secondary scattering angle",
		       &cos_theta_carb_limit[1], &Default_Max, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Cosine secondary scattering angle gate adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void X1_gate(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_Min=0., Default_Max=1024.;
  float Min=0., Max=1024.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put gate on X1",
		       &Gate_X1, NULL) ||
      UI_FltSetupRead (&CtxRead, "Lower limit for X1",
		       &X1_limit[0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for X1",
		       &X1_limit[1], &Default_Max, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("X1 gate adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void vertz_gate(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_Min=-1000., Default_Max=2000.;
  float Min=-1000., Max=2000.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put gate on z vertex",
		       &Gate_vertz, NULL) ||
      UI_FltSetupRead (&CtxRead, "Lower limit for z",
		       &vertz_limit[0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for z",
		       &vertz_limit[1], &Default_Max, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("z vertex gate adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void target_gate(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_Min=-100., Default_Max=100.;
  float Min=-100., Max=100.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put gate on target coordinates",
		       &Gate_target, NULL) ||
      UI_FltSetupRead (&CtxRead, "Lower limit for theta target",
		       &Theta_target_limit[0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for theta target",
		       &Theta_target_limit[1], &Default_Max, &Min, &Max)  || 
      UI_FltSetupRead (&CtxRead, "Lower limit for phi target",
		       &Phi_target_limit[0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for phi target",
		       &Phi_target_limit[1], &Default_Max, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("target gate adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void BBS_gate(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default = 10.;
  float Min = -10., Max = 60.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Input BBS angle",
		       &Gate_BBS, NULL) ||
      UI_FltSetupRead (&CtxRead, "BBS angle",
		       &BBS_angle, &Default, &Min, &Max)  < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("BBS angle adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void Scat_gate_1(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_Min=-100., Default_Max=100.;
  float Min=-100., Max=100.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put first gate on real scattering angle ",
		       &Gate_scat_1, NULL) ||
      UI_FltSetupRead (&CtxRead, "Lower limit for scattering angle",
		       &Scat_limit_1[0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for scattering angle",
		       &Scat_limit_1[1], &Default_Max, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Scattering angle gate 1 adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void Scat_gate_2(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_Min=-100., Default_Max=100.;
  float Min=-100., Max=100.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_BooSetupRead (&CtxRead, "Put second gate on real scattering angle ",
		       &Gate_scat_2, NULL) ||
      UI_FltSetupRead (&CtxRead, "Lower limit for scattering angle",
		       &Scat_limit_2[0], &Default_Min, &Min, &Max)  ||
      UI_FltSetupRead (&CtxRead, "Upper limit for scattering angle",
		       &Scat_limit_2[1], &Default_Max, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Scattering angle gate 2 adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}


static void define_offsets(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float Default_VDC0x=-283.7;
  float Min_VDC0x=-400., Max_VDC0x=-200.;
  float Default_VDC0y=0.;
  float Min_VDC0y=-100., Max_VDC0y=100.;
  float Default_VDC0z=-500.6;
  float Min_VDC0z=-600, Max_VDC0z=-400.;
  float Default_D2x_offset=0.;
  float Min_CAz_offset=-100., Max_CAz_offset=100.;
  float Default_CAz_offset=0.;
  float Min_D2x_offset=-100., Max_D2x_offset=100.;
  float Default_D2y_offset=0.;
  float Min_D2y_offset=-100., Max_D2y_offset=100.;
  float Default_D2z_offset=0.;
  float Min_D2z_offset=-100., Max_D2z_offset=100.;
  float Default_D3x_offset=0.;
  float Min_D3x_offset=-100., Max_D3x_offset=100.;
  float Default_D3y_offset=0.;
  float Min_D3y_offset=-100., Max_D3y_offset=100.;
  float Default_D3z_offset=0.;
  float Min_D3z_offset=-100., Max_D3z_offset=100.;
  float Default_D4x_offset=0.;
  float Min_D4x_offset=-100., Max_D4x_offset=100.;
  float Default_D4y_offset=0.;
  float Min_D4y_offset=-100., Max_D4y_offset=100.;
  float Default_D4z_offset=0.;
  float Min_D4z_offset=-100., Max_D4z_offset=100.;
  float Default_Phi_offset=0.;
  float Min_Phi_offset=-1., Max_Phi_offset=1.;
  float Default_cr_angle_offset=0.;
  float Min_cr_angle_offset=-1., Max_cr_angle_offset=1.;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_FltSetupRead (&CtxRead, "VDC0x",
		       &VDC0x, &Default_VDC0x, &Min_VDC0x, &Max_VDC0x)  ||
      UI_FltSetupRead (&CtxRead, "VDC0y",
		       &VDC0y, &Default_VDC0y, &Min_VDC0y, &Max_VDC0y)  ||
      UI_FltSetupRead (&CtxRead, "VDC0z",
		       &VDC0z, &Default_VDC0z, &Min_VDC0z, &Max_VDC0z)  ||
      UI_FltSetupRead (&CtxRead, "CAz_offset",
		       &CAz_offset, &Default_CAz_offset, &Min_CAz_offset, &Max_CAz_offset)  ||
      UI_FltSetupRead (&CtxRead, "D2x_offset",
		       &D2x_offset, &Default_D2x_offset, &Min_D2x_offset, &Max_D2x_offset)  ||
      UI_FltSetupRead (&CtxRead, "D2y_offset",
		       &D2y_offset, &Default_D2y_offset, &Min_D2y_offset, &Max_D2y_offset)  ||
      UI_FltSetupRead (&CtxRead, "D2z_offset",
		       &D2z_offset, &Default_D2z_offset, &Min_D2z_offset, &Max_D2z_offset)  ||
      UI_FltSetupRead (&CtxRead, "D3x_offset",
		       &D3x_offset, &Default_D3x_offset, &Min_D3x_offset, &Max_D3x_offset)  ||
      UI_FltSetupRead (&CtxRead, "D3y_offset",
		       &D3y_offset, &Default_D3y_offset, &Min_D3y_offset, &Max_D3y_offset)  ||
      UI_FltSetupRead (&CtxRead, "D3z_offset",
		       &D3z_offset, &Default_D3z_offset, &Min_D3z_offset, &Max_D3z_offset)  ||
      UI_FltSetupRead (&CtxRead, "D4x_offset",
		       &D4x_offset, &Default_D4x_offset, &Min_D4x_offset, &Max_D4x_offset)  ||
      UI_FltSetupRead (&CtxRead, "D4y_offset",
		       &D4y_offset, &Default_D4y_offset, &Min_D4y_offset, &Max_D4y_offset)  ||
      UI_FltSetupRead (&CtxRead, "D4z_offset",
		       &D4z_offset, &Default_D4z_offset, &Min_D4z_offset, &Max_D4z_offset)  ||
      UI_FltSetupRead (&CtxRead, "Phi_offset",
		       &Phi_offset, &Default_Phi_offset, &Min_Phi_offset, &Max_Phi_offset)  ||
      UI_FltSetupRead (&CtxRead, "cr_angle_offset",
		       &cr_angle_offset, &Default_cr_angle_offset, &Min_cr_angle_offset, &Max_cr_angle_offset) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Offset setup", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void Change_zero(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  int Default=335, Min=0, Max=512;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if (UI_IntSetupRead (&CtxRead, "Zero time for plane VDC1-X",
		       &VDC_T0[0], &Default, &Min, &Max)  ||
      UI_IntSetupRead (&CtxRead, "Zero time for plane VDC1-U",
		       &VDC_T0[1], &Default, &Min, &Max)  ||
      UI_IntSetupRead (&CtxRead, "Zero time for plane VDC2-X",
		       &VDC_T0[2], &Default, &Min, &Max)  ||
      UI_IntSetupRead (&CtxRead, "Zero time for plane VDC2-U",
		       &VDC_T0[3], &Default, &Min, &Max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Zero time adjustment", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");

  if (Use_VESNA)
    {
      /* transfer zero times */
      VESNA_set_T0(VDC_T0[0], VDC_T0[1], VDC_T0[2], VDC_T0[3]);
    }
  
  return;
}

static void Calc_estim(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status = 0;
  float Default=0, Min=0, Max=50;
  float lower=0, upper=0;
  int i, j, lch, hch;
  float numcos=0., numsin = 0., den=0.;
  FILE * ifp;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  /* write data to file */

  if ((ifp=fopen("P_lower.dat","w"))==NULL) 
    UI_MsgOk("Can't open data file");
  else
    {
      for (i=3;i<16;i++)
	{
	  numcos = numsin = den = 0.;
	  for (j=5*i;j<=100;j++)
	    {
	      numcos += estim_cos[j];
	      numsin += estim_sin[j];
	      den += estim_den[j];
	    }
	  fprintf(ifp,"%2d\t%f\t%f\n",i,2*numcos/den,2*numsin/den);
	}
      fclose(ifp);
    }

  if ((ifp=fopen("P_upper.dat","w"))==NULL) 
    UI_MsgOk("Can't open data file");
  else
    {
      for (i=10;i<21;i++)
	{
	  numcos = numsin = den = 0.;
	  for (j=25;j<=5*i;j++)
	    {
	      numcos += estim_cos[j];
	      numsin += estim_sin[j];
	      den += estim_den[j];
	    }
	  fprintf(ifp,"%2d\t%f\t%f\n",i,2*numcos/den,2*numsin/den);
	}
      fclose(ifp);
    }

  while (status!=1) /* only stop when cancelled */
    {
      if (UI_FltSetupRead (&CtxRead, "Lower theta angle",
			   &lower, &Default, &Min, &Max)  ||
	  UI_FltSetupRead (&CtxRead, "Upper theta angle",
			   &upper, &Default, &Min, &Max)  < 0) 
	{
	  FatalError ("UI_SetupRead failed");
	}
      
      status = UI_Read("Calculate estimator", &CtxRead, NULL, NULL);
      if (status < 0) FatalError ("UI_Read failed");
      
      lch = 5 * lower;
      hch = 5 * upper;
      
      numcos = numsin = den = 0.;

      for (i=lch;i<=hch;i++)
	{
	  numcos += estim_cos[i];
	  numsin += estim_sin[i];
	  den += estim_den[i];
	}
      
      UI_Msg("Value for numerator cosine   = %f", numcos);
      UI_Msg("Value for numerator sine     = %f", numsin);
      UI_Msg("Value for denominator        = %f", den);
      if (den!=0)
	{
	  UI_Msg("Value for cos_estimator   = %f", 2*numcos/den);
	  UI_Msg("Value for sin_estimator   = %f", 2*numsin/den);
	}
    }
  
  return;
}

static void Save_oas (void)
{
  int status;
  UI_CtxRead CtxRead = NULL;
  unsigned int ONE = 1;
  const unsigned int FileNameLenMax = 256;

  if (UI_StrSetupRead (&CtxRead, "Setup filename",
		       Ana_par.oas_parfile, NULL,
		       &ONE, &FileNameLenMax, NULL) < 0)
    
      FatalError ("UI_SetupRead failed");
   
  
  status = UI_Read("Save online analysis setup", &CtxRead, NULL, NULL);
  if (status < 0) 
    {
      FatalError ("UI_Read failed");
      return;
    }

  if (save_analysis_setup(Ana_par.oas_parfile)<0)
    UI_MsgOk("Error while writing file %s.oas.",Ana_par.oas_parfile);
}

static void Load_oas (void)
{
  int status;
  UI_CtxRead CtxRead = NULL;
  unsigned int ONE = 1;
  const unsigned int FileNameLenMax = 256;

  if (UI_StrSetupRead (&CtxRead, "Setup filename",
		       Ana_par.oas_parfile, NULL,
		       &ONE, &FileNameLenMax, NULL) < 0)
    
      FatalError ("UI_SetupRead failed");
   
  
  status = UI_Read("Load online analysis setup file", &CtxRead, NULL, NULL);
  if (status < 0) 
    {
      FatalError ("UI_Read failed");
      return;
    }

  if (load_analysis_setup(Ana_par.oas_parfile) < 0)
    UI_MsgOk("Error while reading file %s.oas.",Ana_par.oas_parfile);
}

static void compose_ntuple (Boolean * Running)
{
  int status;
  UI_CtxRead CtxRead = NULL;
  
  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }

  if (UI_BooSetupRead (&CtxRead, "FPDS data",
		       &Ntup_Block[0], NULL) ||
      UI_BooSetupRead (&CtxRead, "FPP data",
		       &Ntup_Block[1], NULL) ||
      UI_BooSetupRead (&CtxRead, "PID data",
		       &Ntup_Block[2], NULL) ||
      UI_BooSetupRead (&CtxRead, "TAR data",
		       &Ntup_Block[3], NULL) < 0)
    FatalError ("UI_SetupRead failed");
  
  status = UI_Read("Compose ntuple", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");

  return;
}

static void type_gate (Boolean * Running)
{
  int status;
  UI_CtxRead CtxRead = NULL;
  
  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }

  if (UI_BooSetupRead (&CtxRead, "Select FPP events",
		       &Select_type[EVENT_FPP], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select ANGLE_CUT events",
		       &Select_type[EVENT_ANGLECUT], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select UNCHECKED events",
		       &Select_type[EVENT_UNCHECKED], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select PULSER events",
		       &Select_type[EVENT_PULSER], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select SCALER events",
		       &Select_type[EVENT_SCALER], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select MULTIPLE HIT events",
		       &Select_type[EVENT_MULT], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select REST events",
		       &Select_type[EVENT_REST], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select REPAIRED MULTIPLE events",
		       &Select_type[EVENT_MULT_REP], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select REPAIRED REST events",
		       &Select_type[EVENT_REST_REP], NULL) < 0)
    FatalError ("UI_SetupRead failed");
  
  status = UI_Read("Gate eventtype", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");

  return;
}

static void pol_gate (Boolean * Running)
{
  int status;
  UI_CtxRead CtxRead = NULL;
  
  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }

  if (UI_BooSetupRead (&CtxRead, "Select Polarization UP events",
		       &Select_pol[0], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select Polarization DOWN events",
		       &Select_pol[1], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select Polarization OFF events",
		       &Select_pol[2], NULL) ||
      UI_BooSetupRead (&CtxRead, "Select Polarization UNDEFINED events",
		       &Select_pol[3], NULL) < 0 )
    FatalError ("UI_SetupRead failed");
  
  status = UI_Read("Gate polarization", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");

  return;
}

static void QDC (Boolean * Running)
{
  int status;

  UI_CtxRead CtxRead = NULL;
  
  if (UI_BooSetupRead (&CtxRead, "QDC in read out",
		       &QDC_in, NULL) < 0)
    FatalError ("UI_SetupRead failed");
  
  status = UI_Read("Enable read out of QDC", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void VESNA_VDC_handling (Boolean * Running)
{
  int status;
  
  UI_CtxRead CtxRead = NULL;
  
  if (UI_BooSetupRead (&CtxRead, "Use VESNA routine to handle VDC data",
		       &Use_VESNA, NULL) < 0)
    FatalError ("UI_SetupRead failed");
  
  status = UI_Read("Enable VESNA VDC handling", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");

  if (Use_VESNA)
    {
      /* read VESNA configuration file */
      VESNA(0,0,0,0,0,NULL);
      
      /* transfer zero times */
      VESNA_set_T0(VDC_T0[0], VDC_T0[1], VDC_T0[2], VDC_T0[3]);
    }
  
  return;
}

static void Corrections (Boolean * Running)
{
  int status;

  UI_CtxRead CtxRead = NULL;
  
  if (UI_BooSetupRead (&CtxRead, "Correct for switched cards in D4",
		       &Correct_D4, NULL) ||
      UI_BooSetupRead (&CtxRead, "Correct for ringing wires",
		       &Correct_ringing, NULL) ||
      UI_BooSetupRead (&CtxRead, "Correct for missing wires",
		       &Correct_missing, NULL) ||
      UI_BooSetupRead (&CtxRead, "Correct for multiple hits",
		       &Correct_multiple, NULL) ||
      UI_BooSetupRead (&CtxRead, "Correct unchecked events",
		       &Correct_unchecked, NULL) < 0 )
    FatalError ("UI_SetupRead failed");
  
  status = UI_Read("Enable corrections", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}

static void vfp_params(Boolean * Running)
{
  UI_CtxRead CtxRead = NULL;
  int status;
  float L_min = -1000., L_max = 1000. ;
  float psi_min = -180., psi_max = 180. ;

  if (*Running)
    {
      UI_MsgOk("System must be in idle state !");
      return;
    }
  
  if  ( UI_FltSetupRead (&CtxRead, 
			 "Distance detector <-> virtual focal plane [mm]",
			 &vfp_L, NULL, &L_min, &L_max)  ||
	UI_FltSetupRead (&CtxRead, "Tilt angle virtual focal plane [deg.]",
			 &vfp_psi, NULL, &psi_min, &psi_max) < 0) 
    {
      FatalError ("UI_SetupRead failed");
    }
  
  status = UI_Read("Virtual focal plane params", &CtxRead, NULL, NULL);
  if (status < 0) FatalError ("UI_Read failed");
  
  return;
}


int Online_ana_command (Boolean Running)
{
  UI_CtxRead CtxRead = NULL;
  char title[50];
  int NrCmds = 26;
  unsigned int LastCmd = NrCmds - 1;
  const char * cmdnames[] = { "Load setup",
			      "Save setup",
			      "Clear spectra",
			      "Redefine histograms",

			      "Compose ntuple",
			      "Corrections",
			      "Debugging",
			      "QDC data",

			      "Change zero times",
			      "Define offsets",
			      "Maximum cluster size",
			      "Calculate estimator",

			      "Polarization gate",
			      "Event type gate",
			      "Variance gate",
			      "TOF gate",
				  

			      "Target gate", 
			      "Theta gate",
			      "X1 gate",
			      "Z vertex gate",
				  
				  "BBS angle",
			      "Scattering angle gate 1",
			      "Scattering angle gate 2",

			      "Cosine secondary gate",
			      "VESNA VDC handling",
				  "Virtual focal plane"};

  UI_CmdFunc * commands[] = { (UI_CmdFunc*)Load_oas,
			      (UI_CmdFunc*)Save_oas,
			      (UI_CmdFunc*)Clear_spectra,
			      (UI_CmdFunc*)Redef_histo,

			      (UI_CmdFunc*)compose_ntuple,
			      (UI_CmdFunc*)Corrections,
			      (UI_CmdFunc*)Set_debug,
			      (UI_CmdFunc*)QDC,

			      (UI_CmdFunc*)Change_zero,
			      (UI_CmdFunc*)define_offsets,
			      (UI_CmdFunc*)ClusterSize,
			      (UI_CmdFunc*)Calc_estim,

			      (UI_CmdFunc*)pol_gate,
			      (UI_CmdFunc*)type_gate,
			      (UI_CmdFunc*)Variance_gate,
			      (UI_CmdFunc*)TOF_gate,

			      (UI_CmdFunc*)target_gate,
			      (UI_CmdFunc*)Theta_gate,
			      (UI_CmdFunc*)X1_gate,
			      (UI_CmdFunc*)vertz_gate,
				  
				  (UI_CmdFunc*)BBS_gate,
			      (UI_CmdFunc*)Scat_gate_1,
			      (UI_CmdFunc*)Scat_gate_2,

			      (UI_CmdFunc*)cos_theta_carb_gate,
			      (UI_CmdFunc*)VESNA_VDC_handling,
				  (UI_CmdFunc*)vfp_params};

  if (UI_CmdSetupRead (&CtxRead, "Analysis commands",
		       &LastCmd, cmdnames,
		       commands, &Running) != 0)
    FatalError ("UI_SetupRead failure");
  
  strcpy(title,"Online analysis control v");
  strcat(title,VERSION);
  strcat(title," (");
  strcat(title,UPDATE);
  strcat(title,")");

  switch (UI_Read (title, &CtxRead, NULL, NULL)) 
    {
    case 0:
      break;	/* Normal */
    case 1:
      return 0;	/* Cancelled */
    default:
      FatalError ("UI_Read failed");
    }

  return 0;
}



/*********************/
/* clear all spectra */
/*********************/
static void reset_histograms(void)
{
  int i,j;
  
  for (i=0;i<250;i++)
      estim_cos[i]=estim_sin[i]=estim_den[i]=0.; 

  /* Clear VDC data */

  UI_Msg("Clearing histograms");

  HCDIR("\\\\\\\\"," ");

  HRESET(0," ");

  HCDIR("TARGET"," ");
  HRESET(0," ");
  HCDIR("\\"," ");

  for(i=0;i<2;i++)
    {

      switch(i) 
	{
	case 0:
	  HCDIR("VDC1"," ");
	  break;
	case 1:
	  HCDIR("VDC2"," ");
	  break;
	}

      for(j=0;j<2;j++)
	{
	  switch(j)
	    {
	    case 0:
	      HCDIR("X-PLANE"," ");
	      break;
	    case 1:
	      HCDIR("U-PLANE"," ");
	      break;
	    }
	  
	  HRESET(0," ");
	  
	  HCDIR("DRIFTTIMES"," ");
	  HRESET(0," ");
	  HCDIR("\\"," ");

	  HCDIR("\\"," ");
	}
      HCDIR("\\"," ");	  
    }
  
  HCDIR("S1"," ");
  HRESET(0," ");
  HCDIR("TOT SPECTRA"," ");
  HRESET(0," ");
  HCDIR("\\\\"," ");

  HCDIR("S2"," ");
  HRESET(0," ");
  HCDIR("TOT SPECTRA"," ");
  HRESET(0," ");
  HCDIR("\\\\"," ");

  HCDIR("QDC"," ");
  HRESET(0," ");
  HCDIR("\\"," ");

  return;
}



/*************************************/
/* (re-)definition of all histograms */
/*************************************/
int define_histograms(void)
{
  int i,j;
  char name1[4][32] = {"X1  Drifttime Wire xxx","U1  Drifttime Wire xxx",
		       "X2  Drifttime Wire xxx","U2  Drifttime Wire xxx"};
  char name2[4][32] = {"X1  TOT Wire xxx","U1  TOT Wire xxx",
		       "X2  TOT Wire xxx","U2  TOT Wire xxx"}; 

  Histopar_t HiPar;

  UI_Msg("Defining histograms");

  if (histo_getpar(&HiPar) < 0) 
    {
      UI_Msg("define_histograms: Error in histo_getpar");
      return -1;
    }
  
  UI_Msg("Shared memory ID = %s, size = %d",Mem_id, HiPar.MEM_Size);

  HLIMAP(HiPar.MEM_Size,Mem_id);


  /* top directory */

  HBOOK1(1,"Buffer error (FIFO number)",5,-0.5,4.5,0.);
  HBOOK1(3,"Time out (FIFO number)",5,-0.5,4.5,0.);
  HBOOK1(4,"Beam Polarization Value",10,-0.5,9.5,0.);

  HBOOK1(5,"Countrates (raw)",20,0.5,20.5,0.);
  HBOOK1(6,"Countrates (interpreted)",20,0.5,20.5,0.);

  HBOOK1(7,"Eventtypes (ungated)",14,-1.5,12.5,0.);
  HBOOK1(8,"Eventtypes (gated)",14,-1.5,12.5,0.);

  HBOOK1(11,"Time of flight (0.5 ns units)", 
	 HiPar.TOF_Bins, HiPar.TOF_Min, HiPar.TOF_Max,0.);  
  HBOOK1(12,"RF cyclotron (0.5 ns units)",200,0.,100.,0.);

  HBOOK1(21,"THETA (rad)",200,-0.4,0.4,0.);
  HBOOK1(22,"PHI (rad)",200,-0.4,0.4,0.);
  HBOOK1(23,"Y in X1-PLANE (mm)",800,-200,200,0.);
  HBOOK1(24,"X in X1-PLANE (mm)",1024,0,1024,0.);
  HBOOK2(25,"Y vs X IN X1-PLANE (mm)", HiPar.X_Bins, HiPar.X_Min,
	 HiPar.X_Max, HiPar.Y_Bins, HiPar.Y_Min, HiPar.Y_Max ,0.);
  HBOOK2(26,"THETA vs X IN X1-PLANE (rad vs mm)", HiPar.X_Bins,
	 HiPar.X_Min, HiPar.X_Max,200,-0.4,0.4,0.);

  /*  HBOOK2(27,"PHI vs Y IN X1-PLANE (rad vs mm)", HiPar.Y_Bins, 
	 HiPar.Y_Min, HiPar.Y_Max ,200,-0.4,0.4,0.); 
      HBOOK2(28,"Sum_S1 vs X IN X1-PLANE", 
	 HiPar.X_Bins, HiPar.X_Min, HiPar.X_Max, 250, 0., 1000., 0.);
      HBOOK2(29,"Sum_S2 vs X IN X1-PLANE",
	 HiPar.X_Bins, HiPar.X_Min, HiPar.X_Max, 250, 0., 1000., 0.);
  */

  HBOOK1(30,"X in virtual focal plane (mm)",1024,0,1024,0.);
  HBOOK2(31,"THETA vs X in virtual focal plane (rad vs mm)", HiPar.X_Bins,
	 HiPar.X_Min, HiPar.X_Max,200,-0.4,0.4,0.);
		  
  HBOOK2(101,"Sum all TOT S2 vs S1", HiPar.VTOT_Bins,2* HiPar.STOT_Min,
	 2* HiPar.STOT_Max, HiPar.STOT_Bins,
	 2* HiPar.STOT_Min,2* HiPar.STOT_Max,0.);

  HBOOK2(102,"RF vs X",250, 0.,1000.,50, -40.,20.,0.);
  HBOOK2(103,"SUM TOT S1 vs X", 125, 0., 1000., 125, 0., 1000.,0.); 

  /* only used for debugging  
    HBOOK1(201,"Check cosine theta 2",1000,.5,1,0.);
    HBOOK1(202,"Check theta 2",1000,0,50,0.);
  */

  /* target coordinates subdirectory */
  
  HMDIR("TARGET","S");
  HBOOK1(1,"theta target (mrad)", 200, -100, 100, 0.);
  HBOOK1(2,"phi target (mrad)", 200, -100, 100, 0.);
  HBOOK1(3,"momentum (MeV/c)", 1500, 500, 650, 0.);
  
  HBOOK1(4,"momentum (MeV/c) scattering angle gate 1", 1500, 540, 690, 0.);
  HBOOK1(5,"momentum (MeV/c) scattering angle gate 2", 1500, 540, 690, 0.);
  
  HBOOK2(11,"phi target (mrad) vs theta target (mrad)",100,-100,100,
         100,-100,100,0.);
  HBOOK2(12,"theta target (mrad) vs momentum (MeV/c)",150, 500, 650,
         100,-100,100,0.);
  HBOOK2(13,"phi target (mrad) vs momentum (MeV/c)",150, 500, 650,
         100,-100,100,0.);
  HBOOK1(21,"momentum (MeV/c) beam polarization up", 1500, 500, 650, 0.);
  HBOOK1(22,"momentum (MeV/c) beam polarization down", 1500, 500, 650, 0.);
  HBOOK1(23,"momentum (MeV/c) beam polarization off", 1500, 500, 650, 0.);
  HBOOK1(31,"momentum (MeV/c) with theta gate", 4500, 500, 650, 0.);
  HCDIR("\\"," ");
    
  HMDIR("MWPC1","S");
  HCDIR("\\"," ");
  HMDIR("MWPC2","S");
  HCDIR("\\"," ");
  HMDIR("MWPC3","S");
  HCDIR("\\"," ");
  HMDIR("MWPC4","S");
  HCDIR("\\"," ");
  
  /* vdc directories */

  HMDIR("VDC1","S");
  HMDIR("X-PLANE","S");
  HMDIR("DRIFTTIMES"," "); 
  HBOOK1(1,"ALL WIRES  X1",240,0.5,240.5,0.);
  HBOOK1(2,"MULTIPLICITY = 1  X1",240,0.5,240.5,0.);
  HBOOK1(3,"MULTIPLICITY = 2  X1",240,0.5,240.5,0.); 
  HBOOK1(4,"MULTIPLICITY = 3 OR MORE  X1",240,0.5,240.5,0.); 
  HBOOK1(5,"MISSING WIRES X1",240,0.5,240.5,0.);  
  HBOOK1(8,"CLUSTERSIZE X1",15,0.5,15.5,0.); 
  HBOOK1(9,"NUMBER OF CLUSTERS PER EVENT X1",5,0.5,5.5,0.); 
  HBOOK1(100,"Closest wire X1 (wire number)",240,0.5,240.5,0.);
  HBOOK1(200,"Crossing point by fitting  X1", HiPar.Int_Bins,0.5,240.5,0.);
  HBOOK1(300,"Variance for fitting  X1", 200,0,10,0.);
  HBOOK1(400,"Number of tracks X1", 10,-0.5,9.5,0.);
  HCDIR("\\"," ");

  HMDIR("U-PLANE","S");
  HMDIR("DRIFTTIMES"," ");
  HBOOK1(1,"ALL WIRES  U1",240,0.5,240.5,0.);
  HBOOK1(2,"MULTIPLICITY = 1  U1",240,0.5,240.5,0.);
  HBOOK1(3,"MULTIPLICITY = 2  U1",240,0.5,240.5,0.); 
  HBOOK1(4,"MULTIPLICITY = 3 OR MORE  U1",240,0.5,240.5,0.); 
  HBOOK1(5,"MISSING WIRES U1",240,0.5,240.5,0.);
  HBOOK1(8,"CLUSTERSIZE U1",15,0.5,15.5,0.); 
  HBOOK1(9,"NUMBER OF CLUSTERS PER EVENT U1",5,0.5,5.5,0.);

  HBOOK1(100,"Closest wire U1 (wire number)",240,0.5,240.5,0.);
  HBOOK1(200,"Crossing point by fitting  U1", HiPar.Int_Bins,0.5,240.5,0.);
  HBOOK1(300,"Variance for fitting  U1", 200,0,10,0.);
  HBOOK1(400,"Number of tracks U1", 10,-0.5,9.5,0.);
  HCDIR("\\\\"," ");

  HMDIR("VDC2","S");
  HMDIR("X-PLANE","S");
  HMDIR("DRIFTTIMES"," ");
  HBOOK1(1,"ALL WIRES  X2",240,0.5,240.5,0.);
  HBOOK1(2,"MULTIPLICITY = 1  X2",240,0.5,240.5,0.);
  HBOOK1(3,"MULTIPLICITY = 2  X2",240,0.5,240.5,0.); 
  HBOOK1(4,"MULTIPLICITY = 3 OR MORE  X2",240,0.5,240.5,0.); 
  HBOOK1(5,"MISSING WIRES X2",240,0.5,240.5,0.); 
  HBOOK1(8,"CLUSTERSIZE X2",15,0.5,15.5,0.);
  HBOOK1(9,"NUMBER OF CLUSTERS PER EVENT X2",5,0.5,5.5,0.);

  HBOOK1(100,"Closest wire X2 (wire number)",240,0.5,240.5,0.);
  HBOOK1(200,"Crossing point by fitting  X2", HiPar.Int_Bins,0.5,240.5,0.);
  HBOOK1(300,"Variance for fitting  X2", 200,0,10,0.);
  HBOOK1(400,"Number of tracks X2", 10,-0.5,9.5,0.);
  HCDIR("\\"," ");

  HMDIR("U-PLANE","S");
  HMDIR("DRIFTTIMES"," ");
  HBOOK1(1,"ALL WIRES  U2",240,0.5,240.5,0.);
  HBOOK1(2,"MULTIPLICITY = 1  U2",240,0.5,240.5,0.);
  HBOOK1(3,"MULTIPLICITY = 2  U2",240,0.5,240.5,0.); 
  HBOOK1(4,"MULTIPLICITY = 3 OR MORE  U2",240,0.5,240.5,0.); 
  HBOOK1(5,"MISSING WIRES U2",240,0.5,240.5,0.); 
  HBOOK1(8,"CLUSTERSIZE U2",15,0.5,15.5,0.);
  HBOOK1(9,"NUMBER OF CLUSTERS PER EVENT U2",5,0.5,5.5,0.);

  HBOOK1(100,"Closest wire U2 (wire number)",240,0.5,240.5,0.);
  HBOOK1(200,"Crossing point by fitting  U2", HiPar.Int_Bins,0.5,240.5,0.);
  HBOOK1(300,"Variance for fitting  U2", 200,0,10,0.);
  HBOOK1(400,"Number of tracks U2", 10,-0.5,9.5,0.);
  HCDIR("\\\\"," ");

  /* book drifttime histograms */
  
  for(i=0;i<4;i++)
    {
      switch(i) 
	{
	case 0: case 1: 
	  HCDIR("VDC1"," ");
	  break;
	case 2: case 3:
	  HCDIR("VDC2"," ");
	  break;
	}
      
      switch(i)
	{
	case 0: case 2:
	  HCDIR("X-PLANE"," ");
	  break;
	case 1: case 3:
	  HCDIR("U-PLANE"," ");
	  break;
	}
      
      HCDIR("DRIFTTIMES"," ");
      
      for(j=5;j<=WIRES_VDC;j+=10)
	{
	  name1[i][19] = 48+(j/100);
	  name1[i][20] = 48+(j-(j/100)*100)/10;
	  name1[i][21] = 48+(j-(j/10)*10);
	  name1[i][22] = '\0'; 
	  
	  HBOOK1(j,name1[i], 
		 HiPar.Drift_Bins, HiPar.Drift_Min, HiPar.Drift_Max,0.);  
	  
	  name2[i][13] = 48+(j/100);
	  name2[i][14] = 48+(j-(j/100)*100)/10; 
	  name2[i][15] = 48+(j-(j/10)*10); 
	  name2[i][16] = '\0'; 
	  
      	  /* HBOOK1(1000+j,name2[i],
	     HiPar.VTOT_Bins, HiPar.VTOT_Min, HiPar.VTOT_Max,0.); */
	  
	}
      
      HCDIR("\\\\\\"," ");
    }
  
    
  /* scintillator directories */
  
  HMDIR("S1","S");
  HMDIR("TOT SPECTRA","S");
  HBOOK1(1,"S1 TOT T1", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(2,"S1 TOT B1", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(3,"S1 TOT T2", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(4,"S1 TOT B2", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(5,"S1 TOT T3", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(6,"S1 TOT B3", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(7,"S1 TOT T4", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(8,"S1 TOT B4", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(9,"S1 TOT T5", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(10,"S1 TOT B5", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);


  HCDIR("\\"," ");
  HBOOK1(1,"S1 MULTIPLICITY (PM number)",10,0.5,10.5,0.);
  HBOOK1(2,"S1 LEADING EDGE MISSING (PM number)",10,0.5,10.5,0.);
  HBOOK1(3,"S1 TRAILING EDGE MISSING (PM number)",10,0.5,10.5,0.);
  HBOOK1(4,"SUM ALL TOT S1",512,0.5,2048.5,0.);
  HBOOK1(5,"NUMBER OF PHOTOMULTIPLIERS PER EVENT S1",11,-0.5,10.5,0.);
  HBOOK2(6,"Sum all TOT s1 vs D2x (wirenumber)",
	 112, 0.5, 448.5, 100, 0, 1000, 0.);
  HCDIR("\\"," ");


  HMDIR("S2","S");
  HMDIR("TOT SPECTRA","S");
  HBOOK1(1,"S2 TOT T1", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(2,"S2 TOT B1", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(3,"S2 TOT T2", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(4,"S2 TOT B2", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(5,"S2 TOT T3", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(6,"S2 TOT B3", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(7,"S2 TOT T4", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(8,"S2 TOT B4", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(9,"S2 TOT T5", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);
  HBOOK1(10,"S2 TOT B5", HiPar.STOT_Bins, HiPar.STOT_Min, HiPar.STOT_Max,0.);


  HCDIR("\\"," ");
  HBOOK1(1,"S2 MULTIPLICITY (PM number)",10,0.5,10.5,0.);
  HBOOK1(2,"S2 LEADING EDGE MISSING (PM number)",10,0.5,10.5,0.);
  HBOOK1(3,"S2 TRAILING EDGE MISSING (PM number)",10,0.5,10.5,0.);
  HBOOK1(4,"SUM ALL TOT S2",512,0.5,2048.5,0.);
  HBOOK1(5,"NUMBER OF PHOTOMULTIPLIERS PER EVENT S2",11,-0.5,10.5,0.);
  HBOOK2(6,"Sum all TOT s2 vs D4x (wire number)", 
	 128, 0.5, 512.5, 100, 0, 1000, 0.);
  HCDIR("\\"," ");

  return 0;
}



