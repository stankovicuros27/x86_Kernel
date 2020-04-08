#include "idlepcb.h"

IdlePCB::IdlePCB() : PCB(){
    setState(PCB::IDLE);
    stack = nullptr;
    sp = 0;
    ss = 0;
    bp = 0;
    unlimitedTime = 0;
    timeSlice = 1;
    myLockVal = 0;
    stackSize = 256;
    initializeStack(idleVoid);
}

volatile int IdlePCB::idleCnt = 1;

void IdlePCB::idleVoid(){
    LOCKED(
        cout << "Enter IDLE PCB ! ! !" << endl;
    )
    while(idleCnt == 1) {};
}