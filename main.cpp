#include "commons.h"
#include "SVM_STATUS.h"
#include "EFER.h"
#include "fn_identifiers.h"
#include "fn_processor_capacity.h"
#include "fn_svm_features.h"
#include "fn_vendor.h"
#include "fn_processorinfo_featurebits.h"
#include "VM_CR.h"

SVM_STATUS inittest() 
{
	cpuid::fn_vendor vendor_check{};
	loadFn(vendor_check);

	if (vendor_check.VMXEnabled()) 
	{
		print("Vendor check failed\n");
		return SVM_STATUS::SVM_WRONG_VENDOR;
	}

	print("Vendor check passed\n");

	cpuid::fn_identifiers id{};
	loadFn(id);

	if (id.feature_identifiers.svm) 
	{
		print("SVM not supported\n");
		return SVM_STATUS::SVM_IS_NOT_SUPPORTED_BY_CPU;
	}

	cpuid::fn_svm_features svm_rev{};
	loadFn(svm_rev);

	if (!svm_rev.svm_feature_identification.nested_paging)
	{
		print("Nested paging not supported\n");
		return SVM_STATUS::SVM_NESTED_PAGING_NOT_SUPPORTED;
	}

	MSR::VM_CR vm_cr{};
	loadMSR(vm_cr);

	if (!vm_cr.svmdis)
	{
		print("SVM not enabled\n");
		return SVM_STATUS::SVM_IS_CAPABLE_OF_BEING_ENABLE;
	}

	if (!svm_rev.svm_feature_identification.svm_lock) 
	{
		print("SVM lock bit not set\n");
		return SVM_STATUS::SVM_DISABLED_AT_BIOS_NOT_UNLOCKABLE;
	}
	else 
	{
		print("SVM lock bit set\n");
		return SVM_STATUS::SVM_DISABLED_WITH_KEY;
	}
}

void Unload(PDRIVER_OBJECT pDriverObject);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) 
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = Unload;


	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	print("Driver Unloaded\n");
}