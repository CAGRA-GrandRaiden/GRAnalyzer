
#include "TObject.h"

#include <stdio.h>
#include <ctype.h>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <stdexcept>
#include <sys/types.h>
#include <signal.h>

#define BAD_NUM -441441

using namespace std;

/**************  RCNPEvent Class ****************/
/* All tree variables are referenced by string e.g. "GR_ADC" */

class RCNPEvent : public TObject {            // This indicates that the RCNPEvent class inherits from the TObject class
	                                      // (this inheritence is needed for writing this object to a tree)
  public:
        RCNPEvent()                           // This is the class "constructor" it is run when an object of the class is created
	  : fTimestamp(0) {;}
	virtual ~RCNPEvent() {;}              // This is the class "destructor" it is run when an object of the class is destroyed
	                                      // (virtual indicates that it overrides the destructor of the parent TObject class)

        map<int,vector<double> > data;        // The underlying map from string to vector of data
	void Clear() {data.clear();}

        long GetTimestamp() { return fTimestamp; }
        void SetTimestamp(const long& ts) { fTimestamp = ts; }

        //[[[cog import rootalyze as root; root.definitions() ]]]
        double QTC_TRAILING_CH(const int& i) { return (data.count(0) > 0) ? data[0][i] : BAD_NUM; }
        vector<double>* QTC_TRAILING_CH() { return (data.count(0) > 0) ? &data[0] : nullptr; }
        double QTC_TRAILING_TDC(const int& i) { return (data.count(1) > 0) ? data[1][i] : BAD_NUM; }
        vector<double>* QTC_TRAILING_TDC() { return (data.count(1) > 0) ? &data[1] : nullptr; }
        double QTC_LEADING_CH(const int& i) { return (data.count(2) > 0) ? data[2][i] : BAD_NUM; }
        vector<double>* QTC_LEADING_CH() { return (data.count(2) > 0) ? &data[2] : nullptr; }
        double QTC_LEADING_TDC(const int& i) { return (data.count(3) > 0) ? data[3][i] : BAD_NUM; }
        vector<double>* QTC_LEADING_TDC() { return (data.count(3) > 0) ? &data[3] : nullptr; }
        double GR_RAYID(const int& i) { return (data.count(4) > 0) ? data[4][i] : BAD_NUM; }
        vector<double>* GR_RAYID() { return (data.count(4) > 0) ? &data[4] : nullptr; }
        double GR_RF(const int& i) { return (data.count(5) > 0) ? data[5][i] : BAD_NUM; }
        vector<double>* GR_RF() { return (data.count(5) > 0) ? &data[5] : nullptr; }
        double GR_X(const int& i) { return (data.count(6) > 0) ? data[6][i] : BAD_NUM; }
        vector<double>* GR_X() { return (data.count(6) > 0) ? &data[6] : nullptr; }
        double GR_Y(const int& i) { return (data.count(7) > 0) ? data[7][i] : BAD_NUM; }
        vector<double>* GR_Y() { return (data.count(7) > 0) ? &data[7] : nullptr; }
        double GR_TH(const int& i) { return (data.count(8) > 0) ? data[8][i] : BAD_NUM; }
        vector<double>* GR_TH() { return (data.count(8) > 0) ? &data[8] : nullptr; }
        double GR_PH(const int& i) { return (data.count(9) > 0) ? data[9][i] : BAD_NUM; }
        vector<double>* GR_PH() { return (data.count(9) > 0) ? &data[9] : nullptr; }
        double GR_ADC(const int& i) { return (data.count(10) > 0) ? data[10][i] : BAD_NUM; }
        vector<double>* GR_ADC() { return (data.count(10) > 0) ? &data[10] : nullptr; }
        double GR_TDC(const int& i) { return (data.count(11) > 0) ? data[11][i] : BAD_NUM; }
        vector<double>* GR_TDC() { return (data.count(11) > 0) ? &data[11] : nullptr; }
        double GR_TPOS(const int& i) { return (data.count(12) > 0) ? data[12][i] : BAD_NUM; }
        vector<double>* GR_TPOS() { return (data.count(12) > 0) ? &data[12] : nullptr; }
        double GR_MADC(const int& i) { return (data.count(13) > 0) ? data[13][i] : BAD_NUM; }
        vector<double>* GR_MADC() { return (data.count(13) > 0) ? &data[13] : nullptr; }
        double GR_WIRE_X1(const int& i) { return (data.count(14) > 0) ? data[14][i] : BAD_NUM; }
        vector<double>* GR_WIRE_X1() { return (data.count(14) > 0) ? &data[14] : nullptr; }
        double GR_WIRE_U1(const int& i) { return (data.count(15) > 0) ? data[15][i] : BAD_NUM; }
        vector<double>* GR_WIRE_U1() { return (data.count(15) > 0) ? &data[15] : nullptr; }
        double GR_WIRE_X2(const int& i) { return (data.count(16) > 0) ? data[16][i] : BAD_NUM; }
        vector<double>* GR_WIRE_X2() { return (data.count(16) > 0) ? &data[16] : nullptr; }
        double GR_WIRE_U2(const int& i) { return (data.count(17) > 0) ? data[17][i] : BAD_NUM; }
        vector<double>* GR_WIRE_U2() { return (data.count(17) > 0) ? &data[17] : nullptr; }
        double GR_WTDC_X1(const int& i) { return (data.count(18) > 0) ? data[18][i] : BAD_NUM; }
        vector<double>* GR_WTDC_X1() { return (data.count(18) > 0) ? &data[18] : nullptr; }
        double GR_WTDC_U1(const int& i) { return (data.count(19) > 0) ? data[19][i] : BAD_NUM; }
        vector<double>* GR_WTDC_U1() { return (data.count(19) > 0) ? &data[19] : nullptr; }
        double GR_WTDC_X2(const int& i) { return (data.count(20) > 0) ? data[20][i] : BAD_NUM; }
        vector<double>* GR_WTDC_X2() { return (data.count(20) > 0) ? &data[20] : nullptr; }
        double GR_WTDC_U2(const int& i) { return (data.count(21) > 0) ? data[21][i] : BAD_NUM; }
        vector<double>* GR_WTDC_U2() { return (data.count(21) > 0) ? &data[21] : nullptr; }
        double GR_TDCR_X1(const int& i) { return (data.count(22) > 0) ? data[22][i] : BAD_NUM; }
        vector<double>* GR_TDCR_X1() { return (data.count(22) > 0) ? &data[22] : nullptr; }
        double GR_TDCR_U1(const int& i) { return (data.count(23) > 0) ? data[23][i] : BAD_NUM; }
        vector<double>* GR_TDCR_U1() { return (data.count(23) > 0) ? &data[23] : nullptr; }
        double GR_TDCR_X2(const int& i) { return (data.count(24) > 0) ? data[24][i] : BAD_NUM; }
        vector<double>* GR_TDCR_X2() { return (data.count(24) > 0) ? &data[24] : nullptr; }
        double GR_TDCR_U2(const int& i) { return (data.count(25) > 0) ? data[25][i] : BAD_NUM; }
        vector<double>* GR_TDCR_U2() { return (data.count(25) > 0) ? &data[25] : nullptr; }
        double LAS_RAYID(const int& i) { return (data.count(26) > 0) ? data[26][i] : BAD_NUM; }
        vector<double>* LAS_RAYID() { return (data.count(26) > 0) ? &data[26] : nullptr; }
        double LAS_RF(const int& i) { return (data.count(27) > 0) ? data[27][i] : BAD_NUM; }
        vector<double>* LAS_RF() { return (data.count(27) > 0) ? &data[27] : nullptr; }
        double LAS_X(const int& i) { return (data.count(28) > 0) ? data[28][i] : BAD_NUM; }
        vector<double>* LAS_X() { return (data.count(28) > 0) ? &data[28] : nullptr; }
        double LAS_Y(const int& i) { return (data.count(29) > 0) ? data[29][i] : BAD_NUM; }
        vector<double>* LAS_Y() { return (data.count(29) > 0) ? &data[29] : nullptr; }
        double LAS_TH(const int& i) { return (data.count(30) > 0) ? data[30][i] : BAD_NUM; }
        vector<double>* LAS_TH() { return (data.count(30) > 0) ? &data[30] : nullptr; }
        double LAS_PH(const int& i) { return (data.count(31) > 0) ? data[31][i] : BAD_NUM; }
        vector<double>* LAS_PH() { return (data.count(31) > 0) ? &data[31] : nullptr; }
        double LAS_ADC(const int& i) { return (data.count(32) > 0) ? data[32][i] : BAD_NUM; }
        vector<double>* LAS_ADC() { return (data.count(32) > 0) ? &data[32] : nullptr; }
        double LAS_TDC(const int& i) { return (data.count(33) > 0) ? data[33][i] : BAD_NUM; }
        vector<double>* LAS_TDC() { return (data.count(33) > 0) ? &data[33] : nullptr; }
        double LAS_TPOS(const int& i) { return (data.count(34) > 0) ? data[34][i] : BAD_NUM; }
        vector<double>* LAS_TPOS() { return (data.count(34) > 0) ? &data[34] : nullptr; }
        double LAS_MADC(const int& i) { return (data.count(35) > 0) ? data[35][i] : BAD_NUM; }
        vector<double>* LAS_MADC() { return (data.count(35) > 0) ? &data[35] : nullptr; }
        double LAS_WIRE_X1(const int& i) { return (data.count(36) > 0) ? data[36][i] : BAD_NUM; }
        vector<double>* LAS_WIRE_X1() { return (data.count(36) > 0) ? &data[36] : nullptr; }
        double LAS_WIRE_U1(const int& i) { return (data.count(37) > 0) ? data[37][i] : BAD_NUM; }
        vector<double>* LAS_WIRE_U1() { return (data.count(37) > 0) ? &data[37] : nullptr; }
        double LAS_WIRE_V1(const int& i) { return (data.count(38) > 0) ? data[38][i] : BAD_NUM; }
        vector<double>* LAS_WIRE_V1() { return (data.count(38) > 0) ? &data[38] : nullptr; }
        double LAS_WIRE_X2(const int& i) { return (data.count(39) > 0) ? data[39][i] : BAD_NUM; }
        vector<double>* LAS_WIRE_X2() { return (data.count(39) > 0) ? &data[39] : nullptr; }
        double LAS_WIRE_U2(const int& i) { return (data.count(40) > 0) ? data[40][i] : BAD_NUM; }
        vector<double>* LAS_WIRE_U2() { return (data.count(40) > 0) ? &data[40] : nullptr; }
        double LAS_WIRE_V2(const int& i) { return (data.count(41) > 0) ? data[41][i] : BAD_NUM; }
        vector<double>* LAS_WIRE_V2() { return (data.count(41) > 0) ? &data[41] : nullptr; }
        double LAS_TDCR_X1(const int& i) { return (data.count(42) > 0) ? data[42][i] : BAD_NUM; }
        vector<double>* LAS_TDCR_X1() { return (data.count(42) > 0) ? &data[42] : nullptr; }
        double LAS_TDCR_U1(const int& i) { return (data.count(43) > 0) ? data[43][i] : BAD_NUM; }
        vector<double>* LAS_TDCR_U1() { return (data.count(43) > 0) ? &data[43] : nullptr; }
        double LAS_TDCR_V1(const int& i) { return (data.count(44) > 0) ? data[44][i] : BAD_NUM; }
        vector<double>* LAS_TDCR_V1() { return (data.count(44) > 0) ? &data[44] : nullptr; }
        double LAS_TDCR_X2(const int& i) { return (data.count(45) > 0) ? data[45][i] : BAD_NUM; }
        vector<double>* LAS_TDCR_X2() { return (data.count(45) > 0) ? &data[45] : nullptr; }
        double LAS_TDCR_U2(const int& i) { return (data.count(46) > 0) ? data[46][i] : BAD_NUM; }
        vector<double>* LAS_TDCR_U2() { return (data.count(46) > 0) ? &data[46] : nullptr; }
        double LAS_TDCR_V2(const int& i) { return (data.count(47) > 0) ? data[47][i] : BAD_NUM; }
        vector<double>* LAS_TDCR_V2() { return (data.count(47) > 0) ? &data[47] : nullptr; }

