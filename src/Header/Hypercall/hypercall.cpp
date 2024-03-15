#include "../commons.h"
#include "hypercall.h"
#include "../../Source/SVM/svm.h"

void hypercall_handler(vcpu_t* vcpu) {
	__debugbreak();
	print("hypercall: %d\n", vcpu->guest_stack_frame.rcx);
	switch (static_cast<hypercall_code>(vcpu->guest_stack_frame.rcx)) {
	case hypercall_code::UNLOAD:
	{
		devirtualize();
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