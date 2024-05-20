#include <vmexit/handlers.h>

void svm_handler(vcpu_t& cpu)
{
	// No need to handle cpl > 0 to inject a GP, cpu checks exceptions before the intercept for instructions that require svme so it will throw GP by itself
	
	// #UD(0) cause svme is "disabled" in the guest
	cpu.inject_exception(exception_vector::UD, 0);
}