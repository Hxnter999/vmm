#pragma once
/* ------------------ */

//0x8 bytes (sizeof)
struct _JOBOBJECT_WAKE_FILTER
{
    ULONG HighEdgeFilter;                                                   //0x0
    ULONG LowEdgeFilter;                                                    //0x4
};
/* Used in */
// _EJOB
// _PS_PROCESS_WAKE_INFORMATION

