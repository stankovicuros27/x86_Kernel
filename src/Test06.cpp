#include "thread.h"
#include "types.h"
#include "pcb.h"

int syncPrintf(const char *format, ...);
extern PCB *mainPCB;

/*
 	 Test: idleThread
*/


void tick(){}

class TestThread : public Thread
{
private:
	TestThread *t;

public:

	TestThread(TestThread *thread): Thread(), t(thread){}
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{
	t->waitToComplete();
}


int userMain(int argc, char** argv)
{
	syncPrintf("Test starts.\n");
	TestThread *t1,*t2;
	t1 = new TestThread(t2);
	t2 = new TestThread(t1);
	t1->start();
	t2->start();
	delete t1;
	delete t2;
	//mainPCB->waitToComplete();
	for (int i = 0; i < 50; i++){
		loop
	}
	syncPrintf("Test ends.\n");
	return 0;
}


