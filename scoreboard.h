#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include "clock.h"
#include "instructions.h"
#include "memory.h"
#include "registers.h"
#include "units.h"


void init(char* cfgPath, char* meminPath, char* memoutPath, char* regoutPath, char* traceinstPath, char* traceunitPath);
void exitScoreboard();
int findLastInstDone();

#endif