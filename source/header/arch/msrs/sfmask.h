#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct sfmask
	{
		static constexpr uint32_t number = 0xC000'0084;

		union {
			struct {
				uint32_t syscall_flag_mask : 32;
				uint32_t reserved : 32;
			};
			uint64_t value;
		};
	};
};