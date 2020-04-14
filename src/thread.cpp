#include "thread.h"
#include "locks.h"
#include "types.h"
#include "pcb.h"
#include "timer.h"

extern volatile bool contextSwitchOnDemand;

//-----constructors & destructors-----
Thread::Thread(StackSize stackSize, Time timeSlice) {
    LOCKED(
        myPCB = new PCB(stackSize, timeSlice, this, PCB::CREATED);
    )
}

Thread::~Thread(){          //ovo treba jos nesto?
    LOCKED(
        this->waitToComplete();
        delete myPCB;
        myPCB = nullptr;
    )
}

//-----util funcs-----
void Thread::start(){ LOCKED(myPCB->startPCB();) }
void Thread::waitToComplete() { LOCKED(myPCB->waitToComplete();) }
ID Thread::getId(){ return myPCB->getId(); }
ID Thread::getRunningId(){ return running->getId(); }

Thread* Thread::getThreadById(ID id){ return PCB::getPCBById(id)->getMyThread(); }

void dispatch(){
    DISABLED_INTR(
        contextSwitchOnDemand = true;
        asm int timerEntry
    )
        //Timer::timerIntr(); u disableintr
}