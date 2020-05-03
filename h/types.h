#ifndef TYPES_H
#define TYPES_H

//16bit types
typedef unsigned    Word;
typedef unsigned    Reg;

//from thread.h
typedef unsigned long   StackSize;
typedef unsigned int    Time; // time, x 55ms
typedef int             ID;

//stack constants
#define INIT_PSW 0x0200
#define MAX_STACK 0x1000
#define MIN_STACK 0x0100

//interrupt routine pointer
typedef void interrupt  (*pInterrupt)(...);
typedef unsigned char   ivtEntry;

//function wrapper
typedef void    (*pFunction)();

//boolean
typedef int     bool;
#define false   0
#define true    1

#define nullptr 0

#define IVT_SIZE 256

#define loop for (int jff = 0; jff < 32000; jff++){\
                for (int kff = 0; kff < 32000; kff++);\
            }\

#define shortloop for(int fff = 0; fff < 32000; fff++);

typedef void (*SignalHandler)();
typedef unsigned SignalId;

#endif