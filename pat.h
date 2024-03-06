#pragma once
#include "msrs.h"

namespace MSR {
	struct PAT // Page-Attribute Table Mechanism (7.8)
	{
		static constexpr uint64_t MSR_PAT = 0x00000277;

		union {
			struct {
				uint64_t pa0 : 3;
				uint64_t reserved3 : 5;
				uint64_t pa1 : 3;
				uint64_t reserved11 : 5;
				uint64_t pa2 : 3;
				uint64_t reserved19 : 5;
				uint64_t pa3 : 3;
				uint64_t reserved27 : 5;
				uint64_t pa4 : 3;
				uint64_t reserved35 : 5;
				uint64_t pa5 : 3;
				uint64_t reserved43 : 5;
				uint64_t pa6 : 3;
				uint64_t reserved51 : 5;
				uint64_t pa7 : 3;
				uint64_t reserved59 : 5;
			};
			uint64_t bits;
		};
	};

	template<>
	void loadMSR(PAT& fn)
	{
		fn.bits = { __readmsr(PAT::MSR_PAT) };
	}

	template<>
	void storeMSR(PAT& fn)
	{
		__writemsr(PAT::MSR_PAT, fn.bits);
	}
};