#pragma once
#include <vcpu/vcpu.h>

constexpr uint64_t hypercall_key = 'AMDV'; // AMD-V is the best :D

enum class hypercall_code : uint64_t
{
	unload,
	ping,
	get_base_address,
	get_process_cr3,
	get_physical_address,
	hide_physical_page,
	unhide_physical_page,
	read_physical_memory,
	write_physical_memory,
	read_virtual_memory,
	write_virtual_memory,
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