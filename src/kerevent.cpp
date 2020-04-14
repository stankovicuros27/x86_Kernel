#include "kerevent.h"
#include "ivtentry.h"
#include "schedule.h"

 KernelEvent::KernelEvent(IVTNo ivtNox){
    ivtNo = ivtNox;
    myCreator = running;
    val = 0;
    IVTEntry::IVTable[ivtNo]->initializeEvent(this);
 }

KernelEvent::~KernelEvent(){
    IVTEntry::IVTable[ivtNo]->restoreEvent();
}

void KernelEvent::wait(){
    LOCKED(
        PCB *toBlock = running;
        val--;
            blockedList.insertBack(toBlock);
            toBlock->setState(PCB::BLOCKED);
            dispatch();
    )
}

void KernelEvent::signal(){
    LOCKED(
        if(val < 0){
            val++;
            if(!blockedList.isEmpty()){
                PCB *toUnblock = blockedList.getFront();
                blockedList.deleteFront();
                toUnblock->setState(PCB::READY);
                Scheduler::put(toUnblock);
            }
        }
    )
}