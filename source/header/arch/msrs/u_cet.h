#pragma once
#include <msrs/msrs.h>

namespace msr {
	struct u_cet : BASE_MSR
	{
		static constexpr uint32_t number = 0x0000'06A0;

		union {
			struct {
				uint64_t sh_stk_en : 1; // Shadow stack enable in user mode
				uint64_t wr_shstk_en : 1; // WRSS instruction enable in user mode
				uint64_t reserved1 : 62;
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(u_cet::number) };
		}

		void store()
		{
			__writemsr(u_cet::number, value);
		}
	};
};
