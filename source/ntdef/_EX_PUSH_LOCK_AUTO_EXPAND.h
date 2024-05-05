#pragma once
/* ------------------ */

//0x10 bytes (sizeof)
struct _EX_PUSH_LOCK_AUTO_EXPAND
{
    struct _EX_PUSH_LOCK LocalLock;                                         //0x0
    struct _EX_PUSH_LOCK_AUTO_EXPAND_STATE State;                           //0x8
    ULONG Stats;                                                            //0xc
};
