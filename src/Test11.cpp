
#include "types.h"
#include "semaphor.h"
#include <iostream.h>
#include <stdlib.h>

#include "thread.h"

int syncPrintf(const char *format, ...);

/*
 	 Test: Semafori sa spavanjem 3
*/

int t=-1;

Semaphore s(0);

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
	s.signal();
	syncPrintf("Thread %d finished: r = %d\n", getId(),r);
}

void tick()
{
	/*
	t++;
	if(t)
		syncPrintf("%d\n",t);
		*/
}


#include <iostream.h>

Semaphore* mutex = 0;

class Znak : public Thread
{
public:
	Znak(char znak, int n) : Thread(), znak(znak), n(n) {}
	virtual ~Znak() { waitToComplete(); }

	void run()
	{
		for (long i = 0; i < 100000; i++)
		{
			if (mutex->wait(n)) {
				for (int p = 0; p < 27; p++)
					shortloop
				cout << znak;
				mutex->signal();
			}

		}

		if (mutex->wait(n)) {
			cout << endl << znak << " finished" << endl;
			mutex->signal();
		}
	}

private:
	char znak;
	int n;

};


int userMain(int argc, char* argv[]) {
	mutex = new Semaphore(1);

	Znak* a = new Znak('a', 10);
	Znak* b = new Znak('b', 15);
	Znak* c = new Znak('c', 20);

	a->start();
	b->start();
	c->start();

	delete a;
	delete b;
	delete c;

	if (mutex->wait(1)) {
		cout << endl << "userMain finished" << endl;
		mutex->signal();
	}

	delete mutex;



	return 0;
}




