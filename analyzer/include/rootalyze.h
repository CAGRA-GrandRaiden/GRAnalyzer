#include "TObject.h"

#include <vector>
#include <string>
#include <map>
#include <utility>

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

using namespace std;

/**************  RCNPTREE CLASS ****************/
/* Variables for the tree */
/* Multiplicity pretty useless at the moment, but might become usefull later */
class RCNPTREE : public TObject {
	public :
	RCNPTREE() {;}
	virtual ~RCNPTREE() {;}


	void Clear() {data.clear();}
	map<string,vector<double> > data;

	ClassDef(RCNPTREE,0);

	// // GR VALUES
	// vector<double>	GR_RAYID;
	// vector<double>	GR_RF;
	// vector<double>	GR_X;
	// vector<double>	GR_Y;
	// vector<double>	GR_TH;
	// vector<double>	GR_PH;
	// vector<double>	GR_ADC;
	// vector<int>	GR_ADC_CHAN;
	// vector<double>	GR_TDC;
	// vector<int>	GR_TDC_CHAN;
	// vector<double>	GR_TPOS;
	// vector<double>	GR_TPOS_CHAN;
	// vector<double>	GR_MADC;
	// vector<double>	GR_MADC_CHAN;
	// vector<double>	GR_WIRE_X1;
	// vector<double>	GR_WIRE_U1;
	// vector<double>	GR_WIRE_X2;
	// vector<double>	GR_WIRE_U2;
	// vector<double>	GR_WTDC_X1;
	// vector<double>	GR_WTDC_U1;
	// vector<double>	GR_WTDC_X2;
	// vector<double>	GR_WTDC_U2;
	// vector<double>	GR_TDCR_X1;
	// vector<double>	GR_TDCR_U1;
	// vector<double>	GR_TDCR_X2;
	// vector<double>	GR_TDCR_U2;

	// // LAS VALUES
	// vector<double>	LAS_RAYID;
	// vector<double>	LAS_RF;
	// vector<double>	LAS_RF_CHAN;
	// vector<double>	LAS_X;
	// vector<double>	LAS_Y;
	// vector<double>	LAS_TH;
	// vector<double>	LAS_PH;
	// vector<double>	LAS_ADC;
	// vector<double>	LAS_ADC_CHAN;
	// vector<double>	LAS_TDC;
	// vector<double>	LAS_TDC_CHAN;
	// vector<double>	LAS_TPOS;
	// vector<double>	LAS_TPOS_CHAN;
	// vector<double>	LAS_MADC;
	// vector<double>	LAS_MADC_CHAN;
	// vector<double>	LAS_WIRE_X1;
	// vector<double>	LAS_WIRE_U1;
	// vector<double>	LAS_WIRE_V1;
	// vector<double>	LAS_WIRE_X2;
	// vector<double>	LAS_WIRE_U2;
	// vector<double>	LAS_WIRE_V2;
	// vector<double>	LAS_TDCR_X1;
	// vector<double>	LAS_TDCR_U1;
	// vector<double>	LAS_TDCR_V1;
	// vector<double>	LAS_TDCR_X2;
	// vector<double>	LAS_TDCR_U2;
	// vector<double>	LAS_TDCR_V2;

	// // BLP VALUES
	// vector<double>	BLP_ADC;
	// vector<double>	BLP_ADC_CHAN;
	// vector<double>	BLP_TDC;
	// vector<double>	BLP_TDC_CHAN;

	// // QTCs
	// vector<int>	QTC_TRAI_CHAN;
	// vector<int>	QTC_TRAI_CHAN_mult;
	// vector<double>	QTC_TRAI;
	// vector<int>	QTC_LEAD_CHAN;
	// vector<int>	QTC_LEAD_CHAN_mult;
	// vector<double>	QTC_LEAD;

