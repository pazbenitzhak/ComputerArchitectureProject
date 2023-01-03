#include "stdio.h"
#include "stdlib.h"

#define true 1
#define ADD 0
#define SUB 1
#define MULT 2
#define DIV 3
#define LOAD 4
#define STORE 5

static int instructionQueueTop; /*points to the current instruction to be handled*/
static int instructionQueueBottom; /* points to instructionQueueTop+15 to keep
the instruction Queue at length 16 at most*/
static char* traceInst;
static int instructionsNum; /* to be initialized in initInstructions*/
static int* instructionsMode;
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
int issueInstruction(int index, int* instInfo);
void readOpInstruction(int index);
void executeInstruction(int index);
void writeResultInstruction(int index);
void exitInstructions();