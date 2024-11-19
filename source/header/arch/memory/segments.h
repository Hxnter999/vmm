#pragma once
#include <commons.h>

extern "C" {
	void _sgdt(struct descriptor_table_register_t* gdtr);
	void _lgdt(struct descriptor_table_register_t* gdtr);

	uint16_t __read_cs();
	uint16_t __read_ss();
	uint16_t __read_ds();
	uint16_t __read_es();
	uint16_t __read_fs();
	uint16_t __read_gs();
	uint16_t __read_tr();
	uint16_t __read_ldtr();

	void __write_ds(uint16_t selector);
	void __write_es(uint16_t selector);
	void __write_fs(uint16_t selector);
	void __write_gs(uint16_t selector);
	void __write_tr(uint16_t selector);
	void __write_ldtr(uint16_t selector);
}
void setup_host_idt();

#pragma pack(push, 1)

struct segment_selector_t {
	union {
		uint16_t value;
		struct {
			uint16_t rpl : 2;
			uint16_t table : 1;
			uint16_t index : 13;
		};
	};
};

struct segment_attribute_t {
	union {
		uint16_t value;
		struct {
			uint16_t type : 4;
			uint16_t system : 1;
			uint16_t dpl : 2;
			uint16_t present : 1;
			uint16_t avl : 1;
			uint16_t longmode : 1;
			uint16_t default_bit : 1;
			uint16_t granularity : 1;
			uint16_t reserved : 4;
		};
	};
};

struct segment_descriptor_t {
	union
	{
		uint64_t value;
		struct
		{
			uint64_t limit_low : 16;
			uint64_t base_low : 16;
			uint64_t base_middle : 8;
			uint64_t type : 4;
			uint64_t system : 1;
			uint64_t dpl : 2;
			uint64_t present : 1;
			uint64_t limit_high : 4;
			uint64_t avl : 1;
			uint64_t long_mode : 1;
			uint64_t default_bit : 1;
			uint64_t granularity : 1;
			uint64_t base_high : 8;
		};
	};
};

struct descriptor_table_register_t {
	uint16_t limit;
	uint64_t base;
};

struct segment_register_t
{
	segment_selector_t selector;
	segment_attribute_t attributes;
	uint32_t limit;
	uint64_t base;

	void get_attributes(uint64_t descriptor)
	{
		auto& desc = reinterpret_cast<segment_descriptor_t*>(descriptor)[selector.index];

		attributes.type = desc.type;
		attributes.system = desc.system;
		attributes.dpl = desc.dpl;
		attributes.present = desc.present;
		attributes.avl = desc.avl;
		attributes.longmode = desc.long_mode;
		attributes.default_bit = desc.default_bit;
		attributes.granularity = desc.granularity;
		attributes.present = desc.present;
		attributes.reserved = 0;
	}
};

struct interrupt_gate_descriptor_t {
	union {
		uint64_t value1[2];
		struct {
			uint64_t offset_low : 16;
			uint64_t selector : 16;

			uint64_t interrupt_stack_table : 3;
			uint64_t zero : 5;
			uint64_t type : 4;
			uint64_t zero1 : 1;
			uint64_t descriptor_privilege_level : 2;
			uint64_t present : 1;
			uint64_t offset_middle : 16;

			uint64_t offset_high : 32;
			uint64_t reserved : 32;
		};
	};
};

struct task_state_segment_t {
	uint32_t reserved0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t reserved2;
	uint16_t reserved3;
	uint16_t io_map_base_address;
};

#pragma pack(pop)