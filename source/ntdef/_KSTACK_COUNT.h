#pragma once
/* ------------------ */

//0x4 bytes (sizeof)
union _KSTACK_COUNT
{
    LONG Value;                                                             //0x0
    ULONG State:3;                                                          //0x0
    ULONG StackCount:29;                                                    //0x0
};
/* Used in */
// _KPROCESS

