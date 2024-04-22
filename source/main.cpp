#include <commons.h>
#include "hypervisor.h"
		 
#include <vcpu/vcpu.h>
#include <msrs/vm_cr.h>
#include <msrs/pat.h>
#include <msrs/hsave.h>
#include <msrs/efer.h>

void Unload(PDRIVER_OBJECT pDriverObject);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING)
{
	pDriverObject->DriverUnload = Unload;
	Hypervisor::instance = static_cast<Hypervisor*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(Hypervisor), 'hv'));
	
	if (!HV->init())
	{
		print("Hypervisor failed to initialize\n");
		return STATUS_UNSUCCESSFUL;
	}

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