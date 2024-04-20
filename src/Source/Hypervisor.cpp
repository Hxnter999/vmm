#include "Hypervisor.h"
#include <shared.h>

#include <cpuid/standard-features/fn_vendor.h>
#include <cpuid/extended-features/fn_identifiers.h>
#include <cpuid/extended-features/fn_svm_features.h>

#include <msrs/vm_cr.h>

#include <PAGES/npts.h>

extern "C" void vmenter(uint64_t * guest_vmcb_pa);

Hypervisor* Hypervisor::instance = nullptr;


void Hypervisor::devirtualize(vcpu_t* const vcpu) //maybe move this into vcpu?
{
	static uint32_t current_count = vcpus.vcpu_count;

	current_count--;

	print("Exiting [%p]...\n", vcpu);

	// devirtualize current vcpu, later in the vmrun loop we restore rsp and jump to guest_rip.
	vcpu->guest_rip = vcpu->guest_vmcb.control.nrip;
	vcpu->guest_rsp = vcpu->guest_vmcb.save_state.rsp;

	__svm_vmload(vcpu->guest_vmcb_pa);

	_disable();
	__svm_stgi();

	MSR::EFER efer{}; efer.load(); efer.svme = 0; efer.store();
	__writeeflags(vcpu->guest_vmcb.save_state.rflags.value);

	if (!current_count)
		destroy();
}

void Hypervisor::destroy() 
{
	print("Destroy\n");

	if (instance == nullptr) {
		print("Hypervisor::destroy() called without instance\n");
		return; //should never happen
	}

	if (vcpus.buffer) {
		ExFreePoolWithTag(vcpus.buffer, 'sgmA');
		vcpus.buffer = nullptr;
	}
	if (shared_msrpm) {
		MmFreeContiguousMemory(shared_msrpm);
		shared_msrpm = nullptr;
	}
	if (npt) {
		MmFreeContiguousMemory(npt);
		npt = nullptr;
	}

	ExFreePoolWithTag(instance, 'sgmA');
	instance = nullptr;
}

void Hypervisor::unload()
{
	if (instance == nullptr) {
		print("Hypervisor::unload() called without instance\n");
		return; //should never happen
	}

	print("Unloading Hypervisor...\n");

	for (auto& cvcpu : vcpus) // alert all other vcpus
		cvcpu.should_exit = true;

	execute_on_all_cpus([](uint32_t index) -> bool {
		print("Devirtualizing [%d]...\n", index);
		testcall(HYPERCALL_CODE::UNLOAD);
		return true;
	}, vcpus.vcpu_count);
}

bool Hypervisor::init()
{
	instance = static_cast<Hypervisor*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(Hypervisor), 'sgmA'));
	if (!instance) 
	{
		print("Hypervisor failed to allocate (insufficient memory)\n");
		return false;
	}

	instance->vcpus = {};
	instance->shared_msrpm = nullptr;
	instance->npt = nullptr;

	print("Initializing Hypervisor...\n");

	instance->vcpus = { KeQueryActiveProcessorCount(nullptr) };

	instance->shared_msrpm = static_cast<MSR::msrpm_t*>(MmAllocateContiguousMemory(sizeof(MSR::msrpm_t), { .QuadPart = -1 }));
	if (instance->shared_msrpm == nullptr) {
		print("Failed to allocate msrpm\n");
		return false;
	}
	memset(instance->shared_msrpm, 0, sizeof(MSR::msrpm_t));

	HV->setup_host_pt();

	print("Setup\n");
	return true;
}

bool Hypervisor::virtualize(uint32_t index)
{
	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();

	CONTEXT* ctx = static_cast<CONTEXT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(CONTEXT), 'xtc'));
	memset(ctx, 0, sizeof(CONTEXT));
	RtlCaptureContext(ctx);

	print("Checking efer\n");
	// efer.svme will be 0 when we read it in a virtualized state, this is how we have the msr handler setup.
	MSR::EFER guest_efer{}; guest_efer.load();
	if (!guest_efer.svme) { 
		return true; 
	}

	print("Failed efer check\n");

	print("Setting up vmcb\n");
	auto& vcpu = *vcpus.get(index);
	setup_vmcb(vcpu, *ctx);

	ExFreePoolWithTag(ctx, 'xtc'); //somebody forgot to free...
	print("Entering vm\n");
	vmenter(&vcpu.guest_vmcb_pa);

	// shouldnt reach this point, if so something went wrong
	print("shouldnt reach this point, if so something went wrong\n");
	return false;
}

