#include <stdio.h>

#define registersSize 16
#define used 1
#define available 0

static float registers[registersSize];
static int registerResultsStatus[registersSize];
/* TODO: create mapping from unit name to int*/
static int isRegisterUsed[registersSize]; /* 1 if used=no, 0 if yes*/
static char* regoutAddress;

void initRegisters(char* path);
float readRegister(int address);
void writeRegister(int address, float value);
float readRegisterStatus(int register_index);
void writeRegisterStatus(int register_index, int value);
int isRegUsed(int register_index);
void flipRegUsed(int register_index);
void exitRegisters();