#pragma once
#include <msrs/msrs.h>

namespace MSR {
	struct TSC : BASE_MSR
	{
		static constexpr uint32_t MSR_TSC = 0x10;

		uint64_t value;

		void load()
		{
			value = { __readmsr(TSC::MSR_TSC) };
		}

		void store()
		{
			__writemsr(TSC::MSR_TSC, value);
		}
	};
};