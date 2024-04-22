#include <vmexit/handlers.h>

void svm_handler(vcpu_t& vcpu)
{
	if (vcpu.guest_vmcb.state.cpl == 0) {
		vcpu.inject_event(exception_vector::UD);
	}
	else {
		vcpu.inject_event(exception_vector::GP);
	}
}