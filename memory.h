#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define memorySize 4096
#define MEMORY_LINE_LEN 10


static float memory[memorySize];
static char* memOutAddress; /* will be saved during init*/

float readMemory(int address);
void writeMemory(int address, float value);
void initMemory(char* dmemin, char* dmemout);
void exitMemory();
int findMemLastIndex();
unsigned int getUnionFormat(float memind);
float getUnionFloatFormat(uint32_t memVal);
float hexaToFloat(const char* hexa);
