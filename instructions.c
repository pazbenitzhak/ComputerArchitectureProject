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

/* TODO: handle halt command*/


void initInstructions(char* path) {
    traceInst = path;
    instructionsNum = findInstNumFromMem();
    instructions = (struct instruction*) malloc(instructionsNum*sizeof(struct instruction));
    instructionsMode = (int*) calloc(instructionsNum,sizeof(int)); /* initialize zeros*/
    if (instructionsMode==NULL) {
        printf("memory allocation failed for instructionMode\n");
        exit(1);
    }
    instructionsS0 = (float*) calloc(instructionsNum,sizeof(float)); /* initialize zeros*/
    if (instructionsS0==NULL) {
        printf("memory allocation failed for instructionsS0\n");
        exit(1);
    }
    instructionsS1 = (float*) calloc(instructionsNum,sizeof(float)); /* initialize zeros*/
    if (instructionsS1==NULL) {
        printf("memory allocation failed for instructionsS1\n");
        exit(1);
    }
    instructionsDST = (float*) calloc(instructionsNum,sizeof(float)); /* initialize zeros*/
    if (instructionsDST==NULL) {
        printf("memory allocation failed for instructionsDST\n");
        exit(1);
    }
}

int findInstNumFromMem() {
    int counter;
    float memVal;
    int instNum;
    counter = 0;
    instNum = 0;
    while (true) /*TODO: change condition*/
    {
        memVal = readMemory(counter);
        int* ptr = (int *) (&memVal);
        *ptr >>= 28; 
        if ((*ptr & 1111)==0)  { /* it's an instruction*/
            instNum++;
        }
        counter++;
    }
    return instNum;
    
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

void writeInstructionCycleReadOperands(int index, unsigned long value) {
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

float readS0ByInstruction(int index) {
    return instructionsS0[index];
}

void writeS0ByInstruction(int index, float value) {
    instructionsS0[index] = value;
}

float readS1ByInstruction(int index) {
    return instructionsS1[index];
}

void writeS1ByInstruction(int index, float value) {
    instructionsS1[index] = value;
}

float readDSTByInstruction(int index) {
    return instructionsDST[index];
}

void writeDSTByInstruction(int index, float value) {
    instructionsDST[index] = value;
}

int readInstructionMode(int index) {
    return instructionsMode[index];
}

void incrementInstructionMode(int index) {
    instructionsMode[index] += 1;
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
    writeInstructionUnit(index,unit);
    /* write to unit the relevant information*/
    flipUnitBusy(unit); /* claim that the unit is busy*/
    writeUnitDest(unit,dst);
    writeUnitSrc0(unit,s0);
    writeUnitSrc1(unit,s1);
    writeUnitImm(unit, imm);
    /*end of writing to unit*/
    writeRegisterStatus(dst,unit);
    if (!isRegUsed) { /* right now we can't read dst because we await its result*/
        flipRegUsed(dst);
    }
    writeInstructionCycleIssued(index,getCycle());
    return 1;
}

int readOpInstruction(int index) { /* index of instruction*/
    int unit;
    int s0;
    int s1;
    int isReady;
    float s0_val;
    float s1_val;
    unit = readInstructionUnit(index);
    s0 = readUnitSrc0(unit);
    s1 = readUnitSrc0(unit);
    isReady = !(isRegUsed(s0) || isRegUsed(s1)); /* isReady = 1 then we can proceed*/
    if (!isReady) {
        return 0; /* NOT READY YET, KEEP DOING SOMETHING ELSE*/
    }
    /* we are ready to read*/
    s0_val = readRegister(s0);
    writeS0ByInstruction(index,s0_val);
    s1_val = readRegister(s1);
    writeS1ByInstruction(index,s1_val);

    flipRegUsed(s0); /* right now it's used, handle WAR*/
    flipRegUsed(s1); /* right now it's used, handle WAR*/

    writeUnitCurrDelay(unit, readUnitCurrDelay(unit)-1);
    writeInstructionCycleReadOperands(index,getCycle());
    return 1; /*SUCCESS*/
}

int executeInstruction(int index) {
    float s0_val;
    float s1_val;
    int unit;
    int type;
    int imm;
    unit = readInstructionUnit(index);
    imm = readUnitImm(unit);
    s0_val = readS0ByInstruction(s0);
    s1_val = readS1ByInstruction(s1);
    writeUnitCurrDelay(unit, readUnitCurrDelay(unit)-1); /*another cycle passed*/

    if (readUnitCurrDelay(unit)) /* not zero*/ {
        return 0;
    }
    /* TODO: finish function*/
    type = readUnitType(unit);
    switch (type) {
        case 0:
            writeDSTByInstruction(index, s0_val+s1_val);
            break;
        
        case 1:
            writeDSTByInstruction(index, s0_val-s1_val);
            break;

        case 2:
            writeDSTByInstruction(index, s0_val*s1_val);
            break;

        case 3:
            writeDSTByInstruction(index, s0_val/s1_val)
            break;

        case 4:
            writeDSTByInstruction(index, readMemory(imm));
            break;
    }

    writeInstructionCycleExecuteEnd(index,getCycle());
    return 1;
}

void writeResultInstruction(int index) {
    int unit;
    unit = readInstructionUnit(index);
    /* TODO: handle store, load*/
    writeRegister(readUnitDest(unit),readDSTByInstruction(index));
    flipRegUsed(readUnitSrc0(unit)); /* change it to "Yes"*/
    flipRegUsed(readUnitSrc1(unit)); /* change it to "Yes"*/
    writeRegisterStatus(readUnitDest(unit),-1);
    flipUnitBusy(unit); /* now it's zero - available*/
    updateUnitDelay(unit);
    writeInstructioncycleWriteResult(index, getCycle());
}

void exitInstructions() {
    /* TODO: after figuring out halt functionality,
    add handling it*/
    int i;
    float instr;
    int pc;
    int unit;
    char* unitName;
    int cy_issued;
    int cy_read;
    int cy_execute;
    int cy_write_res;
    FILE* traceInstFile;
    traceInstFile = fopen(traceInst,"w");
    if (!traceInstFile) {
        printf("error in initUnits in reading cfg file: %s\n", traceUnitPath);
        exit(1);
    }
    for (i=0;i<instructionsNum;i++) {
        instr = readInstructionInst(i);
        pc = readInstructionPC(i);
        unit = readInstructionUnit(i);
        unitName = readUnitName(unit);
        cy_issued = readInstructionCycleIssued(i);
        cy_read = readInstructionCycleReadOperands(i);
        cy_execute = readInstructionCycleExecuteEnd(i);
        cy_write_res = readInstructioncycleWriteResult(i);
        /*TODO: check right format for inst*/
        fprintf("%f %i %s %i %i %i %i\n", instr, pc,unitName,cy_issued,cy_read,cy_execute,cy_write_res);

    }
    fclose(traceUnitFile);
    free(instructions);
    free(instructionsMode);
    free(instructionsS0);
    free(instructionsS1);
    free(instructionsDST);
}