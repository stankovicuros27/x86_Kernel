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
        waitToComplete();
        delete myPCB;
    )
}

//-----util funcs-----
void Thread::start(){ myPCB->startPCB(); }
void Thread::waitToComplete() { myPCB->waitToComplete(); }
ID Thread::getId(){ return myPCB->getId(); }
ID Thread::getRunningId(){ return running->getId(); }

Thread* Thread::getThreadById(ID id){
    Thread *ret = nullptr;
    List<PCB*>::Elem *iter = allPCBs.head;
    while(iter != nullptr){
        if (iter->data->getId() == id){
            return iter->data->getMyThread();
        }
        iter = iter->next;
    }
    return nullptr;
}

void dispatch(){
    contextSwitchOnDemand = true;
    Timer::timerIntr();
}