#include <arch/vmexit/handlers.h>

HANDLER_STATUS nmi_handler(vcpu_t& vcpu) {
	UNREFERENCED_PARAMETER(vcpu);

	print("Nmi\n");

	return HANDLER_STATUS::INJECT_NMI;
}