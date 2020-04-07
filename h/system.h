#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream.h>
#include "idlepcb.h"
#include "timer.h"
#include "list.h"
#include <dos.h>

extern PCB *mainPCB;
extern PCB *running;
extern PCB *idlePCB;
extern List<PCB*> allPCBs;

class System {
public:
    static void initializeSystem();     
    static void restoreSystem();     
};

#endif