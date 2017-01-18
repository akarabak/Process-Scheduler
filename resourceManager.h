#ifndef resourcemanager_h
#define resourcemanager_h

#include <string>
#include "processManager.h"
#include "resources.h"

struct PCB; //forward declaring struct PCB
extern PCB running;

bool Request(std::string RID, int amount);
bool Release(std::string RID, int amount, bool scheduler = true, PCB &process = running);
void releaseResource(PCB &process, RCB &r, int amount); //used by Release()
void acquireResource(PCB &process, RCB &r, int amount); //used by Request()
void printResources(); //helper function

#endif