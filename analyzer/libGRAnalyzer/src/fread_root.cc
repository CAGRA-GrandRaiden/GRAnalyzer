#include <stdio.h>
#include <cstdlib>

#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TClonesArray.h"

#include "GRUTinizerInterface.h"
#include "RCNPEvent.h"
#include "freadin.h"
#include "histogram.h"

//FLAG

static int ndstvar=0;
static char *dstvar[MaxNDSTVar];
static int dstvarref[MaxNDSTVar];

using namespace std;

static DSTMap* table;
static RCNPEvent* rootevent;
static TFile *RootFile;
static TTree *tree;
static bool SaveRCNPTree;

void Init(std::function<void(RCNPEvent*)> func) {
	std:: cout << "Here" << std::endl;
	StoreData = func;
	SaveRCNPTree = RCNPInterface::Instance().SaveTree();
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
				if (SaveRCNPTree) { table->Set(ndstvar,dstvar[ndstvar]); }
				dr_ref_n(str, &dstvarref[ndstvar++]);
				/* dstvar and dstvarref are the main string and
				   integer lookup-pairs for the DST_VARs */
			}
			if(!c) break;
		}
	}


	return(0);
}

/* Read the data */
int root_write_data()
{
	int ref, min, max;
	int index1, index2;
	double d; // data value
	const int numbtoken = 11; // number of *_CHAN variables in rootalyze.h
	int token[numbtoken] = {0};
	int mult, chan;

	int multchan[128];
	for(int i=0; i<128; i++) {
		multchan[i]=0;
	}

	rootevent = new RCNPEvent;
	// loop on all the variables
	for(int iv=0; iv<ndstvar; iv++){
		ref = dstvarref[iv]; // get the index of the dst variable (built in header)
		min = dr_min_ref(ref);
		max = dr_max_ref(ref);

		/// Check if current variable is not empty ///
		if(dr_exists(d=dr_get_ref(ref,min))){
			/// Scroll the multiplicity of current variable ///
			for(int i=min; i<max; i++){
				if(dr_exists(d=dr_get_ref(ref,i))){
					rootevent->data[iv].push_back(d);
					//evt->data[iv].push_back(d);
				}
			}
		}
	}

	// Check that the scalars flag is not set
	// Only save nonscalar data
	// (ie only save data with a timestamp)
	UShort_t ipr = (UShort_t)dr_get(IPR);
	ipr = (ipr & 0x8000) >> 15;
	if (ipr != 1) {
		StoreData(rootevent);
	}

	if (SaveRCNPTree) { tree->Fill(); }
	//rootevent->Clear(); // do not clear -c.s.
	return(0);
}

/* initialize root objects */
int root_init(int nrun){
	int res;

	if (SaveRCNPTree) {
		char rootname[128];
		sprintf(rootname, "rootfiles/run_%04d.root", nrun);
		RootFile = new TFile(rootname,"RECREATE");
		tree = new TTree("rcnptree","RCNP Data Tree");
		// rootevent = new RCNPEvent; // allocated in root_write_data
		table = new DSTMap;
		fprintf(stderr,"");
		tree->Branch("rcnpevent", &rootevent);
	}

	// make sure hist.def hasn't changed since compiling
	RCNPEvent::HistDefCheckSum();

	if((res=root_write_header((char*)NULL))) {
		return(res);
	}
	return(0);
}

/* exit */
int root_exit(){
	if (SaveRCNPTree) {
		if(!RootFile) { return(0); }
		RootFile->cd();
		tree->Write("", TObject::kOverwrite);
		table->Write("", TObject::kOverwrite);
		RootFile->Close();
	}
}
