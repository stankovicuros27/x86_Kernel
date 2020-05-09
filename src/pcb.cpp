#include "pcb.h"
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
        for(int i = 0; i < 16; i++){
            localSignalStatus[i] = 1;
        }
        allPCBs.insertBack(this);
    )
}

PCB::PCB(){ //used only to make idlePCB
    parent = nullptr;
    for(int i = 0; i < 16; i++){
        localSignalStatus[i] = 1;
    }
    toKill = 0;
} 

PCB::~PCB(){
    LOCKED(
        if (stack != nullptr) delete[] stack;
        stack = nullptr;
        List<PCB*>::Iterator iter = allPCBs.begin();
        while(iter != allPCBs.end()){
            if(*iter == this){
                allPCBs.remove(iter.current);
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

//pozivam iz runninga za thread za koji zelim da vidim da li me ceka
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

void PCB::waitAll(){
    if(running != mainPCB) return;  //only mainPCB can wait for all other threads
    LOCKED(
        List<PCB*>::Iterator iter = allPCBs.begin();
        for(;iter != allPCBs.end(); iter++){
            (*iter)->waitToComplete();
        }
    )
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

        if(running->parent) running->parent->myThread->signal(1);
        running->signal(2);
        handleSignals();

        dispatch();
    )
}

//---Signals---

void PCB::signal(SignalId signal){
    if(signal > 15) return;
    LOCKED(
        mySignals.insertBack(signal);
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
    //TODO
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
    PCB *pcb = (PCB*)running;
    List<SignalId>::Iterator it = pcb->mySignals.begin();
    while (it != pcb->mySignals.end()) {
		SignalId id = *it;
		if (pcb->localSignalStatus[id] && globalSignalStatus[id]) {
			if (id == 0){
                pcb->toKill = 1;
                return;
            }
			List<SignalHandler>::Iterator handit = (pcb->signalHandlers)[id].begin();
			while (handit != pcb->signalHandlers[id].end()) {
				(*handit)();
				handit++;
			}
			List<SignalId>::Elem *toDelete = it.current;
            it++;
            pcb->mySignals.remove(toDelete);
		}
        else {
            it++;
        }
	}
}
void PCB::kill(PCB *toKill){
    /*if(toKill == nullptr) return;
    toKill->myThread->myPCB=nullptr;
	toKill->awakeMyAsleep();
    toKill->changeChildsParents();
    //delete toKill;
    //toKill = nullptr;*/
}
void PCB::changeChildsParents(){
    if(this == nullptr) return;
    LOCKED(
        List<PCB*>::Iterator iter = allPCBs.begin();
        while(iter != allPCBs.end()){
            if(*iter && (*iter)->parent == this){
                (*iter)->parent = nullptr;
            }
            iter++;
        }
    )
}