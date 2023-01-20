
#include "memory.h"
//#include "units.h"

int main(int argc, char** argv) {
    char name [5];
    name[0] = 'A';
    name[1] = 'D';
    name[2] = 'D';
    name[3] = (char) 55;
    name[4] = '\0';
    char* hi;
    hi = name;
    FILE* memFile = fopen("hello.txt","w");
    fprintf(memFile,"hi: %s\n",hi);
}