	// // MADC32
	// vector<int>	MADC32_0_CHAN;
	// vector<int>	MADC32_0_CHAN_mult;
	// vector<double>	MADC32_0;
	// vector<int>	MADC32_1_CHAN;
	// vector<int>	MADC32_1_CHAN_mult;
	// vector<double>	MADC32_1;
	// vector<int>	MADC32_2_CHAN;
	// vector<int>	MADC32_2_CHAN_mult;
	// vector<double>	MADC32_2;

	// // SSD TDCs
	// vector<int>	SSD_TRAI_CHAN;
	// vector<int>	SSD_TRAI_CHAN_mult;
	// vector<double>	SSD_TRAI;
	// vector<int>	SSD_LEAD_CHAN;
	// vector<int>	SSD_LEAD_CHAN_mult;
	// vector<double>	SSD_LEAD;

	// // Others
	// vector<double>	GR_TIMESTAMP;

	//void TreeBranch(TTree *tree);
};

// /* Create branches for the tree */
// void RCNPTREE::TreeBranch(TTree *tree) {
// 	// GR
// 	tree->Branch("GR_RAYID_mult", &GR_RAYID_mult,"GR_RAYID_mult/I");
// 	tree->Branch("GR_RAYID", GR_RAYID,"GR_RAYID[GR_RAYID_mult]/D");
// 	tree->Branch("GR_RF_mult", &GR_RF_mult,"GR_RF_mult/I");
// 	tree->Branch("GR_RF", GR_RF,"GR_RF[GR_RF_mult]/D");
// 	tree->Branch("GR_X_mult", &GR_X_mult,"GR_X_mult/I");
// 	tree->Branch("GR_X", GR_X,"GR_X[GR_X_mult]/D");
// 	tree->Branch("GR_Y_mult", &GR_Y_mult,"GR_Y_mult/I");
// 	tree->Branch("GR_Y", GR_Y,"GR_Y[GR_Y_mult]/D");
// 	tree->Branch("GR_TH_mult", &GR_TH_mult,"GR_TH_mult/I");
// 	tree->Branch("GR_TH", GR_TH,"GR_TH[GR_TH_mult]/D");
// 	tree->Branch("GR_PH_mult", &GR_PH_mult,"GR_PH_mult/I");
// 	tree->Branch("GR_PH", GR_PH,"GR_PH[GR_PH_mult]/D");
// 	tree->Branch("GR_ADC_mult", &GR_ADC_mult,"GR_ADC_mult/I");
// 	tree->Branch("GR_ADC", GR_ADC,"GR_ADC[GR_ADC_mult]/D");
// 	tree->Branch("GR_ADC_CHAN", GR_ADC_CHAN,"GR_ADC_CHAN[GR_ADC_mult]/I");
// 	tree->Branch("GR_TDC_mult", &GR_TDC_mult,"GR_TDC_mult/I");
// 	tree->Branch("GR_TDC", GR_TDC,"GR_TDC[GR_TDC_mult]/D");
// 	tree->Branch("GR_TDC_CHAN", GR_TDC_CHAN,"GR_TDC_CHAN[GR_TDC_mult]/I");
// 	tree->Branch("GR_TPOS_mult", &GR_TPOS_mult,"GR_TPOS_mult/I");
// 	tree->Branch("GR_TPOS", GR_TPOS,"GR_TPOS[GR_TPOS_mult]/D");
// 	tree->Branch("GR_TPOS_CHAN", GR_TPOS_CHAN,"GR_TPOS_CHAN[GR_TPOS_mult]/D");
// 	tree->Branch("GR_MADC_mult", &GR_MADC_mult,"GR_MADC_mult/I");
// 	tree->Branch("GR_MADC", GR_MADC,"GR_MADC[GR_MADC_mult]/D");
// 	tree->Branch("GR_MADC_CHAN", GR_MADC_CHAN,"GR_MADC_CHAN[GR_MADC_mult]/D");
// 	tree->Branch("GR_WIRE_X1_mult", &GR_WIRE_X1_mult,"GR_WIRE_X1_mult/I");
// 	tree->Branch("GR_WIRE_X1", GR_WIRE_X1,"GR_WIRE_X1[GR_WIRE_X1_mult]/D");
// 	tree->Branch("GR_WIRE_U1_mult", &GR_WIRE_U1_mult,"GR_WIRE_U1_mult/I");
// 	tree->Branch("GR_WIRE_U1", GR_WIRE_U1,"GR_WIRE_U1[GR_WIRE_U1_mult]/D");
// 	tree->Branch("GR_WIRE_X2_mult", &GR_WIRE_X2_mult,"GR_WIRE_X2_mult/I");
// 	tree->Branch("GR_WIRE_X2", GR_WIRE_X2,"GR_WIRE_X2[GR_WIRE_X2_mult]/D");
// 	tree->Branch("GR_WIRE_U2_mult", &GR_WIRE_U2_mult,"GR_WIRE_U2_mult/I");
// 	tree->Branch("GR_WIRE_U2", GR_WIRE_U2,"GR_WIRE_U2[GR_WIRE_U2_mult]/D");
// 	tree->Branch("GR_WTDC_X1_mult", &GR_WTDC_X1_mult,"GR_WTDC_X1_mult/I");
// 	tree->Branch("GR_WTDC_X1", GR_WTDC_X1,"GR_WTDC_X1[GR_WTDC_X1_mult]/D");
// 	tree->Branch("GR_WTDC_U1_mult", &GR_WTDC_U1_mult,"GR_WTDC_U1_mult/I");
// 	tree->Branch("GR_WTDC_U1", GR_WTDC_U1,"GR_WTDC_U1[GR_WTDC_U1_mult]/D");
// 	tree->Branch("GR_WTDC_X2_mult", &GR_WTDC_X2_mult,"GR_WTDC_X2_mult/I");
// 	tree->Branch("GR_WTDC_X2", GR_WTDC_X2,"GR_WTDC_X2[GR_WTDC_X2_mult]/D");
// 	tree->Branch("GR_WTDC_U2_mult", &GR_WTDC_U2_mult,"GR_WTDC_U2_mult/I");
// 	tree->Branch("GR_WTDC_U2", GR_WTDC_U2,"GR_WTDC_U2[GR_WTDC_U2_mult]/D");
// 	tree->Branch("GR_TDCR_X1_mult", &GR_TDCR_X1_mult,"GR_TDCR_X1_mult/I");
// 	tree->Branch("GR_TDCR_X1", GR_TDCR_X1,"GR_TDCR_X1[GR_TDCR_X1_mult]/D");
// 	tree->Branch("GR_TDCR_U1_mult", &GR_TDCR_U1_mult,"GR_TDCR_U1_mult/I");
// 	tree->Branch("GR_TDCR_U1", GR_TDCR_U1,"GR_TDCR_U1[GR_TDCR_U1_mult]/D");
// 	tree->Branch("GR_TDCR_X2_mult", &GR_TDCR_X2_mult,"GR_TDCR_X2_mult/I");
// 	tree->Branch("GR_TDCR_X2", GR_TDCR_X2,"GR_TDCR_X2[GR_TDCR_X2_mult]/D");
// 	tree->Branch("GR_TDCR_U2_mult", &GR_TDCR_U2_mult,"GR_TDCR_U2_mult/I");
// 	tree->Branch("GR_TDCR_U2", GR_TDCR_U2,"GR_TDCR_U2[GR_TDCR_U2_mult]/D");

