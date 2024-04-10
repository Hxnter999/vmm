#include "Hypervisor.h"

#include <cpuid/standard-features/fn_vendor.h>
#include <cpuid/extended-features/fn_identifiers.h>
#include <cpuid/extended-features/fn_svm_features.h>
#include <msrs/msrs.h>
#include <msrs/hsave_pa.h>
#include <msrs/pat.h>
#include <msrs/vm_cr.h>
#include <hypercall/hypercall.h>
#include <pages/npts.h>

extern "C" int64_t testcall(hypercall_code code);
extern "C" void vmenter(uint64_t * guest_vmcb_pa);

Hypervisor* Hypervisor::instance = nullptr;


void Hypervisor::devirtualize(vcpu_t* const vcpu) //maybe move this into vcpu?
{
	print("Exiting [%d]...\n", (vcpu - vcpus.begin()) / sizeof(vcpu_t*));

	for (auto& cvcpu : vcpus) // alert all other vcpus
		cvcpu.should_exit = true;

	// devirtualize current vcpu, later in the vmrun loop we restore rsp and jump to guest_rip.
	vcpu->guest_rip = vcpu->guest_vmcb.control.nrip;
	vcpu->guest_rsp = vcpu->guest_vmcb.save_state.rsp;

	__svm_vmload(vcpu->guest_vmcb_pa);

	_disable();
	__svm_stgi();

	MSR::EFER efer{}; efer.load(); efer.svme = 0; efer.store();
	__writeeflags(vcpu->guest_vmcb.save_state.rflags.value);
}

void Hypervisor::unload()
{
	if (instance == nullptr) return; //should never happen

	print("Unloading Hypervisor...\n");

	execute_on_all_cpus([](uint32_t index) -> bool {
		print("Devirtualizing [%d]...\n", index);
		testcall(hypercall_code::UNLOAD);
		return true;
	});

	if (vcpus.buffer) {
		ExFreePoolWithTag(vcpus.buffer, 'hv');
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

	ExFreePoolWithTag(instance, 'hv');
	instance = nullptr;
}

void Hypervisor::init()
{
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

	vcpus = { KeQueryActiveProcessorCount(nullptr) };

	shared_msrpm = reinterpret_cast<MSR::msrpm_t*>(MmAllocateContiguousMemory(sizeof(MSR::msrpm_t), { .QuadPart = -1 }));
	if (shared_msrpm == nullptr) {
		print("Failed to allocate msrpm\n");
		return;
	}

	HV->setup_host_pt();

	print("Setup\n");
	vaild = true;
}

bool Hypervisor::virtualize(uint32_t index)
{
	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();

	vcpu_t* vcpu = vcpus.get(index);
	CONTEXT* ctx = reinterpret_cast<CONTEXT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(CONTEXT), 'sgma'));
	memset(ctx, 0, sizeof(CONTEXT));
	RtlCaptureContext(ctx);

	print("Checking efer\n");
	// efer.svme will be 0 when we read it in a virtualized state, this is how we have the msr handler setup.
	MSR::EFER guest_efer{}; guest_efer.load();
	if (!guest_efer.svme) return true;

	print("Setting up vmcb\n");
	setup_vmcb(vcpu, ctx);

	print("Entering vm\n");
	vmenter(&vcpu->guest_vmcb_pa);

	// shouldnt reach this point, if so something went wrong
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
		}))
	{
		print("Failed to virtualize all vcpus\n");
		return false;
	}

	return true;
}

