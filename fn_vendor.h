#pragma once
#include "cpuid_t.h"

namespace cpuid {
	// 0x0000_0000
	struct fn_vendor // Fn_Vendor
	{
		union
		{
			struct {
				struct
				{
					uint64_t stepping_id : 4;
					uint64_t model : 4;
					uint64_t family_id : 4;
					uint64_t processor_type : 2;
					uint64_t reserved1 : 2;
					uint64_t extended_model_id : 4;
					uint64_t extended_family_id : 8;
					uint64_t reserved2 : 4;
				} version_information;

				struct
				{
					uint64_t brand_index : 8;
					uint64_t clflush_line_size : 8;
					uint64_t max_addressable_ids : 8;
					uint64_t initial_apic_id : 8;
				} additional_information;

				struct
				{
					uint64_t streaming_simd_extensions_3 : 1;
					uint64_t pclmulqdq_instruction : 1;
					uint64_t ds_area_64bit_layout : 1;
					uint64_t monitor_mwait_instruction : 1;
					uint64_t cpl_qualified_debug_store : 1;
					uint64_t virtual_machine_extensions : 1;
					uint64_t safer_mode_extensions : 1;
					uint64_t enhanced_intel_speedstep_technology : 1;
					uint64_t thermal_monitor_2 : 1;
					uint64_t supplemental_streaming_simd_extensions_3 : 1;
					uint64_t l1_context_id : 1;
					uint64_t silicon_debug : 1;
					uint64_t fma_extensions : 1;
					uint64_t cmpxchg16b_instruction : 1;
					uint64_t xtpr_update_control : 1;
					uint64_t perfmon_and_debug_capability : 1;
					uint64_t reserved1 : 1;
					uint64_t process_context_identifiers : 1;
					uint64_t direct_cache_access : 1;
					uint64_t sse41_support : 1;
					uint64_t sse42_support : 1;
					uint64_t x2apic_support : 1;
					uint64_t movbe_instruction : 1;
					uint64_t popcnt_instruction : 1;
					uint64_t tsc_deadline : 1;
					uint64_t aesni_instruction_extensions : 1;
					uint64_t xsave_xrstor_instruction : 1;
					uint64_t osx_save : 1;
					uint64_t avx_support : 1;
					uint64_t half_precision_conversion_instructions : 1;
					uint64_t rdrand_instruction : 1;
					uint64_t reserved2 : 1;
				} feature_ecx;

				struct
				{
					uint64_t floating_point_unit_on_chip : 1;
					uint64_t virtual_8086_mode_enhancements : 1;
					uint64_t debugging_extensions : 1;
					uint64_t page_size_extension : 1;
					uint64_t timestamp_counter : 1;
					uint64_t rdmsr_wrmsr_instructions : 1;
					uint64_t physical_address_extension : 1;
					uint64_t machine_check_exception : 1;
					uint64_t cmpxchg8b : 1;
					uint64_t apic_on_chip : 1;
					uint64_t reserved : 1;
					uint64_t sysenter_sysexit_instructions : 1;
					uint64_t memory_type_range_registers : 1;
					uint64_t page_global_bit : 1;
					uint64_t machine_check_architecture : 1;
					uint64_t conditional_move_instructions : 1;
					uint64_t page_attribute_table : 1;
					uint64_t page_size_extension_36bit : 1;
					uint64_t processor_serial_number : 1;
					uint64_t clflush : 1;
					uint64_t reserved2 : 1;
					uint64_t debug_store : 1;
					uint64_t thermal_control_msrs_for_acpi : 1;
					uint64_t mmx_support : 1;
					uint64_t fxsave_fxrstor_instructions : 1;
					uint64_t sse_support : 1;
					uint64_t sse2_support : 1;
					uint64_t self_snoop : 1;
					uint64_t hyper_threading_technology : 1;
					uint64_t thermal_monitor : 1;
					uint64_t reserved3 : 1;
					uint64_t pending_break_enable : 1;
				} feature_edx;
			};

			cpuid_t cpuid;
		};

		bool VMXEnabled() const
		{
			return feature_ecx.virtual_machine_extensions;
		}
	};

	template<>
	void loadFn(fn_vendor& fn)
	{
		__cpuid(reinterpret_cast<int*>(&fn.cpuid), 0x00000000);
	}
};