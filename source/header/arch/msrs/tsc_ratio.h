#pragma once
#include <msrs/msrs.h>

namespace MSR {
	struct TSC_RATIO : BASE_MSR
	{
		static constexpr uint32_t MSR_TSC_RATIO = 0xC0000104;

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
			value = { __readmsr(TSC_RATIO::MSR_TSC_RATIO) };
		}

		void store()
		{
			__writemsr(TSC_RATIO::MSR_TSC_RATIO, value);
		}
	};
};