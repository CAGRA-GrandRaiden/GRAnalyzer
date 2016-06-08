#include "GRUTinizerInterface.h"
#include "GRAnalyzer.h"
#include "RCNPEvent.h"
#include <chrono>
#include <thread>

void StartGRAnalyzer(const char* filename, std::function<void(RCNPEvent*)> func)
{
	Init(func);
	start_analyzer(filename);
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000000000));
}
