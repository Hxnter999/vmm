#pragma once
#pragma warning(disable : 4996)

struct EFER
{
	static constexpr const __int64 MSR_EFER = 0xC0000080;

	union {
		struct {
			unsigned __int64 SCE : 1; // System Call Extensions
			unsigned __int64 reserved1 : 7;
			unsigned __int64 LME : 1; // Long Mode Enable
			unsigned __int64 reserved2 : 1;
			unsigned __int64 LMA : 1; // Long Mode Active
			unsigned __int64 NXE : 1; // No-Execute Enable
			unsigned __int64 SVME : 1; // Secure Virtual Machine Enable
			unsigned __int64 LMSLE : 1; // Long Mode Segment Limit Enable
			unsigned __int64 FFXSR : 1; // Fast FXSAVE/FXRSTOR
			unsigned __int64 TCE : 1; // Translation Cache Extension
			unsigned __int64 reserved3 : 1;
			unsigned __int64 MCOMMIT : 1; // Enable MCOMMIT instruction
			unsigned __int64 INTWB : 1; // Interruptible WBINVD/WBNOINVD enable
			unsigned __int64 reserved4 : 1;
			unsigned __int64 UAIE : 1; // Upper Address Ignore Enable
			unsigned __int64 AIBRSE : 1; // Automatic IBRS Enable
			unsigned __int64 reserved5 : 42;
		};
		unsigned __int64 bits;
	};
};