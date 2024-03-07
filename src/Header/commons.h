#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>
#include <wdm.h>
#include <intrin.h>
#include "datatypes.h"

#pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union

void print(const char* fmt, ...);
