#include "system.h"
#include "scheduler.h"
#include "list.h"
#include <iostream.h>

int main(int argc, char* argv[]){

    cout << "Starting OS..." << endl;  

    Thread *t1 = new Thread();
    Thread *t2 = new Thread();
    t1->start();
    t2->start();

    System::initializeSystem();

    for (int i = 0; i < 30; i++){
        LOCKED(
            cout << "Main..." << endl;
        )
        for (int j = 0; j < 30000; j++){
            for (int k = 0; k < 30000; k++);
        }
    }
    
    System::restoreSystem();

    cout << "Terminating OS..." << endl;
    return 0;
}