        void HistDefCheckSum(int pid=0) {

          string record = "5  def   DST_VAR = \"QTC_TRAILING_CH:QTC_TRAILING_TDC:QTC_LEADING_CH:QTC_LEADING_TDC:GR_RAYID:GR_RF:GR_X:GR_Y:GR_TH:GR_PH:GR_ADC:GR_TDC:GR_TPOS:GR_MADC:GR_WIRE_X1:GR_WIRE_U1:GR_WIRE_X2:GR_WIRE_U2:GR_WTDC_X1:GR_WTDC_U1:GR_WTDC_X2:GR_WTDC_U2:GR_TDCR_X1:GR_TDCR_U1:GR_TDCR_X2:GR_TDCR_U2:LAS_RAYID:LAS_RF:LAS_X:LAS_Y:LAS_TH:LAS_PH:LAS_ADC:LAS_TDC:LAS_TPOS:LAS_MADC:LAS_WIRE_X1:LAS_WIRE_U1:LAS_WIRE_V1:LAS_WIRE_X2:LAS_WIRE_U2:LAS_WIRE_V2:LAS_TDCR_X1:LAS_TDCR_U1:LAS_TDCR_V1:LAS_TDCR_X2:LAS_TDCR_U2:LAS_TDCR_V2\"";
          string line, dstvars = "DST_VAR";

          ifstream histdef ("/projects/ceclub/sullivan/cagragr/GRAnalyzer/hist.def");
          if (histdef.is_open())
          {
            while ( getline (histdef,line) )
            {
              if (line.find(dstvars) != string::npos) {
                break;
              }
            }
            histdef.close();
          }
          line.erase(line.begin(), std::find_if(line.begin(), line.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
          line.erase(std::find_if(line.rbegin(), line.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), line.end());
          fprintf(stderr,"%s\n",line.c_str());
          if (line == record) {
            return;
          } else {
            fprintf(stderr,"%s vs %s\n",record.c_str(),line.c_str());
            const char* msg =
              "\n###################### RUNTIME ERROR ######################\n"
              "DST_VAR line in hist.def has changed since compilation.\n"
              "Please recompile the analyzer before attempting to run.\n"
              "###########################################################\n";
            fprintf(stderr,"%s",msg);
            kill(pid,SIGINT);
            throw runtime_error(msg);
          }
        }
        //[[[end]]]


  private:
        long fTimestamp;
  public:
	ClassDef(RCNPEvent,1);                // This is a ROOT macro that is needed for creating a ROOT dictionary
                                              // # corresponds to version number, if = 0, events will NOT be saved
};

class DSTMap : public TObject {

  map<string,int> strtoint;
  map<int,string> inttostr;

  public:
        DSTMap() {;}
        virtual ~DSTMap() {;}

        int operator()(const string& var) {return strtoint.at(var);}
        string operator()(const int& varnum) {return inttostr.at(varnum);}
        void Set(const int& varnum, const string& var) {
	  strtoint[var] = varnum;
	  inttostr[varnum] = var;
	}

        ClassDef(DSTMap,1);
};
