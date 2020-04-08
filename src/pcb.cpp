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
   
    if (size > MAX_STACK) size = MAX_STACK; 
    if (size < MIN_STACK) size = MIN_STACK;
    stackSize = size / sizeof(Word); 
    initializeStack(runWrapper);

    allPCBs.insertBack(this);
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
    allPCBs.insertBack(this);
}

PCB::PCB(){} //used only to make idlePCB

PCB::~PCB(){                        //mozda da dodam da se brise iz liste svih PCBova?
    awakeMyAsleep();
    if (stackSize != 0) delete[] stack;
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
        if (this != nullptr && (state == PCB::CREATED || state == PCB::BLOCKED)){
            setState(PCB::READY);
            Scheduler::put(this);
        } 
    )
}

void PCB::awakeMyAsleep(){
    LOCKED(
        while(!waitingForMe.isEmpty()){
            PCB *toStart = waitingForMe.getFront();
            waitingForMe.deleteFront();
            toStart->startPCB(); 
        }
    )
    //ovde sam obrisao if(toStart != nullptr) ispred toStart->
}

void PCB::waitToComplete(){
    //pazi ovde uslove!!!!!!
    LOCKED(
        if (this != nullptr &&
        running != this && 
        this->state != PCB::TERMINATED && 
        this->state != PCB::IDLE && 
        this->state != PCB::CREATED &&
        (!this->isWaitingForMe() && !allowDeadLock))
        {        
            running->state = PCB::BLOCKED;
            this->waitingForMe.insertBack(running);
            dispatch();
        }
    )
}

//pozivam iz runninga za thread za koji zelim da vidim da li me ceka
bool PCB::isWaitingForMe(){
    bool ret = false;
    List<PCB*>::Elem *iter = running->waitingForMe.head;
    while(iter != nullptr){
        if (iter->data->getId() == this->getId()){
            ret = true;
            break;
        }
        iter = iter->next;
    }
    return ret;
}

void PCB::waitAll(){
    //pazi ovde uslove!!!!!!
    if(running != mainPCB) return;  //only mainPCB can wait for all other threads
    LOCKED(
        List<PCB*>::Elem *iter;
        iter = allPCBs.head;
        while(iter != nullptr){
            iter->data->waitToComplete();
            iter = iter->next;
        }
    )
}

PCB* PCB::getPCBById(ID id){
    List<PCB*>::Elem *iter = allPCBs.head;
    while(iter != nullptr){
        if (iter->data->getId() == id){
            return iter->data;
        }
        iter = iter->next;
    }
    return nullptr;
}

void PCB::runWrapper(){
    running->getMyThread()->run();
    LOCKED(
        running->awakeMyAsleep();
        running->setState(PCB::TERMINATED);
        dispatch();
    )
}
