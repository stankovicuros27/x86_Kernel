// File: thread.h
#ifndef THREAD_H
#define THREAD_H

#include "types.h"

StackSize const defaultStackSize = 4096UL;
Time const defaultTimeSlice = 2U; // default = 2*55ms

class PCB; // Kernel's implementation of a user's thread

typedef void (*SignalHandler)();
typedef unsigned SignalId;

class Thread {

public:
    void start();
    void waitToComplete();
    virtual ~Thread();
    ID getId();
    static ID getRunningId();
    static Thread * getThreadById(ID id);

    //---Signals---
    void signal(SignalId signal);

    void registerHandler(SignalId signal, SignalHandler handler);
    void unregisterAllHandlers(SignalId id);

    void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);

    void blockSignal(SignalId signal);
    static void blockSignalGlobally(SignalId signal);

    void unblockSignal(SignalId signal);
    static void unblockSignalGlobally(SignalId signal);
    //---/Signals---
    
protected: 
    friend class PCB;
    Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
    virtual void run(){}

protected:
    PCB* myPCB;
};

void dispatch ();

#endif