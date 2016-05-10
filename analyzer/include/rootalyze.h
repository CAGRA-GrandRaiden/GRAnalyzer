const int multmax = 256;	/* max multiplicity of each variable (necessary for RCNPTREE declaration) */
const int var_subsets = 7; // number of subsets of variables (first index of variable_table)
const int var_GR = 35; // number of GR variables
const int var_LAS = 56; // number of LAS variables
const int var_BLP = 16; // number of BLP variables
const int var_QTC = 4; // number of QTC variables
const int var_MADC32 = 6; // number of MADC32 variables
const int var_SSDTDC = 4; // number of SSD variables
const int var_Others = 1; // number of Others variables

const int QTCchan = 16; // number of QTC chan
const int MADC32chan = 32; // number of MADC32 chan
const int TDCchan = 128; // number of SSD chan

/**************  ARRAYINDEX CLASS ****************/
/* Making table to "translate" variable index between analyzer and Root Tree*/
class ARRAYINDEX {
	public :
		int index_subset;
		int index_variable;
};

/**************  ROOTVARTABLE CLASS ****************/
/* Making table to list all Root Tree variables*/
class ROOTVARTABLE {
	public :
		char variable_name[var_subsets][56][32]; // second index is the larger var_* subset size
		int table_size[var_subsets];
		int table_var_GR;
		int table_var_LAS;
		int table_var_BLP;
		int table_var_QTC;
		int table_var_MADC32;
		int table_var_SSDTDC;
		int table_var_Others;
		
		void FillVarTable(ROOTVARTABLE &variable_table);
};

