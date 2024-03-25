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

extern "C" NTSTATUS ZwQuerySystemInformation(
	uint32_t SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	ULONG * ReturnLength);

inline void print(const char* fmt, ...)
{
    va_list args;
    __va_start(&args, fmt);
    vDbgPrintExWithPrefix("[VMM] ", 0, 0, fmt, args);
}



inline uint32_t __stdcall GetSystemFirmwareTable(uint32_t FirmwareTableProviderSignature, uint32_t FirmwareTableID, void* pFirmwareTableBuffer, uint32_t BufferSize)
{

	uint32_t* pool = static_cast<uint32_t*>(ExAllocatePool(NonPagedPool, BufferSize));
	if (!pool) return 0;

	pool[0] = FirmwareTableProviderSignature;
	pool[2] = FirmwareTableID;
	pool[3] = BufferSize;
	pool[1] = 1;

	constexpr uint32_t SystemFirmwareTableInformation = 0x4C;
	ULONG ReturnLength = 0;
	NTSTATUS nstatus = ZwQuerySystemInformation(SystemFirmwareTableInformation, pool, BufferSize + 16, &ReturnLength);
	uint32_t ret{};

	if ((int)(nstatus + 0x80000000) < 0 || nstatus == -1073741789)
		ret = pool[3];
	if (nstatus >= 0 && pFirmwareTableBuffer)
		memcpy(pFirmwareTableBuffer, pool + 4, pool[3]);

	ExFreePool(pool);
	return ret;
}