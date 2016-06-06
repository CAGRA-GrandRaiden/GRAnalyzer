#include "GRUTinizerInterface.h"
#include "GRAnalyzer.h"
#include "RCNPEvent.h"

void StartGRAnalyzer(const char* filename, std::function<void(RCNPEvent*)> func)
{
	Init(func);
	start_analyzer(filename);
}
