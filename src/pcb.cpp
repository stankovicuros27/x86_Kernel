#include "pcb.h"

PCB::PCB(StackSize size, Time timeSl, Thread *myThr, State s){   

    timeSlice = timeSl;
    myThread = myThr;
    stack = nullptr;
    ss=sp=bp=0;
    myLockVal = 0;

    if (size > MAX_STACK) size = MAX_STACK; 
    if(size < MIN_STACK) size = MIN_STACK;
    stackSize = size / sizeof(Word); 
    if(myThr != nullptr) initializeStack(runWrapper);
   
    state = s; 
}

PCB::PCB(int mainPCB){          //used only for creating mainPCB
    state = PCB::CREATED; 
    stack = nullptr;
    sp = 0;
    ss = 0;
    bp = 0;
    timeSlice = defaultTimeSlice;
    myLockVal = 0;
}

PCB::~PCB(){} //overrajduj

void PCB::initializeStack(pFunction fp){

    stack = new Word[stackSize];

    stack[stackSize - 1] = INIT_PSW;       
    stack[stackSize - 2] = FP_SEG(fp);
    stack[stackSize - 3] = FP_OFF(fp);

    ss = FP_SEG(stack + stackSize - 12);
    sp = FP_OFF(stack + stackSize - 12);
    bp = sp;
}

void PCB::startPCB(){
    if (state != PCB::IDLE) state = PCB::READY;
    Scheduler::put(this);
}

void PCB::runWrapper(){
    running->getMyThread()->run();
    DISABLED_INTR(
        running->state = PCB::TERMINATED;
        dispatch();
    )
    
}

