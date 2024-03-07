#pragma once
#include "datatypes.h"

#pragma pack(push, 1)
union dtr {
	struct {
		uint8_t dtlimit;
		uint64_t dtbase;
	};

};
#pragma pack(pop)