void Hypervisor::setup_vmcb(vcpu_t* vcpu, CONTEXT* ctx) //should make it a reference
{
	vcpu->is_virtualized = true;

	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.bits = MmGetPhysicalAddress(&vcpu->host_vmcb).QuadPart;
	hsave_pa.store();

	vcpu->guest_vmcb.control.msrpm_base_pa = MmGetPhysicalAddress(shared_msrpm).QuadPart;

	// ------------------- Setup guest state -------------------
	vcpu->guest_vmcb.control.vmrun = 1; // VMRUN intercepts muse be enabled 15.5.1
	vcpu->guest_vmcb.control.vmmcall = 1; // explicit vmexits back to host
	vcpu->guest_vmcb.control.vmload = 1;
	vcpu->guest_vmcb.control.vmsave = 1;
	vcpu->guest_vmcb.control.clgi = 1;
	vcpu->guest_vmcb.control.msr_prot = 1; 

	vcpu->guest_vmcb.control.guest_asid = 1; // Address space identifier "ASID [cannot be] equal to zero" 15.5.1 ASID 0 is for the host
	vcpu->guest_vmcb.control.v_intr_masking = 1; // 15.21.1 & 15.22.2

	vcpu->guest_vmcb.control.cpuid = 1;

	if (npt) {
		vcpu->guest_vmcb.control.np_enable = 1;
		vcpu->guest_vmcb.control.n_cr3 = MmGetPhysicalAddress(npt).QuadPart;
		print("NPT: %p\n", MmGetPhysicalAddress(npt).QuadPart);
	}
	
	// Set up the guest state
	vcpu->guest_vmcb.save_state.cr0.value = __readcr0();
	vcpu->guest_vmcb.save_state.cr2.value = __readcr2();
	vcpu->guest_vmcb.save_state.cr3.value = __readcr3();
	vcpu->guest_vmcb.save_state.cr4.value = __readcr4();
	vcpu->guest_vmcb.save_state.efer.bits = __readmsr(MSR::EFER::MSR_EFER);
	vcpu->guest_vmcb.save_state.g_pat = __readmsr(MSR::PAT::MSR_PAT); // very sigma (kinda like MTRRs but for page tables)

	descriptor_table_register idtr{}, gdtr{}; __sidt(&idtr); _sgdt(&gdtr);
	vcpu->guest_vmcb.save_state.idtr.base = idtr.base;
	vcpu->guest_vmcb.save_state.idtr.limit = idtr.limit;

	vcpu->guest_vmcb.save_state.gdtr.base = gdtr.base;
	vcpu->guest_vmcb.save_state.gdtr.limit = gdtr.limit;

	//TODO: need to set RSP, RIP, and RFLAGS (This is where the guest will start executing)
	vcpu->guest_vmcb.save_state.rsp = ctx->Rsp;
	vcpu->guest_vmcb.save_state.rip = ctx->Rip;
	vcpu->guest_vmcb.save_state.rflags.value = ctx->EFlags;

	//Setup all the segment registers
	vcpu->guest_vmcb.save_state.cs.limit = __segmentlimit(ctx->SegCs);
	vcpu->guest_vmcb.save_state.ds.limit = __segmentlimit(ctx->SegDs);
	vcpu->guest_vmcb.save_state.es.limit = __segmentlimit(ctx->SegEs);
	vcpu->guest_vmcb.save_state.ss.limit = __segmentlimit(ctx->SegSs);

	vcpu->guest_vmcb.save_state.cs.selector.value = ctx->SegCs;
	vcpu->guest_vmcb.save_state.ds.selector.value = ctx->SegDs;
	vcpu->guest_vmcb.save_state.es.selector.value = ctx->SegEs;
	vcpu->guest_vmcb.save_state.ss.selector.value = ctx->SegSs;

	vcpu->guest_vmcb.save_state.cs.get_attributes(gdtr.base);
	vcpu->guest_vmcb.save_state.ds.get_attributes(gdtr.base);
	vcpu->guest_vmcb.save_state.es.get_attributes(gdtr.base);
	vcpu->guest_vmcb.save_state.ss.get_attributes(gdtr.base);

	vcpu->guest_vmcb_pa = MmGetPhysicalAddress(&vcpu->guest_vmcb).QuadPart;
	vcpu->host_vmcb_pa = MmGetPhysicalAddress(&vcpu->host_vmcb).QuadPart;
	vcpu->self = vcpu;

	// ------------------- Setup host state -------------------
	auto& host_cr3 = vcpu->host_vmcb.save_state.cr3;
	host_cr3.value = 0;
	host_cr3.pml4 = MmGetPhysicalAddress(&shared_host_pt.pml4).QuadPart >> 12;

	print("Writing host cr3: %zX\n", host_cr3.value);
	__writecr3(host_cr3.value);
	__svm_vmsave(vcpu->host_vmcb_pa);


	__svm_vmsave(vcpu->guest_vmcb_pa); // needed here cause the vmrun loop loads guest state before everything, if there isnt a guest saved already it wont work properly
}

