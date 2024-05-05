#pragma once
/* ------------------ */

//0x8 bytes (sizeof)
struct _EX_FAST_REF
{
    union
    {
        VOID* Object;                                                       //0x0
        ULONGLONG RefCnt:4;                                                 //0x0
        ULONGLONG Value;                                                    //0x0
    };
};
/* Used in */
// _CONTROL_AREA
// _EPROCESS
// _ETW_SILO_TRACING_BLOCK
// _SHARED_CACHE_MAP
// _WMI_LOGGER_CONTEXT

