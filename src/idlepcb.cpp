#include "idlepcb.h"

IdlePCB::IdlePCB() : PCB(0, 1){
    setState(PCB::IDLE);
    initializeStack(idleVoid);
}

volatile int IdlePCB::idleCnt = 1;

void IdlePCB::idleVoid(){
    DISABLED_INTR(
        cout << "Uso u idle" << endl;
    )
    while(idleCnt == 1) {};
}