#include "../../Header/commons.h"
#include "svm.h"
#include "../../Header/Hypervisor.h"

bool vmexit_handler(vcpu_t* vcpu) {
	vcpu->guest_vmcb.save_state.rip = vcpu->guest_vmcb.control.nrip;

	// guest rax overwriten by host after vmexit
	vcpu->guest_stack_frame.rax.value = vcpu->guest_vmcb.save_state.rax;
	switch (vcpu->guest_vmcb.control.exit_code) {

	case svm_exit_code::VMEXIT_VMMCALL:
		hypercall_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_MSR:
		msr_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_INVALID:
		print("INVALID GUEST STATE, EXITING...\n");
		vcpu->should_exit = true;
		break;

	case svm_exit_code::VMEXIT_NPF:
		print("[NPF] Error code: %X\n", vcpu->guest_vmcb.control.exit_info_1.page_fault.error_code);
		print("[NPF] Address: %p\n", vcpu->guest_vmcb.control.exit_info_2.page_fault.faulting_address);

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

bool setup_msrpm() {
	using namespace MSR;

	HV->shared_msrpm = reinterpret_cast<msrpm_t*>(MmAllocateContiguousMemory(sizeof(msrpm_t), { .QuadPart = -1 }));
	if (HV->shared_msrpm == nullptr)
		return false;

	//memset(global.shared_msrpm, 0, sizeof(msrpm_t));
	//shouldnt be needed cause our class sets all bits to 0 by default

	// msrpm->set(msr, bit, value = true)
	// bit is either 0 (read) or 1 (write)

	HV->shared_msrpm->set(EFER::MSR_EFER, access::read);
	HV->shared_msrpm->set(EFER::MSR_EFER, access::write);

	HV->shared_msrpm->set(HSAVE_PA::MSR_VM_HSAVE_PA, access::read);
	HV->shared_msrpm->set(HSAVE_PA::MSR_VM_HSAVE_PA, access::write);
	return true;
}