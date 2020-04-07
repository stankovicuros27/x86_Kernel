#include "testthr.h"

void TestThread::run(){
    for (int i = 0; i < 30; i++){
        LOCKED(
            cout << "TestThread run() : " << i << endl;
        )
        loop;
    }
}