#pragma once
/* ------------------ */

//0x700 bytes (sizeof)
struct _KPRCB
{
    ULONG MxCsr;                                                            //0x0
    UCHAR LegacyNumber;                                                     //0x4
    UCHAR ReservedMustBeZero;                                               //0x5
    UCHAR InterruptRequest;                                                 //0x6
    UCHAR IdleHalt;                                                         //0x7
    struct _KTHREAD* CurrentThread;                                         //0x8
    struct _KTHREAD* NextThread;                                            //0x10
    struct _KTHREAD* IdleThread;                                            //0x18
    UCHAR NestingLevel;                                                     //0x20
    UCHAR ClockOwner;                                                       //0x21
    union
    {
        UCHAR PendingTickFlags;                                             //0x22
        struct
        {
            UCHAR PendingTick:1;                                            //0x22
            UCHAR PendingBackupTick:1;                                      //0x22
        };
    };
    UCHAR IdleState;                                                        //0x23
    ULONG Number;                                                           //0x24
    ULONGLONG RspBase;                                                      //0x28
    ULONGLONG PrcbLock;                                                     //0x30
    CHAR* PriorityState;                                                    //0x38
    CHAR CpuType;                                                           //0x40
    CHAR CpuID;                                                             //0x41
    union
    {
        USHORT CpuStep;                                                     //0x42
        struct
        {
            UCHAR CpuStepping;                                              //0x42
            UCHAR CpuModel;                                                 //0x43
        };
    };
    ULONG MHz;                                                              //0x44
    ULONGLONG HalReserved[8];                                               //0x48
    USHORT MinorVersion;                                                    //0x88
    USHORT MajorVersion;                                                    //0x8a
    UCHAR BuildType;                                                        //0x8c
    UCHAR CpuVendor;                                                        //0x8d
    UCHAR CoresPerPhysicalProcessor;                                        //0x8e
    UCHAR LogicalProcessorsPerCore;                                         //0x8f
    ULONGLONG TscFrequency;                                                 //0x90
    ULONGLONG PrcbPad04[5];                                                 //0x98
    struct _KNODE* ParentNode;                                              //0xc0
    ULONGLONG GroupSetMember;                                               //0xc8
    UCHAR Group;                                                            //0xd0
    UCHAR GroupIndex;                                                       //0xd1
    UCHAR PrcbPad05[2];                                                     //0xd2
    ULONG InitialApicId;                                                    //0xd4
    ULONG ScbOffset;                                                        //0xd8
    ULONG ApicMask;                                                         //0xdc
    VOID* AcpiReserved;                                                     //0xe0
    ULONG CFlushSize;                                                       //0xe8
    ULONGLONG PrcbPad11[2];                                                 //0xf0
    struct _KPROCESSOR_STATE ProcessorState;                                //0x100
    struct _XSAVE_AREA_HEADER* ExtendedSupervisorState;                     //0x6c0
    ULONG ProcessorSignature;                                               //0x6c8
    ULONG ProcessorFlags;                                                   //0x6cc
    ULONGLONG PrcbPad12a;                                                   //0x6d0
    ULONGLONG PrcbPad12[3];                                                 //0x6d8
};
/* Used in */
// _KPCR
// _KPRCB
// _KTHREAD

