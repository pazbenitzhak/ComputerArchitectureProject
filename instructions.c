#include "instructions.h"



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
    fetchQueue = (short*) calloc(instructionsNum,sizeof(short)); /* initialize zeros*/
    if (fetchQueue==NULL) {
        printf("memory allocation failed for instructionsDST\n");
        exit(1);
    }
}

int getInstructionsNum() {
    return instructionsNum;
}

int findInstNumFromMem() {
    int counter;
    float memVal;
    int instNum;
    counter = 0;
    instNum = 0;
    while (counter<getMemLastIndex())
    {
        memVal = readMemory(counter);
        int tr = getUnionFormat(memVal);
        tr >>= 24;
        instNum++;
        if (tr==6)  { /* it's an halt instruction*/
            return instNum;
        }
        counter++;
    }
    return instNum;
    
}

int* translateInstruction(int index) {
    int parsedIns[5];
    float instr;
    instr = readMemory(index);
    unsigned int intinstr = getUnionFormat(instr);
    int tr1 = intinstr >>24;
    int opcode = tr1 & 0xf;
    parsedIns[0] = opcode;
    int tr2 = intinstr >> 20;
    int dst = tr2 & 0xf;
    parsedIns[1] = dst;
    int tr3 = intinstr >> 16;
    int s0 = tr3 & 0xf;
    parsedIns[2] = s0;
    int tr4 = intinstr >> 12;
    int s1 = tr4 & 0xf;
    parsedIns[3] = s1;
    int imm = intinstr & 0xfff;
    parsedIns[4] = imm;
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
    isDstOccupied = 0;
    if (opcode==6) {
        /* halt instruction*/
        for (int i=0;i<4;i++) {
            incrementInstructionMode(index);
        }
        /* increment the mode to finished so the scoreboard won't 
        handle this instruction in any way*/
        return 1;
    }
    if (readRegisterStatus(dst)!=-1 && opcode!=1) { /* dst not relevant for store*/
        isDstOccupied = 1;
    }
    if (isDstOccupied) {
        return 0; /* need to wait due to WAW hazard*/
        /* makes us get stuck and not issue more instructions*/
    }
    /* Dst is available, no risk of WAW*/
    unit = findAvailableUnitType(opcode);
    if (unit==-1) { /* no available unit*/
        return 0; /* makes us get stuck and not issue more instructions*/
    }
    writeInstructionUnit(index,unit);
    /* write to unit the relevant information*/
    flipUnitBusy(unit); /* claim that the unit is busy*/
    writeUnitImm(unit, imm);
    if (opcode==1) { /* store, need only imm and s1*/
        writeUnitSrc1(unit,s1);
        writeInstructionCycleIssued(index,getClock());
        return 1;
    }
    else if (opcode!=0) { /* add/sub/div/mult*/
        writeUnitSrc0(unit,s0);
        writeUnitSrc1(unit,s1);
    }
    writeUnitDest(unit,dst);
    /*end of writing to unit*/
    writeRegisterStatus(dst,unit);
    if (!isRegUsed(dst)) { /* right now we can't read dst because we await its result*/
        flipRegUsed(dst);
    }
    writeInstructionCycleIssued(index,getClock());
    return 1;
}

int readOpInstruction(int index) { /* index of instruction*/
    int unit;
    int s0;
    int s1;
    int dst;
    int isReady;
    float s0_val;
    float s1_val;
    unit = readInstructionUnit(index);
    if (readUnitType(unit)==4) { /* load */
        writeUnitCurrDelay(unit, readUnitCurrDelay(unit)-1);
        writeInstructionCycleReadOperands(index,getClock());
        return 1;
    }
    s0 = readUnitSrc0(unit);
    s1 = readUnitSrc1(unit);
    dst = readUnitDest(unit);
    if (readUnitType(unit)==5) { /* store*/
        if (isRegUsed(s1)) { /* we can't proceed need to exit*/
            return 0;
        }
        s1_val = readRegister(s1);
        writeS1ByInstruction(index,s1_val);
        writeUnitCurrDelay(unit, readUnitCurrDelay(unit)-1);
        writeInstructionCycleReadOperands(index,getClock());
        flipUnitReadOpBit(unit);
        return 1;
    }
    if (dst == s0 && dst == s1) {
        isReady = 1; /*in any case we shouldn't block*/
    }
    else if (dst == s0) {
        isReady = !isRegUsed(s1); /* because s0 would obviously be blocked as dst had already
                                  been stamped used in issueInstructions*/
    }
    else if (dst == s1) {
        isReady = !isRegUsed(s0); /* same reason*/
    }

    else if (dst != s0 || dst != s1) {
        isReady = !(isRegUsed(s0) || isRegUsed(s1)); /* isReady = 1 then we can proceed*/
    }
    if (!isReady) {
        return 0; /* NOT READY YET, KEEP DOING SOMETHING ELSE*/
    }
    /* we are ready to read*/
    s0_val = readRegister(s0);
    writeS0ByInstruction(index,s0_val);
    s1_val = readRegister(s1);
    writeS1ByInstruction(index,s1_val);

    flipUnitReadOpBit(unit);
    writeUnitCurrDelay(unit, readUnitCurrDelay(unit)-1);
    writeInstructionCycleReadOperands(index,getClock());
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
    s0_val = readS0ByInstruction(index);
    s1_val = readS1ByInstruction(index);
    writeUnitCurrDelay(unit, readUnitCurrDelay(unit)-1); /*another cycle passed*/
    if (readUnitCurrDelay(unit)) /* not zero*/ {
        return 0;
    }
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
            writeDSTByInstruction(index, s0_val / s1_val);
            break;

    }

    writeInstructionCycleExecuteEnd(index,getClock());
    return 1;
}

