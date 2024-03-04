#pragma once
#include "cpuid_t.h"
// 0x8000_0000

namespace cpuid {

	struct fn_vendor {
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
	};

	template<>
	void loadFn(fn_vendor& fn)
	{
		__cpuid(reinterpret_cast<int*>(&fn.cpuid), 0x80000000);
	}
};