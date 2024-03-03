#pragma once
#include "MSRs.h"

struct EFER
{
	static constexpr uint64_t MSR_EFER = 0xC0000080;

	union {
		struct {
			uint64_t SCE : 1; // System Call Extensions
			uint64_t reserved1 : 7;
			uint64_t LME : 1; // Long Mode Enable
			uint64_t reserved2 : 1;
			uint64_t LMA : 1; // Long Mode Active
			uint64_t NXE : 1; // No-Execute Enable
			uint64_t SVME : 1; // Secure Virtual Machine Enable
			uint64_t LMSLE : 1; // Long Mode Segment Limit Enable
			uint64_t FFXSR : 1; // Fast FXSAVE/FXRSTOR
			uint64_t TCE : 1; // Translation Cache Extension
			uint64_t reserved3 : 1;
			uint64_t MCOMMIT : 1; // Enable MCOMMIT instruction
			uint64_t INTWB : 1; // Interruptible WBINVD/WBNOINVD enable
			uint64_t reserved4 : 1;
			uint64_t UAIE : 1; // Upper Address Ignore Enable
			uint64_t AIBRSE : 1; // Automatic IBRS Enable
			uint64_t reserved5 : 42;
		};
		uint64_t bits;
	};
};

template<>
void loadMSR(EFER& fn)
{
	fn.bits = __readmsr(EFER::MSR_EFER);
}