#include <arch/vmexit/handlers.h>

void npf_handler(vcpu_t& vcpu) 
{
	print("[NPF] %zX\n", vcpu.guest_vmcb.control.exit_info_1.info);
	print("[NPF] %zX\n", vcpu.guest_vmcb.control.exit_info_2.nested_page_fault.faulting_gpa);
}