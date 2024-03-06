#pragma once
#include "datatypes.h"

namespace SEGMENT {
	struct segment_attribute {
		union {
			uint16_t attribute;
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

	struct segment_descriptor
	{
		uint16_t selector;
		segment_attribute attributes; // 16 bits
		uint32_t limit;
		uint64_t base; // only lower 32 bits are implemented
	};
}