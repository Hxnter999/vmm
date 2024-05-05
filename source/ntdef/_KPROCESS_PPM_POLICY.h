#pragma once
/* ------------------ */

//0x4 bytes (sizeof)
enum _KPROCESS_PPM_POLICY
{
    ProcessPpmDefault = 0,
    ProcessPpmThrottle = 1,
    ProcessPpmSemiThrottle = 2,
    ProcessPpmNoThrottle = 3,
    ProcessPpmWindowMinimized = 4,
    ProcessPpmWindowOccluded = 5,
    ProcessPpmWindowVisible = 6,
    ProcessPpmWindowInFocus = 7,
    MaxProcessPpmPolicy = 8
};
