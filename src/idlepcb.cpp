#include "idlepcb.h"

IdlePCB::IdlePCB() : PCB() {
    setState(PCB::IDLE);
    timeSlice = 1;
    myLockVal = 0;
    stackSize = 256;
    initializeStack(idleVoid);
}

volatile int IdlePCB::idleCnt = 1;

void IdlePCB::idleVoid(){
    while(idleCnt == 1) {
        dispatch();
    }
}