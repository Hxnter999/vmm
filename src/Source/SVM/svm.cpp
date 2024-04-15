#include <commons.h>
#include "svm.h"
#include <hypervisor.h>
#include <MSRs/tsc_ratio.h>

bool vmexit_handler(vcpu_t& vcpu, uint64_t last_exited) {
	UNREFERENCED_PARAMETER(last_exited);

	__svm_vmload(vcpu.host_vmcb_pa);

	print("ve %llx\n", vcpu.guest_vmcb.control.exit_code);

	// guest rax overwriten by host after vmexit
	vcpu.guest_stack_frame.rax.value = vcpu.guest_vmcb.save_state.rax;

	HANDLER_STATUS status{ HANDLER_STATUS::INCREMENT_RIP };
	switch (vcpu.guest_vmcb.control.exit_code) {

	case SVM_EXIT_CODE::VMEXIT_VMMCALL:
		status = hypercall_handler(vcpu);
		break;

	case SVM_EXIT_CODE::VMEXIT_MSR:
		status = msr_handler(vcpu);
		break;

	case SVM_EXIT_CODE::VMEXIT_CPUID:
		status = cpuid_handler(vcpu);
		break;

	case SVM_EXIT_CODE::VMEXIT_INVALID:
		print("INVALID GUEST STATE, EXITING...\n");
		vcpu.should_exit = true;
		break;

	case SVM_EXIT_CODE::VMEXIT_NPF:
		status = npf_handler(vcpu);
		break;

	case SVM_EXIT_CODE::VMEXIT_HV: // event injection exception
		print("Failed to inject event\n");
		vcpu.guest_vmcb.control.event_injection.bits = 0; // reset to avoid infinite loop
		break;

	case SVM_EXIT_CODE::VMEXIT_RDTSC:
		break;

	//commenting this out so u get the intent (not done)
	//case svm_exit_code::VMEXIT_IDTR_READ:
	//case svm_exit_code::VMEXIT_GDTR_READ:
	//	dtr_load_handler(*vcpu);
	//	break;

	//case svm_exit_code::VMEXIT_IDTR_WRITE:
	//case svm_exit_code::VMEXIT_GDTR_WRITE:
	//	dtr_save_handler(*vcpu);
	//	break;

	case SVM_EXIT_CODE::VMEXIT_VMRUN:
	case SVM_EXIT_CODE::VMEXIT_VMLOAD:
	case SVM_EXIT_CODE::VMEXIT_VMSAVE:
	case SVM_EXIT_CODE::VMEXIT_CLGI:
		status = HANDLER_STATUS::INJECT_UD;
		break;

	case SVM_EXIT_CODE::VMEXIT_XSETBV:
		status = xsetbv_handler(vcpu);
		break;

	default:
		print("UNHANDLED EXIT CODE: %-4X || INFO1: %p | INFO2: %p\n", vcpu.guest_vmcb.control.exit_code, vcpu.guest_vmcb.control.exit_info_1.info, vcpu.guest_vmcb.control.exit_info_2.info);
		break;
	}
	// the cpu handles guest rax for us
	vcpu.guest_vmcb.save_state.rax = vcpu.guest_stack_frame.rax.value;

	switch (status) 
	{
		case HANDLER_STATUS::INCREMENT_RIP:
			vcpu.guest_vmcb.save_state.rip = vcpu.guest_vmcb.control.nrip;
			break;
		case HANDLER_STATUS::INJECT_GP:
			vcpu.inject_event<EXCEPTION_VECTOR::GP>();
			break;
		case HANDLER_STATUS::INJECT_UD:
			vcpu.inject_event<EXCEPTION_VECTOR::UD>();
			break;
		case HANDLER_STATUS::INJECT_PF:
			vcpu.inject_event<EXCEPTION_VECTOR::PF>();
			break;
	}

	if (vcpu.should_exit) {
		print("Exiting!!\n");
		HV->devirtualize(&vcpu); // devirtualize current vcpu and alert all others
		return false; 
	};

	return true; // true to continue looping
};