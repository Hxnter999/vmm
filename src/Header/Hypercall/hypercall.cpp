#include <commons.h>
#include <hypercall/hypercall.h>
#include <pages/translations.h>
//#include "../ARCH/VMCB/vmcb.h"
//#include "../ARCH/MSRs/efer.h"
#include <shared.h>

HANDLER_STATUS hypercall_handler(vcpu_t& vcpu) {

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
			uint64_t gva = vcpu.guest_stack_frame.rdx.value;
			print("gva %p\n", gva);
			uint64_t safely_modifiable{};
		
			uintptr_t hva = gva_to_hva(vcpu, gva, safely_modifiable); // host virtual address, can be accessed directly.
			print("hva %p | size: %d\n", hva, safely_modifiable);
			break;
		}
		default:
		{
			break;
		}
	}

	return HANDLER_STATUS::INCREMENT_RIP;
}