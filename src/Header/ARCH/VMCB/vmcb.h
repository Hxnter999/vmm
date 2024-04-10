#pragma once
#include <vmcb/vmcb_control.h>
#include <vmcb/vmcb_state_save.h>
#include <stack_frame.h>
#include <msrs/msrs.h>
#include <pages/pages.h>

struct vmcb_t {
	// table b-1 (control area)
	vmcb_control control;
	static_assert(sizeof(vmcb_control) == 0x400, "vmcb control is not 0x400");

	// table b-2 (state save area)
	vmcb_state_save save_state;
	static_assert(sizeof(vmcb_state_save) == 0xC00, "vmcb state save is not 0xC00");
};
static_assert(sizeof(vmcb_t) == 0x1000, "vmcb size is not 0x1000");

struct alignas(0x1000) vcpu_t {
	union {
		uint8_t host_stack[0x6000]; //0x6000 default size of KM stack
		struct {
			uint8_t stack_contents[0x6000 - (sizeof(uint64_t) * 10) - sizeof(stack_frame_t)];
			stack_frame_t guest_stack_frame;
			uint64_t guest_vmcb_pa;
			uint64_t host_vmcb_pa;
			vcpu_t* self;
			uint64_t guest_rip; // used when devirtualizing along with rsp, these are copies just to make it easier to access them thru the vmrun loop
			uint64_t guest_rsp;
			uint64_t is_virtualized; // 16byte aligned
			uint64_t should_exit;
			struct {
				uint64_t last_exited;
				ULARGE_INTEGER g_shadow;
			} timing;
			uint64_t aligned;
		};
	};
	vmcb_t host_vmcb; // on vmrun and exits processor saves/restores host state to/from this field, we can also directly manipulate it as long as its considered legal
	vmcb_t guest_vmcb;

	template<typename T>
	bool read_virtual(virtual_address_t va, T& out)
	{
		return read_virtual_w(va, out, sizeof(T));
	}

	template<typename T>
	bool write_virtual(virtual_address_t va, const T& value)
	{
		return write_virtual_w(va, value, sizeof(T));
	}


	template<EXCEPTION_VECTOR exception>
	void inject_event()
	{
		auto& ei = guest_vmcb.control.event_injection;
		ei.valid = 1;
		ei.type = INTERRUPT_TYPE::HARDWARE_EXCEPTION;
		ei.evector = exception;
	}

private:
	//trickery
	bool read_virtual_w(virtual_address_t va, void* out, size_t size);
	bool write_virtual_w(virtual_address_t va, void* value, size_t size);
};
static_assert(sizeof(vcpu_t) == 0x8000, "vcpu size is not 0x8000");
