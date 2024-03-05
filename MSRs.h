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
		union {
			struct {
				// 0x0 - 0x7FF
				util::bitset<0x800> vector1;

				// 0x800 - 0xFFF
				util::bitset<0x800> vector2;

				// 0x1000 - 0x17FF
				util::bitset<0x800> vector3;

				// 0x1800 - 0x1FFF
				util::bitset<0x800> vector4;
			};
			util::bitset<0x2000> vector;
		};

	private:
		constexpr bool at(uint64_t MSR) const 
		{
			return vector.at(MSR - base);
		}

		constexpr void set(size_t index, bool value)
		{
			vector.set(index, value);
		}

		static constexpr uint64_t base = 0xC0000000;
	};
};