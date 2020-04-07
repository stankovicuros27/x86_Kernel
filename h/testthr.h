#ifndef TESTTHR_H
#define TESTTHR_H

#include "thread.h"
#include "types.h"
#include "locks.h"
#include <iostream.h>

class TestThread : public Thread {
public:
    void run();
};

#endif