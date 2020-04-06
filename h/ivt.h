#ifndef IVT_H
#define IVT_H

#include "locks.h"
#include "types.h"
#include "timer.h"
#include <dos.h>

#define timerEntry 0x08                 //pocetni ulaz za timer
#define utilEntry 0x60                  //pomocni ulaz za timer
extern pInterrupt oldTimerInterrupt;
//extern pInterrupt timerInterrupt;       //vidi gde zelis da stavis ove interruptove? za sada extern

void interrupt timer(...);

class IVT{

public:
    static void initializeIvt();     
    static void restoreIvt();        
};

#endif
