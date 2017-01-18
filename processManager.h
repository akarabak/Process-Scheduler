#ifndef processmanager_h
#define processmanager_h

#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <deque>
#include "resourceManager.h"

struct resourceStatus { //struct used for resource field for PCB
	int resourcesAcquired;
	int resourcesRequested;
};

struct PCB {
	std::string PID;
	struct {
		std::string Type;
		std::string List;
	}status;

	struct {
		std::string parent = "";
		std::string created = "";
		std::string youngest = "";
		std::string oldest = "";
	} creationTree;

	int priority;
	std::unordered_map < std::string, resourceStatus > resources;
};

extern std::unordered_map<std::string, PCB> processes; //global map defined in processManager.cpp holding all non terminated processes
extern PCB running; //global variable representing currently running process defined in processManager.cpp

void init();
void Scheduler();
void Timeout();
bool Create(std::string PID, int priority); //return false on failure
bool Destroy(std::string PID); //returns false on failure

//internally used functions
bool insert_ReadyList(PCB process); //returns false on failure
bool remove_ReadyList(PCB process); //returns false on failure
void fill_in_creationTree(PCB process);
void removeFromDeque(std::deque<std::string> &deq, std::string PID);
void reassignParent(std::string PID);
void killTree(std::string PID, bool remYoungest);
void printCreationTree();
void printReadyList();

#endif
