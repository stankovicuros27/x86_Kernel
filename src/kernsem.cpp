#include "kernsem.h"
#include "schedule.h"

List<KernelSemaphore*> allKernelSemaphores;

KernelSemaphore::KernelSemaphore(int init){
    val = (init > 0 ? init : 0);
    LOCKED(
        allKernelSemaphores.insertBack(this);
    )
}

KernelSemaphore::~KernelSemaphore(){} //TODO

int KernelSemaphore::wait(Time maxTimeToWait){
    if(maxTimeToWait < 0) return -1;
    int ret = 1;
    LOCKED(
        if(--val < 0){
            PCB *toBlockPCB = running;
            toBlockPCB->manuallyUnblocked = 0;
            if(maxTimeToWait == 0) blockedInfTime.insertBack(toBlockPCB);
            else {
                semPCB toBlockSemPCB(toBlockPCB, maxTimeToWait);
                insertTimePCB(&toBlockSemPCB);
            }
            toBlockPCB->setState(PCB::BLOCKED);
            dispatch();
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
            toUnblock->setState(PCB::READY);
            Scheduler::put(toUnblock);
            toUnblock->manuallyUnblocked = 1;
            n--;
        }
        while(n > 0 && !blockedWithTime.isEmpty()){
            PCB *toUnblock = blockedWithTime.getFront()->myPCB;
            blockedWithTime.deleteFront();
            toUnblock->setState(PCB::READY);
            Scheduler::put(toUnblock);
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
            (*iter)->timeToWait-=toInsert->timeToWait;
            iter.insertBefore(toInsert);
    )
}

void KernelSemaphore::tickSem(){
    if(blockedWithTime.isEmpty()) return;
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
    if(allKernelSemaphores.isEmpty()) return;
    LOCKED(
        List<KernelSemaphore*>::Iterator iter = allKernelSemaphores.begin();
        for (;iter != allKernelSemaphores.end(); iter++){
            if((*iter) != nullptr) (*iter)->tickSem();
        }
    )
}






