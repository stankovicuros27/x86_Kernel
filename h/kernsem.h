#ifndef KERNELSEM_H
#define KERNELSEM_H

#include "locks.h"
#include "types.h"
#include "list.h"

class KernelSemaphore;
class PCB;

extern List<KernelSemaphore*> allKernelSemaphores;

class KernelSemaphore {
public:
    struct semPCB {
        PCB *myPCB; 
        Time timeToWait;
        semPCB(PCB *myPCB, Time timeToWait) : myPCB(myPCB) , timeToWait(timeToWait) {}
    };

    KernelSemaphore(int init);
    ~KernelSemaphore();

    int wait(Time maxTimeToWait);
    int signal(int n);
    int getVal() const { return val; }

    friend class Timer;
    friend class KernelEvent;
    friend class PCB;

protected:
    int val;
    List<semPCB*> blockedWithTime;
    List<PCB*> blockedInfTime;

    void insertTimePCB(semPCB *toInsert);
    void unblockPCBs(int &n);
    void tickSem();
    static void tickAllSems();
};

#endif