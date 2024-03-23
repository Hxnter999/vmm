#pragma once
#include "msrs.h"

namespace MSR {
	struct PL3_SSP : BASE_MSR {

		static constexpr uint32_t MSR_PL3_SSP = 0x6A7;

		union {
			struct {
				uint64_t ssp;
			};
			uint64_t bits;
		};

		void load()
		{
			bits = { __readmsr(PL3_SSP::MSR_PL3_SSP) };
		}

		void store()
		{
			__writemsr(PL3_SSP::MSR_PL3_SSP, bits);
		}
	};
}