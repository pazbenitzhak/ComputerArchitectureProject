#include "clock.h"

static unsigned long cycle;

void updateClock() {
    cycle++;
}

unsigned long getClock() {
    return cycle;
}

void initClock() {
    cycle = 0;
}