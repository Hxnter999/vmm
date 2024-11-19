#include <vmexit/handlers.h>

void svm_handler(vcpu_t& cpu)
{
	// #GP is handled by the cpu when cpl>0

	if (!cpu.shadow.efer.svme) { // incase the guest has written to efer.svme before executing the instruction
		cpu.inject_exception(exception_vector::UD);
		return;
	}

	// TODO: implement simple functionality if shadow svme is enabled (TOO MUCH WORK, cant be bothered)
	cpu.skip_instruction();
}