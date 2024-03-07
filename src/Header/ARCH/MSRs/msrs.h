#pragma once
#include "../../datatypes.h"
#include "../../Util/bitset.h"
#include <intrin.h>
#pragma warning(disable : 4996)

namespace MSR {

	struct BASE_MSR // all MSRs *MUST* inherit from this (mostly to assure these functions are defined)
	{
		template<class Self>
		void load(this Self&& tis)
		{
			tis.load();
		}

		template<class Self>
		void store(this Self&& tis)
		{
			tis.store();
		}
	};

	// The MSRPM (MSR Permission Map) is a bitmap (2 bits per MSR [R,W], 1 == operation is intercepted) that determines the access permissions for each MSR.
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

		//bool at(uint64_t msr, bool read)
		
		void set(uint64_t msr, bool read, bool value) {
			uint64_t index;

			if (msr >= vector1_start && msr <= vector1_end) {
				index = msr - vector1_start;
			}
			else if (msr >= vector2_start && msr <= vector2_end) {
				index = msr - vector2_start + sizeof(vector1);
			}
			else if (msr >= vector3_start && msr <= vector3_end) {
				index = msr - vector3_start + sizeof(vector1) + sizeof(vector2);
			}
			else {
				return; 
			}

			index = index * 2 + (read ? 0 : 1);
			vector.set(index, value);
		}

		static constexpr uint64_t vector1_start = 0x0000'0000, vector1_end = 0x0000'1FFF;
		static constexpr uint64_t vector2_start = 0xC000'0000, vector2_end = 0xC000'1FFF;
		static constexpr uint64_t vector3_start = 0xC001'0000, vector3_end = 0xC001'1FFF;
	};
};