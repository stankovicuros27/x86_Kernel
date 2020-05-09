#ifndef KEREVENT_H
#define KEREVENT_H

#include "types.h"
#include "locks.h"
#include "event.h"
#include "pcb.h"
#include "kernsem.h"
#include "list.h"

class KernelEvent {
public:
    KernelEvent(IVTNo ivtNox);
    ~KernelEvent();
    void wait();
    void signal();

    friend class PCB;

private:
    IVTNo ivtNo;
    PCB *myCreator;

    List<PCB*> blockedList;
    int val;
};

#endif