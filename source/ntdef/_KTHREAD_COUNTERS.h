#pragma once
/* ------------------ */

//0x1a8 bytes (sizeof)
struct _KTHREAD_COUNTERS
{
    ULONGLONG WaitReasonBitMap;                                             //0x0
    struct _THREAD_PERFORMANCE_DATA* UserData;                              //0x8
    ULONG Flags;                                                            //0x10
    ULONG ContextSwitches;                                                  //0x14
    ULONGLONG CycleTimeBias;                                                //0x18
    ULONGLONG HardwareCounters;                                             //0x20
    struct _COUNTER_READING HwCounter[16];                                  //0x28
};
/* Used in */
// _KTHREAD

