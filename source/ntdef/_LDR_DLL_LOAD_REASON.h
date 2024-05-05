#pragma once
/* ------------------ */

//0x4 bytes (sizeof)
enum _LDR_DLL_LOAD_REASON
{
    LoadReasonStaticDependency = 0,
    LoadReasonStaticForwarderDependency = 1,
    LoadReasonDynamicForwarderDependency = 2,
    LoadReasonDelayloadDependency = 3,
    LoadReasonDynamicLoad = 4,
    LoadReasonAsImageLoad = 5,
    LoadReasonAsDataLoad = 6,
    LoadReasonEnclavePrimary = 7,
    LoadReasonEnclaveDependency = 8,
    LoadReasonUnknown = -1
};
/* Used in */
// _LDR_DATA_TABLE_ENTRY

