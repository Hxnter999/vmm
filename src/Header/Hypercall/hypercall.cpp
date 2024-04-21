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
			uint64_t gva = vcpu.guest_stack_frame.rdx.value;
			print("gva %p\n", gva);
			uint64_t safely_modifiable{};

			uintptr_t hva{};
			if(vcpu.gva_to_hva(gva, safely_modifiable, hva)) // host virtual address, can be accessed directly.
				print("hva %p | size: %d\n", hva, safely_modifiable);
			else 
				print("Failed to translate gva to hva\n");
			break;
		}
		case HYPERCALL_CODE::test2:
		{
			print("test2\n");

			for (uint64_t i = 0; i < plm4e_address_range; i = i + pdpes_address_range * 100) {
				uint64_t val{};
				HV->readPhys(reinterpret_cast<void*>(i), val);

				//uint64_t val2{};
				//size_t size{};
				//MM_COPY_ADDRESS s{};
				//s.PhysicalAddress.QuadPart = z;
				//MmCopyMemory(&val2, s, sizeof(uint64_t), MM_COPY_MEMORY_PHYSICAL, &size);

				print("Read phys [%p -%p, %d, %d]: %llu\n", i, plm4e_address_range - i, i / pdpes_address_range, i / pdes_address_range, val);//, val2);
			}
			print("test2 done\n");
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