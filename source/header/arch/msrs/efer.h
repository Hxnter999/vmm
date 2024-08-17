#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct efer : BASE_MSR //Extended Feature Enable Register
	{
		static constexpr uint32_t number = 0xC000'0080;

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
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(efer::number) };
		}

		void store()
		{
			__writemsr(efer::number, value);
		}
	};
};