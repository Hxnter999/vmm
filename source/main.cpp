#include <vmm.h>

extern "C" 
NTSTATUS DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING)
{
	if (driver_object)
		driver_object->DriverUnload = [](PDRIVER_OBJECT) {
			devirtualize(); 
			print("-------------------\n");
		};

	if (!check_svm_support())
		return STATUS_DEVICE_FEATURE_NOT_SUPPORTED;

	if (!virtualize())
		return STATUS_UNSUCCESSFUL;

	return STATUS_SUCCESS;
}