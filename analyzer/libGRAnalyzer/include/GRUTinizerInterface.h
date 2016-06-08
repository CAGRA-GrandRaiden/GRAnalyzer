#include <functional>
#include <atomic>

class RCNPEvent;
class RCNPInterface;

void StartGRAnalyzer(const char* filename, std::atomic<int>* terminate,std::function<void(RCNPEvent*)> func, bool save_tree=false);


class RCNPInterface {
	bool save_rcnptree;
public:
	void SaveTree(bool set) { save_rcnptree = set; }
	bool SaveTree() { return save_rcnptree; }





//// casual user can ignore below code ////////////////////////////////
        static RCNPInterface& Instance()
		{
			static RCNPInterface    instance;
			return instance;
		}
private:
        RCNPInterface() {};
public:
        RCNPInterface(RCNPInterface const&)               = delete;
        void operator=(RCNPInterface const&)  = delete;
};
