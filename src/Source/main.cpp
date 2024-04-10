#include <commons.h>
#include "svm/svm.h"
#include <pages/npts.h>
#include <hypervisor.h>
		 
#include <vmcb/vmcb.h>
#include <msrs/vm_cr.h>
#include <msrs/pat.h>
#include <msrs/hsave_pa.h>
#include <msrs/efer.h>

void Unload(PDRIVER_OBJECT pDriverObject);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = Unload;

	if (!HV->is_valid())
	{
		print("Hypervisor failed to initialize\n");
		return STATUS_UNSUCCESSFUL;
	}

	if (!HV->setup_npts())
	{
		print("NPT setup failed\n");
		return STATUS_UNSUCCESSFUL;
	}

	// Setup msrpm, this determines which msrs and their instructions get intercepted
	HV->msrpm().set(MSR::EFER::MSR_EFER, MSR::access::read);
	HV->msrpm().set(MSR::EFER::MSR_EFER, MSR::access::write);

	HV->msrpm().set(MSR::HSAVE_PA::MSR_VM_HSAVE_PA, MSR::access::read);
	HV->msrpm().set(MSR::HSAVE_PA::MSR_VM_HSAVE_PA, MSR::access::write);

	//for (LONGLONG i = 0; i < 10; i++) 
	//{
	//	uint64_t zuh{};
	//	PHYSICAL_ADDRESS pa{ .QuadPart = i * 1000 };
	//	MM_COPY_ADDRESS adr{ .PhysicalAddress = pa };
	//	size_t bread{};
	//	if (!MmCopyMemory(&zuh, adr, sizeof(zuh), MM_COPY_MEMORY_PHYSICAL, &bread) && bread == sizeof(uint64_t))
	//	{
	//		uint64_t guh{};
	//		__try {
	//			HV->read_phys(pa, &guh, sizeof(uint64_t));

	//			print("%llu, %llu\n", zuh, guh);
	//		}
	//		__except (EXCEPTION_EXECUTE_HANDLER)
	//		{
	//			print("Catched %d\n", i);
	//		}
	//	}
	//}

	if (!HV->virtualize())
	{
		print("Virtualization failed\n");
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	HV->unload();

	print("---------\n\n");
}