#include "registers.h"


static float registers[registersSize];
static int registerResultsStatus[registersSize];
/* TODO: create mapping from unit name to int*/
static int isRegisterUsed[registersSize]; /* 1 if used, 0 else*/
static char* regoutAddress;



void initRegisters(char* path) {
    int i;
    for (i=0;i<registersSize;i++) {
        registers[i] = (float) i;
        registerResultsStatus[i] = -1; /* because it represents number of unit*/
        isRegisterUsed[i] = 0;
    }
    regoutAddress = path;
}

float readRegister(int address) {
    return registers[address];
}

void writeRegister(int address, float value) {
    registers[address] = value;
}

float readRegisterStatus(int register_index) {
    return registerResultsStatus[register_index];
}

void writeRegisterStatus(int register_index, int value) {
    registerResultsStatus[register_index] = value;
}

int isRegUsed(int register_index) {
    return isRegisterUsed[register_index]; /* 1 iff register is used*/
}

void flipRegUsed(int register_index) {
    isRegisterUsed[register_index] = 1 - isRegisterUsed[register_index];
}

/* need to write to a file named "regout.txt" the registers' content*/
void exitRegisters() {
     int lastIndex;
    int i;
    FILE* regoutFile = fopen(regoutAddress, "w");
    if (!regoutFile) {
        printf("error in exitMemory in writing to dmemout: %s\n", regoutAddress);
        exit(1);
    }
    for(i = 0; i <= registersSize; i++) {
		fprintf(regoutFile, "%08X\n", registers[i]);
        /* TODO: make sure we write the values correctly in hexa-float fashion*/
    }
    fclose(regoutFile);
}