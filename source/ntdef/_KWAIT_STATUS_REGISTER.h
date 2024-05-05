#pragma once
/* ------------------ */

//0x1 bytes (sizeof)
union _KWAIT_STATUS_REGISTER
{
    UCHAR Flags;                                                            //0x0
    UCHAR State:3;                                                          //0x0
    UCHAR Affinity:1;                                                       //0x0
    UCHAR Priority:1;                                                       //0x0
    UCHAR Apc:1;                                                            //0x0
    UCHAR UserApc:1;                                                        //0x0
    UCHAR Alert:1;                                                          //0x0
};
/* Used in */
// _KTHREAD

