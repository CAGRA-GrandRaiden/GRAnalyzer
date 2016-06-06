#include <functional>

class RCNPEvent;

void StartGRAnalyzer(const char* filename, std::function<void(RCNPEvent*)> func);
