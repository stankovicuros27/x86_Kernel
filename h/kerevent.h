#ifndef KEREVENT_H
#define KEREVENT_H

#include "types.h"
#include "locks.h"
#include "event.h"
#include "pcb.h"
#include "ivtentry.h"
#include "semaphore.h"

class KernelEvent : private Semaphore {
public:
    KernelEvent(IVTNo ivtNox);
    ~KernelEvent();
    void wait();
    void signal();

private:
    IVTNo ivtNo;
    PCB *myCreator;
};

#endif