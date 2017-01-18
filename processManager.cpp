#include "processManager.h"
using namespace std;

PCB running;
extern unordered_map<std::string, RCB> resources;

struct {
	deque<string> priority0;
	deque<string> priority1;
	deque<string> priority2;
} readyList;

unordered_map<string, PCB> processes;

void Scheduler() {
	//finding the highest priority process
	string highest;
	if (!readyList.priority2.empty()) 
		highest = readyList.priority2.front();
	else if (!readyList.priority1.empty()) 
		highest = readyList.priority1.front();
	else if (!readyList.priority0.empty())
		highest = readyList.priority0.front();

	if (running.priority < processes[highest].priority || running.status.Type != "running") {
		if (running.status.Type == "running") { //if there is currently running process, it is on top of queue. Set it to ready
			processes[running.PID].status.Type = "ready";
		}
		processes[highest].status.Type = "running";
		running = processes[highest];
	}
	cout << running.PID << " ";
}

struct creationTreeNode{
	int priority;
	string parent = "";
	string created = "";
	string youngest = "";
	string oldest = "";
};

void init() {
	//clear all lists, hash table
	running = { "" };
	processes.clear();
	readyList.priority0.clear();
	readyList.priority1.clear();
	readyList.priority2.clear();

	for (auto i = resources.begin(); i != resources.end(); i++) {
		resources[i->first].status = i->second.maxResources;
		resources[i->first].waitingList.clear();
	}
	Create("init", 0); //recreate init process
}

void fill_in_creationTree(PCB process) {
	processes[process.PID] = process;
	
	if (running.PID == "")
		return;
	processes[process.PID].creationTree.parent = running.PID;
	if (running.creationTree.created == "") { //if parent process has not created processes yet, assign child process to its created field
		running.creationTree.created = process.PID;
		processes[running.PID] = running;
	}
	else { //else find the closes child that has not assigned younger child
		string child = running.creationTree.created;
		while (processes[child].creationTree.youngest != "") {
			child = processes[child].creationTree.youngest;
		}
		processes[child].creationTree.youngest = process.PID;
		processes[process.PID].creationTree.oldest = child;
	}
}

bool insert_ReadyList(PCB process) { //returns true if able to insert to readyList
	if (process.priority == 0) 
		readyList.priority0.push_back(process.PID);		
	else if (process.priority == 1)
		readyList.priority1.push_back(process.PID);
	else if (process.priority == 2)
		readyList.priority2.push_back(process.PID);
	else
		return false;
	return true;
}

void removeFromDeque(deque<string> &deq, string PID) {
	if (deq.front() == PID) {
		deq.pop_front();
		return;
	}
    deque<string>::iterator i;
	for (i = deq.begin(); *i != PID; i++);
	deq.erase(i);
}

bool remove_ReadyList(PCB process) {
	if (process.priority == 0)
		removeFromDeque(readyList.priority0, process.PID);
	else if (process.priority == 1)
		removeFromDeque(readyList.priority1, process.PID);
	else if (process.priority == 2)
		removeFromDeque(readyList.priority2, process.PID);
	else
		return false;
	return true;
}

bool Create(string PID, int priority) {
	if (processes.find(PID) != processes.end()) //check if process already exists
		return false;

	PCB process;
	process.PID = PID;
	process.priority = priority;
	process.status.Type = "ready";
	process.status.List = "ready list";

	if (insert_ReadyList(process)) { //insertion fails if priority is not 0, 1, or 2
		fill_in_creationTree(process);
		Scheduler();
		return true;
	}
	return false;
}

void reassignParent(string PID) {
	string process = processes[PID].creationTree.oldest;
	if (process != "") { //reassign oldest sibling youngest field
		processes[process].creationTree.youngest = processes[PID].creationTree.youngest;
		processes[processes[PID].creationTree.youngest].creationTree.oldest = process;
	}
	else { //reassign parent's created field
		processes[processes[PID].creationTree.parent].creationTree.created = processes[PID].creationTree.youngest;
	}
}

void killTree(string PID, bool remYoungest = true) {
	if (PID == "") //base case
		return;
	if (running.PID == PID) //if running process is getting terminated change global's values
		running.status.Type = "terminated";
	processes[PID].status.Type = "terminated";
	PCB process = processes[PID];

	if (remYoungest) //youngest children should be removed only for created by process processes
		killTree(process.creationTree.youngest);
	killTree(process.creationTree.created);

	if (process.status.List == "ready list")
		remove_ReadyList(process);
	else {
		deque<string> &deq = getRCB(process.status.List).waitingList;
		deque<string>::iterator i;
		for (i = deq.begin(); *i != PID; i++);
		deq.erase(i);
	}

	//Releasing resources
	for (unordered_map<string, resourceStatus>::iterator i = process.resources.begin(); i != process.resources.end(); i++) {
		Release(i->first, process.resources[i->first].resourcesAcquired, false, processes[PID]);
	}

	processes.erase(PID);
}

bool Destroy(string PID) {
	if (processes.find(PID) == processes.end())
		return false;

	reassignParent(PID);
	killTree(PID, false);
	
	Scheduler();
	return true;
}

void Timeout() {
	remove_ReadyList(running);
	running.status.Type = "ready";
	processes[running.PID] = running;
	insert_ReadyList(running);
	Scheduler();
}

void printCreationTree() { //helper function
	for (unordered_map<string, PCB>::iterator key = processes.begin(); key != processes.end(); key++) {
		cout << "Process name: " << key->first << endl;
		cout << "  Priority: " << key->second.priority << endl;
		cout << "  Parent process: " << key->second.creationTree.parent << endl;
		cout << "  Process created: " << key->second.creationTree.created << endl;
		cout << "  Oldest sibling: " << key->second.creationTree.oldest << endl;
		cout << "  Youngest sibling: " << key->second.creationTree.youngest << endl;
		
		for (unordered_map<string, resourceStatus>::iterator i = key->second.resources.begin(); i != key->second.resources.end(); i++) {
			cout << i->first << " : " << key->second.resources[i->first].resourcesAcquired << endl;
		}
	}
}

void printReadyList() { //helper function
	string PID;
	cout << "Priority 2 processes in ready list: " << endl;
	for (int i = 0; i < readyList.priority2.size(); i++) {
		PID = readyList.priority2[i];
		cout << "  " << PID << " : " << processes[PID].status.Type << " ";
	}
	cout << endl;

	cout << "Priority 1 processes in ready list: " << endl;
	for (int i = 0; i < readyList.priority1.size(); i++) {
		PID = readyList.priority1[i];
		cout << "  " << PID << " : " << processes[PID].status.Type << " ";
	}
	cout << endl;

	cout << "Priority 0 processes in ready list: " << endl;
	for (int i = 0; i < readyList.priority0.size(); i++) {
		PID = readyList.priority0[i];
		cout << "  " << PID << " : " << processes[PID].status.Type << " ";	
	}
	cout << endl;
}