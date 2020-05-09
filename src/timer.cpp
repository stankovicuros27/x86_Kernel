#include "timer.h"
#include "kernsem.h"

pInterrupt Timer::oldTimerInterrupt = 0;
volatile bool contextSwitchOnDemand = false;
volatile Time remainingTime = defaultTimeSlice;
volatile Word lockVal = 0;

void tick();
volatile Reg tss;
volatile Reg tsp;
volatile Reg tbp;

PCB* killTarget = nullptr;

void interrupt Timer::timerIntr(...) {
    if (!contextSwitchOnDemand) {
        if(remainingTime > 0) remainingTime--;
        asm int utilEntry
        tick();
        KernelSemaphore::tickAllSems();
    }
    
    if(contextSwitchOnDemand || (remainingTime == 0 && lockVal == 0 && !running->getUnlimitedTime())){
        asm {
            mov tss, ss
            mov tsp, sp
            mov tbp, bp
        }
        running->ss = tss;
        running->sp = tsp;
        running->bp = tbp;
        running->myLockVal = lockVal;
        
        if(running->state == PCB::RUNNING){
            running->state = PCB::READY;
            Scheduler::put(running);
        }

        killTarget = nullptr;

        while (true) {
            running = Scheduler::get();

            if (running == nullptr || running->state != PCB::READY){
                running = idlePCB;
            } else {
                running->state = PCB::RUNNING;
            }

            lockVal = running->myLockVal;
            remainingTime = running->timeSlice;
            tss = running->ss;
            tsp = running->sp;
            tbp = running->bp;
            asm {
                mov ss, tss
                mov sp, tsp
                mov bp, tbp
            }

            // if (!contextSwitchOnDemand) {
            PCB::handleSignals();
            if (killTarget != nullptr) {
                PCB::kill(killTarget);
            }
            if (running->toKill) {
                killTarget = running;
                killTarget->state = PCB::READY;
                continue;
            }
            // }
            break;
        }
    }

    contextSwitchOnDemand = false;
}

void Timer::initializeTimerIntr(){
    DISABLED_INTR(
        Timer::oldTimerInterrupt = getvect(timerEntry);
        setvect(utilEntry, Timer::oldTimerInterrupt);
        setvect(timerEntry, Timer::timerIntr);
    )
}

void Timer::restoreTimerIntr(){
    DISABLED_INTR(
        setvect(timerEntry, Timer::oldTimerInterrupt);
    )
}


