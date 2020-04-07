#include "pcb.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>

ID PCB::currentID = 0;
List<PCB*> allPCBs;

PCB::PCB(StackSize size, Time timeSl, Thread *myThr, State s){
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
    state = PCB::CREATED; 
    stack = nullptr;
    sp = 0;
    ss = 0;
    bp = 0;
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
        if (state != PCB::IDLE){
            state = PCB::READY;
            Scheduler::put(this);
        } 
    )
}

void PCB::awakeMyAsleep(){
    LOCKED(
        while(!waitingForMe.isEmpty()){
            PCB *toStart = waitingForMe.getFront();
            waitingForMe.deleteFront();
            if(!(toStart->getState() == PCB::IDLE || toStart->getState() == PCB::TERMINATED)){        
                toStart->startPCB();
            }
        }
    )
}

void PCB::waitToComplete(){
    LOCKED(
        if(running != this && running->state != PCB::IDLE && state != PCB::TERMINATED){
            running->state = PCB::BLOCKED;
            waitingForMe.insertBack(running);
            dispatch();
        }
    )
}

void PCB::waitAll(){
    LOCKED(
        while(allPCBs.size() != 0){
            PCB *toWait = allPCBs.getFront();
            if (toWait->state != PCB::IDLE && toWait->state != PCB::TERMINATED){
                toWait->waitToComplete();
            }
            allPCBs.deleteFront();
        }
    )
}

void PCB::runWrapper(){
    running->getMyThread()->run();
    LOCKED(
        running->awakeMyAsleep();
        running->setState(PCB::TERMINATED);
        dispatch();
    )
}
