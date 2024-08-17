#pragma once
#include <msrs/msrs.h>

namespace msr {
	// VM_HSAVE_PA holds page alligned physical address of the host save area
	// (not guaranteed to be used, other parts of the chip could be used too) 
	struct hsave_pa : BASE_MSR 
	{
		static constexpr uint32_t number = 0xC001'0117;

		union {
			struct { // a page aligned physical address
				uint64_t must_be_zero : 12;
				uint64_t host_save_area_pa : 52;
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(hsave_pa::number) };
		}

		void store()
		{
			__writemsr(hsave_pa::number, value);
		}
	};

};