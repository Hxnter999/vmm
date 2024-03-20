#pragma once
#include "ARCH/VMCB/vmcb.h"
#include "ARCH/PAGES/npts.h"

class Hypervisor
{
	static Hypervisor* instance;
	
protected:
	void init()
	{
		//set all to members to 0
		current_vcpu = nullptr;
		vcpus = nullptr;
		vcpu_count = 0;
		shared_msrpm = nullptr;
		npt = nullptr;
		vaild = false;

		print("Initializing Hypervisor...\n");

		if (!init_check()) {
			print("SVM not supported\n");
			return;
		}
		print("SVM supported\n");

		//init npts //should just be a inside function (breaks encasulation)
		if (!initnpts(&npt)) {
			print("NPT failed\n");
			return;
		}

		vcpu_count = KeQueryActiveProcessorCount(nullptr);
		vcpus = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, vcpu_count * sizeof(vcpu_t), 'sgma'));
		memset(vcpus, 0, vcpu_count * sizeof(vcpu_t));

		//same comment as above
		if (!setup_msrpm()) {
			print("Failed to allocate msrpm\n");
			return;
		}

		for (uint32_t i = 0; i < vcpu_count; i++)
		{
			current_vcpu = &vcpus[i];
			print("Virtualizing [%d]...\n", i);

			auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);

			if (!virtualize(&vcpus[i])) {
				print("Failed to virtualize\n");
				return;
			}

			KeRevertToUserAffinityThreadEx(original_affinity);
		}

		print("Virtualized\n");
		vaild = true;
	}

	static SVM_STATUS init_check();

public:
	vcpu_t* current_vcpu;
	vcpu_t* vcpus;
	uint32_t vcpu_count;
	MSR::msrpm_t* shared_msrpm;
	uint64_t* npt;

	bool vaild;

	Hypervisor(Hypervisor&) = delete;
	void operator=(const Hypervisor&) = delete;

	static Hypervisor* Get() 
	{
		//naive because we know when its first called
		if (instance == nullptr)
		{
			instance = static_cast<Hypervisor*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(Hypervisor), 'hv'));
			instance->init();
		}
		return instance;
	}

	bool isVaild() 
	{
		return vaild;
	}

	void Unload() //this should only be called once (in Unload)
	{
		if (vcpus)
			ExFreePoolWithTag(vcpus, 'sgma');
		if (shared_msrpm)
			MmFreeContiguousMemory(shared_msrpm);
		if (npt)
			MmFreeContiguousMemory(npt);

		if (instance != nullptr)
		{
			ExFreePoolWithTag(instance, 'hv');
			instance = nullptr;
		}
	}
};

inline SVM_STATUS Hypervisor::init_check()
{
	CPUID::fn_vendor vendor_check{};
	vendor_check.load();

	if (!vendor_check.is_amd_vendor())
	{
		print("Vendor check failed... get off intel nerd\n");
		return SVM_STATUS::SVM_WRONG_VENDOR;
	}

	print("Vendor check passed\n");

	CPUID::fn_identifiers id{};
	id.load();

	if (!id.feature_identifiers.svm)
	{
		print("SVM not supported\n");
		return SVM_STATUS::SVM_IS_NOT_SUPPORTED_BY_CPU;
	}

	CPUID::fn_svm_features svm_rev{};
	svm_rev.load();

	if (!svm_rev.svm_feature_identification.nested_paging)
	{
		print("Nested paging not supported\n");
		return SVM_STATUS::SVM_NESTED_PAGING_NOT_SUPPORTED;
	}

	if (!svm_rev.svm_feature_identification.n_rip) // necessary otherwise we have to emulate it which is a pain
	{
		print("Uh oh! Next RIP not supported\n");
		return SVM_STATUS::SVM_NEXT_RIP_NOT_SUPPORTED;
	}

	MSR::VM_CR vm_cr{};
	vm_cr.load();

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

inline Hypervisor* Hypervisor::instance = nullptr;
#define HV Hypervisor::Get()
