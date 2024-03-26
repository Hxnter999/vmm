#include <commons.h>
#include <hypercall/hypercall.h>
#include <pages/translations.h>

void hypercall_handler(vcpu_t& vcpu) {
	vcpu.guest_vmcb.save_state.rip = vcpu.guest_vmcb.control.nrip;

	switch (static_cast<hypercall_code>(vcpu.guest_stack_frame.rcx.value)) {
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
		//uint64_t gva = vcpu.guest_stack_frame.rdx.value;
		//print("gva %p\n", gva);
		//uint64_t gay{};
		//uint64_t hva = gva_to_hva(vcpu, gva, gay);
		//print("hva %p || %zX\n", hva, gay);
		//print("*hva %p\n", *(uint64_t*)(hva));
		uint64_t gay = 69;
		uint64_t pa = MmGetPhysicalAddress(&gay).QuadPart;
		print("%d\n", *(uint64_t*) (pa + host_pt_t::host_pa_base));
		break;
	}
	default:
	{
		break;
	}
	}
}