/* fill the analyzer-to-branches reference table */
/* Names reported here HAVE to be exactly the same as in the "hist.def" file */
void ROOTVARTABLE::FillVarTable(ROOTVARTABLE &variable_table)
{
	// index of variable subsets in the ROOTVARTABLE 2D member variable_name
	variable_table.table_var_GR = 0;
	variable_table.table_var_LAS = 1;
	variable_table.table_var_BLP = 2;
	variable_table.table_var_QTC = 3;
	variable_table.table_var_MADC32 = 4;
	variable_table.table_var_SSDTDC = 5;
	variable_table.table_var_Others = 6;

	variable_table.table_size[variable_table.table_var_GR]=var_GR;
	variable_table.table_size[variable_table.table_var_LAS]=var_LAS;
	variable_table.table_size[variable_table.table_var_BLP]=var_BLP;
	variable_table.table_size[variable_table.table_var_QTC]=var_QTC;
	variable_table.table_size[variable_table.table_var_MADC32]=var_MADC32;
	variable_table.table_size[variable_table.table_var_SSDTDC]=var_SSDTDC;
	variable_table.table_size[variable_table.table_var_Others]=var_Others;
	
	// GR             
	strcpy (variable_table.variable_name[variable_table.table_var_GR][0],"GR_RAYID");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][1],"GR_RF");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][2],"GR_X");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][3],"GR_Y");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][4],"GR_TH");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][5],"GR_PH");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][6],"GR_ADC1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][7],"GR_ADC2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][8],"GR_ADC3");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][9],"GR_ADC4");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][10],"GR_ADC5");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][11],"GR_ADC6");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][12],"GR_TDC1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][13],"GR_TDC2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][14],"GR_TDC3");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][15],"GR_TDC4");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][16],"GR_TDC5");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][17],"GR_TDC6");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][18],"GR_TPOS1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][19],"GR_TPOS2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][20],"GR_MADC1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][21],"GR_MADC2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][22],"GR_MADC3");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][23],"GR_WIRE_X1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][24],"GR_WIRE_U1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][25],"GR_WIRE_X2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][26],"GR_WIRE_U2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][27],"GR_WTDC_X1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][28],"GR_WTDC_U1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][29],"GR_WTDC_X2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][30],"GR_WTDC_U2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][31],"GR_TDCR_X1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][32],"GR_TDCR_U1");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][33],"GR_TDCR_X2");
	strcpy (variable_table.variable_name[variable_table.table_var_GR][34],"GR_TDCR_U2");
	// LAS                
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][0],"LAS_RAYID");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][1],"LAS_RF1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][2],"LAS_RF2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][3],"LAS_RF3");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][4],"LAS_X");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][5],"LAS_Y");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][6],"LAS_TH");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][7],"LAS_PH");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][8],"LAS_ADC1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][9],"LAS_ADC2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][10],"LAS_ADC3");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][11],"LAS_ADC4");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][12],"LAS_ADC5");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][13],"LAS_ADC6");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][14],"LAS_ADC7");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][15],"LAS_ADC8");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][16],"LAS_ADC9");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][17],"LAS_ADC10");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][18],"LAS_ADC11");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][19],"LAS_ADC12");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][20],"LAS_TDC1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][21],"LAS_TDC2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][22],"LAS_TDC3");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][23],"LAS_TDC4");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][24],"LAS_TDC5");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][25],"LAS_TDC6");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][26],"LAS_TDC7");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][27],"LAS_TDC8");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][28],"LAS_TDC9");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][29],"LAS_TDC10");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][30],"LAS_TDC11");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][31],"LAS_TDC12");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][32],"LAS_TPOS1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][33],"LAS_TPOS2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][34],"LAS_TPOS3");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][35],"LAS_TPOS4");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][36],"LAS_TPOS5");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][37],"LAS_TPOS6");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][38],"LAS_MADC1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][39],"LAS_MADC2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][40],"LAS_MADC3");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][41],"LAS_MADC4");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][42],"LAS_MADC5");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][43],"LAS_MADC6");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][44],"LAS_WIRE_X1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][45],"LAS_WIRE_U1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][46],"LAS_WIRE_V1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][47],"LAS_WIRE_X2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][48],"LAS_WIRE_U2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][49],"LAS_WIRE_V2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][50],"LAS_TDCR_X1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][51],"LAS_TDCR_U1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][52],"LAS_TDCR_V1");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][53],"LAS_TDCR_X2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][54],"LAS_TDCR_U2");
	strcpy (variable_table.variable_name[variable_table.table_var_LAS][55],"LAS_TDCR_V2");
	// BLP               
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][0],"BLP_ADC1");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][1],"BLP_ADC2");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][2],"BLP_ADC3");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][3],"BLP_ADC4");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][4],"BLP_ADC5");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][5],"BLP_ADC6");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][6],"BLP_ADC7");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][7],"BLP_ADC8");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][8],"BLP_TDC1");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][9],"BLP_TDC2");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][10],"BLP_TDC3");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][11],"BLP_TDC4");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][12],"BLP_TDC5");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][13],"BLP_TDC6");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][14],"BLP_TDC7");
	strcpy (variable_table.variable_name[variable_table.table_var_BLP][15],"BLP_TDC8");
	// QTCs             
	strcpy (variable_table.variable_name[variable_table.table_var_QTC][0],"QTC_TRAILING_CH");
	strcpy (variable_table.variable_name[variable_table.table_var_QTC][1],"QTC_TRAILING_TDC");
	strcpy (variable_table.variable_name[variable_table.table_var_QTC][2],"QTC_LEADING_CH");
	strcpy (variable_table.variable_name[variable_table.table_var_QTC][3],"QTC_LEADING_TDC");
	// MADC32          
	strcpy (variable_table.variable_name[variable_table.table_var_MADC32][0],"MADC32_0_CH");
	strcpy (variable_table.variable_name[variable_table.table_var_MADC32][1],"MADC32_0_ADC");
	strcpy (variable_table.variable_name[variable_table.table_var_MADC32][2],"MADC32_1_CH");
	strcpy (variable_table.variable_name[variable_table.table_var_MADC32][3],"MADC32_1_ADC");
	strcpy (variable_table.variable_name[variable_table.table_var_MADC32][4],"MADC32_2_CH");
	strcpy (variable_table.variable_name[variable_table.table_var_MADC32][5],"MADC32_2_ADC");
	// SSD TDC
	strcpy (variable_table.variable_name[variable_table.table_var_SSDTDC][0],"SSD_TRAI_CH");
	strcpy (variable_table.variable_name[variable_table.table_var_SSDTDC][1],"SSD_TRAI_TDC");
	strcpy (variable_table.variable_name[variable_table.table_var_SSDTDC][2],"SSD_LEAD_CH");
	strcpy (variable_table.variable_name[variable_table.table_var_SSDTDC][3],"SSD_LEAD_TDC");	
	// Others         
	strcpy (variable_table.variable_name[variable_table.table_var_Others][0],"GR_TIMESTAMP");
}


