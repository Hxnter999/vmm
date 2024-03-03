#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>
#include <wdm.h>
#include <intrin.h>

#pragma warning(disable: 4100) // unreferenced parameter
#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
#pragma warning(disable: 4996) // deprecated functions

/* stdint */
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

void print(const char* fmt, ...);

// idc abt the tag
template <typename T, POOL_TYPE type = NonPagedPool>
T* allocate_pool(size_t size = sizeof(T))
{
	return reinterpret_cast<T*>(ExAllocatePoolWithTag(type, size, 'sgma'));
}

void free_pool(void* pool);
