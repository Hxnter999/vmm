#pragma once
#include <commons.h>
#include <svm_status.h>
#include <vmcb/vmcb.h>

class Hypervisor
{
	static Hypervisor* instance;
	
	static inline bool execute_on_all_cpus(bool(*func)(uint32_t), uint32_t cpu_count) //this is cancer!
	{
		for (uint32_t i = 0; i < cpu_count; i++)
		{
			auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);
			bool result = func(i);
			KeRevertToUserAffinityThreadEx(original_affinity);
			if (!result) return false;
		}
		return true;
	}

	Hypervisor() = default;	

	bool virtualize(uint32_t index);

	void setup_vmcb(vcpu_t& vcpu, CONTEXT const* const ctx);

	void setup_host_pt();

	uint64_t* npt;
	MSR::msrpm_t* shared_msrpm;
	host_pt_t shared_host_pt;

	struct vcpus_t {
		vcpus_t() : vcpu_count(0), buffer(nullptr) {}
		vcpus_t(uint32_t size) : vcpu_count(size) { buffer = static_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, size * sizeof(vcpu_t), 'sgmA')); }

		vcpu_t* buffer;
		uint32_t vcpu_count;

		vcpu_t* get(uint32_t i) { return buffer + i; }
		vcpu_t* begin() { return buffer; }
		vcpu_t* end() { return buffer + vcpu_count; }

	} vcpus;

public:
	Hypervisor(Hypervisor&) = delete;
	void operator=(const Hypervisor&) = delete;

	static svm_status init_check();

	MSR::msrpm_t& msrpm() { return *shared_msrpm; }

	static bool init();

	static Hypervisor* get() 
	{
		return instance;
	}

	bool virtualize();

	bool setup_npts();

	void devirtualize(vcpu_t* const vcpu);

	void unload(); //this should only be called once (in Unload)

	void destroy();
};

#define HV Hypervisor::get()