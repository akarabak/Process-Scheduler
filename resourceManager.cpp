#include "resourceManager.h"

using namespace std;

extern unordered_map<std::string, RCB> resources;


void acquireResource(PCB &process, RCB &r, int amount) {
	r.status -= amount; //remove from RCB
	process.resources[r.RID].resourcesAcquired += amount; //add to process acquired
	process.resources[r.RID].resourcesRequested -= amount; //remove from amount needed for process
	processes[process.PID] = process; //update processes map
}


bool Request(string RID, int amount) { //assign resource to running process
	if (resources.find(RID) == resources.end())
		return false;
	RCB &r = resources[RID];
	running.resources[r.RID].resourcesRequested = amount;
	if (r.status >= amount) {
		acquireResource(running, r, amount);
	}

	else if (r.maxResources - running.resources[r.RID].resourcesAcquired < amount) {
		return false;
	}
	else {
		running.status.Type = "blocked";
		running.status.List = r.RID;
		processes[running.PID] = running;
		remove_ReadyList(running);
		r.waitingList.push_back(running.PID);
	}
	Scheduler();
	return true;
}

void releaseResource(PCB &process, RCB &r, int amount) {
	process.resources[r.RID].resourcesAcquired -= amount;
	r.status += amount;
	processes[process.PID] = process;
}


bool Release(string RID, int amount, bool scheduler, PCB &process) {
	if (resources.find(RID) == resources.end())
		return false;
	RCB &r = resources[RID];
	if (process.resources[r.RID].resourcesAcquired < amount)
		return false;
	releaseResource(process, r, amount);

	if (!r.waitingList.empty()) {
		PCB process = processes[r.waitingList.front()];
		int requested = process.resources[r.RID].resourcesRequested;
		if (requested <= r.status) {
			r.waitingList.pop_front();
			process.status.Type = "ready";
			process.status.List = "ready list";
			acquireResource(process, r, requested);
			processes[process.PID] = process;
			insert_ReadyList(process);
		}
	}
	if (scheduler)
		Scheduler();
	return true;
}

void printResources() {
	for (auto i = resources.begin(); i != resources.end(); i++) {
		cout << "Available for R1: " << i->second.status << endl;
	}
}