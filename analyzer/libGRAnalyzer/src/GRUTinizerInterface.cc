#include "GRUTinizerInterface.h"
#include "GRAnalyzer.h"
#include "RCNPEvent.h"
#include <chrono>
#include <thread>

std::atomic<int>* TerminateSig;
void StartGRAnalyzer(const char* filename, std::atomic<int>* terminate,std::function<void(RCNPEvent*)> func, bool save_tree)
{
	TerminateSig = terminate;
	RCNPInterface::Instance().SaveTree(save_tree);

	Init(func);
	start_analyzer(filename);
}

int CheckTerminateSignal() {
	return int(*TerminateSig);
}

extern "C" int CheckSignal () {
	return CheckTerminateSignal();
}
