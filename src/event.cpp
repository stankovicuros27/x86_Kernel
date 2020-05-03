#include "locks.h"
#include "event.h"
#include "kerevent.h"
#include "types.h"
#include "ivtentry.h"


Event::Event(IVTNo ivtNo) {
    LOCKED(
        myImpl = new KernelEvent(ivtNo);
    )
}

Event::~Event() {
    LOCKED(
        delete myImpl;
        myImpl = nullptr;
    )
}

void Event::wait() {
    myImpl->wait();
}

void Event::signal() {
    myImpl->signal();
}