#include "system.h"

PCB *mainPCB = nullptr;
PCB *running = nullptr;
PCB *idlePCB = nullptr;

void System::initializeSystem(){
    mainPCB = new PCB(0);   //int constructor used for main pcb
    mainPCB->startPCB();
    running = mainPCB;
    idlePCB = new IdlePCB();
    Timer::initializeTimerIntr();    
}

void System::restoreSystem(){
    mainPCB->waitAll();
    Timer::restoreTimerIntr();
    delete mainPCB;
    delete idlePCB;
    delete running;
}



