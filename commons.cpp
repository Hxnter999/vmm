#include "commons.h"

void print(const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);
	vDbgPrintExWithPrefix("[VMM] ", 0, 0, fmt, args); // DPFLTR
	DbgPrintEx(0, 0, "\n"); // for wndbg... remove later
}