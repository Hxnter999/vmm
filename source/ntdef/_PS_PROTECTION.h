#pragma once
/* ------------------ */

//0x1 bytes (sizeof)
struct _PS_PROTECTION
{
    union
    {
        UCHAR Level;                                                        //0x0
        struct
        {
            UCHAR Type:3;                                                   //0x0
            UCHAR Audit:1;                                                  //0x0
            UCHAR Signer:4;                                                 //0x0
        };
    };
};
/* Used in */
// _EPROCESS