/**************  RCNPTREE CLASS ****************/
/* Variables for the tree */
/* Multiplicity pretty useless at the moment, but might become usefull later */
class RCNPTREE {
	public :
	
		// GR VALUES
		int 	GR_RAYID_mult;
		double 	GR_RAYID[multmax];
		int 	GR_RF_mult;
		double 	GR_RF[multmax];
		int 	GR_X_mult;
		double	GR_X[multmax];
		int 	GR_Y_mult;
		double 	GR_Y[multmax];
		int 	GR_TH_mult;
		double 	GR_TH[multmax];
		int 	GR_PH_mult;
		double 	GR_PH[multmax];
		int 	GR_ADC_mult;
		double 	GR_ADC[multmax];
		int 	GR_ADC_CHAN[multmax];
		int 	GR_TDC_mult;
		double 	GR_TDC[multmax];
		int 	GR_TDC_CHAN[multmax];
		int 	GR_TPOS_mult;
		double 	GR_TPOS[multmax];
		double 	GR_TPOS_CHAN[multmax];
		int 	GR_MADC_mult;
		double 	GR_MADC[multmax];
		double 	GR_MADC_CHAN[multmax];
		int 	GR_WIRE_X1_mult;
		double	GR_WIRE_X1[multmax];
		int 	GR_WIRE_U1_mult;
		double	GR_WIRE_U1[multmax];
		int 	GR_WIRE_X2_mult;
		double	GR_WIRE_X2[multmax];
		int 	GR_WIRE_U2_mult;
		double	GR_WIRE_U2[multmax];
		int 	GR_WTDC_X1_mult;
		double	GR_WTDC_X1[multmax];
		int 	GR_WTDC_U1_mult;
		double	GR_WTDC_U1[multmax];
		int 	GR_WTDC_X2_mult;
		double	GR_WTDC_X2[multmax];
		int 	GR_WTDC_U2_mult;
		double	GR_WTDC_U2[multmax];
		int 	GR_TDCR_X1_mult;
		double	GR_TDCR_X1[multmax];
		int 	GR_TDCR_U1_mult;
		double	GR_TDCR_U1[multmax];
		int 	GR_TDCR_X2_mult;
		double	GR_TDCR_X2[multmax];
		int 	GR_TDCR_U2_mult;
		double	GR_TDCR_U2[multmax];
		
		// LAS VALUES
		int 	LAS_RAYID_mult;
		double 	LAS_RAYID[multmax];
		int 	LAS_RF_mult;
		double 	LAS_RF[multmax];
		double 	LAS_RF_CHAN[multmax];
		int 	LAS_X_mult;
		double	LAS_X[multmax];
		int 	LAS_Y_mult;
		double 	LAS_Y[multmax];
		int 	LAS_TH_mult;
		double 	LAS_TH[multmax];
		int 	LAS_PH_mult;
		double 	LAS_PH[multmax];
		int 	LAS_ADC_mult;
		double 	LAS_ADC[multmax];
		double 	LAS_ADC_CHAN[multmax];
		int 	LAS_TDC_mult;
		double 	LAS_TDC[multmax];
		double 	LAS_TDC_CHAN[multmax];
		int 	LAS_TPOS_mult;
		double 	LAS_TPOS[multmax];
		double 	LAS_TPOS_CHAN[multmax];		
		int 	LAS_MADC_mult;
		double 	LAS_MADC[multmax];
		double 	LAS_MADC_CHAN[multmax];		
		int 	LAS_WIRE_X1_mult;
		double	LAS_WIRE_X1[multmax];
		int 	LAS_WIRE_U1_mult;
		double	LAS_WIRE_U1[multmax];
		int 	LAS_WIRE_V1_mult;
		double	LAS_WIRE_V1[multmax];
		int 	LAS_WIRE_X2_mult;
		double	LAS_WIRE_X2[multmax];
		int 	LAS_WIRE_U2_mult;
		double	LAS_WIRE_U2[multmax];
		int 	LAS_WIRE_V2_mult;
		double	LAS_WIRE_V2[multmax];
		int 	LAS_TDCR_X1_mult;
		double	LAS_TDCR_X1[multmax];
		int 	LAS_TDCR_U1_mult;
		double	LAS_TDCR_U1[multmax];
		int 	LAS_TDCR_V1_mult;
		double	LAS_TDCR_V1[multmax];
		int 	LAS_TDCR_X2_mult;
		double	LAS_TDCR_X2[multmax];
		int 	LAS_TDCR_U2_mult;
		double	LAS_TDCR_U2[multmax];
		int 	LAS_TDCR_V2_mult;
		double	LAS_TDCR_V2[multmax];
	
