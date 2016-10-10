#ifndef _RCNPEVENT_H_
#define _RCNPEVENT_H_

#include "TObject.h"

#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <algorithm>
#include <functional>
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
        RCNPEvent(RCNPEvent& other)
	  : TObject(other) {
	  fTimestamp = other.GetTimestamp();
	  data.swap(other.data);
        }
        map<int,vector<double> > data;        // The underlying map from string to vector of data
	void Clear() {data.clear();}

        long GetTimestamp() { return fTimestamp; }
        void SetTimestamp(const long& ts) { fTimestamp = ts; }
       // TODO: make HistCheck static

        //[[[cog import rootalyze as root; root.definitions() ]]]
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

#ifndef __CINT__
static std::function<void(RCNPEvent*)> StoreData;
extern void Init(std::function<void(RCNPEvent*)> func);
#endif

#endif
