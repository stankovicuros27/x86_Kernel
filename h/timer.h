#ifndef TIMER_H
#define TIMER_H

#include "locks.h"
#include "SCHEDULE.H"
#include "pcb.h"
#include "idlepcb.h"
#include <dos.h>
#include <iostream.h>       //posle obrisi?

#define timerEntry 0x08    
#define utilEntry 0x60  

extern PCB *running;
extern PCB *idlePCB;
extern PCB *mainPCB;

extern volatile bool contextSwitchOnDemand;
extern volatile Time remainingTime;

class Timer{
public:
    static void interrupt timerIntr(...);
    static pInterrupt oldTimerInterrupt;
    static void initializeTimerIntr();
    static void restoreTimerIntr();
};

#endif