#include "timer.h"

volatile unsigned cnt = 20;

void interrupt timer(...){
    H_LOCKED(
        cout << "CAO" << endl;
    )
    asm int 0x60
}

//pInterrupt timerInterrupt = timer;
pInterrupt oldTimerInterrupt;