		// BLP VALUES	
		int 	BLP_ADC_mult;
		double 	BLP_ADC[multmax];
		double 	BLP_ADC_CHAN[multmax];
		int 	BLP_TDC_mult;
		double 	BLP_TDC[multmax];
		double 	BLP_TDC_CHAN[multmax];

		// QTCs
		int		QTC_TRAI_mult;
		int		QTC_TRAI_CHAN[multmax];
		int		QTC_TRAI_CHAN_mult[QTCchan];		
		double	QTC_TRAI[multmax];
		int		QTC_LEAD_mult;
		int		QTC_LEAD_CHAN[multmax];
		int		QTC_LEAD_CHAN_mult[QTCchan];
		double	QTC_LEAD[multmax];
		
		// MADC32
		int		MADC32_0_mult;
		int		MADC32_0_CHAN[multmax];
		int		MADC32_0_CHAN_mult[MADC32chan];		
		double	MADC32_0[multmax];
		int		MADC32_1_mult;
		int		MADC32_1_CHAN[multmax];
		int		MADC32_1_CHAN_mult[MADC32chan];		
		double	MADC32_1[multmax];
		int		MADC32_2_mult;
		int		MADC32_2_CHAN[multmax];
		int		MADC32_2_CHAN_mult[MADC32chan];		
		double	MADC32_2[multmax];
		
		// SSD TDCs
		int		SSD_TRAI_mult;
		int		SSD_TRAI_CHAN[multmax];
		int		SSD_TRAI_CHAN_mult[TDCchan];		
		double	SSD_TRAI[multmax];
		int		SSD_LEAD_mult;
		int		SSD_LEAD_CHAN[multmax];
		int		SSD_LEAD_CHAN_mult[TDCchan];
		double	SSD_LEAD[multmax];
		
		// Others
		int 	GR_TIMESTAMP_mult;
		double 	GR_TIMESTAMP[multmax];
				  
		void TreeBranch(TTree *tree);
};

