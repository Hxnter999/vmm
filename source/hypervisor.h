#pragma once
#include <commons.h>
#include <svm_status.h>
#include <vmcb/vmcb.h>

class Hypervisor
{
	using per_cpu_callback_t = bool(*)(uint32_t);
	inline bool execute_on_all_cpus(per_cpu_callback_t callback) //this is cancer!
	{
		for (uint32_t i = 0; i < vcpus.vcpu_count; i++)
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

	void setup_host(vcpu_t& vcpu);

	void map_physical_memory();

	static svm_status init_check();

	uint64_t* npt;
	MSR::msrpm_t* shared_msrpm;
	host_pt_t shared_host_pt;

	struct vcpus_t {
		vcpus_t() : vcpu_count(0), buffer(nullptr) {}
		vcpus_t(uint32_t size) : vcpu_count(size) { buffer = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, size * sizeof(vcpu_t), 'sgma')); }

		vcpu_t* buffer;
		uint32_t vcpu_count;

		//vcpu_t* get(uint32_t i) { return buffer + i; }
		vcpu_t& get(uint32_t i) { return buffer[i]; }
		vcpu_t* begin() { return buffer; }
		vcpu_t* end() { return buffer + vcpu_count; }
	} vcpus;

public:
	static Hypervisor* instance;

	Hypervisor(Hypervisor&) = delete;
	void operator=(const Hypervisor&) = delete;

	MSR::msrpm_t& msrpm() { return *shared_msrpm; }

	//static Hypervisor* get();

	bool init();

	bool virtualize();

	bool setup_npts();

	template<exception_vector exception>
	void inject_event(vcpu_t& vcpu)
	{
		auto& ei = vcpu.guest_vmcb.control.event_injection;
		ei.valid = 1;
		ei.type = interrupt_type::HARDWARE_EXCEPTION;
		ei.evector = exception;
	}

	void devirtualize(vcpu_t* const vcpu);

	void unload(); //this should only be called once (in Unload)
};

#define HV Hypervisor::instance	