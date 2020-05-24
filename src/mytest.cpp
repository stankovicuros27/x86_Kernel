#include <iostream.h>
#include <stdlib.h>
#include <assert.h>

#include "bounded.h"
#include "intLock.h"
#include "keyevent.h"
#include "user.h"
#include "Event.h"
#include "semaphor.h"
#include "locks.h"
#include "list.h"

int syncPrintf(const char *format, ...);
volatile bool finish = false;
Semaphore sem(0);
List<Thread*> toReceiveSignals;

volatile int cnt = 0;

void signalIspis4(){
    syncPrintf("\nHello44! %d  From thread : %d\n", ++cnt, running->getId());
}

void signalIspis7(){
    syncPrintf("\nHello77! %d  From thread : %d\n", ++cnt, running->getId());
}

void signalIspis10(){
    syncPrintf("\nHello1010! %d  From thread : %d\n", ++cnt, running->getId());
}

class TestThread : public Thread {
public:
	TestThread (int makeChilds) : Thread(defaultStackSize, 2) {
        this->makeChilds = makeChilds;
        id = ++id0;
        if (id % 2) myPCB->toSendSignal = true;
    }
	virtual ~TestThread() {waitToComplete(); }

protected:
	virtual void run (){
        TestThread **childs;
        if(makeChilds > 0){
            childs = new TestThread*[2];
            for (int i = 0; i < 2; i++){
                childs[i] = new TestThread(makeChilds - 1);
                if(makeChilds == 1) toReceiveSignals.insertBack(childs[0]);
                childs[i]->start();
            }
        } else {
            sem.signal();
        }

        while(!finish);

        if(makeChilds > 0){
            for(int i = 0; i < 2; i++)
                delete (childs[i]);

            delete [] childs;
        }

        syncPrintf("\nTestThread %d finished!\n", this->getId());
    }

private:
    int makeChilds;
    static int id0;
    int id;
};

int TestThread::id0 = 0;

int userMain (int argc, char* argv[]){

    TestThread **childs = new TestThread*[2];

    for(int i = 0; i < 2; i++){
        childs[i] = new TestThread(2);
        childs[i]->registerHandler(10, signalIspis10);
        childs[i]->registerHandler(7, signalIspis7);
        childs[i]->registerHandler(4, signalIspis4);
        //childs[i]->swap(4, signalIspis4, signalIspis5);
        childs[i]->start();
    }

    sem.wait(0);
    sem.wait(0);
    sem.wait(0);
    sem.wait(0);
    sem.wait(0);
    sem.wait(0);
    sem.wait(0);

    int cnt = 0;
    for(List<Thread*>::Iterator it = toReceiveSignals.begin(); it != toReceiveSignals.end(); it++){
        if(++cnt%2)(*it)->signal(10);
        if(++cnt%2)(*it)->signal(7);
        if(++cnt%2)(*it)->signal(4);
    }

    finish = true;

    for(i = 0; i < 2; i++){
        delete (childs[i]);
    }

    delete [] childs;

    syncPrintf("\nUserMain Finished!\n");
    return 0;
}