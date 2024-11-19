#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct pl3_ssp {
		static constexpr uint32_t number = 0x0000'06A7;

		union {
			struct {
				uint64_t ssp : 64;
			};
			uint64_t value;
		};
	};
}