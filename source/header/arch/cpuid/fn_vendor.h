#pragma once
#include <cpuid/cpuid_t.h>
// 0x8000_0000

namespace cpuid {

	struct fn_vendor : BASE_CPUID {
		static constexpr int32_t id = 0x80000000;
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
			return (vendor1 == 'htuA') && (vendor2 == 'DMAc') && (vendor3 == 'itne');
		}

		void load()
		{
			__cpuid(reinterpret_cast<int*>(&this->cpuid), fn_vendor::id);
		}
	};
};