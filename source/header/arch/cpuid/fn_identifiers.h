#pragma once
#include <cpuid/cpuid_t.h>

namespace cpuid {
	// 8000_0001
	struct fn_identifiers : BASE_CPUID {
		static constexpr int32_t id = 0x80000001;
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

				struct
				{
					uint32_t brand_id : 16;
					uint32_t reserved16 : 12;
					uint32_t pkg_type : 4;
				} brand_id_identifiers;

				struct {
					uint32_t lahf_sahf : 1;
					uint32_t cmp_legacy : 1;
					uint32_t svm : 1;
					uint32_t ext_apic_state : 1;
					uint32_t alt_mov_cr8 : 1;
					uint32_t abm : 1;
					uint32_t sse4a : 1;
					uint32_t mis_align_sse : 1;
					uint32_t _3d_now_prefetch : 1;
					uint32_t osvw : 1;
					uint32_t ibs : 1;
					uint32_t xop : 1;
					uint32_t skinit_stgi : 1;
					uint32_t wdt : 1;
					uint32_t reserved14 : 1;
					uint32_t lwp : 1;
					uint32_t fma4 : 1;
					uint32_t reserved17 : 2;
					uint32_t node_id : 1;
					uint32_t reserved20 : 1;
					uint32_t tbm : 1;
					uint32_t topology_extensions : 1;
					uint32_t reserved23 : 9;
				} feature_identifiers;

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
					uint32_t reserved18 : 2;
					uint32_t nx : 1;
					uint32_t reserved21 : 1;
					uint32_t mmxext : 1;
					uint32_t mmx : 1;
					uint32_t fxsr : 1;
					uint32_t ffxsr : 1;
					uint32_t page_1gb : 1;
					uint32_t rdtscp : 1;
					uint32_t reserved28 : 1;
					uint32_t long_mode : 1;
					uint32_t _3DNowExt : 1;
					uint32_t _3DNow : 1;
				}feature_identifiers_ext;
			};

			cpuid_t cpuid;
		};

		void load()
		{
			__cpuid(reinterpret_cast<int*>(&this->cpuid), fn_identifiers::id);
		}
	};

}