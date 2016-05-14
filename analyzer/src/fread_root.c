#include <stdio.h>
#include <cstdlib>
#include <vector>

#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TClonesArray.h"

#include "rootalyze.h"
#include "freadin.h"
#include "histogram.h"

//FLAG

static int ndstvar=0;
static char *dstvar[MaxNDSTVar];
static int dstvarref[MaxNDSTVar];

using namespace std;

static RCNPTREE rootevent;
static ROOTVARTABLE variable_table;
static ARRAYINDEX conversion_table[MaxNDSTVar];
static TFile *RootFile;
static TTree *tree;
static vector<vector<vector<double> > > temparray(var_subsets); // Array to temporary store the event

/* Initialize the ROOT-analyzer variable conversion table subset values to 256 to avoid accidental mismatch */
void InitTable()
{
	for(int i=0;i<MaxNDSTVar;i++) {
		conversion_table[i].index_subset = 256;
	}
}

/* Fills the ROOT-analyzer variable conversion table */
void FindVar(int dstvarlabel)
{
	int asize = 0;
	for(int i=0;i<var_subsets;i++) {
		asize = variable_table.table_size[i];
		for(int j=0;j<asize;j++) {
			if(!strcmp(dstvar[dstvarlabel],variable_table.variable_name[i][j])) {
				conversion_table[dstvarlabel].index_subset = i;
				conversion_table[dstvarlabel].index_variable = j;
				i=j=1000;
			}
		}
	}
}

/** Get variable names
    Parses the DST_VAR in hist.def for all the variable names to be saved in TTree
 */
static int root_write_header(char *comment)
{
	char *name, *p, *d, c;
	char str[256];

	// variables to write
	name = dr_get_sval(DST_VAR);
	ndstvar = 0;
	if(name){
		d = p = name;
		c = 1;  // any number other than 0 is OK
		for(int i=0;c;i++){
			c = *d++;
			if((!c && i) || c==':'){ // seperate by ':'
				if(i>255) i=255;
				strncpy(str, p, i);
				str[i] = 0x00;
				p = d;
				i = -1;
				if(ndstvar>=MaxNDSTVar){ // probably need to increase MaxNDSTVar size
					break;
				}
				dstvar[ndstvar] = strdup(str);
				dr_ref_n(str, &dstvarref[ndstvar++]);
				/* dstvar and dstvarref are the main string and
				   integer lookup-pairs for the DST_VARs */
			}
			if(!c) break;
		}
	}

	InitTable(); // set all rootalyze variables to 256 (large number)
	for(int i=0;i<ndstvar;i++) {
		FindVar(i); // associate each dstvar with variable in conversion table
	}
	/*for(int i=0;i<ndstvar;i++) {
		showerr("%d \t %d \t %d \n",i,conversion_table[i].index_subset,conversion_table[i].index_variable);
	}*/
	return(0);
}

