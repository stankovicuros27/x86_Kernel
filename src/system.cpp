#include "system.h"

PCB *mainPCB;
PCB *running;
PCB *idlePCB;

void System::initializeSystem(){
    mainPCB = new PCB(0);   //int constructor used for main pcb
    running = mainPCB;
    idlePCB = new IdlePCB();

    Timer::initializeTimerIntr();    
}

void System::restoreSystem(){
    Timer::restoreTimerIntr();

    delete mainPCB;
    delete idlePCB;
}