// 	// LAS
// 	tree->Branch("LAS_RAYID_mult", &LAS_RAYID_mult,"LAS_RAYID_mult/I");
// 	tree->Branch("LAS_RAYID", LAS_RAYID,"LAS_RAYID[LAS_RAYID_mult]/D");
// 	tree->Branch("LAS_RF_mult", &LAS_RF_mult,"LAS_RF_mult/I");
// 	tree->Branch("LAS_RF", LAS_RF,"LAS_RF[LAS_RF_mult]/D");
// 	tree->Branch("LAS_RF_CHAN", LAS_RF_CHAN,"LAS_RF_CHAN[LAS_RF_mult]/D");
// 	tree->Branch("LAS_X_mult", &LAS_X_mult,"LAS_X_mult/I");
// 	tree->Branch("LAS_X", LAS_X,"LAS_X[LAS_X_mult]/D");
// 	tree->Branch("LAS_Y_mult", &LAS_Y_mult,"LAS_Y_mult/I");
// 	tree->Branch("LAS_Y", LAS_Y,"LAS_Y[LAS_Y_mult]/D");
// 	tree->Branch("LAS_TH_mult", &LAS_TH_mult,"LAS_TH_mult/I");
// 	tree->Branch("LAS_TH", LAS_TH,"LAS_TH[LAS_TH_mult]/D");
// 	tree->Branch("LAS_PH_mult", &LAS_PH_mult,"LAS_PH_mult/I");
// 	tree->Branch("LAS_PH", LAS_PH,"LAS_PH[LAS_PH_mult]/D");
// 	tree->Branch("LAS_ADC_mult", &LAS_ADC_mult,"LAS_ADC_mult/I");
// 	tree->Branch("LAS_ADC", LAS_ADC,"LAS_ADC[LAS_ADC_mult]/D");
// 	tree->Branch("LAS_ADC_CHAN", LAS_ADC_CHAN,"LAS_ADC_CHAN[LAS_ADC_mult]/D");
// 	tree->Branch("LAS_TDC_mult", &LAS_TDC_mult,"LAS_TDC_mult/I");
// 	tree->Branch("LAS_TDC", LAS_TDC,"LAS_TDC[LAS_TDC_mult]/D");
// 	tree->Branch("LAS_TDC_CHAN", LAS_TDC_CHAN,"LAS_TDC_CHAN[LAS_TDC_mult]/D");
//  	tree->Branch("LAS_TPOS_mult", &LAS_TPOS_mult,"LAS_TPOS_mult/I");
// 	tree->Branch("LAS_TPOS", LAS_TPOS,"LAS_TPOS[LAS_TPOS_mult]/D");
// 	tree->Branch("LAS_TPOS_CHAN", LAS_TPOS_CHAN,"LAS_TPOS_CHAN[LAS_TPOS_mult]/D");
// 	tree->Branch("LAS_MADC_mult", &LAS_MADC_mult,"LAS_MADC_mult/I");
// 	tree->Branch("LAS_MADC", LAS_MADC,"LAS_MADC[LAS_MADC_mult]/D");
// 	tree->Branch("LAS_MADC_CHAN", LAS_MADC_CHAN,"LAS_MADC_CHAN[LAS_MADC_mult]/D");
// 	tree->Branch("LAS_WIRE_X1_mult", &LAS_WIRE_X1_mult,"LAS_WIRE_X1_mult/I");
// 	tree->Branch("LAS_WIRE_X1", LAS_WIRE_X1,"LAS_WIRE_X1[LAS_WIRE_X1_mult]/D");
// 	tree->Branch("LAS_WIRE_U1_mult", &LAS_WIRE_U1_mult,"LAS_WIRE_U1_mult/I");
// 	tree->Branch("LAS_WIRE_U1", LAS_WIRE_U1,"LAS_WIRE_U1[LAS_WIRE_U1_mult]/D");
// 	tree->Branch("LAS_WIRE_V1_mult", &LAS_WIRE_V1_mult,"LAS_WIRE_V1_mult/I");
// 	tree->Branch("LAS_WIRE_V1", LAS_WIRE_V1,"LAS_WIRE_V1[LAS_WIRE_V1_mult]/D");
// 	tree->Branch("LAS_WIRE_X2_mult", &LAS_WIRE_X2_mult,"LAS_WIRE_X2_mult/I");
// 	tree->Branch("LAS_WIRE_X2", LAS_WIRE_X2,"LAS_WIRE_X2[LAS_WIRE_X2_mult]/D");
// 	tree->Branch("LAS_WIRE_U2_mult", &LAS_WIRE_U2_mult,"LAS_WIRE_U2_mult/I");
// 	tree->Branch("LAS_WIRE_U2", LAS_WIRE_U2,"LAS_WIRE_U2[LAS_WIRE_U2_mult]/D");
// 	tree->Branch("LAS_WIRE_V2_mult", &LAS_WIRE_V2_mult,"LAS_WIRE_V2_mult/I");
// 	tree->Branch("LAS_WIRE_V2", LAS_WIRE_V2,"LAS_WIRE_V2[LAS_WIRE_V2_mult]/D");
// 	tree->Branch("LAS_TDCR_X1_mult", &LAS_TDCR_X1_mult,"LAS_TDCR_X1_mult/I");
// 	tree->Branch("LAS_TDCR_X1", LAS_TDCR_X1,"LAS_TDCR_X1[LAS_TDCR_X1_mult]/D");
// 	tree->Branch("LAS_TDCR_U1_mult", &LAS_TDCR_U1_mult,"LAS_TDCR_U1_mult/I");
// 	tree->Branch("LAS_TDCR_U1", LAS_TDCR_U1,"LAS_TDCR_U1[LAS_TDCR_U1_mult]/D");
// 	tree->Branch("LAS_TDCR_V1_mult", &LAS_TDCR_V1_mult,"LAS_TDCR_V1_mult/I");
// 	tree->Branch("LAS_TDCR_V1", LAS_TDCR_V1,"LAS_TDCR_V1[LAS_TDCR_V1_mult]/D");
// 	tree->Branch("LAS_TDCR_X2_mult", &LAS_TDCR_X2_mult,"LAS_TDCR_X2_mult/I");
// 	tree->Branch("LAS_TDCR_X2", LAS_TDCR_X2,"LAS_TDCR_X2[LAS_TDCR_X2_mult]/D");
// 	tree->Branch("LAS_TDCR_U2_mult", &LAS_TDCR_U2_mult,"LAS_TDCR_U2_mult/I");
// 	tree->Branch("LAS_TDCR_U2", LAS_TDCR_U2,"LAS_TDCR_U2[LAS_TDCR_U2_mult]/D");
// 	tree->Branch("LAS_TDCR_V2_mult", &LAS_TDCR_V2_mult,"LAS_TDCR_V2_mult/I");
// 	tree->Branch("LAS_TDCR_V2", LAS_TDCR_V2,"LAS_TDCR_V2[LAS_TDCR_V2_mult]/D");

