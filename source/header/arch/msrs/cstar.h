#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct cstar : BASE_MSR
	{
		static constexpr uint32_t number = 0xC000'0083;

		union {
			struct {
				uint64_t syscall_entry_point : 64; 
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(cstar::number) };
		}

		void store()
		{
			__writemsr(cstar::number, value);
		}
	};
};