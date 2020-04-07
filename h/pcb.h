#ifndef PCB_H
#define PCB_H

#include "locks.h"
#include "types.h"
#include "thread.h"
#include "SCHEDULE.H"
#include <dos.h>
#include <iostream.h>
#include "list.h"

#define INIT_PSW 0x0200
#define MAX_STACK 0x1000
#define MIN_STACK 0x0100

extern PCB *running;
extern PCB *mainPCB; 

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

    //---constructors & destructors---
    PCB(StackSize size, Time timeSl, Thread *myThr = nullptr, State s = CREATED);
    virtual ~PCB();

    //---getters---
    Thread *getMyThread() const { return myThread; }
    StackSize getStackSize() const { return stackSize; }
    State getState() const { return state; }
    Time getTimeSlice() const { return timeSlice; }
    Word getId() const { return myID; }

    //---setters---
    void setState(State s){ state = s; } //mozda da bacim u private pa da terminate stavljam preko funkcije


    //TODO : unlimitedTime, waitToComplete, sleep...

    //---util funcs---
    void startPCB();
    void waitToComplete();
    void awakeMyAsleep();


protected: 
    StackSize stackSize;
    Word *stack;
    volatile Reg ss, sp, bp;

    Thread *myThread;
    List<PCB*> waitingForMe;

    Time timeSlice;
    PCB::State state;
    Word myLockVal;

    Word myID;
    static Word currentID;

    PCB(int mainPCB);  

    static void runWrapper();
    void initializeStack(pFunction fp);

    friend class Timer;
    friend class Thread;
    friend class System;
};


#endif