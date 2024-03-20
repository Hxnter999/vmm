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
	Hypervisor::Get()->vcpu_count = KeQueryActiveProcessorCount(nullptr);
	Hypervisor::Get()->vcpus = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, Hypervisor::Get()->vcpu_count * sizeof(vcpu_t), 'sgma'));
	memset(Hypervisor::Get()->vcpus, 0, Hypervisor::Get()->vcpu_count * sizeof(vcpu_t));

	if (!setup_msrpm()) {
		print("Failed to allocate msrpm\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	for (uint32_t i = 0; i < Hypervisor::Get()->vcpu_count; i++)
	{
		Hypervisor::Get()->current_vcpu = &Hypervisor::Get()->vcpus[i];
		print("Virtualizing [%d]...\n", i);

		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);

		if (!virtualize(&Hypervisor::Get()->vcpus[i])) {
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

	for (uint32_t i = 0; i < Hypervisor::Get()->vcpu_count; i++)
	{
		print("Devirtualizing [%d]...\n", i);
		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);

		testcall(hypercall_code::UNLOAD);

		KeRevertToUserAffinityThreadEx(original_affinity);
	}

	if (Hypervisor::Get()->vcpus)
		ExFreePoolWithTag(Hypervisor::Get()->vcpus, 'sgma');
	if (Hypervisor::Get()->shared_msrpm)
		MmFreeContiguousMemory(Hypervisor::Get()->shared_msrpm);
	if(Hypervisor::Get()->npt)
		MmFreeContiguousMemory(Hypervisor::Get()->npt);

	print("---------\n\n");
}