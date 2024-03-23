#pragma once
#include "msrs.h"

namespace MSR {
	struct U_CET : BASE_MSR
	{
		static constexpr uint32_t MSR_U_CET = 0x6A0;

		union {
			struct {
				uint64_t sh_stk_en : 1; // Shadow stack enable in user mode
				uint64_t wr_shstk_en : 1; // WRSS instruction enable in user mode
				uint64_t reserved1 : 62;
			};
			uint64_t bits;
		};

		void load()
		{
			bits = { __readmsr(U_CET::MSR_U_CET) };
		}

		void store()
		{
			__writemsr(U_CET::MSR_U_CET, bits);
		}
	};
};
