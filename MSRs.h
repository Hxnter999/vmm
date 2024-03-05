#pragma once
#include "datatypes.h"
#include "bitset.h"
#include <intrin.h>
#pragma warning(disable : 4996)

namespace MSR {
	template<typename T>
	void loadMSR(T& fn);

	template<typename T>
	void storeMSR(T& fn);

	struct alignas(0x1000) msrpm {
		// 0x0 - 0x7FF
		util::bitset<0x800> vector1;

		// 0x800 - 0xFFF
		util::bitset<0x800> vector2;

		// 0x1000 - 0x17FF
		util::bitset<0x800> vector3;

		// 0x1800 - 0x1FFF
		util::bitset<0x800> vector4;
	};
};