#include "units.h"




void initUnits(char* cfgPath, char* tracePath) {
    traceUnitPath = tracePath;
    /* we assume the file is written correctly and fully i.e.
        for any operation type there would exist num of units
        and delay */
    char line[UNIT_MEMORY_LINE_LEN];
    char traceLine[UNIT_MEMORY_LINE_LEN];
    int currVal;
    int type;
    int unitsNum;
    type = 0;
    FILE* cfgFile = fopen(cfgPath,"r");
    if (!cfgFile) {
        printf("error in initUnits in reading cfg file: %s\n", cfgPath);
        exit(1);    
    }
    while (fgets(line, UNIT_MEMORY_LINE_LEN, cfgFile)) {
        currVal = findNum(line);
        type = findRowNum(line);
        if (type==12) {/* need to wait and pass over the entire file, so we'll save the line and do the right function afterwards*/
            memcpy(traceLine,line,UNIT_MEMORY_LINE_LEN);
            continue; /* need to skip to the next iteration*/
        }
        assignCorrectNum(currVal,type);
    }
    traceUnit = findTraceUnit(line);
    fclose(cfgFile);
    /* initialize units*/
    unitsNum = addNum+subNum+multNum+divNum+loadNum+storeNum; 
    units = (struct unit *) malloc(unitsNum*sizeof(struct unit));
    if (units==NULL) {
        printf("memory allocation failed for units\n");
        exit(1);
    }
    for (int i=0;i<unitsNum;i++) {
        if (i<=addLast) {
            units[i].name[0] = 'A';
            units[i].name[1] = 'D';
            units[i].name[2] = 'D';
            /* single digits*/
            units[i].name[3] = (char) (i-addFirst+48);
            units[i].name[4] = '\0';
            units[i].name[5] = '\0';
            if (i - addFirst >= 10) {
                /* double digits*/
                int decVal = i - addFirst;
                units[i].name[3] = (char)((decVal / 10) + 48);
                units[i].name[4] = (char)((decVal % 10) + 48);
            }
            initUnit(i, addDelay, 0);
        }
        else if (i<=subLast) {
            units[i].name[0] = 'S';
            units[i].name[1] = 'U';
            units[i].name[2] = 'B';
            /* single digits*/
            units[i].name[3] = (char)(i - subFirst + 48);
            units[i].name[4] = '\0';
            units[i].name[5] = '\0';
            if (i - subFirst >= 10) {
                /* double digits*/
                int decVal = i - subFirst;
                units[i].name[3] = (char)((decVal / 10) + 48);
                units[i].name[4] = (char)((decVal % 10) + 48);
            }
            initUnit(i, subDelay, 1);
        }
        else if (i<=multLast) {
            units[i].name[0] = 'M';
            units[i].name[1] = 'U';
            units[i].name[2] = 'L';
            /* single digits*/
            units[i].name[3] = (char)(i - multFirst + 48);
            units[i].name[4] = '\0';
            units[i].name[5] = '\0';
            if (i - multFirst >= 10) {
                /* double digits*/
                int decVal = i - multFirst;
                units[i].name[3] = (char)((decVal / 10) + 48);
                units[i].name[4] = (char)((decVal % 10) + 48);
            }
            initUnit(i, multDelay, 2);

        }
        else if (i<=divLast) {
            units[i].name[0] = 'D';
            units[i].name[1] = 'I';
            units[i].name[2] = 'V';
            /* single digits*/
            units[i].name[3] = (char)(i - divFirst + 48);
            units[i].name[4] = '\0';
            units[i].name[5] = '\0';
            if (i - divFirst >= 10) {
                /* double digits*/
                int decVal = i - divFirst;
                units[i].name[3] = (char)((decVal / 10) + 48);
                units[i].name[4] = (char)((decVal % 10) + 48);
            }
            initUnit(i, divDelay, 3);
        }
        else if (i<=loadLast) {
            units[i].name[0] = 'L';
            units[i].name[1] = 'D';
            /* single digits*/
            units[i].name[2] = (char)(i - loadFirst + 48);
            units[i].name[3] = '\0';
            units[i].name[4] = '\0';
            units[i].name[5] = '\0';
            if (i - loadFirst >= 10) {
                /* double digits*/
                int decVal = i - loadFirst;
                units[i].name[2] = (char)((decVal / 10) + 48);
                units[i].name[3] = (char)((decVal % 10) + 48);
            }
            initUnit(i, loadDelay, 4);
        }
        else if (i<=storeLast) {
            units[i].name[0] = 'S';
            units[i].name[1] = 'T';
            /* single digits*/
            units[i].name[2] = (char)(i - storeFirst + 48);
            units[i].name[3] = '\0';
            units[i].name[4] = '\0';
            units[i].name[5] = '\0';
            if (i - storeFirst >= 10) {
                /* double digits*/
                int decVal = i - storeFirst;
                units[i].name[2] = (char)((decVal / 10) + 48);
                units[i].name[3] = (char)((decVal % 10) + 48);
            }
            initUnit(i, storeDelay, 5);
        }
    }
}

