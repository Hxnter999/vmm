#include <vmm.h>

void Unload(PDRIVER_OBJECT);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING)
{
	pDriverObject->DriverUnload = Unload;

	if (!check_svm_support())
	{
		print("SVM is not supported\n");
		return STATUS_UNSUCCESSFUL;
	}
	print("SVM is supported\n");

	if (!virtualize())
	{
		print("Failed to virtualize\n");
		return STATUS_UNSUCCESSFUL;
	}
	
	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT)
{
	devirtualize();
	print("---------\n\n");
}