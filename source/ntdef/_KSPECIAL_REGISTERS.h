#pragma once
/* ------------------ */

//0xf0 bytes (sizeof)
struct _KSPECIAL_REGISTERS
{
    ULONGLONG Cr0;                                                          //0x0
    ULONGLONG Cr2;                                                          //0x8
    ULONGLONG Cr3;                                                          //0x10
    ULONGLONG Cr4;                                                          //0x18
    ULONGLONG KernelDr0;                                                    //0x20
    ULONGLONG KernelDr1;                                                    //0x28
    ULONGLONG KernelDr2;                                                    //0x30
    ULONGLONG KernelDr3;                                                    //0x38
    ULONGLONG KernelDr6;                                                    //0x40
    ULONGLONG KernelDr7;                                                    //0x48
    struct _KDESCRIPTOR Gdtr;                                               //0x50
    struct _KDESCRIPTOR Idtr;                                               //0x60
    USHORT Tr;                                                              //0x70
    USHORT Ldtr;                                                            //0x72
    ULONG MxCsr;                                                            //0x74
    ULONGLONG DebugControl;                                                 //0x78
    ULONGLONG LastBranchToRip;                                              //0x80
    ULONGLONG LastBranchFromRip;                                            //0x88
    ULONGLONG LastExceptionToRip;                                           //0x90
    ULONGLONG LastExceptionFromRip;                                         //0x98
    ULONGLONG Cr8;                                                          //0xa0
    ULONGLONG MsrGsBase;                                                    //0xa8
    ULONGLONG MsrGsSwap;                                                    //0xb0
    ULONGLONG MsrStar;                                                      //0xb8
    ULONGLONG MsrLStar;                                                     //0xc0
    ULONGLONG MsrCStar;                                                     //0xc8
    ULONGLONG MsrSyscallMask;                                               //0xd0
    ULONGLONG Xcr0;                                                         //0xd8
    ULONGLONG MsrFsBase;                                                    //0xe0
    ULONGLONG SpecialPadding0;                                              //0xe8
};
/* Used in */
// _KPROCESSOR_STATE

