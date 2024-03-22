#include "../Header/Hypervisor.h"

#include "../Header/ARCH/CPUID/Standard Features/fn_vendor.h"

#include "../Header/ARCH/MSRs/msrs.h"
#include "../Header/Hypercall/hypercall.h"
#include "../Header/ARCH/MSRs/hsave_pa.h"
#include "../Header/ARCH/MSRs/pat.h"
#include "../Header/ARCH/CPUID/Extended Features/fn_identifiers.h"
#include "../Header/ARCH/CPUID/Extended Features/fn_svm_features.h"
#include "../Header/ARCH/MSRs/vm_cr.h"
#include "../Header/ARCH/PAGES/npts.h"

extern "C" int64_t testcall(hypercall_code code);
extern "C" void vmenter(uint64_t * guest_vmcb_pa);

Hypervisor* Hypervisor::instance = nullptr;

Hypervisor* Hypervisor::Get()
{
	//naive because we know when its first called
	if (instance == nullptr)
	{
		instance = static_cast<Hypervisor*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(Hypervisor), 'hv'));
		instance->init();
	}
	return instance;
}

void Hypervisor::devirtualize(vcpu_t* vcpu)
{

	print("Exiting [%d]...\n", (vcpu - vcpus.begin()) / sizeof(vcpu_t*));

	if (!vcpu->should_exit)
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

void Hypervisor::Unload()
{
	if (instance == nullptr) return; //should never happen

	print("Unloading Hypervisor...\n");

	runOnAllVCpus([](uint32_t index) -> bool {
		print("Unvirtualizing [%d]...\n", index);
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
	//set all to members to 
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

	vcpus = { KeQueryActiveProcessorCount(nullptr) };

	(shared_msrpm) = reinterpret_cast<MSR::msrpm_t*>(MmAllocateContiguousMemory(sizeof(MSR::msrpm_t), { .QuadPart = -1 }));
	if (shared_msrpm == nullptr) {
		print("Failed to allocate msrpm\n");
		return;
	}

	print("Setup\n");
	vaild = true;
}

bool Hypervisor::virtualize(uint32_t index)
{
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

bool Hypervisor::setup_npts() 
{
	return initnpts(&npt);
}

bool Hypervisor::virtualize() 
{
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
		return false;
	}

	return true;
}

void Hypervisor::setup_vmcb(vcpu_t* vcpu, CONTEXT* ctx)
{
	vcpu->is_virtualized = true;

	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();

	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.bits = MmGetPhysicalAddress(&vcpu->host_vmcb).QuadPart;
	hsave_pa.store();

	vcpu->guest_vmcb.control.msrpm_base_pa = MmGetPhysicalAddress(shared_msrpm).QuadPart;

	//Set up control area
	//TODO: set interupts
	vcpu->guest_vmcb.control.vmrun = 1; // VMRUN intercepts muse be enabled 15.5.1
	vcpu->guest_vmcb.control.vmmcall = 1; // explicit vmexits back to host
	vcpu->guest_vmcb.control.vmmload = 1;
	vcpu->guest_vmcb.control.vmmsave = 1;
	vcpu->guest_vmcb.control.msr_prot = 1; // enable this once msrpm and handler is fixed up

	vcpu->guest_vmcb.control.guest_asid = 1; // Address space identifier "ASID [cannot be] equal to zero" 15.5.1 ASID 0 is for the host
	vcpu->guest_vmcb.control.v_intr_masking = 1; // 15.21.1 & 15.22.2

	vcpu->guest_vmcb.control.np_enable = 1;
	vcpu->guest_vmcb.control.n_cr3 = MmGetPhysicalAddress(npt).QuadPart;
	print("NPT: %p\n", MmGetPhysicalAddress(npt).QuadPart);

	// Set up the guest state
	vcpu->guest_vmcb.save_state.cr0.value = __readcr0();
	vcpu->guest_vmcb.save_state.cr2.value = __readcr2();
	vcpu->guest_vmcb.save_state.cr3.value = __readcr3();
	vcpu->guest_vmcb.save_state.cr4.value = __readcr4();
	vcpu->guest_vmcb.save_state.efer = efer.bits;
	vcpu->guest_vmcb.save_state.g_pat = __readmsr(MSR::PAT::MSR_PAT); // very sigma (kinda like MTRRs but for page tables)

	SEGMENT::descriptor_table_register idtr{}, gdtr{}; __sidt(&idtr); _sgdt(&gdtr);
	vcpu->guest_vmcb.save_state.idtr.base = idtr.base;
	vcpu->guest_vmcb.save_state.idtr.limit = idtr.limit;

	vcpu->guest_vmcb.save_state.gdtr.base = gdtr.base;
	vcpu->guest_vmcb.save_state.gdtr.limit = gdtr.limit;

	//TODO: need to set RSP, RIP, and RFLAGS (This is where the guest will start executing)
	vcpu->guest_vmcb.save_state.rsp = ctx->Rsp;
	vcpu->guest_vmcb.save_state.rip = ctx->Rip;
	vcpu->guest_vmcb.save_state.rflags = ctx->EFlags;

	//vcpu->guest_vmcb.save_state.rax = ctx->Rax;

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
	vcpu->self = vcpu;

	__svm_vmsave(vcpu->guest_vmcb_pa); // needed here cause the vmrun loop loads guest state before everything, if there isnt a guest saved already it wont work properly
}

SVM_STATUS Hypervisor::init_check()
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