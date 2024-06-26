#pragma once
#include <msrs/msrs.h>

namespace MSR {
	struct LSTAR : BASE_MSR 
	{
		static constexpr uint32_t MSR_LSTAR = 0xC0000082;

		union {
			struct {
				uint64_t syscall_entry_point : 64; // The address of the SYSCALL/SYSRET handler
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(LSTAR::MSR_LSTAR) };
		}

		void store()
		{
			__writemsr(LSTAR::MSR_LSTAR, value);
		}
	};
};