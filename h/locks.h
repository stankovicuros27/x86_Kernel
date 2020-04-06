#ifndef LOCKS_H
#define LOCKS_H

#include "types.h"

void dispatch(); //for soft lock, to change context after timeout


extern Word volatile softLock; //global var 


#define S_LOCK              ++softLock;
#define S_UNLOCK            if (--softLock == 0 && Context::lockTimedOut()) { dispatch(); } // pogledaj jos
#define S_LOCKED(izraz)     S_LOCK; izraz; S_UNLOCK;


#define H_LOCK              asm { pushf; cli; }
#define H_UNLOCK            asm { popf; }
#define H_LOCKED(izraz)     H_LOCK; izraz; H_UNLOCK;


#endif