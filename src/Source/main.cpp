#include "../Header/commons.h"
#include "SVM/svm.h"
#include "../Header/ARCH/PAGES/npts.h"
#include "../Header/Hypervisor.h"

extern "C" int64_t testcall(hypercall_code code);
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

	for (uint32_t i = 0; i < HV->vcpu_count; i++)
	{
		print("Devirtualizing [%d]...\n", i);
		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);

		testcall(hypercall_code::UNLOAD);

		KeRevertToUserAffinityThreadEx(original_affinity);
	}

	HV->Unload();

	print("---------\n\n");
}