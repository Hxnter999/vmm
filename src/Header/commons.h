#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>
#include <wdm.h>
#include <intrin.h>
#include "datatypes.h"

#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
#pragma warning(disable: 4996)  // deprecated
#pragma warning(disable: 4200) // nonstandard extension used: zero-sized array in struct/union


void print(const char* fmt, ...);

uint32_t __stdcall GetSystemFirmwareTable(
    uint32_t FirmwareTableProviderSignature,
    uint32_t FirmwareTableID,
    void* pFirmwareTableBuffer,
    uint32_t BufferSize);