#include "pcb.h"
#include "kernsem.h"
#include "kerevent.h"
#include "ivtentry.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>

ID PCB::currentID = 0;
List<PCB*> allPCBs;

PCB::PCB(StackSize size, Time timeSl, Thread *myThr, State s){
    timeSlice = timeSl;
    myThread = myThr;
    myLockVal = 0;    
    myID = ++currentID;
    state = s; 
    toKill = 0;
   
    if (size > MAX_STACK) size = MAX_STACK; 
    if (size < MIN_STACK) size = MIN_STACK;
    stackSize = size / sizeof(Word); 
    initializeStack(runWrapper);

    LOCKED(
        parent = running;
        for(int i = 0; i < 16; i++){
            localSignalStatus[i] = parent->localSignalStatus[i];
            signalHandlers[i] = parent->signalHandlers[i];
        }
        allPCBs.insertBack(this);
    )
}

PCB::PCB(int mainPCB){  //used only for creating mainPCB
    state = PCB::RUNNING; 
    stack = nullptr;
    sp = 0;
    ss = 0;
    bp = 0;
    timeSlice = defaultTimeSlice;
    myLockVal = 0;
    myID = ++currentID;
    toKill = 0;

    LOCKED(
        parent = nullptr;
        for (int i = 0; i < 16; i++) {
            localSignalStatus[i] = true;
        }
        allPCBs.insertBack(this);
    )
}

PCB::PCB(){ //used only to make idlePCB
    parent = nullptr;
    for (int i = 0; i < 16; i++) {
        localSignalStatus[i] = false;
    }
    toKill = 0;
} 

PCB::~PCB(){
    LOCKED(
        if (stack != nullptr) delete[] stack;
        stack = nullptr;
        
        /*Deleting PCB from allPCBs list*/
        List<PCB*>::Iterator iter = allPCBs.begin();
        while(iter != allPCBs.end()){
            if(*iter == this){
                iter.remove();
                break;
            }
            iter++;
        }
    )
}

void PCB::initializeStack(pFunction fp){
    LOCKED(
        stack = new Word[stackSize];
    )
    stack[stackSize - 1] = INIT_PSW;       
    stack[stackSize - 2] = FP_SEG(fp);
    stack[stackSize - 3] = FP_OFF(fp);
    ss = FP_SEG(stack + stackSize - 12);
    sp = FP_OFF(stack + stackSize - 12);
    bp = sp;
}

void PCB::startPCB(){
    LOCKED(
        if (state == PCB::CREATED){
            setState(PCB::READY);
            Scheduler::put(this);
        } 
    )
}

void PCB::unblockPCB(){
    LOCKED(
        if(state == PCB::BLOCKED){
            setState(PCB::READY);
            Scheduler::put(this);
        }
    )
}

void PCB::blockPCB(){
    LOCKED(
        if((state != PCB::TERMINATED) && (state != PCB::IDLE)){
            setState(PCB::BLOCKED);
        }
    )
}

void PCB::awakeMyAsleep(){
    if(this == nullptr) return;
    LOCKED( 
        while(!waitingForMe.isEmpty()){
            PCB *toStart = waitingForMe.getFront();
            waitingForMe.deleteFront();
            if (toStart != nullptr) toStart->unblockPCB(); 
        }
    )
    //ovde sam dodao if(toStart != nullptr) ispred toStart->
}

void PCB::waitToComplete(){
    //pazi ovde uslove!!!!!!
    if(this == nullptr) return;
    if(state == TERMINATED) return;
    LOCKED(
        if (running != this && 
        this->state != PCB::TERMINATED && 
        this->state != PCB::IDLE && 
        this->state != PCB::CREATED 
        /*&& !this->isWaitingForMe()*/ ) /* used for deadlocks */
        {        
            this->waitingForMe.insertBack(running);
            running->blockPCB();
            dispatch();
        }
    )
}

