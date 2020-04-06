#ifndef PCB_H
#define PCB_H

#include "locks.h"
#include "types.h"
#include "thread.h"
#include <dos.h>

//PSW_I = 1
#define INIT_PSW 0x0200

//64KB
#define MAX_STACK 0x1000

class PCB {
public:
    enum state {
        CREATED,
        READY,
        RUNNING,
        BLOCKED,
        TERMINATED,
        IDLE
    };

    PCB(StackSize size, Time timeSl, pFunction fp = nullptr); //Treba da povezes sa threadom! preko runWrappera
    ~PCB();

    static void initializeIdlePCB();



private:
    Thread *myThread;
    StackSize stackSize;
    Word *stack;
    volatile Reg ss, sp, bp;
    Time timeSlice;
    PCB::state state;

    static void runWrapper();
    void initializeStack(pFunction fp);
    PCB(int kernelMain);

    friend class Timer;
    friend class Thread;
};


#endif