#include "timer.h"

pInterrupt Timer::oldTimerInterrupt = 0;

// --- New timer interrupt ---
//void tick();
Word tss;
Word tsp;
Word tbp;
volatile Time Timer::remainingTime = defaultTimeSlice;

void interrupt Timer::timerIntr(...){
    //tick();
    asm int utilEntry

    if(--Timer::remainingTime == 0){
        asm {
            mov tss, ss
            mov tsp, sp
            mov tbp, bp
        }
        running->ss = tss;
        running->sp = tsp;
        running->bp = tbp;

        if(running->state == PCB::RUNNING){
            Scheduler::put(running);
        }
        running = Scheduler::get();
        
        if(running == nullptr){
            running = idlePCB;
        }

        Timer::remainingTime = running->timeSlice;
        tss = running->ss;
        tsp = running->sp;
        tbp = running->bp;
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