int findTraceUnit(char* line) {
    int numVal;
    int num;
    for (int i=0; i<UNIT_MEMORY_LINE_LEN;i++) {
        if (line[i]>=48 && line[i]<=57) {
            numVal = ((int) line[i]-48);
            switch(line[i-1]) {
                case 'B':
                    num = subFirst+numVal;
                    break;

                case 'L':
                    num = multFirst+numVal;
                    break;

                case 'V':
                    num = divFirst+numVal;
                    break;

                case 'T':
                    num = storeFirst+numVal;
                    break;
                
                case 'D':
                    if (line[i-2]=='D') /* unit type is add*/ {
                        num = addFirst+numVal;
                    }
                    else {/* unit type is load*/
                        num = loadFirst+numVal;
                    }
                    break;
            }
        }
    }
    return num;
}



int findNum(char* line) {
    int inMidstOfNum;
    int num;
    num=0;
    inMidstOfNum = 0;
    for (int i=0; i<UNIT_MEMORY_LINE_LEN;i++) {
        if (line[i]>=48 && line[i]<=57) {
            if (!inMidstOfNum) {
                inMidstOfNum = 1;
            }
            else {
                num*=10;
            }
            num += ((int) line[i] -48);
        }
        if (line[i]=='\n') {
            /* we have gone over the whole number*/
            return num;
        }
    }
    return num;
}

void initUnit(int index, int currDelay, int type) {
    units[index].currentDelay = currDelay;
    units[index].type = type;
    units[index].busy = 0;
    units[index].dest = -1;
    units[index].s0 = -1;
    units[index].s1 = -1;
    units[index].imm = -1;
    units[index].isAfterReadOp = 0;
}


void assignCorrectNum(int currVal,int count) {
    switch(count) {
        case 0:
            addNum = currVal;
            addFirst = 0;
            addLast = addFirst+addNum-1;
            break;

        case 1:
            subNum = currVal;
            subFirst = addLast+1;
            subLast = subFirst+subNum-1;
            break;

        case 2:
            multNum = currVal;
            multFirst = subLast+1;
            multLast = multFirst+multNum-1;
            break;

        case 3:
            divNum = currVal;
            divFirst = multLast+1;
            divLast = divFirst+divNum-1;
            break;


        case 4:
            loadNum = currVal;
            loadFirst = divLast+1;
            loadLast = loadFirst+loadNum-1;
            break;


        case 5:
            storeNum = currVal;
            storeFirst = loadLast+1;
            storeLast = storeFirst+storeNum-1;
            break;


        case 6:
            addDelay = currVal;
            break;

        
        case 7:
            subDelay = currVal;
            break;


        case 8:
            multDelay = currVal;
            break;


        case 9:
            divDelay = currVal;
            break;

        
        case 10:
            loadDelay = currVal;
            break;


        case 11:
            storeDelay = currVal;
            break;


    }
}

/* gets a line from cfg.txt and returns a line number which corresponds
to that of the line in the example file cfg.txt
used to identify to which type of unit and kind (delay, number of units)
the line belongs to.
The mapping is done according to the example file cfg.txt*/
int findRowNum(char * line) {
    if (line[0]=='a') {
        if (line[4]=='n') /* add_nr_units*/ {
            return 0;
        }
        else if (line[4]=='d') /*add_delay*/ {
            return 6;
        }
    }
    else if (line[0]=='s') {
        if (line[1]=='u') /*sub */ {
            if (line[4]=='n') /* sub_nr_units*/ {
                return 1;
            }
            else if (line[4]=='d') /*sub_delay*/ {
                return 7;
            }
        }

        else if (line[1]=='t') /*st*/ {
            if (line[3]=='n') /* st_nr_units*/ {
                return 5;
            }
            else if (line[3]=='d') /*st_delay*/ {
                return 11;
            }
        }
    }
    else if (line[0]=='m') { /*mul*/
            if (line[4]=='n') /* mul_nr_units*/ {
                return 2;
            }
            else if (line[4]=='d') /*mul_delay*/ {
                return 8;
            }
    }
    else if (line[0]=='d') { /*div*/
            if (line[4]=='n') /* div_nr_units*/ {
                return 3;
            }
            else if (line[4]=='d') /*div_delay*/ {
                return 9;
            }
    }
    else if (line[0]=='l') { /*ld*/
            if (line[3]=='n') /* ld_nr_units*/ {
                return 4;
            }
            else if (line[3]=='d') /*ld_delay*/ {
                return 10;
            }
    }
    else if (line[0]=='t') { /* trace_unit*/
        return 12;
    }
    return -1; /*just for the record*/
}

