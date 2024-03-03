#include "commons.h"

void print(const char* fmt, ...)
{
	va_list args;
	__va_start(&args, fmt);
	vDbgPrintExWithPrefix("[SVM] ", 0, 0, fmt, args); // DPFLTR
	DbgPrintEx(0, 0, "\n"); // for wndbg... remove later
}

void free_pool(void* pool)
{
	ExFreePoolWithTag(pool, 'sgma');
}
