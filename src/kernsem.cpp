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
    LOCKED(
        while(!blockedInfTime.isEmpty()){
            PCB *toUnblock = blockedInfTime.getFront();
            blockedInfTime.deleteFront();
            if (toUnblock != nullptr) toUnblock->unblockPCB();
        }
        while(!blockedWithTime.isEmpty()){
            PCB *toUnblock = blockedWithTime.getFront()->myPCB;
            blockedWithTime.deleteFront();
            if (toUnblock != nullptr) toUnblock->unblockPCB();
        }
        for(List<KernelSemaphore*>::Iterator it = allKernelSemaphores.begin();
            it != allKernelSemaphores.end(); it++)
        {
            if((*it) == this){
                it.remove();
                break;
            }
        }
    )
}

int KernelSemaphore::wait(Time maxTimeToWait){
    if(maxTimeToWait < 0) return -1;    //not defined
    int ret = 1;
    LOCKED(
        if(--val < 0){
            PCB *toBlockPCB = running;
            semPCB *toBlockSemPCB = nullptr;    /*wrapper struct*/
            toBlockPCB->manuallyUnblocked = 0;  

            if (maxTimeToWait == 0) {
                blockedInfTime.insertBack(toBlockPCB);  
            }
            else {
                toBlockSemPCB = new semPCB(toBlockPCB, maxTimeToWait);
                insertTimePCB(toBlockSemPCB);
            }

            toBlockPCB->blockPCB();
            dispatch();
            if(toBlockSemPCB != nullptr) delete toBlockSemPCB;   /*we MUST delete it (we can do that just now, or in tickSem! not both)*/
            
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
            if (toUnblock != nullptr) toUnblock->unblockPCB();
            if (toUnblock != nullptr) toUnblock->manuallyUnblocked = 1;
            n--;
        }
        while(n > 0 && !blockedWithTime.isEmpty()){
            PCB *toUnblock = blockedWithTime.getFront()->myPCB;
            Time addWaitTime = blockedWithTime.getFront()->timeToWait;
            blockedWithTime.deleteFront();
            if(blockedWithTime.getFront() != nullptr)
                blockedWithTime.getFront()->timeToWait += addWaitTime;
            if (toUnblock != nullptr) toUnblock->unblockPCB();
            if (toUnblock != nullptr) toUnblock->manuallyUnblocked = 1;
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
    if(blockedWithTime.isEmpty()) return;
    LOCKED(
        if(--(blockedWithTime.getFront()->timeToWait) == 0){
            while(!blockedWithTime.isEmpty() && blockedWithTime.getFront()->timeToWait == 0){
                PCB *toUnblock = blockedWithTime.getFront()->myPCB;
                blockedWithTime.deleteFront();
                toUnblock->unblockPCB();
                toUnblock->manuallyUnblocked = 0;
                val++;
            }
        }
    )
}

void KernelSemaphore::tickAllSems(){
    if(allKernelSemaphores.isEmpty()) return;
    LOCKED(
        for (List<KernelSemaphore*>::Iterator iter = allKernelSemaphores.begin(); iter != allKernelSemaphores.end(); iter++){
            (*iter)->tickSem();
        }
    )
}