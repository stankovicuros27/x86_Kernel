#ifndef IDLEPCB_H
#define IDLEPCB_H

#include "pcb.h"

class IdlePCB : public PCB {
public:
    IdlePCB();
private:
    static volatile int idleCnt;
    static void idleVoid();
};

#endif