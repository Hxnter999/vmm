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

// TODO: port my logger class to make use of cpp20 style formatting (fmtlib) and remove this dbgprint
inline void print(const char* fmt, ...)
{
   va_list args;
   __va_start(&args, fmt);
   vDbgPrintExWithPrefix("[VMM] ", 0, 0, fmt, args);
}