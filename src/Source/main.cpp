#include "../Header/commons.h"
#include "SVM/svm.h"
#include "../Header/ARCH/PAGES/npts.h"
#include "../Header/Hypervisor.h"

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

	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	HV->Unload();

	print("---------\n\n");
}