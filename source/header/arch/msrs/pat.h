#pragma once
#include <msrs/msrs.h>

namespace MSR {
	struct PAT : BASE_MSR // Page-Attribute Table Mechanism (7.8)
	{
		static constexpr uint32_t MSR_PAT = 0x00000277;

		enum class attribute_type : uint64_t
		{
			uncacheable = 0,
			write_combining = 1,
			write_through = 4,
			write_protected = 5,
			write_back = 6,
			uncacheable_no_write_combinining = 7
		};

		union {
			struct {
				attribute_type pa0 : 3;
				uint64_t reserved3 : 5;
				attribute_type pa1 : 3;
				uint64_t reserved11 : 5;
				attribute_type pa2 : 3;
				uint64_t reserved19 : 5;
				attribute_type pa3 : 3;
				uint64_t reserved27 : 5;
				attribute_type pa4 : 3;
				uint64_t reserved35 : 5;
				attribute_type pa5 : 3;
				uint64_t reserved43 : 5;
				attribute_type pa6 : 3;
				uint64_t reserved51 : 5;
				attribute_type pa7 : 3;
				uint64_t reserved59 : 5;
			};
			uint64_t bits;
		};

		void load()
		{
			bits = { __readmsr(PAT::MSR_PAT) };
		}

		void store()
		{
			__writemsr(PAT::MSR_PAT, bits);
		}
	};
};