#include "pcb.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>

ID PCB::currentID = 0;
List<PCB*> allPCBs;

PCB::PCB(StackSize size, Time timeSl, Thread *myThr, State s){
    unlimitedTime = (timeSl == 0);
    timeSlice = timeSl;
    myThread = myThr;
    stack = nullptr;
    stackSize = 0;
    ss=sp=bp=0;
    myLockVal = 0;    
    myID = ++currentID;
    state = s; 
   
    if (myThr != nullptr){
        if (size > MAX_STACK) size = MAX_STACK; 
        if (size < MIN_STACK) size = MIN_STACK;
        stackSize = size / sizeof(Word); 
        initializeStack(runWrapper);
    } 
    allPCBs.insertBack(this);
}

PCB::PCB(int mainPCB){  //used only for creating mainPCB
    state = PCB::RUNNING; 
    stack = nullptr;
    sp = 0;
    ss = 0;
    bp = 0;
    unlimitedTime = 0;
    timeSlice = defaultTimeSlice;
    myLockVal = 0;
    myID = ++currentID;
    allPCBs.insertBack(this);
}

PCB::~PCB(){
    awakeMyAsleep();
    if (stackSize != 0) delete[] stack;
    stack = nullptr;
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
            if(toStart != nullptr) toStart->startPCB();
        }
    )
}

void PCB::waitToComplete(){
    //pazi ovde uslove!!!
    LOCKED(
        if(this != nullptr && running != this && this->state == PCB::READY){        
            running->state = PCB::BLOCKED;
            this->waitingForMe.insertBack(running);
            dispatch();
        }
    )
}

void PCB::waitAll(){
    if(running != mainPCB) return;  //only mainPCB can wait for all other threads
    LOCKED(
        List<PCB*>::Elem *iter;
        while(iter != nullptr){
            PCB *toWait = iter->data;
            if (toWait->state != PCB::IDLE && toWait->state != PCB::TERMINATED && toWait->state != PCB::CREATED){
                toWait->waitToComplete();
            }
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

//TODO
void PCB::killAll(){
    if(running != mainPCB) return;  //only mainPCB can kill all other threads
    LOCKED(
        
    )
}

void PCB::runWrapper(){
    running->getMyThread()->run();
    LOCKED(
        running->awakeMyAsleep();
        running->setState(PCB::TERMINATED);
        DISABLED_INTR(
            cout << "Gotova nit!" << endl;
        )
        dispatch();
    )
}
