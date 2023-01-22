#ifndef UNITS_H
#define UNITS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clock.h"
#include "registers.h"
#include "instructions.h"


#define UNIT_MEMORY_LINE_LEN 24
#define TRACE_LINE 12

static int addNum;
static int subNum;
static int multNum;
static int divNum;
static int loadNum;
static int storeNum;
static int addDelay;
static int subDelay;
static int multDelay;
static int divDelay;
static int loadDelay;
static int storeDelay;
static int traceUnit;
static int addFirst;
static int addLast;
static int subFirst;
static int subLast;
static int multFirst;
static int multLast;
static int divFirst;
static int divLast;
static int loadFirst;
static int loadLast;
static int storeFirst;
static int storeLast;
static char* traceUnitPath;
static FILE* traceUnitFile; /* needed for writeTraceUnit function*/
static struct unit* units;


struct unit {
    char name[6];
    int busy;
    int dest;
    int s0;
    int s1;
    int imm; /* represents the immediate value parsed from the file*/
    int currentDelay;
    int type;
    int isAfterReadOp; /*1 if so*/
};

/* goes through the cfg.txt file and saves the parameters
Will read trace_unit and convert it to the right index*/
void initUnits(char* cfgPath, char* tracePath);
char* readUnitName(int index);
int isUnitBusy(int index);
void flipUnitBusy(int index);
void initUnit(int index, int currDelay, int type);
int readUnitDest(int index);
void writeUnitDest(int index, int value);
int readUnitSrc0(int index);
void writeUnitSrc0(int index, int value);
int readUnitSrc1(int index);
void writeUnitSrc1(int index, int value);
int readUnitCurrDelay(int index);
void writeUnitCurrDelay(int index, int value);
int readUnitType(int index);
void writeUnitType(int index, int value);
int readUnitImm(int index);
void writeUnitImm(int index, int value);
int findNum(char* line);
void assignCorrectNum(int currVal,int count);
int findTraceUnit(char* line);
int findRowNum(char * line);
void writeTraceUnit();
int findAvailableUnitType(int type);
int readUnitReadOpBit(int index);
void flipUnitReadOpBit(int index);
void updateUnitDelay(int index);
void exitUnits();

#endif