void writeResultInstruction(int index) {
    int unit;
    int s0;
    int s1;
    int dst;
    unit = readInstructionUnit(index);
    s0 = readUnitSrc0(unit);
    s1 = readUnitSrc1(unit);
    dst = readUnitDest(unit);
    if (readUnitType(unit)==5) {
        /* store*/
        writeMemory(readUnitImm(unit), readRegister(s1)); /* write imm value to src1 register*/
        flipUnitBusy(unit); /* now it's zero - available*/
        updateUnitDelay(unit);
        writeInstructioncycleWriteResult(index, getClock());
        flipUnitReadOpBit(unit);
        return;
    }
    if (readUnitType(unit) == 4) { /*load instruction, write from memory to register*/
        writeDSTByInstruction(index, readMemory(readUnitImm(unit)));
    }
    /* load flow does not include else if conditions, only the below*/
    writeRegister(readUnitDest(unit),readDSTByInstruction(index)); /* write dest*/
    writeRegisterStatus(readUnitDest(unit),-1);
    flipRegUsed(dst);
    flipUnitBusy(unit); /* now it's zero - available*/
    updateUnitDelay(unit);
    flipUnitReadOpBit(unit); /* return it to 0 for the next operation*/
    writeInstructioncycleWriteResult(index, getClock());
}

void fetchInstruction(int index) {
    int count;
    int flag;
    flag = 0;
    count = 0;
    for (int i=fetchCount;i<instructionsNum;i++) {
        if (fetchQueue[i]==1) {
            if (flag || (i!=0)) {
                fetchCount = i;
                flag = 0;
            }
            count++;
        }
        if (count==INSTRUCTION_QUEUE_LEN) {
        /* can't fetch another */
        return;
        }
        else if (fetchQueue[i]==0) {
            incrementFetchQueue(i);
            return;
        }
    }
    return;
}

void incrementFetchQueue(int index) {
    fetchQueue[index]++;
}

void exitInstructions() {
    int i;
    float instr;
    int pc;
    int unit;
    char* unitName;
    int cy_issued;
    int cy_read;
    int cy_execute;
    int cy_write_res;
    unsigned int instrInForm;
    FILE* traceInstFile;
    traceInstFile = fopen(traceInst,"w");
    if (!traceInstFile) {
        printf("error in initUnits in reading cfg file: %s\n", traceUnitPath);
        exit(1);
    }
    for (i=0;i<instructionsNum-1;i++) {
        /* no need to write the last instruction since it's halt*/
        instr = readInstructionInst(i);
        instrInForm = getUnionFormat(instr);
        pc = readInstructionPC(i);
        unit = readInstructionUnit(i);
        unitName = readUnitName(unit);
        cy_issued = readInstructionCycleIssued(i);
        cy_read = readInstructionCycleReadOperands(i);
        cy_execute = readInstructionCycleExecuteEnd(i);
        cy_write_res = readInstructioncycleWriteResult(i);
        fprintf(traceInstFile,"%08x %i %s %i %i %i %i\n", instrInForm, pc,unitName,cy_issued,cy_read,cy_execute,cy_write_res);

    }
    fclose(traceInstFile);
    free(instructions);
    free(instructionsMode);
    free(instructionsS0);
    free(instructionsS1);
    free(instructionsDST);
    free(fetchQueue);
}