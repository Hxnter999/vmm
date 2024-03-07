#pragma once
#include "../cpuid_t.h"
// 0x8000_0000

namespace CPUID {

	struct fn_vendor : BASE_CPUID {
		union
		{
			struct {
				struct
				{
					uint32_t largest_standard_function : 32;
				};

				struct
				{
					uint32_t vendor1 : 32; // 'htuA'
				};

				struct
				{
					uint32_t vendor2 : 32; // 'DMAc'
				};

				struct
				{
					uint32_t vendor3 : 32; // 'itne'
				};
			};

			cpuid_t cpuid;
		};

		bool is_amd_vendor() const
		{
			if (cpuid.cpu_info == 0) return false;

			return (vendor1 == 'htuA') && (vendor2 == 'DMAc') && (vendor3 == 'itne');
		}

		void load()
		{
			__cpuid(reinterpret_cast<int*>(&this->cpuid), 0x80000000);
		}
	};
};