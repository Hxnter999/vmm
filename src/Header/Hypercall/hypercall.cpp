#include <commons.h>
#include <hypercall/hypercall.h>
//#include "../ARCH/VMCB/vmcb.h"
//#include "../ARCH/MSRs/efer.h"
#include <shared.h>
#include <PAGES/npts.h>

HANDLER_STATUS hypercall_handler(vcpu_t& vcpu) {

	print("Hypercall handler\n");

	switch (static_cast<HYPERCALL_CODE>(vcpu.guest_stack_frame.rcx.value)) {
		case HYPERCALL_CODE::UNLOAD:
		{
			print("UNLOAD\n");
			vcpu.should_exit = true;
			break;
		}
		case HYPERCALL_CODE::PING:
		{
			print("PONG\n");
			break;
		}
		case HYPERCALL_CODE::test: 
		{

			cr3_t cr3 = { __readcr3() };
			cr3_t host_cr3{};
			host_cr3.pml4 = MmGetPhysicalAddress(&HV->shared_host_pt.pml4).QuadPart >> 12;
			print("CR3: %p, %p\n", cr3.value, host_cr3.value);
			if (cr3.pml4 != host_cr3.pml4) {
				return HANDLER_STATUS::INCREMENT_RIP;
			}

			for (uint64_t i = 0; i < 100; i++) {
				uint64_t val{};
				HV->readPhys(reinterpret_cast<void*>(i * sizeof(uint64_t)), val);
				print("Read phys: %p\n", val);
			}
			break;
		}
		default:
		{
			print("Default case in hypercall handler\n");
			break;
		}
	}

	return HANDLER_STATUS::INCREMENT_RIP;
}