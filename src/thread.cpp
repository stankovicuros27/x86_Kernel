#include "thread.h"
#include "locks.h"
#include "types.h"
#include "pcb.h"
#include "timer.h"

extern volatile bool contextSwitchOnDemand;

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

void Thread::start(){
    myPCB->startPCB();
}

void dispatch(){
    contextSwitchOnDemand = true;
    Timer::timerIntr();
}

void Thread::run(){
    for (int k = 0; k < 30; k++){
        DISABLED_INTR(
        cout << "Poz iz runa!" << endl;
        )
        for  (int i = 0; i < 30000; i++)
            for (int j = 0; j < 30000; j++);
    }
   
}