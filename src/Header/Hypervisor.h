#pragma once
#include "ARCH/VMCB/vmcb.h"

class Hypervisor
{
	static Hypervisor* instance;

protected:
	Hypervisor() {}

public:
	vcpu_t* current_vcpu;
	vcpu_t* vcpus;
	uint32_t vcpu_count;
	MSR::msrpm_t* shared_msrpm;
	uint64_t* npt;

	Hypervisor(Hypervisor&) = delete;
	void operator=(const Hypervisor&) = delete;

	static Hypervisor* Get() 
	{
		//naive because we know when its first called
		if (instance == nullptr)
		{
			instance = static_cast<Hypervisor*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(Hypervisor), 'hv'));
		}
		return instance;
	}

	void _delete() //this should only be called once (in Unload)
	{
		if (instance != nullptr)
		{
			ExFreePoolWithTag(instance, 'hv');
			instance = nullptr;
		}
	}
};

inline Hypervisor* Hypervisor::instance = nullptr;
#define HV Hypervisor::Get()
