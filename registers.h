#include <stdio.h>

#define registersSize 16
#define used 1
#define free 0

static float registers[registersSize];
static int registerResultsStatus[registersSize];
/* TODO: create mapping from unit name to int*/
static int isRegisterUsed[registersSize]; /* 1 if used, 0 else*/
static char* regoutAddress;

void initRegisters();
float readRegister(int address);
void writeRegister(int address, float value);
float readRegisterStatus(int register_index);
void writeRegister(int register_index, float value);
int isRegUsed(int register_index);
void flipRegUsed(int register_index);
void exitRegisters();