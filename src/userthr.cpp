#include "userthr.h"

int userMain(int argc, char* argv[]);

UserThread::UserThread(int argcx, char** argvx){
    argc = argcx;
    argv = argvx;
    retUserMain = 0;
}

UserThread::~UserThread(){
    waitToComplete();
}

void UserThread::run(){
    retUserMain = userMain(argc, argv);
}