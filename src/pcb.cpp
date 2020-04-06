#include "pcb.h"

PCB::PCB(StackSize size, Time timeSl, void (*fun)()){   
    timeSlice = timeSl;

    if (size > MAX_STACK) size = MAX_STACK - 1; 
    stackSize = size / sizeof(Word); 
    if(fun != nullptr) initializeStack(fun);

    state = PCB::CREATED;
}

void PCB::initializeStack(pFunction fp){
    stack = new Word[stackSize];

    stack[stackSize - 1] = INIT_PSW;       
    stack[stackSize - 2] = FP_SEG(fp);
    stack[stackSize - 3] = FP_OFF(fp);

    ss = FP_SEG(stack + stackSize - 12);
    sp = FP_OFF(stack + stackSize - 12);
}

PCB::PCB(int kernelMain){
    state = running; 
    stack = nullptr;
    sp = 0;
    ss = 0;
    bp = 0;
    timeSlice = defaultTimeSlice;
    
}