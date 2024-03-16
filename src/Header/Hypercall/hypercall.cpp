#include "../commons.h"
#include "hypercall.h"
#include "../../Source/SVM/svm.h"

void hypercall_handler(vcpu_t* vcpu) {
	switch (static_cast<hypercall_code>(vcpu->guest_stack_frame.rcx.value)) {
	case hypercall_code::UNLOAD:
	{
		vcpu->should_exit = true;
		break;
	}
	case hypercall_code::PING:
	{
		print("PONG\n");
		break;
	}
	default:
	{
		break;
	}
	}
}