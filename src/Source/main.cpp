#include "../Header/commons.h"
#include "SVM/svm.h"
#include "../Header/ARCH/PAGES/npts.h"
#include "../Header/Hypervisor.h"

#include "../Header/ARCH/MSRs/vm_cr.h"
#include "..//Header/ARCH/VMCB/vmcb.h"
#include "../Header/ARCH/MSRs/pat.h"
#include "../Header/ARCH/MSRs/hsave_pa.h"
#include "../Header/ARCH/MSRs/efer.h"

void Unload(PDRIVER_OBJECT pDriverObject);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = Unload;

	HV->Get();
	if (!HV->isVaild())
	{
		print("Hypervisor failed to initialize\n");
		return STATUS_UNSUCCESSFUL;
	}

	HV->setup_npts();
	
	HV->msrpm().set(MSR::EFER::MSR_EFER, MSR::access::read);
	HV->msrpm().set(MSR::EFER::MSR_EFER, MSR::access::write);

	HV->msrpm().set(MSR::HSAVE_PA::MSR_VM_HSAVE_PA, MSR::access::read);
	HV->msrpm().set(MSR::HSAVE_PA::MSR_VM_HSAVE_PA, MSR::access::write);

	HV->virtualize();

	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	HV->Unload();

	print("---------\n\n");
}