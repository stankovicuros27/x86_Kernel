#include "semaphor.h"
#include "kernsem.h"

Semaphore::Semaphore(int init){
    LOCKED(
        myImpl = new KernelSemaphore(init);
    )
}

Semaphore::~Semaphore(){
    LOCKED(
        if(myImpl != nullptr){
            delete myImpl;
        }
    )
}

int Semaphore::wait(Time maxTimeToWait){
    int ret = myImpl->maxTimeToWait();
    return ret;
}

int Semaphore::signal(int n) {
    int ret = myImpl->signal(n);
    return ret;
}

int Semaphore::val() const {
    ret = myImpl->val();
    return ret;
}