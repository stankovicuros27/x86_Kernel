#include "system.h"
#include "scheduler.h"
#include <iostream.h>
#include "userthr.h"

int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]){

    cout << endl << "Starting OS..." << endl;  
    System::initializeSystem();

    UserThread *userThr = new UserThread(argc, argv);
    userThr->start();
    userThr->waitToComplete();
    int ret = userThr->retVal();

    System::restoreSystem();
    cout << endl << "Finished userMain(), return code: " << ret << endl;
    cout << "Terminating OS..." << endl;

    delete userThr;
    return 0;
}