bool Hypervisor::setup_npts() 
{
	return initnpts(npt);
}

bool Hypervisor::virtualize() 
{
	if (!execute_on_all_cpus([](uint32_t index) -> bool {
		print("Virtualizing [%d]...\n", index);
		if (!Hypervisor::get()->virtualize(index))
		{
			print("Failed to virtualize\n");
			return false;
		}
		print("Virtualized [%d]\n", index);
		return true;
		}, vcpus.vcpu_count))
	{
		print("Failed to virtualize all vcpus\n");
		return false;
	}

	return true;
}

void Hypervisor::setup_host_pt() {
	print("Setting up host page tables\n");
	// map all the physical memory and share it between the hosts to be able to access it directly.

	//auto system_process = reinterpret_cast<_EPROCESS*>(PsInitialSystemProcess);
	//cr3_t system_cr3{ system_process->Pcb.DirectoryTableBase };

	//auto system_process = reinterpret_cast<uintptr_t>(PsInitialSystemProcess);
	//auto system_cr3 = *reinterpret_cast<cr3_t*>(system_process + 0x28);

	memset(&shared_host_pt, 0, sizeof(shared_host_pt));

	cr3_t system_cr3{ __readcr3() };

	auto system_pml4 = reinterpret_cast<pml4e_t*>(MmGetVirtualForPhysical({ .QuadPart = system_cr3.get_phys_pml4() }));

	memcpy(&shared_host_pt.pml4[256], &system_pml4[256], sizeof(pml4e_t) * 256);

	auto& pml4e = shared_host_pt.pml4[shared_host_pt.phys_pml4e];
	pml4e.present = 1;
	pml4e.write = 1;
	pml4e.page_pa = MmGetPhysicalAddress(&shared_host_pt.pdpt).QuadPart >> 12;

	for (uint32_t i = 0; i < 64; i++) {
		auto& pdpte = shared_host_pt.pdpt[i];
		pdpte.present = 1;
		pdpte.write = 1;
		pdpte.page_pa = MmGetPhysicalAddress(&shared_host_pt.pd[i]).QuadPart >> 12;

		for (uint32_t j = 0; j < 512; j++) {
			auto& pde = shared_host_pt.pd[i][j];
			pde.present = 1;
			pde.write = 1;
			pde.large_page = 1;
			pde.page_pa = i * 512ull + j;
		}
	}
}

svm_status Hypervisor::init_check()
{
	CPUID::fn_vendor vendor_check{};
	vendor_check.load();

	if (!vendor_check.is_amd_vendor())
	{
		print("Vendor check failed... get off intel nerd\n");
		return svm_status::SVM_WRONG_VENDOR;
	}

	print("Vendor check passed\n");

	CPUID::fn_identifiers id{};
	id.load();

	if (!id.feature_identifiers.svm)
	{
		print("SVM not supported\n");
		return svm_status::SVM_IS_NOT_SUPPORTED_BY_CPU;
	}

	CPUID::fn_svm_features svm_rev{};
	svm_rev.load();

	if (!svm_rev.svm_feature_identification.nested_paging)
	{
		print("Nested paging not supported\n");
		return svm_status::SVM_NESTED_PAGING_NOT_SUPPORTED;
	}

	if (!svm_rev.svm_feature_identification.n_rip) // necessary otherwise we have to emulate it which is a pain
	{
		print("Uh oh! N_RIP not supported\n");
		return svm_status::SVM_NEXT_RIP_NOT_SUPPORTED;
	}

	//if (!svm_rev.svm_feature_identification.vnmi) // necessary otherwise we have to emulate it which is a pain
	//{
	//	print("Uh oh! V_NMI not supported\n");
	//	return svm_status::SVM_NEXT_RIP_NOT_SUPPORTED;
	//}

	MSR::VM_CR vm_cr{};
	vm_cr.load();

	if (!vm_cr.svmdis)
	{
		print("SVM not enabled but can be (;\n");
		return svm_status::SVM_IS_CAPABLE_OF_BEING_ENABLE; // Yippe!
	}

	if (!svm_rev.svm_feature_identification.svm_lock)
	{
		print("SVM lock bit not set, disabled by BIOS...\n");
		return svm_status::SVM_DISABLED_AT_BIOS_NOT_UNLOCKABLE;
	}

	print("SVM lock bit set, disabled\n");
	return svm_status::SVM_DISABLED_WITH_KEY;
}