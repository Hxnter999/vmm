#pragma once
#include <vcpu/vmcb.h>

struct alignas(0x1000) vcpu_t {
	union {
		uint8_t host_stack[0x6000]; //0x6000 is more than enough for the host stack
		struct { // for ease of access in vmlaunch
			uint8_t stack_contents[0x6000 - (sizeof(uint64_t) * 6) - sizeof(context_t)];
			context_t ctx;

			uint64_t guest_vmcb_pa;
			uint64_t host_vmcb_pa;

			vcpu_t* self;
			uint64_t guest_rip; // used when devirtualizing along with rsp, these are copies of guest_vmcb.state.rip/rsp

			uint64_t guest_rsp;
			union {
				uint64_t flags;
				struct {
					bool should_exit : 1;
					bool hide_overhead : 1;
					//...
				};
			};
		};
	};

	vmcb_t host; // on vmrun and exits processor saves/restores host state to/from this field, we can also directly manipulate it as long as its considered legal
	vmcb_t guest;
	npt_data_t npts;
	MSR::msrpm_t msrpm;

	// Its cleaner to put these methods in the vmcb struct but i just want them to be in the same place due to some of them being guest specific
	void inject_event(exception_vector e, uint32_t error_code)
	{
		auto& ei = guest.control.event_injection;
		ei.valid = true;
		ei.type = interrupt_type::HARDWARE_EXCEPTION;
		ei.vector = e;
		ei.error_code = error_code;
	}

	inline void flush_tlb(tlb_control_id type) {
		guest.control.tlb_control = type;
	}
	
	/*
	The cpu will implicitly advance RIP in the following cases:
	- PAUSE instruction
	- HLT instruction
	- Write trap instructions (EFER & CR0-15)
	- IDLE_HLT
	- VMGEXIT
	*/
	inline void skip_instruction() { // Handle x86 later if needed
		guest.state.rip = guest.control.nrip;
	}
};
static_assert(sizeof(vcpu_t) == (sizeof(vmcb_t) * 2) + sizeof(npt_data_t) + sizeof(MSR::msrpm_t) + 0x6000, "vcpu_t size mismatch");
