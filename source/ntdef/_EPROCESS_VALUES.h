#pragma once
/* ------------------ */

//0x68 bytes (sizeof)
struct _EPROCESS_VALUES
{
    ULONGLONG KernelTime;                                                   //0x0
    ULONGLONG UserTime;                                                     //0x8
    ULONGLONG ReadyTime;                                                    //0x10
    ULONGLONG CycleTime;                                                    //0x18
    ULONGLONG ContextSwitches;                                              //0x20
    LONGLONG ReadOperationCount;                                            //0x28
    LONGLONG WriteOperationCount;                                           //0x30
    LONGLONG OtherOperationCount;                                           //0x38
    LONGLONG ReadTransferCount;                                             //0x40
    LONGLONG WriteTransferCount;                                            //0x48
    LONGLONG OtherTransferCount;                                            //0x50
    ULONGLONG KernelWaitTime;                                               //0x58
    ULONGLONG UserWaitTime;                                                 //0x60
};
/* Used in */
// _EJOB

