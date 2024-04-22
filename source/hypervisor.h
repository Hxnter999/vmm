#pragma once
#include <commons.h>
#include <svm_status.h>
#include <vcpu/vcpu.h>

class Hypervisor
{
	host_pt_t shared_host_pt;
	vcpu_t* vcpus;
	uint32_t vcpu_count;

	using per_cpu_callback_t = bool(*)(uint32_t);
	inline bool execute_on_all_cpus(per_cpu_callback_t callback)
	{
		for (uint32_t i = 0; i < vcpu_count; i++)
		{
			auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);
			bool result = callback(i);
			KeRevertToUserAffinityThreadEx(original_affinity);
			if (!result) return false;
		}
		return true;
	}

	Hypervisor() = default;


	bool virtualize(uint32_t index);

	void setup_guest(vcpu_t& vcpu, CONTEXT* ctx); 

	void setup_npt(vcpu_t& vcpu);

	void setup_msrpm(vcpu_t& vcpu);

	void setup_host(vcpu_t& vcpu);

	void map_physical_memory();

	static svm_status init_check();

public:
	static Hypervisor* instance;

	Hypervisor(Hypervisor&) = delete;
	void operator=(const Hypervisor&) = delete;

	bool init();

	bool virtualize();


	void devirtualize(vcpu_t* const vcpu);

	void unload(); //this should only be called once (in Unload)
};

#define HV Hypervisor::instance	