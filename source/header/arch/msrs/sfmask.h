#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct sfmask : BASE_MSR
	{
		static constexpr uint32_t number = 0xC000'0084;

		union {
			struct {
				uint32_t syscall_flag_mask : 32;
				uint32_t reserved : 32;
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(sfmask::number) };
		}

		void store()
		{
			__writemsr(sfmask::number, value);
		}
	};
};