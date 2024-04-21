#include <commons.h>
#include "svm/svm.h"
#include <pages/npts.h>
#include <hypervisor.h>
		 
#include <vmcb/vmcb.h>
#include <msrs/vm_cr.h>
#include <msrs/pat.h>
#include <msrs/hsave_pa.h>
#include <msrs/efer.h>

#include <shared.h>

void Unload(PDRIVER_OBJECT pDriverObject);

uint64_t val = 231;
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{

	constexpr double version = 0.1;
	print("Hypervisor v%.1f\n", version);

	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = Unload;

	if (!Hypervisor::init_check()) 
	{
		print("SVM not supported\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	print("SVM supported\n");

	if (!Hypervisor::init()) 
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

	print("pdes_address_range: %p\n", pdes_address_range);
	print("pdpes_address_range: %p\n", pdpes_address_range);
	print("plm4e_address_range: %p\n", plm4e_address_range);

	if (!HV->virtualize())
	{
		print("Virtualization failed\n");
		return STATUS_UNSUCCESSFUL;
	}

	//auto pa = MmGetPhysicalAddress(&val);
	//print("Physical address of val: %llx\n", pa.QuadPart);

	auto waz = reinterpret_cast<uint64_t(*)(HYPERCALL_CODE, void*)>(testcall);
	waz(HYPERCALL_CODE::test, &val);
	print("Val: %llx\n", val);
	testcall(HYPERCALL_CODE::test2);

	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	HV->unload();

	print("---------\n\n");
}