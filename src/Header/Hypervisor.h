#pragma once
#include <commons.h>
#include <svm_status.h>
#include <vmcb/vmcb.h>

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

	void setup_host_pt();

	static svm_status init_check();


	bool vaild;
	uint64_t* npt;
	MSR::msrpm_t* shared_msrpm;
	host_pt_t shared_host_pt;

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

	bool is_valid() { return vaild; }

	bool virtualize();

	bool setup_npts();

	bool get_phys(cr3_t cr3, virtual_address_t va, PHYSICAL_ADDRESS& phy);

	template<typename T>
	T read_phys(PHYSICAL_ADDRESS phy);

	template<typename T>
	void write_phys(PHYSICAL_ADDRESS phy, const T& value);

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

#define HV Hypervisor::get()