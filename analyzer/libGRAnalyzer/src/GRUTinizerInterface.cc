#include "GRUTinizerInterface.h"
#include "GRAnalyzer.h"
#include "RCNPEvent.h"
#include <chrono>
#include <thread>

std::atomic<int>* TerminateSig;
void StartGRAnalyzer(const char* filename, std::atomic<int>* terminate,std::function<void(RCNPEvent*)> func, bool save_tree, bool status)
{
	TerminateSig = terminate;
	RCNPInterface::Instance().SaveTree(save_tree);
	RCNPInterface::Instance().ShowStatus(status);

	Init(func);
	start_analyzer(filename);
}

int CheckTerminateSignal() {
	return int(*TerminateSig);
}

extern "C" int CheckSignal () {
	return CheckTerminateSignal();
}
int Status() {
	return int(RCNPInterface::Instance().ShowStatus());
}

extern "C" int ShowStatus () {
	return Status();
}
