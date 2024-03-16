#include "../Header/commons.h"
#include "SVM/svm.h"
#include "../Header/ARCH/PAGES/npts.h"

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

	initnpts();

	// setup the vcpus
	global.vcpu_count = KeQueryActiveProcessorCount(nullptr);
	global.vcpus = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, global.vcpu_count * sizeof(vcpu_t), 'sgma'));
	memset(global.vcpus, 0, global.vcpu_count * sizeof(vcpu_t));

	if (!setup_msrpm()) {
		print("Failed to allocate msrpm\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	for (uint32_t i = 0; i < global.vcpu_count; i++)
	{
		global.current_vcpu = &global.vcpus[i];
		print("Virtualizing [%d]...\n", i);

		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);

		if (!virtualize(&global.vcpus[i])) {
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

	for (uint32_t i = 0; i < global.vcpu_count; i++)
	{
		print("Devirtualizing [%d]...\n", i);
		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);

		testcall(hypercall_code::UNLOAD);

		KeRevertToUserAffinityThreadEx(original_affinity);
	}

	if (global.vcpus)
		ExFreePoolWithTag(global.vcpus, 'sgma');
	if (global.shared_msrpm)
		MmFreeContiguousMemory(global.shared_msrpm);
	if(global.npt)
		MmFreeContiguousMemory(global.npt);

	print("---------\n\n");
}