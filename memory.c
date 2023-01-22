#include "memory.h"

static float memory[memorySize];
static char* memOutAddress; /* will be saved during init*/



/* Notice that the instructions are ALSO saved in the memory (in the (InstructionsNum) first lines)
after initMemory, initInstructions would be set so it can parse instructions from there*/
void initMemory(char* dmemin, char* dmemout) {
    char line[MEMORY_LINE_LEN];
    int counter = 0;
    int i;
    int val;
    float floatVal;
    FILE* memFile = fopen(dmemin,"r");
    if (!memFile) {
        printf("error in initMemory in reading dmemin: %s\n", dmemin);
        exit(1);
    }
    while (fgets(line, MEMORY_LINE_LEN, memFile)) {
        line[8] = '\0';
        int sc = sscanf(line,"%x", &val);
        floatVal = getUnionFloatFormat(val);
        memory[counter] = floatVal;
        counter++;
    }
    if (counter < memorySize - 1) {
        // need to fill the rest of the memory
        for (i = counter; i < memorySize; i++) {
            memory[i] = 0.0;
        }
    }
    memLastIndex = counter;
    fclose(memFile);
    memOutAddress = dmemout; //save dmemout in variable
}

float readMemory(int address) {
    if (address >= memorySize || address < 0) {
        printf("error in readMemory with address number: %i\n", address);
        exit(1);
    }
    return memory[address];
}

void writeMemory(int address, float value) {
    if (address >= memorySize || address < 0) {
        printf("error in writeMemory with address number: %i", address);
        exit(1);
    }
    memory[address] = value;
}


void exitMemory() {
    int lastIndex;
    int i;
    lastIndex = findMemLastIndex(); //last index that doesn't equal 0
    FILE* dmemoutFile = fopen(memOutAddress, "w");
    if (!dmemoutFile) {
        printf("error in exitMemory in writing to dmemout: %s\n", memOutAddress);
        exit(1);
    }
    for(i = 0; i <= lastIndex; i++) {
        unsigned int val = getUnionFormat(memory[i]);
		fprintf(dmemoutFile, "%08x\n", val);
    }
    fclose(dmemoutFile);
}

int getMemLastIndex() {
    return memLastIndex;
}

int findMemLastIndex() {
    int lastIndex;
    int i;
    lastIndex = 0;
    for (i = 0; i < memorySize; i++) {
        if (memory[i] != 0) {
            lastIndex = i;
        }
    }
    return lastIndex;
}

unsigned int getUnionFormat(float memind) {
    union {
        float f;
        unsigned int i;
    } u;
    u.f = memind;
    return u.i;
}

float getUnionFloatFormat(int memVal) {
    union {
        int i;
        float f;
    } u;
    u.i = memVal;
    float f = u.f;
    return f;
}