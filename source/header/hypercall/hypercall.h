#pragma once
#include <vcpu/vcpu.h>

inline constexpr uint64_t hypercall_key = 'AMDV'; // AMD-V is the best :D

enum class hypercall_code : uint64_t
{
	unload = 0,
	ping,
	get_process_cr3,
	get_physical_address,
	hide_physical_page,
	unhide_physical_page
};

struct hypercall_t {
	union {
		uint64_t value;
		struct {
			hypercall_code code : 8;
			uint64_t key : 56;
		};
	};
};

void hypercall_handler(vcpu_t& cpu);