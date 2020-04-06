#include "ivt.h"

void IVT::initializeIvt(){
    H_LOCKED(
        oldTimerInterrupt = getvect(timerEntry);
        setvect(utilEntry, oldTimerInterrupt);
        setvect(timerEntry, timer);
    )
}

void IVT::restoreIvt(){
    H_LOCKED(
        setvect(timerEntry, oldTimerInterrupt);
    )
}