void Hypervisor::setup_host_pt() {
	print("Setting up host page tables\n");
	// map all the physical memory and share it between the hosts to be able to access it directly.
	auto& pml4e = shared_host_pt.pml4[shared_host_pt.phys_pml4e];
	pml4e.present = 1;
	pml4e.write = 1;
	pml4e.page_pa = MmGetPhysicalAddress(&shared_host_pt.pdpt).QuadPart >> 12;

	for (int i = 0; i < 64; i++) {
		auto& pdpte = shared_host_pt.pdpt[i];
		pdpte.present = 1;
		pdpte.write = 1;
		pdpte.page_pa = MmGetPhysicalAddress(&shared_host_pt.pd[i]).QuadPart >> 12;

		for (int j = 0; j < 512; j++) {
			auto& pde = shared_host_pt.pd[i][j];
			pde.present = 1;
			pde.write = 1;
			pde.large_page = 1;
			pde.page_pa = i * 512 + j;
		}
	}

	//auto system_process = reinterpret_cast<_EPROCESS*>(PsInitialSystemProcess);
	//cr3_t system_cr3{ system_process->Pcb.DirectoryTableBase };

	auto system_process = reinterpret_cast<uintptr_t>(PsInitialSystemProcess);
	auto system_cr3 = *reinterpret_cast<cr3_t*>(system_process + 0x28);

	auto system_pml4 = reinterpret_cast<pml4e_t*>(MmGetVirtualForPhysical({ .QuadPart = static_cast<int64_t>(system_cr3.pml4 << 12) }));

	memcpy(&shared_host_pt.pml4[256], &system_pml4[256], sizeof(pml4e_t) * 256);
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
		print("Uh oh! Next RIP not supported\n");
		return svm_status::SVM_NEXT_RIP_NOT_SUPPORTED;
	}

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

//should use page tables but this is from my other driver
bool Hypervisor::get_phys(uint64_t cr3, virtual_address_t va, PHYSICAL_ADDRESS& phy)
{
	//0x8 bytes (sizeof)
	struct _MMPTE_HARDWARE
	{
		ULONGLONG Valid : 1;                                                      //0x0
		ULONGLONG Dirty1 : 1;                                                     //0x0
		ULONGLONG Owner : 1;                                                      //0x0
		ULONGLONG WriteThrough : 1;                                               //0x0
		ULONGLONG CacheDisable : 1;                                               //0x0
		ULONGLONG Accessed : 1;                                                   //0x0
		ULONGLONG Dirty : 1;                                                      //0x0
		ULONGLONG LargePage : 1;                                                  //0x0
		ULONGLONG Global : 1;                                                     //0x0
		ULONGLONG CopyOnWrite : 1;                                                //0x0
		ULONGLONG Unused : 1;                                                     //0x0
		ULONGLONG Write : 1;                                                      //0x0
		ULONGLONG PageFrameNumber : 36;                                           //0x0
		ULONGLONG ReservedForHardware : 4;                                        //0x0
		ULONGLONG ReservedForSoftware : 4;                                        //0x0
		ULONGLONG WsleAge : 4;                                                    //0x0
		ULONGLONG WsleProtection : 3;                                             //0x0
		ULONGLONG NoExecute : 1;                                                  //0x0
	};


	PHYSICAL_ADDRESS pa{};

	pa.QuadPart = cr3 + va.pml4_index * sizeof(_MMPTE_HARDWARE);
	_MMPTE_HARDWARE pml4e = { read_phys<_MMPTE_HARDWARE>(pa) };

	if (!pml4e.Valid) return false;

	pa.QuadPart = (pml4e.PageFrameNumber << 12) + va.pdpt_index * sizeof(_MMPTE_HARDWARE);
	_MMPTE_HARDWARE pdpe = { read_phys<_MMPTE_HARDWARE>(pa) };

	if (!pdpe.Valid) return false;

	if (pdpe.LargePage) {
		phy.QuadPart = (pdpe.PageFrameNumber << 30) + (va.pd_index << 21) + (va.pt_index << 12) + va.offset;
	}

	pa.QuadPart = (pdpe.PageFrameNumber << 12) + va.pd_index * sizeof(_MMPTE_HARDWARE);
	_MMPTE_HARDWARE pde = { read_phys<_MMPTE_HARDWARE>(pa) };

	if (!pde.Valid) return false;

	if (pde.Valid) {
		phy.QuadPart = (pde.PageFrameNumber << 21) + (va.pt_index << 12) + va.offset;
	}

	pa.QuadPart = (pde.PageFrameNumber << 12) + va.pt_index * sizeof(_MMPTE_HARDWARE);
	_MMPTE_HARDWARE pte = { read_phys< _MMPTE_HARDWARE>(pa) };

	if (!pte.Valid) return false;

	phy.QuadPart = (pte.PageFrameNumber << 12) + va.offset;
	return true;
}
