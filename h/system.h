#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream.h>
#include "ivt.h"

class System {

public:
    static void initializeSystem();     //adjust IVT
    static void restoreSystem();        //restore IVT 
};

#endif