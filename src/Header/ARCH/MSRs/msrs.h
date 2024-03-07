#pragma once
#include "../../datatypes.h"
#include "../../Util/bitset.h"
#include <intrin.h>
#pragma warning(disable : 4996)

namespace MSR {

	//needs better name lowk
	struct commonMSR // all MSRs *MUST* inherit from this (mostly to assure these functions are defined)
	{
		template<class Self>
		void loadMSR(this Self&& tis)
		{
			tis.loadMSR();
		}

		template<class Self>
		void storeMSR(this Self&& tis)
		{
			tis.storeMSR();
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
		constexpr bool at(uint64_t msr) const
		{
			return vector.at(msr - base);
		}

		constexpr void set(uint64_t msr, bool value)
		{
			vector.set(msr - base, value);
		}

		static constexpr uint64_t base = 0xC0000000;
	};
};

//load MSR and store MSR for msrpm (physAddress)