#pragma once
#include <vcpu/vmcb.h>

struct alignas(0x1000) vcpu_t {
	union {
		uint8_t host_stack[0x6000]; //0x6000 is more than enough for the host stack
		struct { // for ease of access in vmrun loop
			uint8_t stack_contents[0x6000 - (sizeof(uint64_t) * 8) - sizeof(stack_frame_t)];
			stack_frame_t guest_stack_frame;

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

	vmcb_t host_vmcb; // on vmrun and exits processor saves/restores host state to/from this field, we can also directly manipulate it as long as its considered legal
	vmcb_t guest_vmcb;
	npt_data_t npts;
	MSR::msrpm_t msrpm;

	void inject_event(exception_vector e, bool valid = true, interrupt_type type = interrupt_type::HARDWARE_EXCEPTION)
	{
		auto& ei = guest_vmcb.control.event_injection;
		ei.valid = valid;
		ei.type = type;
		ei.vector = e;
	}
};
//static_assert(sizeof(vcpu_t) == 0x8000, "vcpu size is not 0x8000");
