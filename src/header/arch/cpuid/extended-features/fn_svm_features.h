#pragma once
#include <cpuid/cpuid_t.h>

namespace CPUID {
	// 8000_000A
	struct fn_svm_features : BASE_CPUID {
		static constexpr uint64_t id = 0x8000000A;

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
					uint32_t avic : 1;
					uint32_t reserved14 : 1;
					uint32_t vnsaveevirt : 1;
					uint32_t vgif : 1;
					uint32_t gmet : 1;
					uint32_t x2avic : 1;
					uint32_t ssscheck : 1;
					uint32_t specctrl : 1;
					uint32_t rogpt : 1;
					uint32_t reserved22 : 1;
					uint32_t host_mce_override : 1;
					uint32_t tlbictl : 1;
					uint32_t vnmi : 1;
					uint32_t lbsvirt : 1;
					uint32_t extlvtavicaccesschg : 1;
					uint32_t nestedvirtvmcbaddrchk : 1;
					uint32_t buslockthreshold : 1;
					uint32_t reserved30 : 2;
				} svm_feature_identification;
			};

			cpuid_t cpuid;
		};
		
		void load()
		{
			__cpuid(reinterpret_cast<int*>(&this->cpuid), id);
		}
	};
};