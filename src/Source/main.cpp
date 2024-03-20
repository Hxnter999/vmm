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

	if (!initialize())
	{
		print("SVM not supported\n");
		return STATUS_UNSUCCESSFUL;
	}
	print("SVM supported\n");

	if (!initnpts()) 
	{
		print("NPT failed\n");
		return STATUS_UNSUCCESSFUL;
	}

	// setup the vcpus
	HV->vcpu_count = KeQueryActiveProcessorCount(nullptr);
	HV->vcpus = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, HV->vcpu_count * sizeof(vcpu_t), 'sgma'));
	memset(HV->vcpus, 0, HV->vcpu_count * sizeof(vcpu_t));

	if (!setup_msrpm()) {
		print("Failed to allocate msrpm\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	for (uint32_t i = 0; i < HV->vcpu_count; i++)
	{
		HV->current_vcpu = &HV->vcpus[i];
		print("Virtualizing [%d]...\n", i);

		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);

		if (!virtualize(&HV->vcpus[i])) {
			print("Failed to virtualize\n");
			return STATUS_UNSUCCESSFUL;
		}

		KeRevertToUserAffinityThreadEx(original_affinity);
	}
	print("Virtualized\n");

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

	if (HV->vcpus)
		ExFreePoolWithTag(HV->vcpus, 'sgma');
	if (HV->shared_msrpm)
		MmFreeContiguousMemory(HV->shared_msrpm);
	if(HV->npt)
		MmFreeContiguousMemory(HV->npt);

	print("---------\n\n");
}