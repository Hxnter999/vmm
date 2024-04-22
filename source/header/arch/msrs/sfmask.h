#pragma once
#include <msrs/msrs.h>

namespace MSR {
	struct SFMASK : BASE_MSR
	{
		static constexpr uint32_t MSR_SFMASK = 0xC0000084;

		union {
			struct {
				uint32_t syscall_flag_mask : 32;
				uint32_t reserved : 32;
			};
			uint64_t bits;
		};

		void load()
		{
			bits = { __readmsr(SFMASK::MSR_SFMASK) };
		}

		void store()
		{
			__writemsr(SFMASK::MSR_SFMASK, bits);
		}
	};
};