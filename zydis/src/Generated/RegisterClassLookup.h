static const ZydisRegisterClassLookupItem REG_CLASS_LOOKUP[] =
{
    /* INVALID */ { ZYDIS_REGISTER_NONE, ZYDIS_REGISTER_NONE, 0, 0 },
    /* GPR8    */ { ZYDIS_REGISTER_AL, ZYDIS_REGISTER_R15B, 8, 8 },
    /* GPR16   */ { ZYDIS_REGISTER_AX, ZYDIS_REGISTER_R15W, 16, 16 },
    /* GPR32   */ { ZYDIS_REGISTER_EAX, ZYDIS_REGISTER_R15D, 32, 32 },
    /* GPR64   */ { ZYDIS_REGISTER_RAX, ZYDIS_REGISTER_R15, 0, 64 },
    /* X87     */ { ZYDIS_REGISTER_ST0, ZYDIS_REGISTER_ST7, 80, 80 },
    /* MMX     */ { ZYDIS_REGISTER_MM0, ZYDIS_REGISTER_MM7, 64, 64 },
    /* XMM     */ { ZYDIS_REGISTER_XMM0, ZYDIS_REGISTER_XMM31, 128, 128 },
    /* YMM     */ { ZYDIS_REGISTER_YMM0, ZYDIS_REGISTER_YMM31, 256, 256 },
    /* ZMM     */ { ZYDIS_REGISTER_ZMM0, ZYDIS_REGISTER_ZMM31, 512, 512 },
    /* TMM     */ { ZYDIS_REGISTER_TMM0, ZYDIS_REGISTER_TMM7, 8192, 8192 },
    /* FLAGS   */ { ZYDIS_REGISTER_NONE, ZYDIS_REGISTER_NONE, 0, 0 },
    /* IP      */ { ZYDIS_REGISTER_NONE, ZYDIS_REGISTER_NONE, 0, 0 },
    /* SEGMENT */ { ZYDIS_REGISTER_ES, ZYDIS_REGISTER_GS, 16, 16 },
    /* TABLE   */ { ZYDIS_REGISTER_NONE, ZYDIS_REGISTER_NONE, 0, 0 },
    /* TEST    */ { ZYDIS_REGISTER_TR0, ZYDIS_REGISTER_TR7, 32, 32 },
    /* CONTROL */ { ZYDIS_REGISTER_CR0, ZYDIS_REGISTER_CR15, 32, 64 },
    /* DEBUG   */ { ZYDIS_REGISTER_DR0, ZYDIS_REGISTER_DR15, 32, 64 },
    /* MASK    */ { ZYDIS_REGISTER_K0, ZYDIS_REGISTER_K7, 64, 64 },
    /* BOUND   */ { ZYDIS_REGISTER_BND0, ZYDIS_REGISTER_BND3, 128, 128 }
};
