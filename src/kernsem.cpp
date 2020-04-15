#include "kernsem.h"
#include "schedule.h"

List<KernelSemaphore*> allKernelSemaphores;

KernelSemaphore::KernelSemaphore(int init){
    val = init > 0 ? init : 0;
    LOCKED(
        allKernelSemaphores.insertBack(this);
    )
}

KernelSemaphore::~KernelSemaphore(){
    // LOCKED(
    //     while(!blockedInfTime.isEmpty()){
    //         PCB *toUnblock = blockedInfTime.getFront();
    //         blockedInfTime.deleteFront();
    //         toUnblock->unblockPCB();
    //     }
    //     while(!blockedWithTime.isEmpty()){
    //         PCB *toUnblock = blockedWithTime.getFront()->myPCB;
    //         blockedWithTime.deleteFront();
    //         toUnblock->unblockPCB();
    //     }
    // )
}

int KernelSemaphore::wait(Time maxTimeToWait){
    //if(maxTimeToWait < 0) return -1;    //not defined
    int ret = 1;
    LOCKED(
        if(--val < 0){
            PCB *toBlockPCB = running;
            semPCB *toBlockSemPCB;
            toBlockPCB->manuallyUnblocked = 0;
            if(maxTimeToWait == 0) blockedInfTime.insertBack(toBlockPCB);
            else {
                toBlockSemPCB = new semPCB(toBlockPCB, maxTimeToWait);
                insertTimePCB(toBlockSemPCB);
            }
            toBlockPCB->blockPCB();
            dispatch();
            /*delete toBlockSemPCB;   /*semPCB is only a wrapper for PCB (to include time) so we MUST delete it (we can do that just not)*/
            ret = toBlockPCB->manuallyUnblocked;
            toBlockPCB->manuallyUnblocked = 0;
        }
    )
    return ret; 
}

int KernelSemaphore::signal(int n){
    if(n < 0) return n;
    int incVal = (n == 0) ? 1 : n;
    int ret = 0;
    LOCKED(
        val += incVal;
        unblockPCBs(incVal);
        ret = n - incVal;
    )
    if(n == 0) return 0;
    return ret;
}

void KernelSemaphore::unblockPCBs(int &n){
    LOCKED(
        while(n > 0 && !blockedInfTime.isEmpty()){
            PCB *toUnblock = blockedInfTime.getFront();
            blockedInfTime.deleteFront();
            toUnblock->unblockPCB();
            toUnblock->manuallyUnblocked = 1;
            n--;
        }
        while(n > 0 && !blockedWithTime.isEmpty()){
            PCB *toUnblock = blockedWithTime.getFront()->myPCB;
            Time addWaitTime = blockedWithTime.getFront()->timeToWait;
            blockedWithTime.deleteFront();
            if(blockedWithTime.getFront() != nullptr)
                blockedWithTime.getFront()->timeToWait += addWaitTime;
            toUnblock->unblockPCB();
            toUnblock->manuallyUnblocked = 1;
            n--;
        }
    )
}

void KernelSemaphore::insertTimePCB(semPCB *toInsert){
    LOCKED(
            List<semPCB*>::Iterator iter = blockedWithTime.begin();
            while(iter != blockedWithTime.end() && (toInsert->timeToWait > (*iter)->timeToWait)){
                toInsert->timeToWait -= (*iter)->timeToWait;
                iter++;
            }
            if((*iter) != nullptr)
                (*iter)->timeToWait-=toInsert->timeToWait;
            iter.insertBefore(toInsert);
    )
}

void KernelSemaphore::tickSem(){
    LOCKED(
        if(--(blockedWithTime.getFront()->timeToWait) == 0){
            while(!blockedWithTime.isEmpty() && blockedWithTime.getFront()->timeToWait == 0){
                PCB *toUnblock = blockedWithTime.getFront()->myPCB;
                blockedWithTime.deleteFront();
                toUnblock->setState(PCB::READY);
                toUnblock->manuallyUnblocked = 0;
                val++;
                Scheduler::put(toUnblock);
            }
        }
    )
}

void KernelSemaphore::tickAllSems(){
    //if(allKernelSemaphores.isEmpty()) return;
    //List<KernelSemaphore*>::Iterator iter = allKernelSemaphores.begin();
    LOCKED(
        for (List<KernelSemaphore*>::Iterator iter = allKernelSemaphores.begin(); iter != allKernelSemaphores.end(); iter++){
            (*iter)->tickSem();
        }
    )
}