#include <stdio.h>
#include <math.h>
#include <stdlib.h>


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
float getUnionFloatFormat(int memVal);