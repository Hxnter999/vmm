#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct cstar
	{
		static constexpr uint32_t number = 0xC000'0083;

		union {
			struct {
				uint64_t syscall_entry_point : 64; 
			};
			uint64_t value;
		};
	};
};