#pragma once
#include <msrs/msrs.h>

namespace MSR {
	// VM_HSAVE_PA holds page alligned physical address of the host save area
	// (not guaranteed to be used, other parts of the chip could be used too) 
	struct HSAVE_PA : BASE_MSR 
	{
		static constexpr uint32_t MSR_VM_HSAVE_PA = 0xC0010117;

		union {
			struct { // a page aligned physical address
				uint64_t must_be_zero : 12;
				uint64_t host_save_area_pa : 52;
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(HSAVE_PA::MSR_VM_HSAVE_PA) };
		}

		void store()
		{
			__writemsr(HSAVE_PA::MSR_VM_HSAVE_PA, value);
		}
	};

};