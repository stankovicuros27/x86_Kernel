#ifndef KERNELSEM_H
#define KERNELSEM_H

#include "locks.h"
#include "types.h"
#include "semaphor.h"
#include "pcb.h"
#include "list.h"

extern List<KernelSemaphore*> allKernelSemaphores;

class KernelSemaphore {
public:
    struct semPCB {
        PCB *myPCB; 
        Time timeToWait;
        semPCB(PCB *myPCB, Time timeToWait) : myPCB(myPCB) , timeToWait(timeToWait) {}
    }

    KernelSemaphore(int init);
    ~KernelSem();

    int wait(Time maxTimeToWait);
    int signal(int n);
    int val() const { return value; }

private:
    int val;
    List<semPCB*> blocked;
}

#endif