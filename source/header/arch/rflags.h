#pragma once
#include <commons.h>

union rflags_t
{
	struct {
		uint64_t carry_flag : 1;
		uint64_t reserved1 : 1;
		uint64_t parity_flag : 1;
		uint64_t reserved2 : 1;
		uint64_t adjust_flag : 1;
		uint64_t reserved3 : 1;
		uint64_t zero_flag : 1;
		uint64_t sign_flag : 1;
		uint64_t trap_flag : 1;
		uint64_t interrupt_flag : 1;
		uint64_t direction_flag : 1;
		uint64_t overflow_flag : 1;
		uint64_t io_privilege_level : 2;
		uint64_t nested_task : 1;
		uint64_t reserved4 : 1;
		uint64_t resume : 1;
		uint64_t virtual_8086 : 1;
		uint64_t alignment_check : 1;
		uint64_t virtual_interrupt : 1;
		uint64_t virtual_interrupt_pending : 1;
		uint64_t cpuid : 1;
		uint64_t reserved5 : 42;
	};
	uint64_t value;
};