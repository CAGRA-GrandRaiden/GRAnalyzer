#include <functional>
#include <atomic>

class RCNPEvent;
class RCNPInterface;

void StartGRAnalyzer(const char* filename, std::atomic<int>* terminate,std::function<void(RCNPEvent*)> func, bool save_tree=false, bool status=false);


class RCNPInterface {
	bool save_rcnptree;
	bool show_status;
public:
	void SaveTree(bool set) { save_rcnptree = set; }
	bool SaveTree() { return save_rcnptree; }
	void ShowStatus(bool set) { show_status = set; }
	bool ShowStatus() { return show_status; }


//// casual user can ignore below code ////////////////////////////////
        static RCNPInterface& Instance() { static RCNPInterface instance; return instance; }
private:
        RCNPInterface() {};
public:
        RCNPInterface(RCNPInterface const&)               = delete;
        void operator=(RCNPInterface const&)  = delete;
};
