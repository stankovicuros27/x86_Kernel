#ifndef TIMER_H
#define TIMER_H

#include "locks.h"
#include "SCHEDULE.H"
#include "system.h"
#include <iostream.h>       //posle obrisi?
#include <dos.h>


#define timerEntry 0x08    
#define utilEntry 0x60  



class Timer{
public:
    static void interrupt timerIntr(...);
    static volatile Word remainingTime;
    static pInterrupt oldTimerInterrupt;
    static void initializeTimerIntr();
    static void restoreTimerIntr();
};

#endif