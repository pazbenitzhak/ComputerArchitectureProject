#include "clock.h"


void updateClock() {
    cycle++;
}

unsigned long getClock() {
    return cycle;
}

void initClock() {
    cycle = 0;
}