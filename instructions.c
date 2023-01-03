#include "instructions.h"
#include "memory.h" /* TODO: check in the end how to correctly link everything*/
#include "registers.h"
#include "units.h"
#include "clock.h"


static int instructionQueueTop; /*points to the current instruction to be handled*/
static int instructionQueueBottom; /* points to instructionQueueTop+15 to keep
the instruction Queue at length 16 at most*/
static char* traceInst;
static int instructionsNum; /* to be initialized in initInstructions*/


void initInstructions(char* path) {
    traceInst = path;
    instructionsNum = findInstNumFromMem();
    instructions = (struct instruction)* malloc(instructionsNum*sizeof(struct instruction));
    instructionsMode = (int)* calloc(instructionsNum,sizeof(int)); /* initialize zeros*/
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

int* translateInstruction(int index) {
    int parsedIns[5];
    float instr;
    instr = readMemory(index);
    int* ptr1 = (int* ) (&instr);
    *ptr1 >>= 24;
    int opcode = *ptr1 && 1111;
    parsedIns[0] = opcode;
    int* ptr2 = (int*) (&instr);
    *ptr2 >>= 20;
    int dst = *ptr2 && 1111;
    parsedIns[1] = dst;
    int* ptr3 = (int*) (&instr);
    *ptr3 >>= 16;
    int s0 = *ptr3 && 1111;
    parsedIns[2] = s0;
    int* ptr4 = (int*) (&instr);
    *ptr4 >>= 12;
    int s1 = *ptr4 && 1111;
    parsedIns[3] = s1;
    float imm = instr && 0x00000fff;
    parsedIns[4] = (int) imm;
    writeInstructionInst(index, instr); /* write it at this stage*/
    writeInstructionPC(index, index); /* pc is the instruction index in the memory array*/
    return parsedIns;
}

float readInstructionInst(int index) {
    return instructions[index].inst;
}

void writeInstructionInst(int index, float value) {
    instructions[index].inst = value;
}

void writeInstructionPC(int index, int value) {
    instructions[index].pc = value;
}

int readInstructionPC(int index) {
    return instructions[index].pc;
}

void writeInstructionUnit(int index, int value) {
    instructions[index].unit = value;
}

int readInstructionUnit(int index) {
    return instructions[index].unit;
}

void writeInstructionCycleIssued(int index, unsigned long value) {
    instructions[index].cycleIssued = value;
}

unsigned long readInstructionCycleIssued(int index) {
    return instructions[index].cycleIssued;
}

void writeInstructionCycleReadOperands(int index, iunsigned longnt value) {
    instructions[index].cycleReadOperands = value;
}

unsigned long readInstructionCycleReadOperands(int index) {
    return instructions[index].cycleReadOperands;
}

void writeInstructionCycleExecuteEnd(int index, unsigned long value) {
    instructions[index].cycleExecuteEnd = value;
}

unsigned long readInstructionCycleExecuteEnd(int index) {
    return instructions[index].cycleExecuteEnd;
}

void writeInstructioncycleWriteResult(int index, unsigned long value) {
    instructions[index].cycleWriteResult = value;
}

unsigned long readInstructioncycleWriteResult(int index) {
    return instructions[index].cycleWriteResult;
}

int issueInstruction(int index, int* instInfo) {
    /* handle WAW */
    int isDstOccupied; /* 1 if yes, 0 if not*/
    int opcode = instInfo[0];
    int dst = instInfo[1];
    int s0 = instInfo[2];
    int s1 = instInfo[3];
    int imm = instInfo[4];
    int unit;
    if (readRegisterStatus(dst)!=-1) {
        isDstOccupied = 1;
    }
    if (isDstOccupied) {
        return -1; /* need to wait due to WAW hazard*/
        /* TODO: need to make sure that we do not issue the next instructions as well*/
    }
    /* Dst is available, no risk of WAW*/
    unit = findAvailableUnitType(opcode);
    if (unit==-1) { /* no available unit*/
        return -2; /* */
    }
    writeInstructionUnit(unit);
    flipUnitBusy(unit); /* claim that the unit is busy*/
    writeRegisterStatus(dst,unit);
    writeInstructionCycleIssued(getCycle());
    return 1;
}

void readOpInstruction(int index);
void executeInstruction(int index);
void writeResultInstruction(int index);

void exitInstructions();