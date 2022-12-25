#include "units.h"




void initUnits(char* cfgPath, char* tracePath) {
    traceUnitPath = tracePath;
    /* we assume the file is written correctly and fully i.e.
        for any operation type there would exist num of units
        and delay */
    char line[MEMORY_LINE_LEN];
    int currVal;
    int count;
    int unitsNum;
    count = 0;
    FILE* cfgFile = fopen(cfgPath,"r");
    if (!cfgFile) {
        printf("error in initUnits in reading cfg file: %s\n", cfgPath);
        exit(1);
    }
    while (fgets(line, MEMORY_LINE_LEN, cfgFile)) {
        if (count==TRACE_LINE) {
            traceUnit = findTraceUnit(line);
            break;
        }
        currVal = findNum(line);
        assignCorrectNum(currVal,count);
        count++;
    }
    fclose(cfgFile);
    /* initialize units*/
    unitsNum = addNum+subNum+multNum+divNum+loadNum+storeNum; 
    units = (struct unit *) malloc(unitsNum *sizeof(struct unit));
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

void writeTraceUnit();

void exitUnits() {
    fclose(traceUnitPath);
}