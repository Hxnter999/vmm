#include <commons.h>
#include <hypercall/hypercall.h>
#include <paging/translation.h>

void hypercall_handler(vcpu_t& vcpu) {
	vcpu.guest_vmcb.state.rip = vcpu.guest_vmcb.control.nrip;

	switch (static_cast<hypercall_code>(vcpu.guest_context.rcx.value)) {
	case hypercall_code::UNLOAD:
	{
		vcpu.should_exit = true;
		break;
	}
	case hypercall_code::PING:
	{
		print("PONG\n");
		break;
	}
	case hypercall_code::test:
	{
		uint64_t gva = vcpu.guest_context.rdx.value;
		print("gva %p\n", gva);
		uint64_t offset{};
		
		uintptr_t hva = gva_to_hva(vcpu, gva, offset); // host virtual address, can be accessed directly.
		print("hva %p | size: %d\n", hva, offset);
		break;
	}
	default:
	{
		break;
	}
	}
}