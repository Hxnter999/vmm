#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>
#include <wdm.h>
#include <intrin.h>

/* stdint */
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

// TODO: support cpp20 style formatting (fmtlib), logging to memory and/or serial port printing
inline void print([[maybe_unused]] const char* fmt, ...)
{
#ifdef _DEBUG
	// beware dbgprintex might cause problems when in host mode and generally not recommended.
   va_list args;
   __va_start(&args, fmt);
   vDbgPrintExWithPrefix("[VMM] ", 0, 0, fmt, args);
#endif // _DEBUG
}