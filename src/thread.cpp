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
this->waitToComplete();
    LOCKED(
        if(myPCB != nullptr)
            delete myPCB;
        myPCB = nullptr;
    )
}

//-----util funcs-----
void Thread::start(){ 
    if(myPCB != nullptr){
        LOCKED(myPCB->startPCB();) 
    }
}
void Thread::waitToComplete() { 
    if(myPCB != nullptr){
        LOCKED(myPCB->waitToComplete();) 
    }
}
ID Thread::getId(){ 
    if(myPCB != nullptr)
        return myPCB->getId(); 
    else return -1;
}
ID Thread::getRunningId(){ 
    if(running != nullptr)
        return running->getId();
    else return -1;
}

Thread* Thread::getThreadById(ID id){ return PCB::getPCBById(id)->getMyThread(); }

void dispatch(){
    DISABLED_INTR(
        contextSwitchOnDemand = true;
        asm int timerEntry
    )
        //Timer::timerIntr(); u disableintr
}

//---Signals---

void Thread::signal(SignalId signal){
    if(signal > 15) return;
    if(myPCB == nullptr) return;
    myPCB->signal(signal);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler){
    if(signal > 15) return;
    if (myPCB == nullptr) return;
	myPCB->registerHandler(signal,handler); 
}

void Thread::unregisterAllHandlers(SignalId id){
    if(id > 15) return;
    if (myPCB == nullptr) return;
	myPCB->unregisterAllHandlers(id);
}

void Thread::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
    if(id > 15) return;
    if (myPCB == nullptr) return;
	myPCB->swap(id, hand1, hand2); 
}

void Thread::blockSignal(SignalId signal){
    if(signal > 15) return;
    if (myPCB == nullptr) return;
	myPCB->blockSignal(signal);
}

void Thread::unblockSignal(SignalId signal){
    if(signal > 15) return;
    if (myPCB == nullptr) return;
	myPCB->unblockSignal(signal);
}

void Thread::blockSignalGlobally(SignalId signal){
    if(signal > 15) return;
    PCB::blockSignalGlobally(signal);
}

void Thread::unblockSignalGlobally(SignalId signal){
    if(signal > 15) return;
    PCB::unblockSignalGlobally(signal);
}