#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct tsc_ratio : BASE_MSR
	{
		static constexpr uint32_t number = 0xC000'0104;

		union {
			struct {
				uint32_t fractional_part : 32;
				uint8_t integer_part : 8;
				uint64_t reserved40 : 24;
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(tsc_ratio::number) };
		}

		void store()
		{
			__writemsr(tsc_ratio::number, value);
		}
	};
};