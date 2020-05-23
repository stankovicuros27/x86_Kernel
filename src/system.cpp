#include "system.h"
#include "pcb.h"
#include "kernsem.h"

PCB *mainPCB = nullptr;
PCB *running = nullptr;
PCB *idlePCB = nullptr;

void System::initializeSystem(){
    mainPCB = new PCB(0);               //int constructor used for main pcb
    running = mainPCB;                  //don't start mainPCB!! already in scheduler after timer!!
    idlePCB = new IdlePCB();    
    Timer::initializeTimerIntr();    
}

void System::restoreSystem(){
    //mainPCB->waitAll();
    Timer::restoreTimerIntr();
    delete mainPCB; //maybe not?
    delete idlePCB; //maybe not?
}
