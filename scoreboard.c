#include "scoreboard.h"

int main(int argc, char** argv) {
    /* args: sim.exe cfg.txt memin.txt memout.txt regout.txt traceinst.txt traceunit.txt*/
    if (argc != 7) {
        printf("Wrong number of arguments, %i given instead of 7", argc);
        exit(1);
    }
    char* cfgPath = argv[1];
    char* meminPath = argv[2];
    char* memoutPath = argv[3];
    char* regoutPath = argv[4];
    char* traceinstPath = argv[5];
    char* traceunitPath = argv[6];
    int* parsedInstruction;
    int isFinished; /* 1 for when we need to finish*/
    int instToIssue; /* keep the index of the last instruciton to be issued*/
    int lastInstDone; /* last instruction that was completley executed*/
    int isWriteResult;
    int flag;
    int instructionsNum;
    int delayReadOp;
    int justIssued;
    isFinished = 0;
    flag = 0;
    instToIssue = 0;
    lastInstDone = 0;
    delayReadOp = -1;
    justIssued = -1;
    init(cfgPath, meminPath, memoutPath, regoutPath, traceinstPath, traceunitPath);
    instructionsNum = getInstructionsNum();
    while (!isFinished)
    {
        writeTraceUnit(); /* need at the beginning to avoid read after write*/
        flag = getClock(); /*flag = 0 for the 0 cycle so we do not issue the first fetched instruction in that cycle*/
        /* issue instruction*/
        /* once getting to halt we'd get to instructionsNum instructions read, so we would not
        issue any more instructions and practically only wait for the program to end*/
        if (flag && instToIssue < instructionsNum) { /* more instructions left to be issued*/
            parsedInstruction = translateInstruction(instToIssue);
            if (issueInstruction(instToIssue, parsedInstruction)) {
                /* instruction issued*/
                incrementInstructionMode(instToIssue);
                incrementFetchQueue(instToIssue);
                justIssued = instToIssue;
                instToIssue++;
            }
            else { /* no instruction issued*/
                justIssued = -1;
            }
        }
        delayReadOp = -1;
        /* do other things (read op, execute, write result)*/
        for (int i = lastInstDone; i < instToIssue; i++) {
            if (i == justIssued) {
                /* just been issued*/
                continue;
            }
            if (readInstructionMode(i) == 1) /* was already issued, need to do read operands*/ {
                int s0CurrInstr = readUnitSrc0(readInstructionUnit(i));
                int s1CurrInstr = readUnitSrc1(readInstructionUnit(i));
                if ((delayReadOp > -1) && (s0CurrInstr == delayReadOp || s1CurrInstr == delayReadOp)) {
                    /* need to delay read operands step in one cycle*/
                    continue;
                }
                int flipBack = 0;
                int justIssuedDest = 0;
                if ((justIssued != -1) && (justIssued != getInstructionsNum()-1)) {
                    justIssuedDest = readUnitDest(readInstructionUnit(justIssued));
                    if (s0CurrInstr == justIssuedDest && s1CurrInstr == justIssuedDest) {
                        flipRegUsed(justIssuedDest);
                        flipBack = 1;
                    }
                    else if (s0CurrInstr == justIssuedDest) {
                        flipRegUsed(justIssuedDest);
                        flipBack = 1;
                    }
                    else if (s1CurrInstr == justIssuedDest) {
                        flipRegUsed(justIssuedDest);
                        flipBack = 1;
                    }
                }
                if (readOpInstruction(i)) { /* was read successfuly*/
                    incrementInstructionMode(i);
                    if (flipBack) {
                        flipRegUsed(justIssuedDest);
                    }
                }
            }
            else if (readInstructionMode(i) == 2) /* was already read, need to do keep executing*/ {
                if (executeInstruction(i)) { /* was executed successfuly*/
                    incrementInstructionMode(i);
                }
            }
            else if (readInstructionMode(i) == 3) /* we need to write result*/ {
                /* scoreboard needs to check every previous instruction which has not
                finished its read operands phase. If there is even ONE of those whose s0/s1
                is our instruction's dst, then we mark a flag and don't execute writeResultInstruction
                in that cycle (wait for read operands to happen). If not, than scoreboard would run this function*/
                isWriteResult = 1;
                int instrUnit = readInstructionUnit(i);
                int instDest = readUnitDest(instrUnit);
                int unitType = readUnitType(instrUnit);
                for (int j = lastInstDone; j < i; j++) {
                    if (unitType == 4) { /* load operation - special case, need to handle
                                         RAW in memory*/
                        int otherInstrUnit = readInstructionUnit(j);
                        if ((readInstructionMode(j) != 4) && (readUnitType(otherInstrUnit)==5) &&
                            (readUnitImm(instrUnit) == readUnitImm(otherInstrUnit))) {
                            isWriteResult = 0;
                            break;
                        }
                    }
                    if (unitType == 5) { /* store operation - special case, need to handle
                     WAR in memory*/
                        int otherInstrUnit = readInstructionUnit(j);
                        if ((readInstructionMode(j) != 4) && (readUnitType(otherInstrUnit) == 4) &&
                            (readUnitImm(instrUnit) == readUnitImm(otherInstrUnit))) {
                            isWriteResult = 0;
                            break;
                        }
                    }
                    else { /* non-load, non-store operation*/
                        if (readInstructionMode(j) == 1 && readUnitSrc0(readInstructionUnit(j)) == instDest || readUnitSrc1(readInstructionUnit(j)) == instDest) {
                            isWriteResult = 0;
                            break;
                        }
                    }
                }
                if (isWriteResult) { /* need to do write result*/
                    writeResultInstruction(i);
                    incrementInstructionMode(i);
                    delayReadOp = readUnitDest(readInstructionUnit(i));
                    /* delayReadOp takes the dest value so it can be compared in the read op stage
                    with the relevant register*/
                }
            }
        }
        lastInstDone = findLastInstDone();
        updateClock();
        if (lastInstDone == instructionsNum) {
            /* all instructions executed*/
            isFinished = 1;
        }
    }

    exitScoreboard();
}

void init(char* cfgPath, char* meminPath, char* memoutPath, char* regoutPath, char* traceinstPath, char* traceunitPath) {
    initClock();
    initMemory(meminPath, memoutPath);
    initInstructions(traceinstPath);
    initRegisters(regoutPath);
    initUnits(cfgPath, traceunitPath);
}

void exitScoreboard() {
    exitInstructions();
    exitMemory();
    exitRegisters();
    exitUnits();
}

int findLastInstDone() {
    int count;
    count = 0;
    for (int i=0;i<getInstructionsNum();i++) {
        if (readInstructionMode(i)<4) {
            /* not done yet*/
            return count;
        }
        /* if we're here then this instruction finished and need to increment count*/
        count++;
    }
    return count;
}