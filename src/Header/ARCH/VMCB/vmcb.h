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
		uint8_t host_stack[0x6'000]; //0x6000 default size of KM stack
		struct {
			uint8_t stack_contents[0x6'000 - (sizeof(uint64_t) * 8) - sizeof(stack_frame_t)];
			stack_frame_t guest_stack_frame;
			uint64_t guest_vmcb_pa;
			uint64_t host_vmcb_pa;
			vcpu_t* self;
			uint64_t guest_rip; // used when devirtualizing along with rsp, these are copies just to make it easier to access them thru the vmrun loop
			uint64_t guest_rsp;
			uint64_t is_virtualized; // 16byte aligned
			uint64_t should_exit;
			uint64_t alignment;
		};
	};
	vmcb_t host_vmcb; // on vmrun and exits processor saves/restores host state to/from this field, we can also directly manipulate it as long as its considered legal
	vmcb_t guest_vmcb;

	template<typename T>
	bool read_guest(virtual_address_t gva, T& out) {
		uint64_t hva{};
		uint64_t modifiable_size{};
		if (!gva_to_hva(gva, modifiable_size, hva))
			return false;

		if (modifiable_size < sizeof(T))
			return false;

		print("readng guest memory at 0x%llx\n", hva);
		out = *reinterpret_cast<T*>(hva);
		return true;
	}

	bool gva_to_gpa(virtual_address_t gva, uint64_t& modifiable_size, _Out_ uint64_t& gpa);


	bool gva_to_hva(virtual_address_t gva, uint64_t& modifiable_size, _Out_ uint64_t& hva) {
		uint64_t gpa{};
		if (!gva_to_gpa(gva, modifiable_size, gpa))
			return false;

		hva = host_pt_t::host_pa_base + gpa;
		return true;
	}

	template<EXCEPTION_VECTOR exception>
	void inject_event()
	{
		auto& ei = guest_vmcb.control.event_injection;
		ei.valid = 1;
		ei.type = INTERRUPT_TYPE::HARDWARE_EXCEPTION;
		ei.evector = exception;
	}

	void inject_nmi() //wmware does not support vnmi
	{
		auto& ei = guest_vmcb.control.event_injection;
		ei.valid = 1;
		ei.type = INTERRUPT_TYPE::NMI;
		ei.vector = 0; // ignored
	}
};