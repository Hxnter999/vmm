#pragma once
#include "msrs.h"

//

namespace MSR {
	struct HSAVE_PA // VM_HSAVE_PA holds page alligned physical address of the host save area
	{
		static constexpr uint64_t MSR_VM_HSAVE_PA = 0xC0010117;

		union {
			struct {
				uint64_t must_be_zero : 12;
				uint64_t host_save_area_pa : 52;
			};
			uint64_t bits;
		};
	};

	template<>
	void loadMSR(HSAVE_PA& fn)
	{
		fn.bits = { __readmsr(HSAVE_PA::MSR_VM_HSAVE_PA) };
	}

	template<>
	void storeMSR(HSAVE_PA& fn)
	{
		__writemsr(HSAVE_PA::MSR_VM_HSAVE_PA, fn.bits);
	}
};