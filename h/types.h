#ifndef TYPES_H
#define TYPES_H

//16bit types
typedef unsigned    Word;
typedef unsigned    Reg;

//from thread.h
typedef unsigned long   StackSize;
typedef unsigned int    Time; // time, x 55ms
typedef int             ID;

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

#endif