//to prevent deadlock
bool PCB::isWaitingForMe(){
    bool ret = false;
    LOCKED(
        List<PCB*>::Iterator iter = running->waitingForMe.begin();
        for(;iter != running->waitingForMe.end(); iter++){
            if ((*iter)->getId() == this->getId()){
                ret = true;
                break;
            }
        }
    )
    return ret;
}

PCB* PCB::getPCBById(ID id){
    List<PCB*>::Iterator iter = allPCBs.begin();
    for(;iter != allPCBs.end(); iter++){
        if ((*iter)->getId() == id) return (*iter);
    }
    return nullptr;
}

void PCB::runWrapper(){
    running->getMyThread()->run();
    LOCKED(
        running->awakeMyAsleep();
        running->setState(PCB::TERMINATED);

        if(running->parent) running->parent->signal(1);
        running->signal(2);
        handleSignals();

        dispatch();
    )
}

//---Signals---

void PCB::signal(SignalId signal){
    if(signal > 15) return;
    LOCKED(
        /*ADDED*/
        /*if(parent != nullptr && signal > 2 && toSendSignal) parent->signal(signal);*/
        if(signal < 3) mySignals.insertBack(signal);
        else if (signal < 6) priorityHigh.insertBack(signal);
        else if (signal < 9) priorityMid.insertBack(signal);
        else priorityLow.insertBack(signal);
    )
}
void PCB::registerHandler(SignalId signal, SignalHandler handler){
    if(signal > 15) return;
    LOCKED(
        signalHandlers[signal].insertBack(handler);
    )
}
void PCB::unregisterAllHandlers(SignalId id){
    if(id > 15) return;
    LOCKED(
        signalHandlers[id].clear();
    )
}
void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2){
    if(id > 15) return; 
    // This is added!
    LOCK
        List<SignalHandler>::Iterator iter = signalHandlers[id].begin();
        List<SignalHandler>::Iterator it1, it2;
        while(iter != signalHandlers[id].end()){
            if (*iter != nullptr && *iter == hand1) it1 = iter;
            if (*iter != nullptr && *iter == hand2) it2 = iter;
            iter++;
        }
        if (*it1 != nullptr && *it2 != nullptr){
            SignalHandler temp = *it1;
            *it1 = *it2;
            *it2 = temp;
        }
    UNLOCK
}

