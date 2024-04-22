#pragma once
#include <msrs/msrs.h>

namespace MSR {
	struct TSC_AUX : BASE_MSR
	{
		static constexpr uint32_t MSR_TSC_AUX = 0xC0000103;

		union {
			//struct {

			//};
			uint64_t bits;
		};

		void load()
		{
			bits = { __readmsr(TSC_AUX::MSR_TSC_AUX) };
		}

		void store()
		{
			__writemsr(TSC_AUX::MSR_TSC_AUX, bits);
		}
	};
};