#include "system.h"

void System::initializeSystem(){
    IVT::initializeIvt();    
}

void System::restoreSystem(){
    IVT::restoreIvt();
}