int findAvailableUnitType(int type) { /* return unit index if available*/
    switch (type)
    {
    case 2: /* add */
        for (int i=addFirst;i<addLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    case 3: /* sub */
        for (int i=subFirst;i<subLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;
    
    case 4: /* mult */
        for (int i=multFirst;i<multLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    case 5: /* div */
        for (int i=divFirst;i<divLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    case 0: /* load */
        for (int i=loadFirst;i<loadLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    case 1: /* store */
        for (int i=storeFirst;i<storeLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    default:
        break;
    }

    return -1;
}



char* readUnitName(int index) {
    return units[index].name;
}


/*1 iff busy*/
int isUnitBusy(int index) {
    return units[index].busy;
}

void flipUnitBusy(int index) {
    units[index].busy = 1 - units[index].busy;
}

int readUnitDest(int index) {
    return units[index].dest;
}

void writeUnitDest(int index, int value) {
    units[index].dest = value;
}

int readUnitSrc0(int index) {
    return units[index].s0;
}

void writeUnitSrc0(int index, int value) {
    units[index].s0 = value;
}
int readUnitSrc1(int index) {
    return units[index].s1;
}
void writeUnitSrc1(int index, int value) {
    units[index].s1 = value;
}

int readUnitImm(int index) {
    return units[index].imm;
}

void writeUnitImm(int index, int value) {
    units[index].imm = value;
}

int readUnitCurrDelay(int index) {
    return units[index].currentDelay;
}

void writeUnitCurrDelay(int index, int value) {
    units[index].currentDelay = value;
}


int readUnitType(int index) {
    return units[index].type;
}

void writeUnitType(int index, int value) {
    units[index].type = value;
}

int readUnitReadOpBit(int index) {
    return units[index].isAfterReadOp;
}
void flipUnitReadOpBit(int index) {
    units[index].isAfterReadOp = 1 - units[index].isAfterReadOp;
}

void updateUnitDelay(int index) {
    switch(units[index].type) {
        case 0:
            units[index].currentDelay = addDelay;
            break;
        case 1:
            units[index].currentDelay = subDelay;
            break;
        case 2:
            units[index].currentDelay = multDelay;
            break;
        case 3:
            units[index].currentDelay = divDelay;
            break;
        case 4:
            units[index].currentDelay = loadDelay;
            break;
        case 5:
            units[index].currentDelay = storeDelay;
            break;
    }
    return;
}


void writeTraceUnit() {
    int cycle;
    int s0;
    int s1;
    int dest;
    char* s0Unit=NULL;
    char* s1Unit=NULL;
    char s0IsAv[4];
    char s1IsAv[4];
    int s0flag;
    int s1flag;
    int ldCount;
    int followS0;
    int followS1;
    int afterReadOp = 0;
    ldCount = 0;
    s0flag = 0;
    s1flag = 0;
    followS0 = 0;
    followS1 = 0;
    cycle = getClock();
    if (!cycle) {/* first iteration, need to open file*/
        traceUnitFile = fopen(traceUnitPath,"w");
        if (!traceUnitFile) {
            printf("error in initUnits in reading cfg file: %s\n", traceUnitPath);
            exit(1);
        }
    }
    if (readUnitType(traceUnit)==4) {
        dest = readUnitDest(traceUnit);
        /* load*/
        afterReadOp = readUnitReadOpBit(traceUnit);
        if (afterReadOp) {
            fprintf(traceUnitFile, "%i %s F%i - - - - No No\n", cycle, readUnitName(traceUnit), dest);
        }
        else {
            fprintf(traceUnitFile, "%i %s F%i - - - - Yes Yes\n", cycle, readUnitName(traceUnit), dest);
        }
        return;
    }
    if (readUnitType(traceUnit)==5) {
        /* store*/
        if (isUnitBusy(traceUnit)) { /* get inside if unit is NOT busy*/
            s1 = readUnitSrc1(traceUnit);
            if (isRegUsed(s1)) /* if register=no=used*/ {
                s1Unit = readUnitName(readRegisterStatus(s1));
                s1flag = 0;
                s1IsAv[0] = 'N';
                s1IsAv[1] = 'o';
                s1IsAv[2] = '\0';
                s1IsAv[3] = '\0';
                }
            else {
                afterReadOp = readUnitReadOpBit(traceUnit);
                s1flag = 1;
                s1IsAv[0] = 'Y';
                s1IsAv[1] = 'e'; 
                s1IsAv[2] = 's'; 
                s1IsAv[3] = '\0';
                }
            if (!s1flag) {
                fprintf(traceUnitFile, "%i %s - - F%i - - Yes %s\n", cycle, readUnitName(traceUnit), s1, s1IsAv);
            }
            else {
                if (afterReadOp) {
                    fprintf(traceUnitFile, "%i %s - - F%i - - No No\n", cycle, readUnitName(traceUnit), s1);
                }
                else {
                    fprintf(traceUnitFile, "%i %s - - F%i - - Yes Yes\n", cycle, readUnitName(traceUnit), s1);
                }
            }
        return;
        }
    }
    if (isUnitBusy(traceUnit)) {
        dest = readUnitDest(traceUnit);
        s0 = readUnitSrc0(traceUnit);
        s1 = readUnitSrc1(traceUnit);
        if (readUnitReadOpBit(traceUnit)) { /* need a certain structure, we are after read operands*/
            char* unitName = readUnitName(traceUnit);
            fprintf(traceUnitFile, "%i %s F%i F%i F%i - - No No\n", cycle, unitName, dest,
                s0, s1);
            return;
        }
        else if (s0 != dest) {
            if (isRegUsed(s0)) /* if register=no=used*/ {
                s0Unit = readUnitName(readRegisterStatus(s0));
                s0flag = 0;
                s0IsAv[0] = 'N';
                s0IsAv[1] = 'o';
                s0IsAv[2] = '\0';
                s0IsAv[3] = '\0';
            }
            else {
                s0flag = 1;
                s0IsAv[0] = 'Y';
                s0IsAv[1] = 'e';
                s0IsAv[2] = 's';
                s0IsAv[3] = '\0';
            }
        }
        else { /*special case where dest=s0*/
            s0flag = 1;
            s0IsAv[0] = 'Y';
            s0IsAv[1] = 'e';
            s0IsAv[2] = 's';
            s0IsAv[3] = '\0';
        }
        if (s1 != dest) {
            if (isRegUsed(s1)) /* if register=no=used*/ {
                s1Unit = readUnitName(readRegisterStatus(s1));
                s1flag = 0;
                s1IsAv[0] = 'N';
                s1IsAv[1] = 'o';
                s1IsAv[2] = '\0';
                s1IsAv[3] = '\0';
            }
            else {
                s1flag = 1;
                s1IsAv[0] = 'Y';
                s1IsAv[1] = 'e';
                s1IsAv[2] = 's';
                s1IsAv[3] = '\0';
            }
        }
        else { /* special case where s1=dest*/
            s1flag = 1;
            s1IsAv[0] = 'Y';
            s1IsAv[1] = 'e';
            s1IsAv[2] = 's';
            s1IsAv[3] = '\0';
        }
        
        char* unitName = readUnitName(traceUnit);
        if (s1flag && s0flag) {
            fprintf(traceUnitFile, "%i %s F%i F%i F%i - - %s %s\n", cycle, unitName, dest,
                s0, s1, s0IsAv, s1IsAv);

        }
        else if (s1flag && !s0flag) {
            fprintf(traceUnitFile, "%i %s F%i F%i F%i %s - %s %s\n", cycle, unitName, dest,
                s0, s1, s0Unit, s0IsAv, s1IsAv);

        }
        else if (!s1flag && s0flag) {
            fprintf(traceUnitFile, "%i %s F%i F%i F%i - %s %s %s\n", cycle, unitName, dest,
                s0, s1, s1Unit, s0IsAv, s1IsAv);

        }
        else {
            fprintf(traceUnitFile, "%i %s F%i F%i F%i %s %s %s %s\n", cycle, unitName, dest,
                s0, s1, s0Unit, s1Unit, s0IsAv, s1IsAv);
        }

        }

    }



void exitUnits() {
    fclose(traceUnitFile);
    free(units);
}