#ifndef PCB_H
#define PCB_H

#include "locks.h"
#include "types.h"
#include "thread.h"
#include <dos.h>


#define INIT_PSW 0x0200
#define MAX_STACK 0x1000
#define MIN_STACK 0x0100

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

public: //treba protected, testing
    Thread *myThread;
    StackSize stackSize;
    Word *stack;
    volatile Reg ss, sp, bp;
    Time timeSlice;
    PCB::state state;

    static void runWrapper();
    void initializeStack(pFunction fp);

    friend class Timer;
    friend class Thread;
    friend class System;

    PCB(int mainPCB);  //only used by MainPCB
};


#endif