#ifndef IVTENTRY_H
#define IVTENTRY_H

#include "kerevent.h"
#include "types.h"
#include "locks.h"

struct IVTEntry {
    pInterrupt oldIntr;
    pInterrupt newIntr;

    static IVTEntry *IVTable[IVT_SIZE];

    IVTEntry(IVTNo ivtNo, pInterrupt pNewIntr);
    ~IVTEntry();

    void initializeEvent(KernelEvent *event);
    void restoreEvent();
    void signal();

    IVTNo myNo;
    KernelEvent *myEvent;
};

//---------my---------
#define PREPAREENTRY(ivtNo,callOld)                 \
void interrupt intr##ivtNo(...);                    \
                                                    \
IVTEntry ivtEntry##ivtNo(ivtNo,intr##ivtNo);        \
void interrupt intr##ivtNo(...){                    \
	if(callOld) ivtEntry##ivtNo.oldIntr();          \
    ivtEntry##ivtNo.signal();                       \
    dispatch();                                     \
}                                                   \
//--------------------


/*
//-----copied-----
#define PREPAREENTRY(ivtNo, callOld)                    \
extern IVTEntry ivtEntry##ivtNo;                        \
                                                        \
void interrupt eventInterrupt##ivtNo(...) {             \
    ivtEntry##ivtNo.signal();                           \
    if (callOld) ivtEntry##ivtNo.oldIntr();             \
    dispatch();                                         \
}                                                       \
                                                        \
IVTEntry ivtEntry##ivtNo(ivtNo, eventInterrupt##ivtNo);
//----------------
*/


#endif