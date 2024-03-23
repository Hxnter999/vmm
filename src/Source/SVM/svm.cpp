#include "../../Header/commons.h"
#include "svm.h"
#include "../../Header/Hypervisor.h"

bool vmexit_handler(vcpu_t* const vcpu) {
	__svm_vmload(vcpu->host_vmcb_pa);
	vcpu->guest_vmcb.save_state.rip = vcpu->guest_vmcb.control.nrip;

	// guest rax overwriten by host after vmexit
	vcpu->guest_stack_frame.rax.value = vcpu->guest_vmcb.save_state.rax;
	switch (vcpu->guest_vmcb.control.exit_code) {

	case svm_exit_code::VMEXIT_VMMCALL:
		hypercall_handler(*vcpu);
		break;

	case svm_exit_code::VMEXIT_MSR:
		msr_handler(*vcpu);
		break;

	case svm_exit_code::VMEXIT_INVALID:
		print("INVALID GUEST STATE, EXITING...\n");
		vcpu->should_exit = true;
		break;

	case svm_exit_code::VMEXIT_NPF:
		print("[NPF] Error code: %p\n", vcpu->guest_vmcb.control.exit_info_1.page_fault.error_code);
		print("[NPF] Address: %p | %p\n", vcpu->guest_vmcb.control.exit_info_2.page_fault.faulting_address, vcpu->guest_vmcb.control.exit_info_2.nested_page_fault.faulting_gpa);

	default:
		// event inject a gp/ud
		print("Unhandled VMEXIT: %d\n", vcpu->guest_vmcb.control.exit_code);
		break;
	}
	// the cpu handles guest rax for us
	vcpu->guest_vmcb.save_state.rax = vcpu->guest_stack_frame.rax.value;

	//true to continue
	//false to devirt
	if (vcpu->should_exit) {
		HV->devirtualize(vcpu); // devirtualize current vcpu and alert all others
		return false;
	};

	return true;
}