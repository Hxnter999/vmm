#pragma once
#include <commons.h>

// Standard Features: 0x0000_0000 - 0x0000_FFFF
// Software Reserved: 0x4000_0000 - 0x4000_00FF 255
// Extended Features: 0x8000_0000 - 0x8000_FFFF

namespace cpuid {
	template <typename T>
	inline void load(T&& t) {
		__cpuid(reinterpret_cast<int*>(&t.cpuid), t.id);
	}


	union cpuid_t
	{
		struct
		{
			uint32_t cpu_info[4];
		};

		struct
		{
			uint32_t eax;
			uint32_t ebx;
			uint32_t ecx;
			uint32_t edx;
		} registers;
	};
};