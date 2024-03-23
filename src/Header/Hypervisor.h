#pragma once
#include "commons.h"
#include "SVM_STATUS.h"
#include "ARCH/VMCB/vmcb.h"

class Hypervisor
{
	static Hypervisor* instance;
	
	inline bool execute_on_all_cpus(bool(*func)(uint32_t)) //this is cancer!
	{
		for (uint32_t i = 0; i < vcpus.vcpu_count; i++)
		{
			auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);
			bool result = func(i);
			KeRevertToUserAffinityThreadEx(original_affinity);
			if (!result) return false;
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
	MSR::msrpm_t* shared_msrpm;

	struct vcpus_t {
		vcpus_t() : vcpu_count(0), buffer(nullptr) {}
		vcpus_t(uint32_t size) : vcpu_count(size) { buffer = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, size * sizeof(vcpu_t), 'sgma')); }

		vcpu_t* buffer;
		uint32_t vcpu_count;

		vcpu_t* get(uint32_t i) { return buffer + i; }
		vcpu_t* begin() { return buffer; }
		vcpu_t* end() { return buffer + vcpu_count; }
	} vcpus;

public:
	Hypervisor(Hypervisor&) = delete;
	void operator=(const Hypervisor&) = delete;

	MSR::msrpm_t& msrpm() { return *shared_msrpm; }

	static Hypervisor* get();

	bool isvalid() { return vaild; }

	bool virtualize();

	bool setup_npts();

	void devirtualize(vcpu_t* vcpu);

	void unload(); //this should only be called once (in Unload)
};

#define HV Hypervisor::get()