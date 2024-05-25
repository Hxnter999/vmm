#pragma once
#include <vmm.h>
#include <segmentation/segments.h>
// shouldnt be in this folder but its whatever

inline constexpr segment_selector_t host_cs = { .rpl = 0, .table = 0, .index = 1 };
inline constexpr segment_selector_t host_ds = { .rpl = 0, .table = 0, .index = 2 };

static interrupt_gate_descriptor_t create_interrupt_gate(register_t handler) {
	static constexpr uint64_t interrupt_gate_descriptor_type = 0x0E; // bit 1-3
	interrupt_gate_descriptor_t gate{ 0 };

	gate.offset_low = handler.word1;
	gate.offset_middle = handler.word2;
	gate.offset_high = handler.dword2;

	gate.interrupt_stack_table = 0;
	gate.descriptor_privilege_level = 0;
	gate.present = 1;
	gate.type = interrupt_gate_descriptor_type;
}