/* Read the data */
int root_write_data()
{
	int ref, min, max;
	int index1, index2;
	double d;							// data value
	const int numbtoken = 11;		// number of *_CHAN variables in rootalyze.h
	int token[numbtoken] = {0};
	int mult, chan;

	int multchan[128];
	for(int i=0; i<128; i++) {
		multchan[i]=0;
	}

	// loop on all the variables
	for(int iv=0; iv<ndstvar; iv++){
		ref = dstvarref[iv]; // get the index of the dst variable (built in header)
		min = dr_min_ref(ref);
		max = dr_max_ref(ref);
		index1 = conversion_table[iv].index_subset;
		index2 = conversion_table[iv].index_variable;

		/// Check if current variable is not empty ///
		if(dr_exists(d=dr_get_ref(ref,min))){
			/// Scroll the multiplicity of current variable ///
			for(int i=min; i<max; i++){
				if(dr_exists(d=dr_get_ref(ref,i))){
					temparray[index1][index2].push_back(d);
				}
			}
		}
	}

	///////////////////////////////////////
	///////////////// GR //////////////////
	///////////////////////////////////////
	index1 = variable_table.table_var_GR;
	// GR main values //
	mult = temparray[index1][0].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_RAYID[i] = temparray[index1][0][i];
	}
	rootevent.GR_RAYID_mult = mult;
	mult = temparray[index1][1].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_RF[i] = temparray[index1][1][i];
	}
	rootevent.GR_RF_mult = mult;
	mult = temparray[index1][2].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_X[i] = temparray[index1][2][i];
	}
	rootevent.GR_X_mult = mult;
	mult = temparray[index1][3].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_Y[i] = temparray[index1][3][i];
	}
	rootevent.GR_Y_mult = mult;
	mult = temparray[index1][4].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TH[i] = temparray[index1][4][i];
	}
	rootevent.GR_TH_mult = mult;
	mult = temparray[index1][5].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_PH[i] = temparray[index1][5][i];
	}
	rootevent.GR_PH_mult = mult;

	// GR ADC //
	mult = temparray[index1][6].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_ADC[token[0]] = temparray[index1][6][i];
		rootevent.GR_ADC_CHAN[token[0]] = 0;
		token[0] += 1;
	}
	rootevent.GR_ADC_mult = token[0];
	mult = temparray[index1][7].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_ADC[token[0]] = temparray[index1][7][i];
		rootevent.GR_ADC_CHAN[token[0]] = 1;
		token[0] += 1;
	}
	rootevent.GR_ADC_mult = token[0];
	mult = temparray[index1][8].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_ADC[token[0]] = temparray[index1][8][i];
		rootevent.GR_ADC_CHAN[token[0]] = 2;
		token[0] += 1;
	}
	rootevent.GR_ADC_mult = token[0];
	mult = temparray[index1][9].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_ADC[token[0]] = temparray[index1][9][i];
		rootevent.GR_ADC_CHAN[token[0]] = 3;
		token[0] += 1;
	}
	rootevent.GR_ADC_mult = token[0];
	mult = temparray[index1][10].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_ADC[token[0]] = temparray[index1][10][i];
		rootevent.GR_ADC_CHAN[token[0]] = 4;
		token[0] += 1;
	}
	rootevent.GR_ADC_mult = token[0];
	mult = temparray[index1][11].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_ADC[token[0]] = temparray[index1][11][i];
		rootevent.GR_ADC_CHAN[token[0]] = 5;
		token[0] += 1;
	}
	rootevent.GR_ADC_mult = token[0];

	// GR TDC //
	mult = temparray[index1][12].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDC[token[1]] = temparray[index1][12][i];
		rootevent.GR_TDC_CHAN[token[1]] = 0;
		token[1] += 1;
	}
	rootevent.GR_TDC_mult = token[1];
	mult = temparray[index1][13].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDC[token[1]] = temparray[index1][13][i];
		rootevent.GR_TDC_CHAN[token[1]] = 1;
		token[1] += 1;
	}
	rootevent.GR_TDC_mult = token[1];
	mult = temparray[index1][14].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDC[token[1]] = temparray[index1][14][i];
		rootevent.GR_TDC_CHAN[token[1]] = 2;
		token[1] += 1;
	}
	rootevent.GR_TDC_mult = token[1];
	mult = temparray[index1][15].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDC[token[1]] = temparray[index1][15][i];
		rootevent.GR_TDC_CHAN[token[1]] = 3;
		token[1] += 1;
	}
	rootevent.GR_TDC_mult = token[1];
	mult = temparray[index1][16].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDC[token[1]] = temparray[index1][16][i];
		rootevent.GR_TDC_CHAN[token[1]] = 4;
		token[1] += 1;
	}
	rootevent.GR_TDC_mult = token[1];
	mult = temparray[index1][17].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDC[token[1]] = temparray[index1][17][i];
		rootevent.GR_TDC_CHAN[token[1]] = 5;
		token[1] += 1;
	}
	rootevent.GR_TDC_mult = token[1];

	// GR TPOS and MADC //
	mult = temparray[index1][18].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TPOS[token[2]] = temparray[index1][18][i];
		rootevent.GR_TPOS_CHAN[token[2]] = 0;
		token[2] += 1;
	}
	rootevent.GR_TPOS_mult = token[2];
	mult = temparray[index1][19].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TPOS[token[2]] = temparray[index1][19][i];
		rootevent.GR_TPOS_CHAN[token[2]] = 1;
		token[2] += 1;
	}
	rootevent.GR_TPOS_mult = token[2];
	mult = temparray[index1][20].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_MADC[token[3]] = temparray[index1][20][i];
		rootevent.GR_MADC_CHAN[token[3]] = 0;
		token[3] += 1;
	}
	rootevent.GR_MADC_mult = token[3];
	mult = temparray[index1][21].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_MADC[token[3]] = temparray[index1][21][i];
		rootevent.GR_MADC_CHAN[token[3]] = 1;
		token[3] += 1;
	}
	rootevent.GR_MADC_mult = token[3];
	mult = temparray[index1][22].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_MADC[token[3]] = temparray[index1][22][i];
		rootevent.GR_MADC_CHAN[token[3]] = 2;
		token[3] += 1;
	}
	rootevent.GR_MADC_mult = token[3];

	// GR WIRE //
	mult = temparray[index1][23].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_WIRE_X1[i] = temparray[index1][23][i];
	}
	rootevent.GR_WIRE_X1_mult = mult;
	mult = temparray[index1][24].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_WIRE_U1[i] = temparray[index1][24][i];
	}
	rootevent.GR_WIRE_U1_mult = mult;
	mult = temparray[index1][25].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_WIRE_X2[i] = temparray[index1][25][i];
	}
	rootevent.GR_WIRE_X2_mult = mult;
	mult = temparray[index1][26].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_WIRE_U2[i] = temparray[index1][26][i];
	}
	rootevent.GR_WIRE_U2_mult = mult;
	mult = temparray[index1][27].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_WTDC_X1[i] = temparray[index1][27][i];
	}
	rootevent.GR_WTDC_X1_mult = mult;
	mult = temparray[index1][28].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_WTDC_U1[i] = temparray[index1][28][i];
	}
	rootevent.GR_WTDC_U1_mult = mult;
	mult = temparray[index1][29].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_WTDC_X2[i] = temparray[index1][29][i];
	}
	rootevent.GR_WTDC_X2_mult = mult;
	mult = temparray[index1][30].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_WTDC_U2[i] = temparray[index1][30][i];
	}
	rootevent.GR_WTDC_U2_mult = mult;
	mult = temparray[index1][31].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDCR_X1[i] = temparray[index1][31][i];
	}
	rootevent.GR_TDCR_X1_mult = mult;
	mult = temparray[index1][32].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDCR_U1[i] = temparray[index1][32][i];
	}
	rootevent.GR_TDCR_U1_mult = mult;
	mult = temparray[index1][33].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDCR_X2[i] = temparray[index1][33][i];
	}
	rootevent.GR_TDCR_X2_mult = mult;
	mult = temparray[index1][34].size();
	for (int i=0;i<mult;i++) {
		rootevent.GR_TDCR_U2[i] = temparray[index1][34][i];
	}
	rootevent.GR_TDCR_U2_mult = mult;

	// clean temparray
	for (int i=0;i<variable_table.table_size[index1];i++) {
		temparray[index1][i].resize(0);
	}


	///////////////////////////////////////
	//////////////// LAS //////////////////
	///////////////////////////////////////
	index1 = variable_table.table_var_LAS;
	// LAS main values //
	mult = temparray[index1][0].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_RAYID[i] = temparray[index1][0][i];
	}
	rootevent.LAS_RAYID_mult = mult;
	mult = temparray[index1][1].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_RF[token[4]] = temparray[index1][1][i];
		rootevent.LAS_RF_CHAN[token[4]] = 0;
		token[4] += 1;
	}
	rootevent.LAS_RF_mult = token[4];
	mult = temparray[index1][2].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_RF[token[4]] = temparray[index1][2][i];
		rootevent.LAS_RF_CHAN[token[4]] = 1;
		token[4] += 1;
	}
	rootevent.LAS_RF_mult = token[4];
	mult = temparray[index1][3].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_RF[token[4]] = temparray[index1][3][i];
		rootevent.LAS_RF_CHAN[token[4]] = 2;
		token[4] += 1;
	}
	rootevent.LAS_RF_mult = token[4];
	mult = temparray[index1][4].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_X[i] = temparray[index1][4][i];
	}
	rootevent.LAS_X_mult = mult;
	mult = temparray[index1][5].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_Y[i] = temparray[index1][5][i];
	}
	rootevent.LAS_Y_mult = mult;
	mult = temparray[index1][6].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TH[i] = temparray[index1][6][i];
	}
	rootevent.LAS_TH_mult = mult;
	mult = temparray[index1][7].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_PH[i] = temparray[index1][7][i];
	}
	rootevent.LAS_PH_mult = mult;

	// LAS ADC //
	mult = temparray[index1][8].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][8][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 0;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][9].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][9][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 1;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][10].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][10][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 2;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][11].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][11][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 3;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][12].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][12][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 4;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][13].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][13][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 5;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][14].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][14][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 6;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][15].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][15][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 7;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][16].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][16][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 8;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][17].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][17][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 9;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][18].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][18][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 10;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];
	mult = temparray[index1][19].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_ADC[token[5]] = temparray[index1][19][i];
		rootevent.LAS_ADC_CHAN[token[5]] = 11;
		token[5] += 1;
	}
	rootevent.LAS_ADC_mult = token[5];

	// LAS TDC //
	mult = temparray[index1][20].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][20][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 0;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][21].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][21][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 1;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][22].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][22][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 2;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][23].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][23][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 3;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][24].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][24][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 4;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][25].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][25][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 5;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][26].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][26][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 6;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][27].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][27][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 7;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][28].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][28][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 8;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][29].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][29][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 9;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][30].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][30][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 10;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];
	mult = temparray[index1][31].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDC[token[6]] = temparray[index1][31][i];
		rootevent.LAS_TDC_CHAN[token[6]] = 11;
		token[6] += 1;
	}
	rootevent.LAS_TDC_mult = token[6];

	// LAS TPOS and MADC //
	mult = temparray[index1][32].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TPOS[token[7]] = temparray[index1][32][i];
		rootevent.LAS_TPOS_CHAN[token[7]] = 0;
		token[7] += 1;
	}
	rootevent.LAS_TPOS_mult = token[7];
	mult = temparray[index1][33].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TPOS[token[7]] = temparray[index1][33][i];
		rootevent.LAS_TPOS_CHAN[token[7]] = 1;
		token[7] += 1;
	}
	rootevent.LAS_TPOS_mult = token[7];
	mult = temparray[index1][34].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TPOS[token[7]] = temparray[index1][34][i];
		rootevent.LAS_TPOS_CHAN[token[7]] = 2;
		token[7] += 1;
	}
	rootevent.LAS_TPOS_mult = token[7];
	mult = temparray[index1][35].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TPOS[token[7]] = temparray[index1][35][i];
		rootevent.LAS_TPOS_CHAN[token[7]] = 3;
		token[7] += 1;
	}
	rootevent.LAS_TPOS_mult = token[7];
	mult = temparray[index1][36].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TPOS[token[7]] = temparray[index1][36][i];
		rootevent.LAS_TPOS_CHAN[token[7]] = 4;
		token[7] += 1;
	}
	rootevent.LAS_TPOS_mult = token[7];
	mult = temparray[index1][37].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TPOS[token[7]] = temparray[index1][37][i];
		rootevent.LAS_TPOS_CHAN[token[7]] = 5;
		token[7] += 1;
	}
	rootevent.LAS_TPOS_mult = token[7];
	mult = temparray[index1][38].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_MADC[token[8]] = temparray[index1][38][i];
		rootevent.LAS_MADC_CHAN[token[8]] = 0;
		token[8] += 1;
	}
	rootevent.LAS_MADC_mult = token[8];
	mult = temparray[index1][39].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_MADC[token[8]] = temparray[index1][39][i];
		rootevent.LAS_MADC_CHAN[token[8]] = 1;
		token[8] += 1;
	}
	rootevent.LAS_MADC_mult = token[8];
	mult = temparray[index1][40].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_MADC[token[8]] = temparray[index1][40][i];
		rootevent.LAS_MADC_CHAN[token[8]] = 2;
		token[8] += 1;
	}
	rootevent.LAS_MADC_mult = token[8];
	mult = temparray[index1][41].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_MADC[token[8]] = temparray[index1][41][i];
		rootevent.LAS_MADC_CHAN[token[8]] = 3;
		token[8] += 1;
	}
	rootevent.LAS_MADC_mult = token[8];
	mult = temparray[index1][42].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_MADC[token[8]] = temparray[index1][42][i];
		rootevent.LAS_MADC_CHAN[token[8]] = 4;
		token[8] += 1;
	}
	rootevent.LAS_MADC_mult = token[8];
	mult = temparray[index1][43].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_MADC[token[8]] = temparray[index1][43][i];
		rootevent.LAS_MADC_CHAN[token[8]] = 5;
		token[8] += 1;
	}
	rootevent.LAS_MADC_mult = token[8];

	// LAS WIRE //
	mult = temparray[index1][44].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_WIRE_X1[i] = temparray[index1][44][i];
	}
	rootevent.LAS_WIRE_X1_mult = mult;
	mult = temparray[index1][45].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_WIRE_U1[i] = temparray[index1][45][i];
	}
	rootevent.LAS_WIRE_U1_mult = mult;
	mult = temparray[index1][46].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_WIRE_V1[i] = temparray[index1][46][i];
	}
	rootevent.LAS_WIRE_V1_mult = mult;
	mult = temparray[index1][47].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_WIRE_X2[i] = temparray[index1][47][i];
	}
	rootevent.LAS_WIRE_X2_mult = mult;
	mult = temparray[index1][48].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_WIRE_U2[i] = temparray[index1][48][i];
	}
	rootevent.LAS_WIRE_U2_mult = mult;
	mult = temparray[index1][49].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_WIRE_V2[i] = temparray[index1][49][i];
	}
	rootevent.LAS_WIRE_V2_mult = mult;
	mult = temparray[index1][50].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDCR_X1[i] = temparray[index1][50][i];
	}
	rootevent.LAS_TDCR_X1_mult = mult;
	mult = temparray[index1][51].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDCR_U1[i] = temparray[index1][51][i];
	}
	rootevent.LAS_TDCR_U1_mult = mult;
	mult = temparray[index1][52].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDCR_V1[i] = temparray[index1][52][i];
	}
	rootevent.LAS_TDCR_V1_mult = mult;
	mult = temparray[index1][53].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDCR_X2[i] = temparray[index1][53][i];
	}
	rootevent.LAS_TDCR_X2_mult = mult;
	mult = temparray[index1][54].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDCR_U2[i] = temparray[index1][54][i];
	}
	rootevent.LAS_TDCR_U2_mult = mult;
	mult = temparray[index1][55].size();
	for (int i=0;i<mult;i++) {
		rootevent.LAS_TDCR_V2[i] = temparray[index1][55][i];
	}
	rootevent.LAS_TDCR_V2_mult = mult;

	for (int i=0;i<variable_table.table_size[index1];i++) {
		temparray[index1][i].resize(0);
	}

	// clean temparray
	for (int i=0;i<variable_table.table_size[index1];i++) {
		temparray[index1][i].resize(0);
	}


	///////////////////////////////////////
	//////////////// BLP //////////////////
	///////////////////////////////////////
