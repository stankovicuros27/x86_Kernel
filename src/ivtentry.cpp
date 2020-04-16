#include "ivtentry.h"
#include "kerevent.h"
#include <dos.h>

IVTEntry* IVTEntry::IVTable[IVT_SIZE] = {nullptr};

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt pNewIntr){
    myNo = ivtNo;
    newIntr = pNewIntr;
    myEvent = nullptr;
    IVTable[ivtNo] = this;
}

IVTEntry::~IVTEntry(){
    restoreEvent();
}

void IVTEntry::signal(){
    myEvent->signal();
}

void IVTEntry::initializeEvent(KernelEvent *event){
    DISABLED_INTR(
        oldIntr = getvect(myNo);
        setvect(myNo, newIntr);
    )
    myEvent = event;
}

void IVTEntry::restoreEvent(){
    DISABLED_INTR(
        setvect(myNo, oldIntr);
    )
    IVTable[myNo] = nullptr;
    myEvent = nullptr;
}
