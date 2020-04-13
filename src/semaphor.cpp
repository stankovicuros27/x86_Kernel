#include "semaphor.h"
#include "kernsem.h"
#include "types.h"
#include "locks.h"

Semaphore::Semaphore(int init){
    LOCKED(
        myImpl = new KernelSemaphore(init);
    )
}

Semaphore::~Semaphore(){
    LOCKED(
        if(myImpl != nullptr){
            delete myImpl;
            myImpl = nullptr;
        }
    )
}

int Semaphore::wait(Time maxTimeToWait){
    int ret = myImpl->wait(maxTimeToWait);
    return ret;
}

int Semaphore::signal(int n) {
    int ret = myImpl->signal(n);
    return ret;
}

int Semaphore::val() const {
    int ret = myImpl->getVal();
    return ret;
}