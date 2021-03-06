#include <DOS.H>
#include <STDIO.H>
#include <STDARG.H>
#include "locks.h"

int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	LOCKED(
		va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	)
		return res;
}