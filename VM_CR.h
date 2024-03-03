#pragma once
#pragma warning(disable : 4996)

//The VM_CR MSR controls certain global aspects of SVM
//The following bits are defined in the AMD64 Architecture Programmer's Manual Volume 2: System Programming

struct VM_CR
{
	static constexpr __int64 MSR_VM_CR = 0xC0010114;

	union {
		struct {
			unsigned __int64 DPD : 1; // If set, disables the external hardware debug port and certain internal debug features.
			unsigned __int64 R_INIT : 1; // If set, non-intercepted INIT signals are converted into an #SX exception.
			unsigned __int64 DIS_A20M : 1; // If set, disables A20 masking.
			unsigned __int64 LOCK : 1; // When this bit is set, writes to LOCK and SVMDIS are silently ignored. When this bit is clear, VM_CR bits 3 and 4 can be written. Once set, LOCK can only be cleared using the SVM_KEY MSR This bit is not affected by INIT or SKINIT.
			unsigned __int64 SVMDIS : 1; //When this bit is set, writes to EFER treat the SVME bit as MBZ. When this bit is clear, EFER.SVME can be written normally.This bit does not prevent CPUID from reporting that SVM is available.Setting SVMDIS while EFER.SVME is 1 generates a #GP fault, regardless of the current state of VM_CR.LOCK.This bit is not affected by SKINIT.It is cleared by INIT when LOCK is cleared to 0; otherwise, it is not affected.
			unsigned __int64 reserved1 : 59;
		};

		unsigned __int64 bits;
	};
};
