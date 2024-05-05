#pragma once
/* ------------------ */

//0x10 bytes (sizeof)
struct _PS_DYNAMIC_ENFORCED_ADDRESS_RANGES
{
    struct _RTL_AVL_TREE Tree;                                              //0x0
    struct _EX_PUSH_LOCK Lock;                                              //0x8
};
/* Used in */
// _EPROCESS

