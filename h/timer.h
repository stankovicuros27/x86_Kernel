#ifndef TIMER_H
#define TIMER_H

#include "system.h"

void interrupt timer(...);

extern volatile unsigned cnt;
//extern pInterrupt timerInterrupt;
extern pInterrupt oldTimerInterrupt;

#endif