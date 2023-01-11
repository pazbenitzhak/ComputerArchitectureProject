#include "scoreboard.h"

int main(int argc, char** argv) {
    /* args: sim.exe cfg.txt memin.txt memout.txt regout.txt traceinst.txt traceunit.txt*/
    if (argc!=7) {
        printf("Wrong number of arguments, %i given instead of 7", argc);
        exit(1);
    }
    char* cfgPath;
    char* meminPath;
    char* memoutPath;
    char* regoutPath;
    char* traceinstPath;
    char* traceunitPath;
    int* parsedInstruction;
    int isFinished; /* 1 for when we need to finish*/
    int instToIssue; /* keep the index of the last instruciton to be issued*/
    int lastInstDone; /* last instruction that was completley executed*/
    int isWriteResult;
    init(cfgPath, meminPath, memoutPath, regoutPath, traceinstPath, traceunitPath);

    while (!isFinished)
    {
        /* issue instruction*/
        if (instToIssue<instructionsNum) { /* more instructions left to be issued*/
            parsedInstruction = translateInstruction(instToIssue);
            if (issueInstruction(instToIssue,parsedInstruction)) {
                /* instruction issued*/
                incrementInstructionMode(instToIssue);
                instToIssue++;
            }
        }
        /* do other things (read op, execute, write result)*/
        for (int i=lastInstDone;i<instToIssue;i++) {
            if (readInstructionMode(i)==1) /* was already issued, need to do read operands*/ {
                if (readOpInstruction(i)) { /* was read successfuly*/
                    incrementInstructionMode(i);
                }
            }
            else if (readInstructionMode(i)==2) /* was already read, need to do keep executing*/{
                if (executeInstruction(i)) { /* was executed successfuly*/
                    incrementInstructionMode(i);
                }
        }
        else if (readInstructionMode(i)==3) /* we need to write result*/ {
            /* scoreboard needs to check every previous instruction which has not
            finished its read operands phase. If there is even ONE of those whose s0/s1
            is our instruction's dst, than we mark a flag and don't execute writeResultInstruction
            in that cycle (wait for read operands to happen). If not, than scoreboard would run this function*/
            isWriteResult = 1;
            int instDest = readDSTByInstruction(i);
            for (int j=lastInstDone;j<i;j++) {
                if (readInstructionMode(j)==1 && (readS0ByInstruction(j)==instDest || readS1ByInstruction(j)==instDest)) {
                    isWriteResult = 0;
                    break;
                }
            }
            if (isWriteResult) { /* need to do write result*/
                writeResultInstruction(i);
                incrementInstructionMode(i);
            }
        }
        lastInstDone = findLastInstDone();
        writeTraceUnit();
        updateClock();
        if (lastInstDone==instructionsNum-1) {
            /* all instructions executed*/
            isFinished = 1;
        }
    }
    exitScoreboard();
    
}

void init(char* cfgPath, char* meminPath, char* memoutPath, char* regoutPath, char* traceinstPath, char* traceunitPath) {
    initClock();
    initInstructions(traceinstPath);
    initMemory(meminPath, memoutPath);
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
    for (int i=0;i<instructionsNum;i++) {
        if (readInstructionMode(i)<4) {
            /* not done yet*/
            return count;
        }
        /* if we're here then this instruction finished and need to increment count*/
        count++;
    }
    return count;
}