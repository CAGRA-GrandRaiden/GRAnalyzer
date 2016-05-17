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


class RCNPTREE : public TObject {             // This indicates that the RCNPTREE class inherits from the TObject class (needed writing this object to a tree)
	public :
	RCNPTREE() {;}                        // This is the class "constructor" it is run when an object of the class is created
	virtual ~RCNPTREE() {;}               // This is the class "destructor" it is run when an object of the class is destroyed (virtual indicates that it overrides the destructor of the parent TObject class)


	void Clear() {data.clear();}
	map<string,vector<double> > data;     // The underlying map from string to vector of data

	ClassDef(RCNPTREE,1);                 // This is a ROOT macro that is needed for creating a ROOT dictionary
};
