#ifndef LOCKS_H
#define LOCKS_H

#include "types.h"

void dispatch(); 

extern volatile Word lockVal;
extern volatile Word remainingTime;

#define LOCK              ++lockVal;
#define UNLOCK            if (--lockVal == 0 && (remainingTime == 0)) { dispatch(); } // pogledaj jos
#define LOCKED(block)     LOCK; block; UNLOCK;

#define DISABLE_INTR                asm { pushf; cli; }
#define RESTORE_INTR                asm { popf; }
#define DISABLED_INTR(block)        DISABLE_INTR; block; RESTORE_INTR;

#endif