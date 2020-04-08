#include "thread.h"

int syncPrintf(const char *format, ...);

/*
 	 Test: frekventni dispatch
*/

const int n = 10;

void tick(){}

class TestThread : public Thread
{
public:

	TestThread(): Thread(){}
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{
	for(int i=0;i<32000;i++)
	{
		for(int j=0;j<16;j++){
		dispatch();
		syncPrintf(".");
		}
	}
}


int userMain(int argc, char** argv)
{
	syncPrintf("Test starts.\n");
	TestThread t[n];
	int i;
	for(i=0;i<n;i++)
	{
		t[i].start();
	}
	for(i=0;i<n;i++)
	{
		t[i].waitToComplete();
	}
	syncPrintf("Test ends.\n");
	return 0;
}


