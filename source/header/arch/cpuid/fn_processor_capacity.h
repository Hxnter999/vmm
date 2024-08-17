#pragma once
#include <cpuid/cpuid_t.h>

namespace cpuid {
	// 8000_0008
	struct fn_processor_capacity : BASE_CPUID {
		static constexpr int32_t id = 0x80000008;
		union
		{
			struct {
				uint32_t physical_address_size : 8;
				uint32_t linear_address_size : 8;
				uint32_t guest_physical_address_size : 8;
				uint32_t reserved24 : 8;
			} address_size_identifiers;

			struct {
				uint32_t clzero : 1; // CLZERO instruction supported
				uint32_t inst_ret_cnt_msr : 1; // Instruction Retired Count MSR supported
				uint32_t rstr_fp_err_ptrs : 1; // FP Error Pointers Restored by XRSTOR
				uint32_t invlpgb : 1; // INVLPG instruction supported
				uint32_t rdpru : 1; // RDPRU instruction supported
				uint32_t reserved1 : 1;
				uint32_t be : 1; // Bandwidth Enforcement supported
				uint32_t reserved2 : 1;
				uint32_t mcommit : 1; // MCOMMIT instruction supported
				uint32_t wbnoinvd : 1; // WBNOINVD instruction supported
				uint32_t reserved3 : 2;
				uint32_t ibpb : 1; // Indirect branch predicrion barrier
				uint32_t int_wbinvd : 1; // WBINVD/WBNOINVD are interruptible.
				uint32_t ibrs : 1; // Indirect Branch Restricted Speculation.
				uint32_t stibp : 1; // Single Thread Indirect Branch Predictors
				uint32_t ibrs_always_on : 1; // Processor prefers that IBRS be left on.
				uint32_t stibp_always_on : 1; // Processor prefers that STIBP be left on.
				uint32_t ibrs_preferred : 1; // IBRS is preferred over software solution.
				uint32_t ibrs_same_mode : 1; // IBRS provides same mode speculation limits.
				uint32_t efer_lmsle_unsupported : 1; // EFER.LMSLE is unsupported.
				uint32_t invlpgb_nested_pages : 1; // INVLPGB support for invalidating guest nested translations.
				uint32_t reserved4 : 2;
				uint32_t ssbd : 1; // Speculative Store Bypass Disable
				uint32_t ssbd_virt_spec_ctrl : 1; // Use VIRT_SPEC_CTL for SSBD.
				uint32_t ssbd_not_required : 1; // SSBD is not required.
				uint32_t cppc : 1; // Collaborative Processor Performance Control
				uint32_t psfd : 1; // Predictive Store Forward Disable.
				uint32_t btc_no : 1; // The processor is not affected by branch type confusion.
				uint32_t ibpb_ret : 1; // The processor clears the return address predictor when MSR PRED_CMD.IBPB is written to 1.
				uint32_t reserved5 : 1;
 			}  extended_feature_identifiers;

			struct {
				uint32_t number_of_physical_threads : 8; // Number of physical threads - 1. The number of threads in the processor is NT+1
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

		void load()
		{
			__cpuid(reinterpret_cast<int*>(&this->cpuid), fn_processor_capacity::id);
		}
	};
};