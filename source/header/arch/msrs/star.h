#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct star : BASE_MSR
	{
		static constexpr uint32_t number = 0xC000'0081;

		union {
			struct {
				uint32_t syscall_target_eip : 32; // 0 : 31
				uint16_t syscall_cs_ss : 16; // 32 : 47
				uint16_t sysret_cc_ss : 16; // 48 : 63
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(star::number) };
		}

		void store()
		{
			__writemsr(star::number, value);
		}
	};
};