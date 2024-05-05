#pragma once
/* ------------------ */

//0x438 bytes (sizeof)
struct _KPROCESS
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
    struct _LIST_ENTRY ProfileListHead;                                     //0x18
    ULONGLONG DirectoryTableBase;                                           //0x28
    struct _LIST_ENTRY ThreadListHead;                                      //0x30
    ULONG ProcessLock;                                                      //0x40
    ULONG ProcessTimerDelay;                                                //0x44
    ULONGLONG DeepFreezeStartTime;                                          //0x48
    struct _KAFFINITY_EX Affinity;                                          //0x50
    ULONGLONG AffinityPadding[12];                                          //0xf8
    struct _LIST_ENTRY ReadyListHead;                                       //0x158
    struct _SINGLE_LIST_ENTRY SwapListEntry;                                //0x168
    volatile struct _KAFFINITY_EX ActiveProcessors;                         //0x170
    ULONGLONG ActiveProcessorsPadding[12];                                  //0x218
    union
    {
        struct
        {
            ULONG AutoAlignment:1;                                          //0x278
            ULONG DisableBoost:1;                                           //0x278
            ULONG DisableQuantum:1;                                         //0x278
            ULONG DeepFreeze:1;                                             //0x278
            ULONG TimerVirtualization:1;                                    //0x278
            ULONG CheckStackExtents:1;                                      //0x278
            ULONG CacheIsolationEnabled:1;                                  //0x278
            ULONG PpmPolicy:3;                                              //0x278
            ULONG VaSpaceDeleted:1;                                         //0x278
            ULONG ReservedFlags:21;                                         //0x278
        };
        volatile LONG ProcessFlags;                                         //0x278
    };
    ULONG ActiveGroupsMask;                                                 //0x27c
    CHAR BasePriority;                                                      //0x280
    CHAR QuantumReset;                                                      //0x281
    CHAR Visited;                                                           //0x282
    union _KEXECUTE_OPTIONS Flags;                                          //0x283
    USHORT ThreadSeed[20];                                                  //0x284
    USHORT ThreadSeedPadding[12];                                           //0x2ac
    USHORT IdealProcessor[20];                                              //0x2c4
    USHORT IdealProcessorPadding[12];                                       //0x2ec
    USHORT IdealNode[20];                                                   //0x304
    USHORT IdealNodePadding[12];                                            //0x32c
    USHORT IdealGlobalNode;                                                 //0x344
    USHORT Spare1;                                                          //0x346
    unionvolatile _KSTACK_COUNT StackCount;                                 //0x348
    struct _LIST_ENTRY ProcessListEntry;                                    //0x350
    ULONGLONG CycleTime;                                                    //0x360
    ULONGLONG ContextSwitches;                                              //0x368
    struct _KSCHEDULING_GROUP* SchedulingGroup;                             //0x370
    ULONG FreezeCount;                                                      //0x378
    ULONG KernelTime;                                                       //0x37c
    ULONG UserTime;                                                         //0x380
    ULONG ReadyTime;                                                        //0x384
    ULONGLONG UserDirectoryTableBase;                                       //0x388
    UCHAR AddressPolicy;                                                    //0x390
    UCHAR Spare2[71];                                                       //0x391
    VOID* InstrumentationCallback;                                          //0x3d8
    union
    {
        ULONGLONG SecureHandle;                                             //0x3e0
        struct
        {
            ULONGLONG SecureProcess:1;                                      //0x3e0
            ULONGLONG Unused:1;                                             //0x3e0
        } Flags;                                                            //0x3e0
    } SecureState;                                                          //0x3e0
    ULONGLONG KernelWaitTime;                                               //0x3e8
    ULONGLONG UserWaitTime;                                                 //0x3f0
    ULONGLONG EndPadding[8];                                                //0x3f8
};
/* Used in */
// _EPROCESS
// _FAST_IO_DISPATCH
// _KAPC_STATE
// _KTHREAD

