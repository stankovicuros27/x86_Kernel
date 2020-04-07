#ifndef PCB_H
#define PCB_H

#include "locks.h"
#include "types.h"
#include "thread.h"
#include "list.h"

extern PCB *running;
extern PCB *mainPCB; 

extern List<PCB*> allPCBs;

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
    void waitAll();
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

    ID myID;
    static ID currentID;

    PCB(int mainPCB);  //Used only to make mainPCB

    static void runWrapper();
    void initializeStack(pFunction fp);

    friend class Timer;
    friend class Thread;
    friend class System;
};


#endif