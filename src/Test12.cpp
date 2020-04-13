
#include "types.h"
#include "semaphor.h"
#include <iostream.h>
#include <stdlib.h>

#include "thread.h"

int syncPrintf(const char *format, ...);

/*
 	 Test: Semafori sa spavanjem 4
*/

int t=-1;

const int n=15;

Semaphore s(1);

class TestThread : public Thread
{
private:
	Time waitTime;

public:

	TestThread(Time WT): Thread(), waitTime(WT){}
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{
	syncPrintf("Thread %d waits for %d units of time.\n",getId(),waitTime);
	int r = s.wait(waitTime);
	if(getId()%2)
		s.signal();
	syncPrintf("Thread %d finished: r = %d\n", getId(),r);
}

void tick()
{
	t++;
	if(t)
		syncPrintf("%d\n",t);
}

int userMain(int argc, char** argv)
{
	syncPrintf("Test starts.\n");
	TestThread* t[n];
	int i;
	for(i=0;i<n;i++)
	{
		t[i] = new TestThread(5*(i+1));
		t[i]->start();
	}
	for(i=0;i<n;i++)
	{
		t[i]->waitToComplete();
	}
	delete t;
	syncPrintf("Test ends.\n");
	return 0;
}


