#pragma once
#include <vcpu/vcpu.h>

constexpr uint64_t hypercall_key = 'AMDV'; // AMD-V is the best :D

enum class hypercall_code : uint64_t
{
	unload,
	ping,
	get_process_cr3,
	get_physical_address,
	hide_physical_page,
	unhide_physical_page
};

struct hypercall_t {
	union {
		struct {
			hypercall_code code : 8;
			uint64_t key : 56;
		};
		uint64_t value;
	};
};

void hypercall_handler(vcpu_t& cpu);