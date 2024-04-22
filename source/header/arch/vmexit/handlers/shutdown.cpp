#include <arch/vmexit/handlers.h>

HANDLER_STATUS shutdown_handler(vcpu_t& vcpu) {
	//state saved in the VMCB is undefined

	print("Shutting down... );\n");

	vcpu.guest_vmcb.control.shutdown = 0;

	return HANDLER_STATUS::NO_ACTION; //should shutdown
}