#ifndef USERTHR_H
#define USERTHR_H

#include "thread.h"
#include "types.h"
#include "locks.h"
#include <iostream.h>

class UserThread : public Thread {
public:
    UserThread(int argcx, char** argvx);
    int retVal(){ return retUserMain; }
protected:
    void run();
private:
    int argc;
    char** argv;
    int retUserMain;
};

#endif