/*	index1 = variable_table.table_var_BLP;
	// Beam Line Polarimeter ADC //
	mult = temparray[index1][0].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_ADC[token[9]] = temparray[index1][0][i];
		rootevent.BLP_ADC_CHAN[token[9]] = 0;
		token[9] += 1;
	}
	rootevent.BLP_ADC_mult = token[9];
	mult = temparray[index1][1].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_ADC[token[9]] = temparray[index1][1][i];
		rootevent.BLP_ADC_CHAN[token[9]] = 1;
		token[9] += 1;
	}
	rootevent.BLP_ADC_mult = token[9];
	mult = temparray[index1][2].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_ADC[token[9]] = temparray[index1][2][i];
		rootevent.BLP_ADC_CHAN[token[9]] = 2;
		token[9] += 1;
	}
	rootevent.BLP_ADC_mult = token[9];
	mult = temparray[index1][3].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_ADC[token[9]] = temparray[index1][3][i];
		rootevent.BLP_ADC_CHAN[token[9]] = 3;
		token[9] += 1;
	}
	rootevent.BLP_ADC_mult = token[9];
	mult = temparray[index1][4].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_ADC[token[9]] = temparray[index1][4][i];
		rootevent.BLP_ADC_CHAN[token[9]] = 4;
		token[9] += 1;
	}
	rootevent.BLP_ADC_mult = token[9];
	mult = temparray[index1][5].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_ADC[token[9]] = temparray[index1][5][i];
		rootevent.BLP_ADC_CHAN[token[9]] = 5;
		token[9] += 1;
	}
	rootevent.BLP_ADC_mult = token[9];
	mult = temparray[index1][6].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_ADC[token[9]] = temparray[index1][6][i];
		rootevent.BLP_ADC_CHAN[token[9]] = 6;
		token[9] += 1;
	}
	rootevent.BLP_ADC_mult = token[9];
	mult = temparray[index1][7].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_ADC[token[9]] = temparray[index1][7][i];
		rootevent.BLP_ADC_CHAN[token[9]] = 7;
		token[9] += 1;
	}
	rootevent.BLP_ADC_mult = token[9];

	// Beam Line Polarimeter TDC //
	mult = temparray[index1][8].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_TDC[token[10]] = temparray[index1][8][i];
		rootevent.BLP_TDC_CHAN[token[10]] = 0;
		token[10] += 1;
	}
	rootevent.BLP_TDC_mult = token[10];
	mult = temparray[index1][9].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_TDC[token[10]] = temparray[index1][9][i];
		rootevent.BLP_TDC_CHAN[token[10]] = 1;
		token[10] += 1;
	}
	rootevent.BLP_TDC_mult = token[10];
	mult = temparray[index1][10].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_TDC[token[10]] = temparray[index1][10][i];
		rootevent.BLP_TDC_CHAN[token[10]] = 2;
		token[10] += 1;
	}
	rootevent.BLP_TDC_mult = token[10];
	mult = temparray[index1][11].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_TDC[token[10]] = temparray[index1][11][i];
		rootevent.BLP_TDC_CHAN[token[10]] = 3;
		token[10] += 1;
	}
	rootevent.BLP_TDC_mult = token[10];
	mult = temparray[index1][12].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_TDC[token[10]] = temparray[index1][12][i];
		rootevent.BLP_TDC_CHAN[token[10]] = 4;
		token[10] += 1;
	}
	rootevent.BLP_TDC_mult = token[10];
	mult = temparray[index1][13].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_TDC[token[10]] = temparray[index1][13][i];
		rootevent.BLP_TDC_CHAN[token[10]] = 5;
		token[10] += 1;
	}
	rootevent.BLP_TDC_mult = token[10];
	mult = temparray[index1][14].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_TDC[token[10]] = temparray[index1][14][i];
		rootevent.BLP_TDC_CHAN[token[10]] = 6;
		token[10] += 1;
	}
	rootevent.BLP_TDC_mult = token[10];
	mult = temparray[index1][15].size();
	for (int i=0;i<mult;i++) {
		rootevent.BLP_TDC[token[10]] = temparray[index1][15][i];
		rootevent.BLP_TDC_CHAN[token[10]] = 7;
		token[10] += 1;
	}
	rootevent.BLP_TDC_mult = token[10];

	// clean temparray
	for (int i=0;i<variable_table.table_size[index1];i++) {
		temparray[index1][i].resize(0);
	}*/


	///////////////////////////////////////
	//////////////// QTC //////////////////
	///////////////////////////////////////
	index1 = variable_table.table_var_QTC;

	// QTC TRAILING //
	mult = temparray[index1][1].size();
	for (int i=0;i<mult;i++) {
		rootevent.QTC_TRAI_CHAN[i] = temparray[index1][0][i];
		rootevent.QTC_TRAI[i] = temparray[index1][1][i];
		multchan[(int)(temparray[index1][0][i]+0.5)] += 1;
	}
	for(int i=0; i<QTCchan; i++) {
		rootevent.QTC_TRAI_CHAN_mult[i] = multchan[i];
		multchan[i]=0;
	}
	rootevent.QTC_TRAI_mult = mult;

	// QTC LEADING //
	mult = temparray[index1][3].size();
	for (int i=0;i<mult;i++) {
		rootevent.QTC_LEAD_CHAN[i] = temparray[index1][2][i];
		rootevent.QTC_LEAD[i] = temparray[index1][3][i];
		multchan[(int)(temparray[index1][2][i]+0.5)] += 1;
	}
	for(int i=0; i<QTCchan; i++) {
		rootevent.QTC_LEAD_CHAN_mult[i] = multchan[i];
		multchan[i]=0;
	}
	rootevent.QTC_LEAD_mult = mult;

	// clean temparray
	for (int i=0;i<variable_table.table_size[index1];i++) {
		temparray[index1][i].resize(0);
	}


	//////////////////////////////////////////
	//////////////// MADC32 //////////////////
	//////////////////////////////////////////
