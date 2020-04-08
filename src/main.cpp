#include "system.h"
#include "scheduler.h"
#include <iostream.h>
#include "userthr.h"

int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]){

    cout << "Starting OS..." << endl;  
    System::initializeSystem();

    //userMain(argc, argv);
    UserThread *user = new UserThread(argc, argv);
    user->start();
    delete user;

    for (int i = 0; i < 3; i++){
        LOCKED(
            cout << "main..." <<endl;
        )
        loop;
    }

    System::restoreSystem();
    cout << "Terminating OS..." << endl;
    return 0;
}