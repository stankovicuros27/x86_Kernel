#include "timer.h"

pInterrupt Timer::oldTimerInterrupt = 0;

// --- New timer interrupt ---
//void tick();
Word tss;
Word tsp;
Word tbp;
volatile Word Timer::remainingTime = 1;

void interrupt Timer::timerIntr(...){
    //tick();
    asm int utilEntry

    if(--Timer::remainingTime == 0){
        asm {
            mov tss, ss
            mov tsp, sp
            mov tbp, bp
        }
        System::running->ss = tss;
        System::running->sp = tsp;
        System::running->bp = tbp;

        if(System::running->state == PCB::RUNNING){
            Scheduler::put(System::running);
        }
        System::running = Scheduler::get();
        
        // if(System::running == nullptr){
        //     System::running = System::idlePCB;
        // }

        Timer::remainingTime = System::running->timeSlice;
        tss = System::running->ss;
        tsp = System::running->sp;
        tbp = System::running->bp;
        asm {
            mov ss, tss
            mov sp, tsp
            mov bp, tbp
        }
    }
}

void Timer::initializeTimerIntr(){
    DISABLED_INTR(
        Timer::oldTimerInterrupt = getvect(timerEntry);
        setvect(utilEntry, Timer::oldTimerInterrupt);
        setvect(timerEntry, timerIntr);
    )
}

void Timer::restoreTimerIntr(){
    DISABLED_INTR(
        setvect(timerEntry, Timer::oldTimerInterrupt);
    )
}