bool PCB::globalSignalStatus[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

void PCB::blockSignal(SignalId signal){
    if(signal > 15) return;
    LOCKED(
        localSignalStatus[signal] = false;
    )
}
void PCB::blockSignalGlobally(SignalId signal){
    if(signal > 15) return;
    LOCKED(
        globalSignalStatus[signal] = false;
    )
}
void PCB::unblockSignal(SignalId signal){
    if(signal > 15) return;
    LOCKED(
        localSignalStatus[signal] = true;
    )
}
void PCB::unblockSignalGlobally(SignalId signal){
    if(signal > 15) return;
    LOCKED(
        globalSignalStatus[signal] = true;
    )
}
void PCB::handleSignals(){
    if (running == nullptr) return;
    LOCKED(
        PCB *pcb = (PCB*)running;

        /*Main signals*/
        for (List<SignalId>::Iterator it = pcb->mySignals.begin(); it != pcb->mySignals.end(); it++) {
            SignalId id = *it;
            if (pcb->localSignalStatus[id] && globalSignalStatus[id]) {
                if (id == 0){
                    pcb->toKill = 1;
                    /*it++;*/
                    continue;
                }
                for (List<SignalHandler>::Iterator it2 = pcb->signalHandlers[id].begin();
                        it2 != pcb->signalHandlers[id].end(); it2++)
                {
                    (*it2)();
                }
                it.remove();
            }
        }

        /*High Priority Signals*/
        for (it = pcb->priorityHigh.begin(); it != pcb->priorityHigh.end(); it++) {
            SignalId id = *it;
            if (pcb->localSignalStatus[id] && globalSignalStatus[id]) {
                if (id == 0){
                    pcb->toKill = 1;
                    /*it++;*/
                    continue;
                }
                for (List<SignalHandler>::Iterator it2 = pcb->signalHandlers[id].begin();
                        it2 != pcb->signalHandlers[id].end(); it2++)
                {
                    (*it2)();
                }
                it.remove();
            }
        }

        /*Mid Priority Signals*/
        for (it = pcb->priorityMid.begin(); it != pcb->priorityMid.end(); it++) {
            SignalId id = *it;
            if (pcb->localSignalStatus[id] && globalSignalStatus[id]) {
                if (id == 0){
                    pcb->toKill = 1;
                    /*it++;*/
                    continue;
                }
                for (List<SignalHandler>::Iterator it2 = pcb->signalHandlers[id].begin();
                        it2 != pcb->signalHandlers[id].end(); it2++)
                {
                    (*it2)();
                }
                it.remove();
            }
        }

        /*Low Priority Signals*/
        for (it = pcb->priorityLow.begin(); it != pcb->priorityLow.end(); it++) {
            SignalId id = *it;
            if (pcb->localSignalStatus[id] && globalSignalStatus[id]) {
                if (id == 0){
                    pcb->toKill = 1;
                    /*it++;*/
                    continue;
                }
                for (List<SignalHandler>::Iterator it2 = pcb->signalHandlers[id].begin();
                        it2 != pcb->signalHandlers[id].end(); it2++)
                {
                    (*it2)();
                }
                it.remove();
            }
        }
    )   
}

void PCB::kill(PCB *killTarget){
    if (killTarget == nullptr) return;
    LOCKED(
        killTarget->setState(PCB::TERMINATED);
        if (killTarget->parent != nullptr) killTarget->parent->myThread->signal(1);
        killTarget->myThread->myPCB = nullptr;
        killTarget->awakeMyAsleep();
        killTarget->killCleanup();

        /*Instead deletion -> freeSpace()*/
        killTarget->freeSpace();

        /*delete killTarget;*/
        killTarget = nullptr;
    )
}

void PCB::killCleanup() {
    /*Remove references to this PCB in other PCBs*/
    for (List<PCB*>::Iterator it_pcb = allPCBs.begin(); it_pcb != allPCBs.end(); it_pcb++) {
        if (*it_pcb) {
            PCB*& pcb = *it_pcb;
            if (pcb->parent == this) pcb->parent = nullptr;

            for (List<PCB*>::Iterator it = pcb->waitingForMe.begin(); it != pcb->waitingForMe.end(); it++) {
                PCB*& waitingPCB = *it;
                if (waitingPCB == this) it.remove();
            }
        }
    }

    /*Remove references to this PCB in semaphores*/
    for (List<KernelSemaphore*>::Iterator it_sem = allKernelSemaphores.begin(); it_sem != allKernelSemaphores.end(); it_sem++) {
        if (*it_sem) { 
            KernelSemaphore* sem = *it_sem;
            for (List<PCB*>::Iterator it1 = sem->blockedInfTime.begin(); it1 != sem->blockedInfTime.end(); it1++) {
                PCB*& pcb = *it1;
                if (pcb == this) it1.remove();
            }
            for (List<KernelSemaphore::semPCB*>::Iterator it2 = sem->blockedWithTime.begin();
                    it2 != sem->blockedWithTime.end(); it2++) {
                PCB*& pcb = (*it2)->myPCB;
                if (pcb == this) it2.remove();
            }
        }
    }

    /*Remove references to this PCB in events*/
    for (int i = 0; i < IVT_SIZE; i++) {
        KernelEvent* evt = IVTEntry::IVTable[i]->myEvent;
        if (evt == nullptr) continue;
        for (List<PCB*>::Iterator it = evt->blockedList.begin(); it != evt->blockedList.end(); it++) {
            PCB*& pcb = *it;
            if (pcb == this) it.remove();
        }
    }
}

void PCB::freeSpace(){
    if(this == nullptr) return;
    if(stack != nullptr) delete[] stack;
    stack = nullptr;
}