// 	// BLP
// 	/*tree->Branch("BLP_ADC_mult", &BLP_ADC_mult,"BLP_ADC_mult/I");
// 	tree->Branch("BLP_ADC", BLP_ADC,"BLP_ADC[BLP_ADC_mult]/D");
// 	tree->Branch("BLP_ADC_CHAN", BLP_ADC_CHAN,"BLP_ADC_CHAN[BLP_ADC_mult]/D");
// 	tree->Branch("BLP_TDC_mult", &BLP_TDC_mult,"BLP_TDC_mult/I");
// 	tree->Branch("BLP_TDC", BLP_TDC,"BLP_TDC[BLP_TDC_mult]/D");
// 	tree->Branch("BLP_TDC_CHAN", BLP_TDC_CHAN,"BLP_TDC_CHAN[BLP_TDC_mult]/D");*/

// 	// QTCs
// 	tree->Branch("QTC_TRAI_mult", &QTC_TRAI_mult,"QTC_TRAI_mult/I");
// 	tree->Branch("QTC_TRAI_CHAN", QTC_TRAI_CHAN,"QTC_TRAI_CHAN[QTC_TRAI_mult]/I");
// 	tree->Branch("QTC_TRAI_CHAN_mult", QTC_TRAI_CHAN_mult,"QTC_TRAI_CHAN_mult[16]/I");
// 	tree->Branch("QTC_TRAI", QTC_TRAI,"QTC_TRAI[QTC_TRAI_mult]/D");
// 	tree->Branch("QTC_LEAD_mult", &QTC_LEAD_mult,"QTC_LEAD_mult/I");
// 	tree->Branch("QTC_LEAD_CHAN", QTC_LEAD_CHAN,"QTC_LEAD_CHAN[QTC_LEAD_mult]/I");
// 	tree->Branch("QTC_LEAD_CHAN_mult", QTC_LEAD_CHAN_mult,"QTC_LEAD_CHAN_mult[16]/I");
// 	tree->Branch("QTC_LEAD", QTC_LEAD,"QTC_LEAD[QTC_LEAD_mult]/D");

