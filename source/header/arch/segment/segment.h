#pragma once
#include <commons.h>

extern "C" void _sgdt(void* gdtr);


extern "C" uint16_t __read_cs();
extern "C" uint16_t __read_ss();
extern "C" uint16_t __read_ds();
extern "C" uint16_t __read_es();
extern "C" uint16_t __read_fs();
extern "C" uint16_t __read_gs();
extern "C" uint16_t __read_tr();
extern "C" uint16_t __read_ldtr();

extern "C" void __write_ds(uint16_t selector);
extern "C" void __write_es(uint16_t selector);
extern "C" void __write_fs(uint16_t selector);
extern "C" void __write_gs(uint16_t selector);
extern "C" void __write_tr(uint16_t selector);
extern "C" void __write_ldtr(uint16_t selector);


#pragma pack(push, 1)

struct segment_selector {
	union {
		uint16_t value;
		struct {
			uint16_t rpl : 2;
			uint16_t ti : 1;
			uint16_t index : 13;
		};
	};
};

struct segment_attribute {
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

struct segment_descriptor {
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

struct descriptor_table_register {
	uint16_t limit;
	uint64_t base;
};

struct segment_register_t
{
	segment_selector selector;
	segment_attribute attributes; // 16 bits
	uint32_t limit;
	uint64_t base;

	void get_attributes(uint64_t descriptor)
	{
		segment_descriptor* desc = reinterpret_cast<segment_descriptor*>(descriptor + selector.index * 8);

		attributes.type = desc->type;
		attributes.system = desc->system;
		attributes.dpl = desc->dpl;
		attributes.present = desc->present;
		attributes.avl = desc->avl;
		attributes.longmode = desc->long_mode;
		attributes.default_bit = desc->default_bit;
		attributes.granularity = desc->granularity;
		attributes.present = desc->present;
		attributes.reserved = 0;
	}
};

#pragma pack(pop)