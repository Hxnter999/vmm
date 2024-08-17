#pragma once
#include <cpuid/cpuid_t.h>

namespace cpuid {
	// 8000_000A
	struct fn_svm_features : BASE_CPUID {
		static constexpr int32_t id = 0x8000000A;
		union
		{
			struct {
				struct {
					uint32_t svm_rev : 8;
					uint32_t reserved8 : 24;
				} svm_revision;

				struct {
					uint32_t number_of_asid : 32;
				} svm_revision_ext;

				struct {
					uint32_t reserved0 : 32;
				} reserved;

				struct {
					uint32_t nested_paging : 1;
					uint32_t lbr_virtualization : 1;
					uint32_t svm_lock : 1;
					uint32_t n_rip : 1;
					uint32_t tsc_rate_msr : 1;
					uint32_t vmcb_clean : 1; // VMCB clean bits
					uint32_t flush_by_asid : 1;
					uint32_t decode_assists : 1;
					uint32_t reserved8 : 2;
					uint32_t pause_filter : 1;
					uint32_t reserved11 : 1;
					uint32_t pause_filter_threshold : 1;
					uint32_t reserved13 : 19;
				} svm_feature_identification;
			};

			cpuid_t cpuid;
		};
		
		void load()
		{
			__cpuid(reinterpret_cast<int*>(&this->cpuid), fn_svm_features::id);
		}
	};
};