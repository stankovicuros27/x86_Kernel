// File: thread.h
#ifndef THREAD_H
#define THREAD_H

#include "types.h"

StackSize const defaultStackSize = 4096UL;

Time const defaultTimeSlice = 2U; // default = 2*55ms

class PCB; // Kernel's implementation of a user's thread

class Thread {

public:
    void start();
    void waitToComplete();
    virtual ~Thread();
    //ID getId(){ return myPCB->getId(); }
    //static ID getRunningId(){ return running->getId(); }
    static Thread * getThreadById(ID id);

protected: //bilo je protected!
    friend class PCB;
    Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
    virtual void run(){}

private:
    PCB* myPCB;
};

void dispatch ();

#endif