#include "idlepcb.h"

IdlePCB::IdlePCB() : PCB(0, 1, IdlePCB::idleVoid){
    state = PCB::IDLE;
}

volatile int IdlePCB::idleCnt = 1;

void IdlePCB::idleVoid(){
    while(idleCnt == 1) {};
}