#pragma once
#include "msrs.h"

namespace MSR {
	struct EFER
	{
		static constexpr uint64_t MSR_EFER = 0xC0000080;

		union {
			struct {
				uint64_t sce : 1; // System Call Extensions
				uint64_t reserved1 : 7;
				uint64_t lme : 1; // Long Mode Enable
				uint64_t reserved9 : 1;
				uint64_t lma : 1; // Long Mode Active
				uint64_t nxe : 1; // No-Execute Enable
				uint64_t svme : 1; // Secure Virtual Machine Enable
				uint64_t lmsle : 1; // Long Mode Segment Limit Enable
				uint64_t ffxsr : 1; // Fast FXSAVE/FXRSTOR
				uint64_t tce : 1; // Translation Cache Extension
				uint64_t reserved16 : 1;
				uint64_t mcommit : 1; // Enable MCOMMIT instruction
				uint64_t intwb : 1; // Interruptible WBINVD/WBNOINVD enable
				uint64_t reserved19 : 1;
				uint64_t uaie : 1; // Upper Address Ignore Enable
				uint64_t aibrse : 1; // Automatic IBRS Enable
				uint64_t reserved22 : 42;
			};
			uint64_t bits;
		};
	};

	template<>
	void loadMSR(EFER& fn)
	{
		fn.bits = { __readmsr(EFER::MSR_EFER) };
	}

	template<>
	void storeMSR(EFER& fn)
	{
		__writemsr(EFER::MSR_EFER, fn.bits);
	}
};