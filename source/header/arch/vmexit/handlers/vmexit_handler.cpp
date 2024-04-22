#include <vmexit/handlers.h>

bool vmexit_handler(vcpu_t& vcpu) {
	__svm_vmload(vcpu.host_vmcb_pa);

	// guest rax overwriten by host after vmexit
	vcpu.guest_stack_frame.rax.value = vcpu.guest_vmcb.state.rax;
	switch (vcpu.guest_vmcb.control.exit_code) {

	case svm_exit_code::VMEXIT_VMMCALL:
		hypercall_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_MSR:
		msr_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_CPUID:
		cpuid_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_INVALID:
		print("INVALID GUEST STATE, EXITING...\n");
		vcpu.should_exit = true;
		break;

	case svm_exit_code::VMEXIT_NPF:
		print("[NPF] %zX\n", vcpu.guest_vmcb.control.exit_info_1.info);
		print("[NPF] %zX\n", vcpu.guest_vmcb.control.exit_info_2.nested_page_fault.faulting_gpa);
		npf_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_HV: // event injection exception
		print("Failed to inject event\n");
		vcpu.guest_vmcb.control.event_injection.bits = 0; // reset to avoid infinite loop incase cpu doesnt clear it
		break;

	case svm_exit_code::VMEXIT_VMLOAD:
		svm_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_VMSAVE:
		svm_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_VMRUN:
		svm_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_CLGI:
		svm_handler(vcpu);
		break;

	default:
		print("UNHANDLED EXIT CODE: %-4X || INFO1: %p | INFO2: %p\n", vcpu.guest_vmcb.control.exit_code, vcpu.guest_vmcb.control.exit_info_1.info, vcpu.guest_vmcb.control.exit_info_2.info);
		break;
	}
	// the cpu handles guest rax for us
	vcpu.guest_vmcb.state.rax = vcpu.guest_stack_frame.rax.value;

	if (vcpu.should_exit) { // TODO: devirtualize by firing IPIs and handling them and remove this dogshit
		HV->devirtualize(&vcpu); // devirtualize current vcpu and alert all others
		return false;
	};

	return true; // continue looping
};