/*	index1 = variable_table.table_var_MADC32;

	// MADC32_0 //
	mult = temparray[index1][1].size();
	for (int i=0;i<mult;i++) {
		rootevent.MADC32_0_CHAN[i] = temparray[index1][0][i];
		rootevent.MADC32_0[i] = temparray[index1][1][i];
		multchan[(int)(temparray[index1][0][i]+0.5)] += 1;
	}
	for(int i=0; i<MADC32chan; i++) {
		rootevent.MADC32_0_CHAN_mult[i] = multchan[i];
		multchan[i]=0;
	}
	rootevent.MADC32_0_mult = mult;

	// MADC32_1 //
	mult = temparray[index1][3].size();
	for (int i=0;i<mult;i++) {
		rootevent.MADC32_1_CHAN[i] = temparray[index1][2][i];
		rootevent.MADC32_1[i] = temparray[index1][3][i];
		multchan[(int)(temparray[index1][2][i]+0.5)] += 1;
	}
	for(int i=0; i<MADC32chan; i++) {
		rootevent.MADC32_1_CHAN_mult[i] = multchan[i];
		multchan[i]=0;
	}
	rootevent.MADC32_1_mult = mult;

	// MADC32_2 //
	mult = temparray[index1][5].size();
	for (int i=0;i<mult;i++) {
		rootevent.MADC32_2_CHAN[i] = temparray[index1][4][i];
		rootevent.MADC32_2[i] = temparray[index1][5][i];
		multchan[(int)(temparray[index1][4][i]+0.5)] += 1;
	}
	for(int i=0; i<MADC32chan; i++) {
		rootevent.MADC32_2_CHAN_mult[i] = multchan[i];
		multchan[i]=0;
	}
	rootevent.MADC32_2_mult = mult;

	// clean temparray
	for (int i=0;i<variable_table.table_size[index1];i++) {
		temparray[index1][i].resize(0);
	}*/


	///////////////////////////////////////////
	//////////////// SSD TDC //////////////////
	///////////////////////////////////////////
