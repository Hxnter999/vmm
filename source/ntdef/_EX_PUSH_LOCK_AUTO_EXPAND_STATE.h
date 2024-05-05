#pragma once
/* ------------------ */

//0x4 bytes (sizeof)
struct _EX_PUSH_LOCK_AUTO_EXPAND_STATE
{
    union
    {
        struct
        {
            ULONG Expanded:1;                                               //0x0
            ULONG Transitioning:1;                                          //0x0
            ULONG Pageable:1;                                               //0x0
        };
        ULONG Value;                                                        //0x0
    };
};
/* Used in */
// _EX_PUSH_LOCK_AUTO_EXPAND

