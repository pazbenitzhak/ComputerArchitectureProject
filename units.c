#include "units.h"
#include "clock.h"
#include "registers.h"



void initUnits(char* cfgPath, char* tracePath) {
    traceUnitPath = tracePath;
    /* we assume the file is written correctly and fully i.e.
        for any operation type there would exist num of units
        and delay */
    char line[MEMORY_LINE_LEN];
    char traceLine[MEMORY_LINE_LEN];
    int currVal;
    int type;
    int unitsNum;
    type = 0;
    FILE* cfgFile = fopen(cfgPath,"r");
    if (!cfgFile) {
        printf("error in initUnits in reading cfg file: %s\n", cfgPath);
        exit(1);    
    }
    while (fgets(line, MEMORY_LINE_LEN, cfgFile)) {
        currVal = findNum(line);
        type = findRowNum(line);
        if (type==12) {/* need to wait and pass over the entire file, so we'll save the line and do the right function afterwards*/
            memcpy(traceLine,line,MEMORY_LINE_LEN);
            continue; /* need to skip to the next iteration*/
        }
        assignCorrectNum(currVal,type);
    }
    traceUnit = findTraceUnit(line);
    fclose(cfgFile);
    /* initialize units*/
    unitsNum = addNum+subNum+multNum+divNum+loadNum+storeNum; 
    units = (struct unit *) malloc(unitsNum*sizeof(struct unit));
    for (int i=0;i<unitsNum;i++) {
        if (i<=addLast) {
            /*TODO: handle case of double/triple digits*/
            char name[6] = "ADD";
            name[3] = (char) (i-addFirst+48);
            printf("name=%s\n",name);
            writeUnitName(i,name);
        }
        else if (i<=subLast) {
            /*TODO: handle case of double/triple digits*/
            char name[6] = "SUB";
            name[3] = (char) (i-addFirst+48);
            printf("name=%s\n",name);
            writeUnitName(i,name);
        }
        else if (i<=multLast) {
            /*TODO: handle case of double/triple digits*/
            char name[6] = "MUL";
            name[3] = (char) (i-addFirst+48);
            printf("name=%s\n",name);
            writeUnitName(i,name);
        }
        else if (i<=divLast) {
            /*TODO: handle case of double/triple digits*/
            char name[6] = "DIV";
            name[3] = (char) (i-addFirst+48);
            printf("name=%s\n",name);
            writeUnitName(i,name);
        }
        else if (i<=loadLast) {
            /*TODO: handle case of double/triple digits*/
            char name[6] = "LD";
            name[3] = (char) (i-addFirst+48);
            printf("name=%s\n",name);
            writeUnitName(i,name);
        }
        else if (i<=storeLast) {
            /*TODO: handle case of double/triple digits*/
            char name[6] = "ST";
            name[3] = (char) (i-addFirst+48);
            printf("name=%s\n",name);
            writeUnitName(i,name);
        }
    }
}

int findTraceUnit(char* line) {
    int numVal;
    int num;
    for (int i=0; i<MEMORY_LINE_LEN;i++) {
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
                
                case 'S':
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
    for (int i=0; i<MEMORY_LINE_LEN;i++) {
        if (line[i]>=48 && line[i]<=57) {
            if (!inMidstOfNum) {
                inMidstOfNum = 1;
            }
            else {
                num*=10;
            }
            num += ((int) line[i] -48);
        }
    }
    return num;
}


void assignCorrectNum(int currVal,int count) {
    switch(count) {
        case 0:
            addNum = currVal;
            /* TODO: make sure that for any unit there exists at least one of its kind*/
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
            if (line[4]=='n') /* st_nr_units*/ {
                return 5;
            }
            else if (line[4]=='d') /*st_delay*/ {
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
            if (line[4]=='n') /* ld_nr_units*/ {
                return 4;
            }
            else if (line[4]=='d') /*ld_delay*/ {
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
    case 0: /* add */
        for (int i=addFirst;i<addLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    case 1: /* sub */
        for (int i=subFirst;i<subLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;
    
    case 2: /* mult */
        for (int i=multFirst;i<multLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    case 3: /* div */
        for (int i=divFirst;i<divLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    case 4: /* load */
        for (int i=loadFirst;i<loadLast+1;i++) {
            if (!isUnitBusy(i)) { /* unit not busy*/
                return i;
            }
        }
        /* no available unit found*/
        break;

    case 5: /* store */
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

void writeUnitName(int index, char* value) {
    units[index].name = value;
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

void writeTraceUnit() {
    int cycle;
    int s0;
    int s1;
    int dest;
    char* s0Unit;
    char* s1Unit;
    char s0IsAv[3];
    char s1IsAv[3];
    /* TODO: add handling st, load*/
    cycle = getClock();
    if (!cycle) {/* first iteration, need to open file*/
        /* TODO: change condition*/
        traceUnitFile = fopen(traceUnitPath,"r");
        if (!traceUnitFile) {
            printf("error in initUnits in reading cfg file: %s\n", traceUnitPath);
            exit(1);
        }
    }
    dest = readUnitDest(traceUnit);
    s0 = readUnitSrc0(traceUnit);
    s1 = readUnitSrc1(traceUnit);
    if (isUnitBusy) {

        if (isRegUsed(s0)) /* if register=no=used*/ {
            s0Unit = readUnitName(readRegisterStatus(s0));
            s0IsAv[0] = 'N';
            s0IsAv[1] = 'o'; 
        }
        else {
           s0Unit[0] = '-';
            s0IsAv[0] = 'Y';
            s0IsAv[1] = 'e'; 
            s0IsAv[2] = 's'; 
        }
        if (isRegUsed(s1)) /* if register=no=used*/ {
            s1Unit = readUnitName(readRegisterStatus(s1));
            s1IsAv[0] = 'N';
            s1IsAv[1] = 'o'; 
        }
        else {
            s1Unit[0] = '-';
            s1IsAv[0] = 'Y';
            s1IsAv[1] = 'e'; 
            s1IsAv[2] = 's'; 
        }
        fprintf(traceUnitFile, "%i %s F%i F%i F%i %s %s %s %s", cycle, readUnitName(traceUnit),dest,
        s0,s1, s0Unit, s1Unit, s0IsAv, s1IsAv);
    }


}

void exitUnits() {
    fclose(traceUnitPath);
    free(units);
}