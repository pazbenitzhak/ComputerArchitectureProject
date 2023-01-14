#include <stdio.h>
#include "memory.h"

#define registersSize 16
#define used 1
#define available 0

static float registers[registersSize];
static int registerResultsStatus[registersSize];
static int isRegisterUsed[registersSize]; /* 1 if used, meaning "No", 0 if not used, meaning "Yes"*/
static char* regoutAddress;

void initRegisters(char* path);
float readRegister(int address);
void writeRegister(int address, float value);
float readRegisterStatus(int register_index);
void writeRegisterStatus(int register_index, int value);
int isRegUsed(int register_index);
void flipRegUsed(int register_index);
void exitRegisters();