/*	index1 = variable_table.table_var_SSDTDC;

	// Trailing edge //
	mult = temparray[index1][1].size();
	for (int i=0;i<mult;i++) {
		rootevent.SSD_TRAI_CHAN[i] = temparray[index1][0][i];
		rootevent.SSD_TRAI[i] = temparray[index1][1][i];
		multchan[(int)(temparray[index1][0][i]+0.5)] += 1;
	}
	for(int i=0; i<TDCchan; i++) {
		rootevent.SSD_TRAI_CHAN_mult[i] = multchan[i];
		multchan[i]=0;
	}
	rootevent.SSD_TRAI_mult = mult;

	// Leading edge //
	mult = temparray[index1][3].size();
	for (int i=0;i<mult;i++) {
		rootevent.SSD_LEAD_CHAN[i] = temparray[index1][2][i];
		rootevent.SSD_LEAD[i] = temparray[index1][3][i];
		multchan[(int)(temparray[index1][2][i]+0.5)] += 1;
	}
	for(int i=0; i<TDCchan; i++) {
		rootevent.SSD_LEAD_CHAN_mult[i] = multchan[i];
		multchan[i]=0;
	}
	rootevent.SSD_LEAD_mult = mult;

	// clean temparray
	for (int i=0;i<variable_table.table_size[index1];i++) {
		temparray[index1][i].resize(0);
	}*/


	/////////////////////////////////////////
	//////////////// Other //////////////////
	/////////////////////////////////////////
	/*index1 = variable_table.table_var_Others;

	// GR TIMESTAMP //
	rootevent.GR_TIMESTAMP[0] = eventnumber;
	rootevent.GR_TIMESTAMP_mult = 1;

	// clean temparray
	for (int i=0;i<variable_table.table_size[index1];i++) {
		temparray[index1][i].resize(0);
	}	*/


	// Fill the tree
	tree->Fill();
	return(0);
}

