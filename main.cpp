#include "commons.h"
#include "SVM_STATUS.h"
#include "efer.h"
#include "fn_identifiers.h"
#include "fn_processor_capacity.h"
#include "fn_svm_features.h"
#include "fn_vendor.h"
#include "vm_cr.h"

SVM_STATUS inittest() 
{
	CPUID::fn_vendor vendor_check{};
	loadFn(vendor_check);

	if (!vendor_check.is_amd_vendor()) 
	{
		print("Vendor check failed... get off intel nerd\n");
		return SVM_STATUS::SVM_WRONG_VENDOR;
	}

	print("Vendor check passed\n");

	CPUID::fn_identifiers id{};
	loadFn(id);

	if (!id.feature_identifiers.svm) 
	{
		print("SVM not supported, womp womp\n");
		return SVM_STATUS::SVM_IS_NOT_SUPPORTED_BY_CPU;
	}

	CPUID::fn_svm_features svm_rev{};
	loadFn(svm_rev);

	if (!svm_rev.svm_feature_identification.nested_paging)
	{
		print("Nested paging not supported, womp womp\n");
		return SVM_STATUS::SVM_NESTED_PAGING_NOT_SUPPORTED;
	}

	MSR::VM_CR vm_cr{};
	loadMSR(vm_cr);

	if (!vm_cr.svmdis)
	{
		print("SVM not enabled but can be (;\n");
		return SVM_STATUS::SVM_IS_CAPABLE_OF_BEING_ENABLE; // Yippe!
	}

	if (!svm_rev.svm_feature_identification.svm_lock) 
	{
		print("SVM lock bit not set, disabled by BIOS...\n");
		return SVM_STATUS::SVM_DISABLED_AT_BIOS_NOT_UNLOCKABLE;
	}
	
	print("SVM lock bit set, disabled\n");
	return SVM_STATUS::SVM_DISABLED_WITH_KEY;
}

void Unload(PDRIVER_OBJECT pDriverObject);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) 
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = Unload;

	if (!inittest()) 
	{
		print("SVM not supported\n");
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	print("Driver Unloaded\n");
}