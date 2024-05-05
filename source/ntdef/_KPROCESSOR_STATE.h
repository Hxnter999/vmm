#pragma once
/* ------------------ */

//0x5c0 bytes (sizeof)
struct _KPROCESSOR_STATE
{
    struct _KSPECIAL_REGISTERS SpecialRegisters;                            //0x0
    struct _CONTEXT ContextFrame;                                           //0xf0
};
/* Used in */
// _KPRCB
// _POP_HIBER_CONTEXT

