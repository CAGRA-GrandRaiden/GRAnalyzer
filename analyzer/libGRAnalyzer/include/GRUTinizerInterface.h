#include <functional>
#include <atomic>

class RCNPEvent;

void StartGRAnalyzer(const char* filename, std::atomic<int>* terminate,std::function<void(RCNPEvent*)> func);
