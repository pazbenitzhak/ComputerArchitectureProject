#include "instructions.h"
#include "memory.h" /* TODO: check in the end how to correctly link everything*/


static int instructionQueueTop; /*points to the current instruction to be handled*/
static int instructionQueueBottom; /* points to instructionQueueTop+15 to keep
the instruction Queue at length 16 at most*/
static char* traceInst;
static int instructionsNum; /* to be initialized in initInstructions*/


void initInstructions(char* path) {
    traceInst = path;
    instructionsNum = findInstNumFromMem();
}

int findInstNumFromMem() {
    int counter;
    float memVal;
    counter = 0;
    /* TODO: check if we can assume that there would be in any case values in MEMORY*/
    while (true)
    {
        memVal = readMemory(counter);
        int* ptr = (int *) (&memVal);
        *ptr >>= 28; 
        if ((*ptr & 1111)!=0)  {
            break;
        }
        counter++;
    }
    return counter+1;
    
}

void translateInstruction(int index);

float readInstructionInst(struct instruction inst) {
    return inst.inst;
}

void writeInstructionInst(struct instruction inst, float value) {
    inst.inst = value;
}

void writeInstructionPC(struct instruction inst, int value) {
    inst.pc = value;
}

int readInstructionPC(struct instruction inst) {
    return inst.pc;
}

void writeInstructionUnit(struct instruction inst, int value) {
    inst.unit = value;
}

int readInstructionUnit(struct instruction inst) {
    return inst.unit;
}

void writeInstructionCycleIssued(struct instruction inst, int value) {
    inst.cycleIssued = value;
}

int readInstructionCycleIssued(struct instruction inst) {
    return inst.cycleIssued;
}

void writeInstructionCycleReadOperands(struct instruction inst, int value) {
    inst.cycleReadOperands = value;
}

int readInstructionCycleReadOperands(struct instruction inst) {
    return inst.cycleReadOperands;
}

void writeInstructionCycleExecuteEnd(struct instruction inst, int value) {
    inst.cycleExecuteEnd = value;
}

int readInstructionCycleExecuteEnd(struct instruction inst) {
    return inst.cycleExecuteEnd;
}

void writeInstructioncycleWriteResult(struct instruction inst, int value) {
    inst.cycleWriteResult = value;
}

int readInstructioncycleWriteResult(struct instruction inst) {
    return inst.cycleWriteResult;
}

void exitInstructions();