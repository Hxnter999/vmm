#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct lstar
	{
		static constexpr uint32_t number = 0xC000'0082;

		union {
			struct {
				uint64_t syscall_entry_point : 64; // The address of the SYSCALL/SYSRET handler
			};
			uint64_t value;
		};
	};
};