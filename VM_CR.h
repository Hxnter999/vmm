#pragma once
#include "MSRs.h"

//The VM_CR MSR controls certain global aspects of SVM
//The following bits are defined in the AMD64 Architecture Programmer's Manual Volume 2: System Programming

struct VM_CR
{
	static constexpr __int64 MSR_VM_CR = 0xC0010114;

	union {
		struct {
			uint64_t dpd : 1; // If set, disables the external hardware debug port and certain internal debug features.
			uint64_t r_init : 1; // If set, non-intercepted INIT signals are converted into an #SX exception.
			uint64_t dis_a20m : 1; // If set, disables A20 masking.
			uint64_t lock : 1; // When this bit is set, writes to LOCK and SVMDIS are silently ignored. When this bit is clear, VM_CR bits 3 and 4 can be written. Once set, LOCK can only be cleared using the SVM_KEY MSR This bit is not affected by INIT or SKINIT.
			uint64_t svmdis : 1; //When this bit is set, writes to EFER treat the SVME bit as MBZ. When this bit is clear, EFER.SVME can be written normally.This bit does not prevent CPUID from reporting that SVM is available.Setting SVMDIS while EFER.SVME is 1 generates a #GP fault, regardless of the current state of VM_CR.LOCK.This bit is not affected by SKINIT.It is cleared by INIT when LOCK is cleared to 0; otherwise, it is not affected.
			uint64_t reserved5 : 59;
		};

		uint64_t bits;
	};
};

template<>
void loadMSR(VM_CR& fn)
{
	fn.bits = __readmsr(VM_CR::MSR_VM_CR);
}
