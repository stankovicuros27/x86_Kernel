#include "thread.h"
#include "locks.h"
#include "types.h"
#include "pcb.h"
#include "timer.h"

extern volatile bool contextSwitchOnDemand;

//---constructors & destructors---
Thread::Thread(StackSize stackSize, Time timeSlice) {
    DISABLED_INTR(
        myPCB = new PCB(stackSize, timeSlice, this, PCB::CREATED);
    )
}

Thread::~Thread(){
    DISABLED_INTR(
        delete myPCB;
    )
}


//---util funcs---
void Thread::start(){ myPCB->startPCB(); }

void Thread::waitToComplete() { myPCB->waitToComplete(); }

void dispatch(){
    contextSwitchOnDemand = true;
    Timer::timerIntr();
}