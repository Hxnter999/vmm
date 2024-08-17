#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct pl3_ssp : BASE_MSR {

		static constexpr uint32_t number = 0x0000'06A7;

		union {
			struct {
				uint64_t ssp : 64;
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(pl3_ssp::number) };
		}

		void store()
		{
			__writemsr(pl3_ssp::number, value);
		}
	};
}