#include "system.h"

void System::initializeSystem(){
    Timer::initializeTimerIntr();    
}

void System::restoreSystem(){
    Timer::restoreTimerIntr();
}