/* Create branches for the tree */
void RCNPTREE::TreeBranch(TTree *tree) { 
	// GR
	tree->Branch("GR_RAYID_mult", &GR_RAYID_mult,"GR_RAYID_mult/I");
	tree->Branch("GR_RAYID", GR_RAYID,"GR_RAYID[GR_RAYID_mult]/D");
	tree->Branch("GR_RF_mult", &GR_RF_mult,"GR_RF_mult/I");
	tree->Branch("GR_RF", GR_RF,"GR_RF[GR_RF_mult]/D");
	tree->Branch("GR_X_mult", &GR_X_mult,"GR_X_mult/I");
	tree->Branch("GR_X", GR_X,"GR_X[GR_X_mult]/D");
	tree->Branch("GR_Y_mult", &GR_Y_mult,"GR_Y_mult/I");
	tree->Branch("GR_Y", GR_Y,"GR_Y[GR_Y_mult]/D");
	tree->Branch("GR_TH_mult", &GR_TH_mult,"GR_TH_mult/I");
	tree->Branch("GR_TH", GR_TH,"GR_TH[GR_TH_mult]/D");
	tree->Branch("GR_PH_mult", &GR_PH_mult,"GR_PH_mult/I");
	tree->Branch("GR_PH", GR_PH,"GR_PH[GR_PH_mult]/D");
	tree->Branch("GR_ADC_mult", &GR_ADC_mult,"GR_ADC_mult/I");
	tree->Branch("GR_ADC", GR_ADC,"GR_ADC[GR_ADC_mult]/D");
	tree->Branch("GR_ADC_CHAN", GR_ADC_CHAN,"GR_ADC_CHAN[GR_ADC_mult]/I");  
	tree->Branch("GR_TDC_mult", &GR_TDC_mult,"GR_TDC_mult/I");
	tree->Branch("GR_TDC", GR_TDC,"GR_TDC[GR_TDC_mult]/D");
	tree->Branch("GR_TDC_CHAN", GR_TDC_CHAN,"GR_TDC_CHAN[GR_TDC_mult]/I"); 
	tree->Branch("GR_TPOS_mult", &GR_TPOS_mult,"GR_TPOS_mult/I");
	tree->Branch("GR_TPOS", GR_TPOS,"GR_TPOS[GR_TPOS_mult]/D");
	tree->Branch("GR_TPOS_CHAN", GR_TPOS_CHAN,"GR_TPOS_CHAN[GR_TPOS_mult]/D");
	tree->Branch("GR_MADC_mult", &GR_MADC_mult,"GR_MADC_mult/I");
	tree->Branch("GR_MADC", GR_MADC,"GR_MADC[GR_MADC_mult]/D");
	tree->Branch("GR_MADC_CHAN", GR_MADC_CHAN,"GR_MADC_CHAN[GR_MADC_mult]/D");
	tree->Branch("GR_WIRE_X1_mult", &GR_WIRE_X1_mult,"GR_WIRE_X1_mult/I");
	tree->Branch("GR_WIRE_X1", GR_WIRE_X1,"GR_WIRE_X1[GR_WIRE_X1_mult]/D");
	tree->Branch("GR_WIRE_U1_mult", &GR_WIRE_U1_mult,"GR_WIRE_U1_mult/I");
	tree->Branch("GR_WIRE_U1", GR_WIRE_U1,"GR_WIRE_U1[GR_WIRE_U1_mult]/D");
	tree->Branch("GR_WIRE_X2_mult", &GR_WIRE_X2_mult,"GR_WIRE_X2_mult/I");
	tree->Branch("GR_WIRE_X2", GR_WIRE_X2,"GR_WIRE_X2[GR_WIRE_X2_mult]/D");
	tree->Branch("GR_WIRE_U2_mult", &GR_WIRE_U2_mult,"GR_WIRE_U2_mult/I");
	tree->Branch("GR_WIRE_U2", GR_WIRE_U2,"GR_WIRE_U2[GR_WIRE_U2_mult]/D");
	tree->Branch("GR_WTDC_X1_mult", &GR_WTDC_X1_mult,"GR_WTDC_X1_mult/I");
	tree->Branch("GR_WTDC_X1", GR_WTDC_X1,"GR_WTDC_X1[GR_WTDC_X1_mult]/D");
	tree->Branch("GR_WTDC_U1_mult", &GR_WTDC_U1_mult,"GR_WTDC_U1_mult/I");
	tree->Branch("GR_WTDC_U1", GR_WTDC_U1,"GR_WTDC_U1[GR_WTDC_U1_mult]/D");
	tree->Branch("GR_WTDC_X2_mult", &GR_WTDC_X2_mult,"GR_WTDC_X2_mult/I");
	tree->Branch("GR_WTDC_X2", GR_WTDC_X2,"GR_WTDC_X2[GR_WTDC_X2_mult]/D");
	tree->Branch("GR_WTDC_U2_mult", &GR_WTDC_U2_mult,"GR_WTDC_U2_mult/I");
	tree->Branch("GR_WTDC_U2", GR_WTDC_U2,"GR_WTDC_U2[GR_WTDC_U2_mult]/D");
	tree->Branch("GR_TDCR_X1_mult", &GR_TDCR_X1_mult,"GR_TDCR_X1_mult/I");
	tree->Branch("GR_TDCR_X1", GR_TDCR_X1,"GR_TDCR_X1[GR_TDCR_X1_mult]/D");
	tree->Branch("GR_TDCR_U1_mult", &GR_TDCR_U1_mult,"GR_TDCR_U1_mult/I");
	tree->Branch("GR_TDCR_U1", GR_TDCR_U1,"GR_TDCR_U1[GR_TDCR_U1_mult]/D");
	tree->Branch("GR_TDCR_X2_mult", &GR_TDCR_X2_mult,"GR_TDCR_X2_mult/I");
	tree->Branch("GR_TDCR_X2", GR_TDCR_X2,"GR_TDCR_X2[GR_TDCR_X2_mult]/D");
	tree->Branch("GR_TDCR_U2_mult", &GR_TDCR_U2_mult,"GR_TDCR_U2_mult/I");
	tree->Branch("GR_TDCR_U2", GR_TDCR_U2,"GR_TDCR_U2[GR_TDCR_U2_mult]/D");
   
	// LAS
	tree->Branch("LAS_RAYID_mult", &LAS_RAYID_mult,"LAS_RAYID_mult/I");
	tree->Branch("LAS_RAYID", LAS_RAYID,"LAS_RAYID[LAS_RAYID_mult]/D");
	tree->Branch("LAS_RF_mult", &LAS_RF_mult,"LAS_RF_mult/I");
	tree->Branch("LAS_RF", LAS_RF,"LAS_RF[LAS_RF_mult]/D"); 
	tree->Branch("LAS_RF_CHAN", LAS_RF_CHAN,"LAS_RF_CHAN[LAS_RF_mult]/D"); 
	tree->Branch("LAS_X_mult", &LAS_X_mult,"LAS_X_mult/I");
	tree->Branch("LAS_X", LAS_X,"LAS_X[LAS_X_mult]/D");
	tree->Branch("LAS_Y_mult", &LAS_Y_mult,"LAS_Y_mult/I");
	tree->Branch("LAS_Y", LAS_Y,"LAS_Y[LAS_Y_mult]/D");
	tree->Branch("LAS_TH_mult", &LAS_TH_mult,"LAS_TH_mult/I");
	tree->Branch("LAS_TH", LAS_TH,"LAS_TH[LAS_TH_mult]/D");
	tree->Branch("LAS_PH_mult", &LAS_PH_mult,"LAS_PH_mult/I");
	tree->Branch("LAS_PH", LAS_PH,"LAS_PH[LAS_PH_mult]/D");
	tree->Branch("LAS_ADC_mult", &LAS_ADC_mult,"LAS_ADC_mult/I");
	tree->Branch("LAS_ADC", LAS_ADC,"LAS_ADC[LAS_ADC_mult]/D"); 
	tree->Branch("LAS_ADC_CHAN", LAS_ADC_CHAN,"LAS_ADC_CHAN[LAS_ADC_mult]/D"); 
	tree->Branch("LAS_TDC_mult", &LAS_TDC_mult,"LAS_TDC_mult/I");
	tree->Branch("LAS_TDC", LAS_TDC,"LAS_TDC[LAS_TDC_mult]/D");
	tree->Branch("LAS_TDC_CHAN", LAS_TDC_CHAN,"LAS_TDC_CHAN[LAS_TDC_mult]/D");
 	tree->Branch("LAS_TPOS_mult", &LAS_TPOS_mult,"LAS_TPOS_mult/I");
	tree->Branch("LAS_TPOS", LAS_TPOS,"LAS_TPOS[LAS_TPOS_mult]/D");
	tree->Branch("LAS_TPOS_CHAN", LAS_TPOS_CHAN,"LAS_TPOS_CHAN[LAS_TPOS_mult]/D");   
	tree->Branch("LAS_MADC_mult", &LAS_MADC_mult,"LAS_MADC_mult/I");
	tree->Branch("LAS_MADC", LAS_MADC,"LAS_MADC[LAS_MADC_mult]/D");
	tree->Branch("LAS_MADC_CHAN", LAS_MADC_CHAN,"LAS_MADC_CHAN[LAS_MADC_mult]/D");
	tree->Branch("LAS_WIRE_X1_mult", &LAS_WIRE_X1_mult,"LAS_WIRE_X1_mult/I");
	tree->Branch("LAS_WIRE_X1", LAS_WIRE_X1,"LAS_WIRE_X1[LAS_WIRE_X1_mult]/D");
	tree->Branch("LAS_WIRE_U1_mult", &LAS_WIRE_U1_mult,"LAS_WIRE_U1_mult/I");
	tree->Branch("LAS_WIRE_U1", LAS_WIRE_U1,"LAS_WIRE_U1[LAS_WIRE_U1_mult]/D");
	tree->Branch("LAS_WIRE_V1_mult", &LAS_WIRE_V1_mult,"LAS_WIRE_V1_mult/I");
	tree->Branch("LAS_WIRE_V1", LAS_WIRE_V1,"LAS_WIRE_V1[LAS_WIRE_V1_mult]/D");
	tree->Branch("LAS_WIRE_X2_mult", &LAS_WIRE_X2_mult,"LAS_WIRE_X2_mult/I");
	tree->Branch("LAS_WIRE_X2", LAS_WIRE_X2,"LAS_WIRE_X2[LAS_WIRE_X2_mult]/D");
	tree->Branch("LAS_WIRE_U2_mult", &LAS_WIRE_U2_mult,"LAS_WIRE_U2_mult/I");
	tree->Branch("LAS_WIRE_U2", LAS_WIRE_U2,"LAS_WIRE_U2[LAS_WIRE_U2_mult]/D");
	tree->Branch("LAS_WIRE_V2_mult", &LAS_WIRE_V2_mult,"LAS_WIRE_V2_mult/I");
	tree->Branch("LAS_WIRE_V2", LAS_WIRE_V2,"LAS_WIRE_V2[LAS_WIRE_V2_mult]/D");
	tree->Branch("LAS_TDCR_X1_mult", &LAS_TDCR_X1_mult,"LAS_TDCR_X1_mult/I");
	tree->Branch("LAS_TDCR_X1", LAS_TDCR_X1,"LAS_TDCR_X1[LAS_TDCR_X1_mult]/D");
	tree->Branch("LAS_TDCR_U1_mult", &LAS_TDCR_U1_mult,"LAS_TDCR_U1_mult/I");
	tree->Branch("LAS_TDCR_U1", LAS_TDCR_U1,"LAS_TDCR_U1[LAS_TDCR_U1_mult]/D");
	tree->Branch("LAS_TDCR_V1_mult", &LAS_TDCR_V1_mult,"LAS_TDCR_V1_mult/I");
	tree->Branch("LAS_TDCR_V1", LAS_TDCR_V1,"LAS_TDCR_V1[LAS_TDCR_V1_mult]/D");
	tree->Branch("LAS_TDCR_X2_mult", &LAS_TDCR_X2_mult,"LAS_TDCR_X2_mult/I");
	tree->Branch("LAS_TDCR_X2", LAS_TDCR_X2,"LAS_TDCR_X2[LAS_TDCR_X2_mult]/D");
	tree->Branch("LAS_TDCR_U2_mult", &LAS_TDCR_U2_mult,"LAS_TDCR_U2_mult/I");
	tree->Branch("LAS_TDCR_U2", LAS_TDCR_U2,"LAS_TDCR_U2[LAS_TDCR_U2_mult]/D");
	tree->Branch("LAS_TDCR_V2_mult", &LAS_TDCR_V2_mult,"LAS_TDCR_V2_mult/I");
	tree->Branch("LAS_TDCR_V2", LAS_TDCR_V2,"LAS_TDCR_V2[LAS_TDCR_V2_mult]/D");
   
	// BLP
	/*tree->Branch("BLP_ADC_mult", &BLP_ADC_mult,"BLP_ADC_mult/I");
	tree->Branch("BLP_ADC", BLP_ADC,"BLP_ADC[BLP_ADC_mult]/D"); 
	tree->Branch("BLP_ADC_CHAN", BLP_ADC_CHAN,"BLP_ADC_CHAN[BLP_ADC_mult]/D"); 
	tree->Branch("BLP_TDC_mult", &BLP_TDC_mult,"BLP_TDC_mult/I");
	tree->Branch("BLP_TDC", BLP_TDC,"BLP_TDC[BLP_TDC_mult]/D");
	tree->Branch("BLP_TDC_CHAN", BLP_TDC_CHAN,"BLP_TDC_CHAN[BLP_TDC_mult]/D");*/

	// QTCs 
	tree->Branch("QTC_TRAI_mult", &QTC_TRAI_mult,"QTC_TRAI_mult/I");
	tree->Branch("QTC_TRAI_CHAN", QTC_TRAI_CHAN,"QTC_TRAI_CHAN[QTC_TRAI_mult]/I");
	tree->Branch("QTC_TRAI_CHAN_mult", QTC_TRAI_CHAN_mult,"QTC_TRAI_CHAN_mult[16]/I");
	tree->Branch("QTC_TRAI", QTC_TRAI,"QTC_TRAI[QTC_TRAI_mult]/D");
	tree->Branch("QTC_LEAD_mult", &QTC_LEAD_mult,"QTC_LEAD_mult/I");
	tree->Branch("QTC_LEAD_CHAN", QTC_LEAD_CHAN,"QTC_LEAD_CHAN[QTC_LEAD_mult]/I");
	tree->Branch("QTC_LEAD_CHAN_mult", QTC_LEAD_CHAN_mult,"QTC_LEAD_CHAN_mult[16]/I");
	tree->Branch("QTC_LEAD", QTC_LEAD,"QTC_LEAD[QTC_LEAD_mult]/D"); 
	
	// MADC32
	/*tree->Branch("MADC32_0_mult", &MADC32_0_mult,"MADC32_0_mult/I");
	tree->Branch("MADC32_0_CHAN", MADC32_0_CHAN,"MADC32_0_CHAN[MADC32_0_mult]/I");
	tree->Branch("MADC32_0_CHAN_mult", MADC32_0_CHAN_mult,"MADC32_0_CHAN_mult[32]/I");
	tree->Branch("MADC32_0", MADC32_0,"MADC32_0[MADC32_0_mult]/D");
	tree->Branch("MADC32_1_mult", &MADC32_1_mult,"MADC32_1_mult/I");
	tree->Branch("MADC32_1_CHAN", MADC32_1_CHAN,"MADC32_1_CHAN[MADC32_1_mult]/I");
	tree->Branch("MADC32_1_CHAN_mult", MADC32_1_CHAN_mult,"MADC32_1_CHAN_mult[32]/I");
	tree->Branch("MADC32_1", MADC32_1,"MADC32_1[MADC32_1_mult]/D");
	tree->Branch("MADC32_2_mult", &MADC32_2_mult,"MADC32_2_mult/I");
	tree->Branch("MADC32_2_CHAN", MADC32_2_CHAN,"MADC32_2_CHAN[MADC32_2_mult]/I");
	tree->Branch("MADC32_2_CHAN_mult", MADC32_2_CHAN_mult,"MADC32_2_CHAN_mult[32]/I");
	tree->Branch("MADC32_2", MADC32_2,"MADC32_2[MADC32_2_mult]/D");*/

	// SSD TDCs
	/*tree->Branch("SSD_TRAI_mult", &SSD_TRAI_mult,"SSD_TRAI_mult/I");
	tree->Branch("SSD_TRAI_CHAN", SSD_TRAI_CHAN,"SSD_TRAI_CHAN[SSD_TRAI_mult]/I");
	tree->Branch("SSD_TRAI_CHAN_mult", SSD_TRAI_CHAN_mult,"SSD_TRAI_CHAN_mult[128]/I");
	tree->Branch("SSD_TRAI", SSD_TRAI,"SSD_TRAI[SSD_TRAI_mult]/D");
	tree->Branch("SSD_LEAD_mult", &SSD_LEAD_mult,"SSD_LEAD_mult/I");
	tree->Branch("SSD_LEAD_CHAN", SSD_LEAD_CHAN,"SSD_LEAD_CHAN[SSD_LEAD_mult]/I");
	tree->Branch("SSD_LEAD_CHAN_mult", SSD_LEAD_CHAN_mult,"SSD_LEAD_CHAN_mult[128]/I");
	tree->Branch("SSD_LEAD", SSD_LEAD,"SSD_LEAD[SSD_LEAD_mult]/D");*/
	
	// Others
	/*tree->Branch("GR_TIMESTAMP_mult", &GR_TIMESTAMP_mult,"GR_TIMESTAMP_mult/I");
	tree->Branch("GR_TIMESTAMP", GR_TIMESTAMP,"GR_TIMESTAMP[GR_TIMESTAMP_mult]/D");*/

}

