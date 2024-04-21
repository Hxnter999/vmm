#include <commons.h>
#include "svm.h"
#include <hypervisor.h>
#include <MSRs/tsc_ratio.h>

//could use a 0x560 byte array for all the cases
inline HANDLER_STATUS cases(vcpu_t& vcpu) 
{
	switch (vcpu.guest_vmcb.control.exit_code) {

	case SVM_EXIT_CODE::VMEXIT_VMMCALL:
		return hypercall_handler(vcpu);

	case SVM_EXIT_CODE::VMEXIT_MSR:
		return msr_handler(vcpu);

	case SVM_EXIT_CODE::VMEXIT_CPUID: //better to not intercept this (explanation in cpuid.cpp)
		return cpuid_handler(vcpu);

	case SVM_EXIT_CODE::VMEXIT_XSETBV: //should NEVER intercept this (dont set in vmcb)
		return xsetbv_handler(vcpu);

	case SVM_EXIT_CODE::VMEXIT_INVALID:
		print("INVALID GUEST STATE, EXITING...\n");
		vcpu.should_exit = true;
		return HANDLER_STATUS::NO_ACTION;

	case SVM_EXIT_CODE::VMEXIT_NPF:
		return npf_handler(vcpu);

	case SVM_EXIT_CODE::VMEXIT_NMI:
		return nmi_handler(vcpu);

	case SVM_EXIT_CODE::VMEXIT_HV: // event injection exception
		print("Failed to inject event\n");
		vcpu.guest_vmcb.control.event_injection.bits = 0; // reset to avoid infinite loop
		return HANDLER_STATUS::NO_ACTION;

	//case SVM_EXIT_CODE::VMEXIT_RDTSC:
	//	break;

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
	case SVM_EXIT_CODE::VMEXIT_CLGI: // why?
		return HANDLER_STATUS::INJECT_UD;

	//case SVM_EXIT_CODE::VMEXIT_CR3_WRITE:

	//case SVM_EXIT_CODE::VMEXIT_CR3_READ:

	case SVM_EXIT_CODE::VMEXIT_SHUTDOWN:
		return shutdown_handler(vcpu);
	}

	print("UNHANDLED EXIT CODE: %-4X || INFO1: %p | INFO2: %p\n", vcpu.guest_vmcb.control.exit_code, vcpu.guest_vmcb.control.exit_info_1.info, vcpu.guest_vmcb.control.exit_info_2.info);
	return HANDLER_STATUS::INCREMENT_RIP;
}

bool vmexit_handler(vcpu_t& vcpu, uint64_t last_exited) {

	__svm_vmload(vcpu.host_vmcb_pa);

	print("ve %llx, last_exited %llu\n", vcpu.guest_vmcb.control.exit_code, last_exited);

	// guest rax overwriten by host after vmexit
	vcpu.guest_stack_frame.rax.value = vcpu.guest_vmcb.save_state.rax_do_not_touch;

	HANDLER_STATUS status{ cases(vcpu) };

	// the cpu handles guest rax for us
	vcpu.guest_vmcb.save_state.rax_do_not_touch = vcpu.guest_stack_frame.rax.value;

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
		case HANDLER_STATUS::INJECT_NMI:
			vcpu.inject_nmi();
			break;
		case HANDLER_STATUS::NO_ACTION:
			break;
	}

	if (vcpu.should_exit) {
		print("Exiting!!\n");
		HV->devirtualize(&vcpu); // devirtualize current vcpu and alert all others
		return false; 
	};

	return true; // true to continue looping
};