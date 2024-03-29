#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "units.h"
#include "clock.h"

#define true 1
#define ADD 0
#define SUB 1
#define MULT 2
#define DIV 3
#define LOAD 4
#define STORE 5
#define INSTRUCTION_QUEUE_LEN 16

static char* traceInst;
static int instructionsNum; /* to be initialized in initInstructions*/
static int* instructionsMode;
static float* instructionsS0; /* saves the s0 register values in the read operands stage*/
static float* instructionsS1; /* saves the s1 register values in the read operands stage*/
static float* instructionsDST; /* saves the dest register values after the operation in the execute stage*/
static short* fetchQueue;
static int fetchCount;
/* for each instruction: 0 - instruction not issued yet
1 - instruction issued
2 - instruction read operands done
3 - instruction executed
4 - instruction wrote result*/
struct instruction* instructions; 

struct instruction
{
    float inst; /*the instruction itself*/
    int pc;
    int unit;
    unsigned long cycleIssued;
    unsigned long cycleReadOperands;
    unsigned long cycleExecuteEnd;
    unsigned long cycleWriteResult;
};




void initInstructions(char* path);
int getInstructionsNum();
int* translateInstruction(int index);
float readInstructionInst(int index);
void writeInstructionInst(int index, float value);
void writeInstructionPC(int index, int value);
int readInstructionPC(int index);
void writeInstructionUnit(int index, int value);
int readInstructionUnit(int index);
void writeInstructionCycleIssued(int index, unsigned long value);
unsigned long readInstructionCycleIssued(int index);
void writeInstructionCycleReadOperands(int index, unsigned long value);
unsigned long readInstructionCycleReadOperands(int index);
void writeInstructionCycleExecuteEnd(int index, unsigned long value);
unsigned long readInstructionCycleExecuteEnd(int index);
void writeInstructioncycleWriteResult(int index, unsigned long value);
unsigned long readInstructioncycleWriteResult(int index);
int readInstructionMode(int index);
void incrementInstructionMode(int index);
float readS0ByInstruction(int index);
void writeS0ByInstruction(int index, float value);
float readS1ByInstruction(int index);
void writeS1ByInstruction(int index, float value);
float readDSTByInstruction(int index);
void writeDSTByInstruction(int index, float value);
int issueInstruction(int index, int* instInfo);
int readOpInstruction(int index);
int executeInstruction(int index);
void writeResultInstruction(int index);
void fetchInstruction(int index);
void incrementFetchQueue(int index);
int findInstNumFromMem();
void exitInstructions();

#endif