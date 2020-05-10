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
    PCB(StackSize size, Time timeSl, Thread *myThr, State s = CREATED);
    virtual ~PCB();

    //---getters---
    Thread *getMyThread() const { return myThread; }
    StackSize getStackSize() const { return stackSize; }
    State getState() const { return state; }
    Time getTimeSlice() const { return timeSlice; }
    Word getId() const { return myID; }
    bool getUnlimitedTime() const { return timeSlice == 0; }

    //---setters---
    void setState(State s){ state = s; }

    //---util funcs---
    void startPCB();
    void blockPCB();
    void unblockPCB();
    void waitToComplete();
    static PCB* getPCBById(ID id);

    //---Signals---
    void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);
	static void handleSignals();
	static void kill(PCB *pcb);


    bool localSignalStatus[16];
    static bool globalSignalStatus[16];
    List<SignalHandler> signalHandlers[16];
    List<SignalId> mySignals;
    PCB *parent;
    bool toKill;
    //---/Signals---

protected: 
    StackSize stackSize;
    Word *stack;
    volatile Reg ss, sp, bp;

    Thread *myThread;
    List<PCB*> waitingForMe;

    Time timeSlice;
    PCB::State state;
    Word myLockVal;

    bool manuallyUnblocked;

    ID myID;
    static ID currentID;

    PCB(int mainPCB);   //Used only to make mainPCB
    PCB();              //Used only to make idlePCB

    void initializeStack(pFunction fp);
    static void runWrapper();

private:  
    void awakeMyAsleep();
    bool isWaitingForMe();
    void killCleanup();
    void freeSpace();

    friend class Timer;
    friend class Thread;
    friend class System;
    friend class KernelSemaphore;
};


#endif