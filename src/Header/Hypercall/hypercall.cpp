#include <commons.h>
#include <hypercall/hypercall.h>
#include <pages/translations.h>
//#include "../ARCH/VMCB/vmcb.h"
//#include "../ARCH/MSRs/efer.h"
#include <shared.h>

HANDLER_STATUS hypercall_handler(vcpu_t& vcpu) {

	print("Hypercall handler\n");

	switch (static_cast<HYPERCALL_CODE>(vcpu.guest_stack_frame.rcx.value)) {
		case HYPERCALL_CODE::UNLOAD:
		{
			print("UNLOAD\n");
			vcpu.should_exit = true;
			break;
		}
		case HYPERCALL_CODE::PING:
		{
			print("PONG\n");
			break;
		}
		case HYPERCALL_CODE::test:
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
			print("Default case inn hypercall handler\n");
			break;
		}
	}

	return HANDLER_STATUS::INCREMENT_RIP;
}