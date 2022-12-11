#include <stdio.h>
#include <math.h>


#define memorySize 4096
#define MEMORY_LINE_LEN 8


static float memory[memorySize];
static char* memOutAddress; /* will be saved during init*/

float readMemory(int address);
void writeMemory(int address, float value);
void initMemory();
void exitMemory();
int findMemLastIndex();