// 	// MADC32
// 	/*tree->Branch("MADC32_0_mult", &MADC32_0_mult,"MADC32_0_mult/I");
// 	tree->Branch("MADC32_0_CHAN", MADC32_0_CHAN,"MADC32_0_CHAN[MADC32_0_mult]/I");
// 	tree->Branch("MADC32_0_CHAN_mult", MADC32_0_CHAN_mult,"MADC32_0_CHAN_mult[32]/I");
// 	tree->Branch("MADC32_0", MADC32_0,"MADC32_0[MADC32_0_mult]/D");
// 	tree->Branch("MADC32_1_mult", &MADC32_1_mult,"MADC32_1_mult/I");
// 	tree->Branch("MADC32_1_CHAN", MADC32_1_CHAN,"MADC32_1_CHAN[MADC32_1_mult]/I");
// 	tree->Branch("MADC32_1_CHAN_mult", MADC32_1_CHAN_mult,"MADC32_1_CHAN_mult[32]/I");
// 	tree->Branch("MADC32_1", MADC32_1,"MADC32_1[MADC32_1_mult]/D");
// 	tree->Branch("MADC32_2_mult", &MADC32_2_mult,"MADC32_2_mult/I");
// 	tree->Branch("MADC32_2_CHAN", MADC32_2_CHAN,"MADC32_2_CHAN[MADC32_2_mult]/I");
// 	tree->Branch("MADC32_2_CHAN_mult", MADC32_2_CHAN_mult,"MADC32_2_CHAN_mult[32]/I");
// 	tree->Branch("MADC32_2", MADC32_2,"MADC32_2[MADC32_2_mult]/D");*/

