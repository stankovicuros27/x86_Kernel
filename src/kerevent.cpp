#include "kerevent.h"

 KernelEvent::KernelEvent(IVTNo ivtNox) : Semaphore(0){
    ivtNo = ivtNox;
    myCreator = running;
 }

KernelEvent::~KernelEvent(){}

void KernelEvent::wait(){
    if(running == myCreator){
        Semaphore::wait(0);
    }
}

void KernelEvent::signal(){
    if (Semaphore::val() < 0) {
        Semaphore::signal(0);
    }
}