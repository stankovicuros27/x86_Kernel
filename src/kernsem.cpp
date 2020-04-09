#include "kernsem.h"

List<KernelSemaphore*> allKernelSemaphores;

KernelSemaphore::KernelSemaphore(int init){
    if(init < 0) {
        value = 0;
    } else {
        value = init;
    }
    LOCKED(
        allKernelSemaphores.insertBack(this);
    )
}

KernelSemaphore::~KernelSemaphore(){
    
}