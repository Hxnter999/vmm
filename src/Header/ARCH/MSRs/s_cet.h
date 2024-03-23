#pragma once
#include "msrs.h"

namespace MSR {
	struct S_CET : BASE_MSR
	{
		static constexpr uint32_t MSR_S_CET = 0x000006A2;

		union {
			struct {
				uint64_t sh_stk_en : 1; // Shadow stack enable in supervisor mode
				uint64_t wr_shstk_en : 1; // WRSS instruction enable in supervisor mode
				uint64_t reserved1 : 62;
			};
			uint64_t bits;
		};

		void load()
		{
			bits = { __readmsr(S_CET::MSR_S_CET) };
		}

		void store()
		{
			__writemsr(S_CET::MSR_S_CET, bits);
		}
	};
};
