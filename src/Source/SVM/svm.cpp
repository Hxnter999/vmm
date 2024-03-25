#include <commons.h>
#include "svm.h"
#include <hypervisor.h>

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

	case svm_exit_code::VMEXIT_CPUID:
		cpuid_handler(*vcpu);
		break;

	case svm_exit_code::VMEXIT_INVALID:
		print("INVALID GUEST STATE, EXITING...\n");
		vcpu->should_exit = true;
		break;

	case svm_exit_code::VMEXIT_NPF:
		print("[NPF] %zX\n", vcpu->guest_vmcb.control.exit_info_1.info);
		print("[NPF] %zX\n", vcpu->guest_vmcb.control.exit_info_2.nested_page_fault.faulting_gpa);
		npf_handler(*vcpu);
		break;

	case svm_exit_code::VMEXIT_HV: // event injection exception
		print("Failed to inject event\n");
		vcpu->guest_vmcb.control.event_injection.bits = 0; // reset to avoid infinite loop
		break;

	case svm_exit_code::VMEXIT_VMLOAD:
		HV->inject_event<exception_vector::UD>(*vcpu);
		break;

	case svm_exit_code::VMEXIT_VMSAVE:
		HV->inject_event<exception_vector::UD>(*vcpu);
		break;

	case svm_exit_code::VMEXIT_VMRUN:
		HV->inject_event<exception_vector::UD>(*vcpu);
		break;

	case svm_exit_code::VMEXIT_CLGI:
		HV->inject_event<exception_vector::UD>(*vcpu);
		break;

	default:
		print("UNHANDLED EXIT CODE: %-4X || INFO1: %p | INFO2: %p\n", vcpu->guest_vmcb.control.exit_code, vcpu->guest_vmcb.control.exit_info_1.info, vcpu->guest_vmcb.control.exit_info_2.info);
		break;
	}
	// the cpu handles guest rax for us
	vcpu->guest_vmcb.save_state.rax = vcpu->guest_stack_frame.rax.value;

	if (vcpu->should_exit) {
		vcpu->should_exit = false; // check devirtualize routine, if statement.

		HV->devirtualize(vcpu); // devirtualize current vcpu and alert all others
		return false; 
	};

	return true; // true to continue looping
};