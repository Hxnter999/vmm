#pragma once
#include "../../../commons.h"

namespace EXITINFO {
	union exitinfo2
	{
		uint64_t info;
		union {
			struct { // 15.10.2 
				uint64_t rip : 64; // The rIP of the instruction following the IN/OUT is saved in EXITINFO2, so that the VMM can easily resume the guest after I / O emulation
			}in_out;

			// Exception intercepts
			// #TS
			struct { // 15.14.1
				uint64_t error_code : 32; // holds the error code to push in the new task, if applicable; otherwise, this field is undefined.
				uint64_t reserved32 : 4;
				uint64_t caused_by_iret : 1; //  EXITINFO2[36] Set to 1 if the task switch was caused by an IRET; else cleared to 0;
				uint64_t reserved37 : 1;
				uint64_t caused_by_far_jmp : 1; //  EXITINFO2[38] Set to 1 if the task switch was caused by a far JMP; else cleared to 0;
				uint64_t reserved39 : 6;
				uint64_t has_error_code : 1; //  EXITINFO2[44] Set to 1 if the task switch has an error code; else cleared to 0.
				uint64_t reserved46 : 2;
				uint64_t eflags_rf : 1; // EXITINFO2[48] The value of EFLAGS.RF that would be saved in the outgoing TSS if the task switch were not intercepted
				uint64_t reserved49 : 15;
			} task_switch;

			// #NP
			struct {
				uint64_t faulting_address : 64; 
			} page_fault;

			// Interrupt intercepts
			// If the SMI wasn't asserted during an I/O instruction, the extra EXITINFO1 and EXITINFO2 bits are undefined
			struct { // 15.13.3
				uint64_t rip : 64; // rIP of the I/O instruction is saved in EXITINFO2.
			} smi;

		};
	}; // union exitinfo2
} // namespace EXITINFO