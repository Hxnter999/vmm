#pragma once
/* ------------------ */

//0x20 bytes (sizeof)
struct _ALPC_PROCESS_CONTEXT
{
    struct _EX_PUSH_LOCK Lock;                                              //0x0
    struct _LIST_ENTRY ViewListHead;                                        //0x8
    volatile ULONGLONG PagedPoolQuotaCache;                                 //0x18
};
/* Used in */
// _EPROCESS

