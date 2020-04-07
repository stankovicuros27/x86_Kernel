#include "system.h"
#include "scheduler.h"
#include <iostream.h>
#include "testthr.h"

int main(int argc, char* argv[]){

    TestThread *t1 = new TestThread();
    TestThread *t2 = new TestThread();
    t1->start();
    t2->start();

    cout << "Starting OS..." << endl;  
    System::initializeSystem();

    for (int i = 0; i < 3; i++){
        LOCKED(
            cout << "main() :" << i <<endl;
        )
        loop
    }

    t1->waitToComplete();
    t2->waitToComplete();
    
    System::restoreSystem();
    cout << "Terminating OS..." << endl;
    return 0;
}