/* initialize root objects */
int root_init(int nrun){
	int res;

	char rootname[128];
	sprintf(rootname, "rootfiles/run_%04d.root", nrun);
	RootFile = new TFile(rootname,"RECREATE");
	tree = new TTree("tree","Data Tree");

	rootevent.TreeBranch(tree);
	variable_table.FillVarTable(variable_table);
	for(int i=0; i<var_subsets; i++) {
		temparray[i].resize(variable_table.table_size[i]);
	}

	// Initialise branches size
	rootevent.GR_RAYID_mult=0;
	rootevent.GR_RF_mult=0;
	rootevent.GR_X_mult=0;
	rootevent.GR_Y_mult=0;
	rootevent.GR_TH_mult=0;
	rootevent.GR_PH_mult=0;
	rootevent.GR_ADC_mult=0;
	rootevent.GR_TDC_mult=0;
	rootevent.GR_TPOS_mult=0;
	rootevent.GR_MADC_mult=0;
	rootevent.GR_WIRE_X1_mult=0;
	rootevent.GR_WIRE_U1_mult=0;
	rootevent.GR_WIRE_X2_mult=0;
	rootevent.GR_WIRE_U2_mult=0;
	rootevent.GR_WTDC_X1_mult=0;
	rootevent.GR_WTDC_U1_mult=0;
	rootevent.GR_WTDC_X2_mult=0;
	rootevent.GR_WTDC_U2_mult=0;
	rootevent.GR_TDCR_X1_mult=0;
	rootevent.GR_TDCR_U1_mult=0;
	rootevent.GR_TDCR_X2_mult=0;
	rootevent.GR_TDCR_U2_mult=0;

	rootevent.LAS_RAYID_mult=0;
	rootevent.LAS_RF_mult=0;
	rootevent.LAS_X_mult=0;
	rootevent.LAS_Y_mult=0;
	rootevent.LAS_TH_mult=0;
	rootevent.LAS_PH_mult=0;
	rootevent.LAS_ADC_mult=0;
	rootevent.LAS_TDC_mult=0;
	rootevent.LAS_TPOS_mult=0;
	rootevent.LAS_MADC_mult=0;
	rootevent.LAS_WIRE_X1_mult=0;
	rootevent.LAS_WIRE_U1_mult=0;
	rootevent.LAS_WIRE_V1_mult=0;
	rootevent.LAS_WIRE_X2_mult=0;
	rootevent.LAS_WIRE_U2_mult=0;
	rootevent.LAS_WIRE_V2_mult=0;
	rootevent.LAS_TDCR_X1_mult=0;
	rootevent.LAS_TDCR_U1_mult=0;
	rootevent.LAS_TDCR_V1_mult=0;
	rootevent.LAS_TDCR_X2_mult=0;
	rootevent.LAS_TDCR_U2_mult=0;
	rootevent.LAS_TDCR_V2_mult=0;

	/*rootevent.BLP_ADC_mult=0;
	rootevent.BLP_TDC_mult=0;*/

	rootevent.QTC_TRAI_mult=0;
	rootevent.QTC_LEAD_mult=0;

	/*rootevent.MADC32_0_mult=0;
	rootevent.MADC32_1_mult=0;
	rootevent.MADC32_2_mult=0;*/

	/*rootevent.SSD_TRAI_mult=0;
	rootevent.SSD_LEAD_mult=0;*/

	/*rootevent.GR_TIMESTAMP_mult=0;*/

	if((res=root_write_header((char*)NULL))) {
		return(res);
	}
	return(0);
}

/* exit */
int root_exit(){
	if(!RootFile)
		return(0);
	RootFile->Write();
	RootFile->Close();
}
