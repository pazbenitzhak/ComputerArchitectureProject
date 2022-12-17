

#define true 1


static int instructionQueueTop; /*points to the current instruction to be handled*/
static int instructionQueueBottom; /* points to instructionQueueTop+15 to keep
the instruction Queue at length 16 at most*/
static char* traceInst;
static int instructionsNum; /* to be initialized in initInstructions*/

struct instruction
{
    float inst; /*the instruction itself*/
    int pc;
    int unit; /*TODO: make sure it needs to be int */
    int cycleIssued;
    int cycleReadOperands;
    int cycleExecuteEnd;
    int cycleWriteResult;
};




void initInstructions(char* path);
void translateInstruction(int index);
float readInstructionInst(struct instruction inst);
void writeInstructionInst(struct instruction inst, float value);
void writeInstructionPC(struct instruction inst, int value);
int readInstructionPC(struct instruction inst);
void writeInstructionUnit(struct instruction inst, int value);
int readInstructionUnit(struct instruction inst);
void writeInstructionCycleIssued(struct instruction inst, int value);
int readInstructionCycleIssued(struct instruction inst);
void writeInstructionCycleReadOperands(struct instruction inst, int value);
int readInstructionCycleReadOperands(struct instruction inst);
void writeInstructionCycleExecuteEnd(struct instruction inst, int value);
int readInstructionCycleExecuteEnd(struct instruction inst);
void writeInstructioncycleWriteResult(struct instruction inst, int value);
int readInstructioncycleWriteResult(struct instruction inst);
void exitInstructions();