#pragma once
#include <msrs/msrs.h>

namespace MSR {
	struct CSTAR : BASE_MSR
	{
		static constexpr uint32_t MSR_CSTAR = 0xC0000083;

		union {
			struct {
				uint64_t syscall_entry_point : 64; // The address of the SYSCALL/SYSRET handler
			};
			uint64_t bits;
		};

		void load()
		{
			bits = { __readmsr(CSTAR::MSR_CSTAR) };
		}

		void store()
		{
			__writemsr(CSTAR::MSR_CSTAR, bits);
		}
	};
};