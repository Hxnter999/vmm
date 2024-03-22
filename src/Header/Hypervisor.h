#pragma once
#include "commons.h"
#include "SVM_STATUS.h"
#include "ARCH/VMCB/vmcb.h"

class Hypervisor
{
	static Hypervisor* instance;
	
	inline bool runOnAllVCpus(bool(*func)(uint32_t)) //this is cancer!
	{
		for (uint32_t i = 0; i < vcpus.vcpu_count; i++)
		{
			auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);
			if(!func(i)) return false;
			KeRevertToUserAffinityThreadEx(original_affinity);
		}
		return true;
	}

	Hypervisor() = default;

	void init();

	bool virtualize(uint32_t index);

	void setup_vmcb(vcpu_t* vcpu, CONTEXT* ctx);

	static SVM_STATUS init_check();

	bool vaild;
	uint64_t* npt;
	vcpu_t* current_vcpu;

	struct vcpus_t {
		vcpus_t() : vcpu_count(0), buffer(nullptr) {}
		vcpus_t(uint32_t size) : vcpu_count(size) { buffer = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, size * sizeof(vcpu_t), 'sgma')); }

		vcpu_t* buffer;
		uint32_t vcpu_count;

		vcpu_t* get(uint32_t i) { return buffer + i; }
		vcpu_t* begin() { return buffer; }
		vcpu_t* end() { return buffer + vcpu_count; }
	} vcpus;
	MSR::msrpm_t* shared_msrpm;
public:
	MSR::msrpm_t& msrpm() { return *shared_msrpm; }

	Hypervisor(Hypervisor&) = delete;
	void operator=(const Hypervisor&) = delete;

	static Hypervisor* Get();

	bool isVaild() { return vaild; }

	bool virtualize();

	bool setup_npts();

	void devirtualize(vcpu_t* vcpu);

	void Unload(); //this should only be called once (in Unload)
};

#define HV Hypervisor::Get()
