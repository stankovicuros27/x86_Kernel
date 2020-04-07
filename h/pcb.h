#ifndef PCB_H
#define PCB_H

#include "locks.h"
#include "types.h"
#include "thread.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>


#define INIT_PSW 0x0200
#define MAX_STACK 0x1000
#define MIN_STACK 0x0100

extern PCB *running;
extern PCB *mainPCB; //ne treba?

class PCB {
public:
    enum State {
        CREATED,
        READY,
        RUNNING,
        BLOCKED,
        TERMINATED,
        IDLE
    };

    PCB(StackSize size, Time timeSl, Thread *myThr = nullptr, State s = CREATED); //Treba da povezes sa threadom! preko runWrappera
    virtual ~PCB();

    void startPCB();

    Thread *getMyThread() const {
        return myThread;
    }

    StackSize getStackSize() const {
        return stackSize;
    }

    State getState() const {
        return state;
    }

    Time getTimeSlice() const {
        return timeSlice;
    }

    void setState(State s){
        state = s;
    }

    //TODO : unlimitedTime, waitToComplete, sleep...

protected: 
    Thread *myThread;
    StackSize stackSize;
    Word *stack;
    volatile Reg ss, sp, bp;
    Time timeSlice;
    PCB::State state;
    Word myLockVal;

    PCB(int mainPCB);  //only used by MainPCB
    static void runWrapper();
    void initializeStack(pFunction fp);

    friend class Timer;
    friend class Thread;
    friend class System;
};


#endif