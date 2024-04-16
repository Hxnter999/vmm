#include <arch/vmexit/handlers.h>
#include <ARCH/MSRs/hsave_pa.h>
#include <ARCH/MSRs/efer.h>
#include <ARCH/MSRs/tsc.h>
#include <ARCH\MSRs\tsc_ratio.h>

HANDLER_STATUS msr_handler(vcpu_t& vcpu) {

	uint32_t msr = vcpu.guest_stack_frame.rcx.low;

	//Microsoft's defined Hypervisor CPUID range (nested virtualization)
	if (msr >= 0x4000'0000 && msr < 0x5000'0000)
		return HANDLER_STATUS::INJECT_GP;
	
	// MSR return value is split between 2 registers, we have to handle them both before passing it back into the guest.
	register_t result{};
	bool read = vcpu.guest_vmcb.control.exit_info_1.msr.is_read();

	print("MSR: %p, R: %d\n", msr, read);

	result.low = vcpu.guest_stack_frame.rax.low;
	result.high = vcpu.guest_stack_frame.rdx.low;

	switch (msr) 
	{
		case MSR::EFER::MSR_EFER: 
		{
			MSR::EFER efer{};
			if (read) {
				efer.load(); efer.svme = 0; /*efer.sce = 1;*/ result.value = efer.bits;
			}
			else {
				efer.bits = result.value;
				efer.svme = 1;
				//efer.sce = 0 (this is needed for syscall hooking, could get a little messy)
				vcpu.guest_vmcb.save_state.efer.bits = efer.bits;
			}
			break;
		}

		//case MSR::TSC::MSR_TSC:
		//{
		//	if (read) {
		//		result.value = vcpu.timing.shadow_tsc.QuadPart;
		//	}
		//	else {
		//		//probs need to emulate the shift msr too "IA32_TSC_ADJUST"
		//		vcpu.timing.shadow_tsc.QuadPart = result.value;
		//	}
		//}

		case MSR::HSAVE_PA::MSR_VM_HSAVE_PA: 
		{
			if (read) {
				result.value = 0;
			}
			else {
				print("HSAVE_PA write???\n");
			}
			break;
		}

		//better just to disable this feature in the cpuid
		case MSR::TSC_RATIO::MSR_TSC_RATIO: 
		{
			static MSR::TSC_RATIO TSC_RATIO_SHADOW{ .bits = 0 }; //the default may be 1 (thats what is said in the manual but testing in vmware shows 0 (maybe its their handler)
			if (read) { 
				result = { TSC_RATIO_SHADOW.bits };
			}
			else {
				TSC_RATIO_SHADOW.bits = result.value;
			}
		}

		default:
		{
			if (read) {
				result.value = __readmsr(msr);
			}
			else {
				__writemsr(msr, result.value);
			}
		}
	}

	if (read) {
		vcpu.guest_stack_frame.rax.value = result.low;
		vcpu.guest_stack_frame.rdx.value = result.high;
	}

	return HANDLER_STATUS::INCREMENT_RIP;
}