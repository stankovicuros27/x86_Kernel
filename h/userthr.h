#ifndef USERTHR_H
#define USERTHR_H

#include "thread.h"

class UserThread : public Thread {
public:
    UserThread(int argcx, char** argvx);
    //~UserThread(); 
    int retVal(){ return retUserMain; }
protected:
    void run();

private:
    int argc;
    char** argv;
    int retUserMain;
};

#endif