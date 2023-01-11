#include "memory.h"

static float memory[memorySize];
static char* memOutAddress; /* will be saved during init*/

/* TODO: check whether memin is in the following form: instructions and then memory
or they could be combined*/


/* Notice that the instructions are ALSO saved in the memory (in the (InstructionsNum) first lines)
after initMemory, initInstructions would be set so it can parse instructions from there
TODO:
make sure that we get the correct values from file in FLOAT */
void initMemory(char* dmemin, char* dmemout) {
    char line[MEMORY_LINE_LEN];
    int counter = 0;
    int i;
    char* ptr;
    FILE* memFile = fopen(dmemin,"r");
    if (!memFile) {
        printf("error in initMemory in reading dmemin: %s\n", dmemin);
        exit(1);
    }
    while (fgets(line, MEMORY_LINE_LEN, memFile)) {
        line[8] = '\0';
        long long decVal = (int) strtol(line, &ptr, 0);
        decVal = decVal & 0xFFF;
        memory[counter] = decVal;
        counter++;
    }
    if (counter < memorySize - 1) {
        // need to fill the rest of the memory
        for (i = counter; i < memorySize; i++) {
            memory[i] = 0;
        }
    }
    fclose(memFile);
    memOutAddress = dmemout; //save dmemout in variable
}

/*TODO: make sure parallelism criteria is met*/
float readMemory(int address) {
    if (address >= memorySize || address < 0) {
        printf("error in readMemory with address number: %i", address);
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
		fprintf(dmemoutFile, "%08X\n", memory[i]);
        /* TODO: make sure we write the values correctly in hexa-float fashion*/
    }
    fclose(dmemoutFile);
}

int findMemLastIndex() {
    int lastIndex;
    int i;
    for (i = 0; i < memorySize; i++) {
        if (memory[i] != 0) {
            lastIndex = i;
        }
    }
    return lastIndex;
}