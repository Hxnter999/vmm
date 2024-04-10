#pragma once
#include <cpuid/cpuid_t.h>

namespace CPUID {
	//Fn0000_0001

	struct fn_processor : BASE_CPUID {
		static constexpr uint64_t id = 0x0000'0001;

		union 
		{
			struct {
				struct {
					uint32_t stepping : 4;
					uint32_t base_model : 4;
					uint32_t base_family : 4;
					uint32_t reserved12 : 4;
					uint32_t ext_model : 4;
					uint32_t ext_family : 8;
					uint32_t reserve28 : 4;
				} model_identifiers;

				struct {
					uint32_t eight_bit_brand_id : 8;
					uint32_t clflush : 8;
					uint32_t logical_processor_count : 8;
					uint32_t local_apic_id : 8;
				} misc;

				struct {
					uint32_t sse3 : 1;
					uint32_t PCLMULQDQ : 1;
					uint32_t reserved2 : 1;
					uint32_t monitor : 1;
					uint32_t reserved4 : 5;
					uint32_t ssse3 : 1;
					uint32_t reserved10 : 2;
					uint32_t fma : 1;
					uint32_t CMPXCHG16B : 1;
					uint32_t reserved14 : 5;
					uint32_t sse41 : 1;
					uint32_t sse42 : 1;
					uint32_t x2apic : 1;
					uint32_t movbe : 1;
					uint32_t popcnt : 1;
					uint32_t reserved24 : 1;
					uint32_t aes : 1;
					uint32_t xsave : 1;
					uint32_t osxsave : 1;
					uint32_t avx : 1;
					uint32_t f16c : 1;
					uint32_t rdrand : 1;
					uint32_t reserved31 : 1;
				}  feature_identifiers1;

				struct {
					uint32_t fpu : 1;
					uint32_t vme : 1;
					uint32_t de : 1;
					uint32_t pse : 1;
					uint32_t tsc : 1;
					uint32_t msr : 1;
					uint32_t pae : 1;
					uint32_t mce : 1;
					uint32_t cmpxch8b : 1;
					uint32_t apic : 1;
					uint32_t reserved10 : 1;
					uint32_t syscall_sysret : 1;
					uint32_t mtrr : 1;
					uint32_t pge : 1;
					uint32_t mca : 1;
					uint32_t cmov : 1;
					uint32_t pat : 1;
					uint32_t pse36 : 1;
					uint32_t reserved18 : 1;
					uint32_t clfsh : 1;
					uint32_t reserved20 : 3;
					uint32_t mmx : 1;
					uint32_t fxsr : 1;
					uint32_t sse : 1;
					uint32_t sse2 : 1;
					uint32_t reserved27 : 1;
					uint32_t htt : 1;
					uint32_t reserved29 : 3;
				}  feature_identifiers2;
			};
			cpuid_t cpuid;
		};

		void load() 
		{
			__cpuid(reinterpret_cast<int*>(&this->cpuid), id);
		}
	};
}