// 	// SSD TDCs
// 	/*tree->Branch("SSD_TRAI_mult", &SSD_TRAI_mult,"SSD_TRAI_mult/I");
// 	tree->Branch("SSD_TRAI_CHAN", SSD_TRAI_CHAN,"SSD_TRAI_CHAN[SSD_TRAI_mult]/I");
// 	tree->Branch("SSD_TRAI_CHAN_mult", SSD_TRAI_CHAN_mult,"SSD_TRAI_CHAN_mult[128]/I");
// 	tree->Branch("SSD_TRAI", SSD_TRAI,"SSD_TRAI[SSD_TRAI_mult]/D");
// 	tree->Branch("SSD_LEAD_mult", &SSD_LEAD_mult,"SSD_LEAD_mult/I");
// 	tree->Branch("SSD_LEAD_CHAN", SSD_LEAD_CHAN,"SSD_LEAD_CHAN[SSD_LEAD_mult]/I");
// 	tree->Branch("SSD_LEAD_CHAN_mult", SSD_LEAD_CHAN_mult,"SSD_LEAD_CHAN_mult[128]/I");
// 	tree->Branch("SSD_LEAD", SSD_LEAD,"SSD_LEAD[SSD_LEAD_mult]/D");*/

// 	// Others
// 	/*tree->Branch("GR_TIMESTAMP_mult", &GR_TIMESTAMP_mult,"GR_TIMESTAMP_mult/I");
// 	tree->Branch("GR_TIMESTAMP", GR_TIMESTAMP,"GR_TIMESTAMP[GR_TIMESTAMP_mult]/D");*/

// }
