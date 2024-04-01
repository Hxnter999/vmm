#pragma once
#include <commons.h>
#include <intrin.h>
#include "../../Util/bitset.h"
#pragma warning(disable : 4996)

namespace MSR {

	enum class access {
		read = 0,
		write = 1
	};

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
	class alignas(0x1000) msrpm_t {
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

	public:
		msrpm_t() : vector1{}, vector2{}, vector3{}, vector4{} {}

		void set(uint64_t msr, access access_bit, bool value = true) {
			[[maybe_unused]] constexpr uint64_t vector1_start = 0x0000'0000, vector1_end = 0x0000'1FFF;
			[[maybe_unused]] constexpr uint64_t vector2_start = 0xC000'0000, vector2_end = 0xC000'1FFF;
			[[maybe_unused]] constexpr uint64_t vector3_start = 0xC001'0000, vector3_end = 0xC001'1FFF;
			// bit::read = 0, bit::write = 1

			util::bitset<0x800>* target = nullptr;
			if (msr >= vector3_start && msr <= vector3_end) {
				target = &vector3;
			}
			else if (msr >= vector2_start && msr <= vector2_end) {
				target = &vector2;
			}
			else if (msr <= vector1_end) {
				target = &vector1;
			}
			else {
				return;
			}

			target->set((msr & 0x7FF) * 2 + static_cast<int>(access_bit), value);
		}
	};
};