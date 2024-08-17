#pragma once
#include <msrs/msrs.h>

//The VM_CR MSR controls certain global aspects of SVM
//The following bits are defined in the AMD64 Architecture Programmer's Manual Volume 2: System Programming
namespace msr {
	struct vmcr : BASE_MSR
	{
		static constexpr uint32_t number = 0xC001'0114;

		union {
			struct {
				uint64_t dpd : 1; // If set, disables the external hardware debug port and certain internal debug features.
				uint64_t r_init : 1; // If set, non-intercepted INIT signals are converted into an #SX exception.
				uint64_t dis_a20m : 1; // If set, disables A20 masking.
				uint64_t lock : 1; // When this bit is set, writes to LOCK and SVMDIS are silently ignored. When this bit is clear, VM_CR bits 3 and 4 can be written. Once set, LOCK can only be cleared using the SVM_KEY MSR This bit is not affected by INIT or SKINIT.
				uint64_t svmdis : 1; // writes to EFER treat the SVME bit as MBZ. When this bit is clear, EFER.SVME can be written normally.
				uint64_t reserved5 : 59;
			};
			uint64_t value;
		};

		void load()
		{
			value = { __readmsr(vmcr::number) };
		}

		void store()
		{
			__writemsr(vmcr::number, value);
		}
	};
};
