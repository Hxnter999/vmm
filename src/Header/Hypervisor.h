#pragma once
#include "ARCH/VMCB/vmcb.h"
#include "ARCH/PAGES/npts.h"

extern "C" int64_t testcall(hypercall_code code);
extern "C" void vmenter(uint64_t * guest_vmcb_pa);

class Hypervisor
{
	static Hypervisor* instance;
	
	bool runOnAllVCpus(bool(*func)(uint32_t)) 
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

	void init()
	{
		//set all to members to 0
		current_vcpu = nullptr;
		vcpus = {};
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

		vcpus = { KeQueryActiveProcessorCount(nullptr) };

		//same comment as above
		if (!setup_msrpm()) {
			print("Failed to allocate msrpm\n");
			return;
		}

		//this is kinda scuffed (maybe should unvirt all the successful vcpus)
		if (!runOnAllVCpus([](uint32_t index) -> bool {
				print("Virtualizing [%d]...\n", index);
				if (!Hypervisor::Get()->virtualize(index))
				{
					print("Failed to virtualize\n");
					return false;
				}
				return true;
			}))
		{
			print("Failed to virtualize all vcpus\n");
			return;
		}

		print("Virtualized\n");
		vaild = true;
	}

	void devirtualize(uint32_t index) {

		vcpu_t* vcpu = vcpus.get(index);
		print("Exiting [%d]...\n", index);
		
		if(!vcpu->should_exit)
			for (auto& cvcpu : vcpus) // alert all other vcpus
				cvcpu.should_exit = true;

		// devirtualize current vcpu, later in the vmrun loop we restore rsp and jump to guest_rip.
		vcpu->guest_rip = vcpu->guest_vmcb.control.nrip;
		vcpu->guest_rsp = vcpu->guest_vmcb.save_state.rsp;

		__svm_vmload(vcpu->guest_vmcb_pa);

		_disable();
		__svm_stgi();

		MSR::EFER efer{}; efer.load(); efer.svme = 0; efer.store();
		__writeeflags(vcpu->guest_vmcb.save_state.rflags);
	}

	bool virtualize(uint32_t index) {
		vcpu_t* vcpu = vcpus.get(index);
		CONTEXT* ctx = reinterpret_cast<CONTEXT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(CONTEXT), 'sgma'));
		memset(ctx, 0, sizeof(CONTEXT));
		RtlCaptureContext(ctx);

		if (Hypervisor::Get()->current_vcpu->is_virtualized) { //hunter said he had a better way
			//__debugbreak();
			return true;
		}
		setup_vmcb(vcpu, ctx);
		vmenter(&vcpu->guest_vmcb_pa);
		return false;
	}

	static SVM_STATUS init_check();

	uint64_t* npt;
	vcpu_t* current_vcpu;
	struct _vcpus {
		_vcpus() : vcpu_count(0), buffer(nullptr) {}
		_vcpus(uint32_t size) : vcpu_count(size) { buffer = reinterpret_cast<vcpu_t*>(ExAllocatePoolWithTag(NonPagedPool, size * sizeof(vcpu_t), 'sgma')); }
		~_vcpus() { if(buffer) ExFreePool(buffer); }

		vcpu_t* buffer;
		uint32_t vcpu_count;

		vcpu_t* get(uint32_t i) { return buffer + i; }
		vcpu_t* begin() { return buffer; }
		vcpu_t* end() { return buffer + vcpu_count; }
	} vcpus;
public:
	MSR::msrpm_t* shared_msrpm;


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
		if(instance == nullptr) return; //should never happen

		print("Unloading Hypervisor...\n");

		runOnAllVCpus([](uint32_t index) -> bool {
			print("Unvirtualizing [%d]...\n", index);
			testcall(hypercall_code::UNLOAD);
			return true;
		});

		vcpus.~_vcpus();
		if (shared_msrpm) {
			MmFreeContiguousMemory(shared_msrpm);
			shared_msrpm = nullptr;
		}
		if (npt) {
			MmFreeContiguousMemory(npt);
			npt = nullptr;
		}

		ExFreePoolWithTag(instance, 'hv');
		instance = nullptr;
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
