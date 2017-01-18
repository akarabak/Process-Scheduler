#ifndef resources_h
#define resources_h

#include <string>
#include <deque>
#include <iostream>

#include <unordered_map>

struct RCB {
	std::string RID;
	int status;
	int maxResources; //maximum number of resources RCB has i.e. 4 for R4
	std::deque<std::string> waitingList;
};


RCB &getRCB(std::string RID); //returns reference to the resource
void printResourcesWaitingList(); //helper function

#endif