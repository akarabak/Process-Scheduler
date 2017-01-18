#include "resources.h"

using namespace std;

unordered_map<string, RCB> resources = {
	{"R1", RCB{"R1", 1, 1}},
	{"R2", RCB{"R2", 2, 2}},
	{"R3", RCB{"R3", 3, 3}},
	{"R4", RCB{"R4", 4, 4}}
};

RCB &getRCB(string RID) {
	return resources[RID];
}

void printResourcesWaitingList() {
	cout << "Waiting lists" << endl;
	for (auto i = resources.begin(); i != resources.end(); i++) {
		cout << "For " << i->first << ": " << endl;
		for (int index = 0; index < i->second.waitingList.size(); i++) {
			cout << i->second.waitingList[index] << endl;
		}
	}

}