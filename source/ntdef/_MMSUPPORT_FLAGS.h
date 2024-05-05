#pragma once
/* ------------------ */

//0x4 bytes (sizeof)
struct _MMSUPPORT_FLAGS
{
    union
    {
        struct
        {
            UCHAR WorkingSetType:3;                                         //0x0
            UCHAR Reserved0:3;                                              //0x0
            UCHAR MaximumWorkingSetHard:1;                                  //0x0
            UCHAR MinimumWorkingSetHard:1;                                  //0x0
            UCHAR SessionMaster:1;                                          //0x1
            UCHAR TrimmerState:2;                                           //0x1
            UCHAR Reserved:1;                                               //0x1
            UCHAR PageStealers:4;                                           //0x1
        };
        USHORT u1;                                                          //0x0
    };
    UCHAR MemoryPriority;                                                   //0x2
    union
    {
        struct
        {
            UCHAR WsleDeleted:1;                                            //0x3
            UCHAR SvmEnabled:1;                                             //0x3
            UCHAR ForceAge:1;                                               //0x3
            UCHAR ForceTrim:1;                                              //0x3
            UCHAR NewMaximum:1;                                             //0x3
            UCHAR CommitReleaseState:2;                                     //0x3
        };
        UCHAR u2;                                                           //0x3
    };
};
/* Used in */
// _MMSUPPORT_INSTANCE

