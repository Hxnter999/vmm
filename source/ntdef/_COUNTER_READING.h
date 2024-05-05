#pragma once
/* ------------------ */

//0x18 bytes (sizeof)
struct _COUNTER_READING
{
    enum _HARDWARE_COUNTER_TYPE Type;                                       //0x0
    ULONG Index;                                                            //0x4
    ULONGLONG Start;                                                        //0x8
    ULONGLONG Total;                                                        //0x10
};
/* Used in */
// _KTHREAD_COUNTERS
// _THREAD_PERFORMANCE_DATA

