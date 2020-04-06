#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream.h>
#include "timer.h"
#include "pcb.h"

class System {
public:
    static PCB *running;
    static void initializeSystem();     //adjust IVT
    static void restoreSystem();        //restore IVT 
};

#endif