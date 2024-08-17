#include <vmexit/handlers.h>

void svm_handler(vcpu_t& cpu)
{
	// #GP is handled by the cpu when cpl>0

	if (!cpu.shadow.efer.svme) { // incase the guest has written to efer.svme before executing the instruction
		cpu.inject_exception(exception_vector::UD, 0);
		return;
	}

	// TODO: implement functionality if shadow svme is enabled (TOO MUCH WORK :sob:)
	cpu.skip_instruction();
}