#pragma once
/* ------------------ */

//0x8 bytes (sizeof)
union _PS_INTERLOCKED_TIMER_DELAY_VALUES
{
    ULONGLONG DelayMs:30;                                                   //0x0
    ULONGLONG CoalescingWindowMs:30;                                        //0x0
    ULONGLONG Reserved:1;                                                   //0x0
    ULONGLONG NewTimerWheel:1;                                              //0x0
    ULONGLONG Retry:1;                                                      //0x0
    ULONGLONG Locked:1;                                                     //0x0
    ULONGLONG All;                                                          //0x0
};
/* Used in */
// _EPROCESS

