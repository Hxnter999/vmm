#include "../Header/commons.h"
#include "SVM/svm.h"

extern "C" void vmenter(uint64_t* guest_vmcb_pa);
extern "C" void testcall();
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

	// setup the vcpus
	global.vcpu_count = KeQueryActiveProcessorCount(nullptr);
	global.vcpus = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, global.vcpu_count * sizeof(vcpu_t), 'sgma'));
	memset(global.vcpus, 0, global.vcpu_count * sizeof(vcpu_t));

	if (!setup_msrpm()) {
		print("failed to allocate msrpm\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	for (uint32_t i = 0; i < global.vcpu_count; i++)
	{
		global.current_vcpu = &global.vcpus[i];
		CONTEXT* ctx = reinterpret_cast<CONTEXT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(CONTEXT), 'sgma')); memset(ctx, 0, sizeof(CONTEXT));
		RtlCaptureContext(ctx);

		if (global.current_vcpu->is_virtualized) {
			//__debugbreak();
			continue;
		}

		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);
		print("attempting to set up vcpu %d\n", KeGetCurrentProcessorIndex());

		//__debugbreak();
		setup_vmcb(&global.vcpus[i], ctx);
		vmenter(&global.vcpus[i].guest_vmcb_pa);

		// this wont be executed anyway
		KeRevertToUserAffinityThreadEx(original_affinity);
	}
	print("Virtualized\n");

	//__debugbreak();

	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	if (global.vcpus)
		ExFreePoolWithTag(global.vcpus, 'sgma');
	if (global.shared_msrpm)
		MmFreeContiguousMemory(global.shared_msrpm);
	if(global.npt)
		MmFreeContiguousMemory(global.npt);

	print("---------\n\n");
}