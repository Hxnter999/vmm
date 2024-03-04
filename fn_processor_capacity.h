#pragma once
#include "cpuid_t.h"
// 8000_0008

namespace cpuid {

	struct fn_processor_capacity {
		union
		{
			struct {
				uint32_t physical_address_size : 8;
				uint32_t linear_address_size : 8;
				uint32_t guest_physical_address_size : 8;
				uint32_t reserved24 : 8;
			} address_size_identifiers;

			struct {
				uint32_t reserved0 : 32;
			} reserved;

			struct {
				uint32_t number_of_physical_cores : 8;
				uint32_t reserved8 : 4;
				uint32_t apic_id_core_size : 4;
				uint32_t perf_tsc_size : 2;
				uint32_t reserved17 : 14;
			} size_identifiers;

			struct {
				uint32_t invlpgb_count_max : 16;
				uint32_t max_rdpru_id : 16;
			} rdpru_identifiers;

			cpuid_t cpuid;
		};
	};

	template<>
	void loadFn(fn_processor_capacity& fn)
	{
		__cpuid(reinterpret_cast<int*>(&fn.cpuid), 0x80000008);
	}
};