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
    delete mainPCB;
    delete idlePCB;
    //deletePCBs();
    //deleteSemaphores();
}

void System::deletePCBs(){
    List<PCB*>::Iterator iter = allPCBs.begin();
    while(iter != allPCBs.end()){
        if(*iter) delete (*iter);
        iter++;
    }
}

void System::deleteSemaphores(){
    List<KernelSemaphore*>::Iterator iter = allKernelSemaphores.begin();
    while(iter != allKernelSemaphores.end()){
        if(*iter) delete (*iter);
        iter++;
    }
}


