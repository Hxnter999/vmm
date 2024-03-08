#include "../Header/commons.h"
#include "../Header/SVM_STATUS.h"
#include "../Header/ARCH/MSRs/efer.h"
#include "../Header/ARCH/CPUID/Extended Features/fn_identifiers.h"
#include "../Header/ARCH/CPUID/Extended Features/fn_processor_capacity.h"
#include "../Header/ARCH/CPUID/Extended Features/fn_svm_features.h"
#include "../Header/ARCH/CPUID/Standard Features/fn_vendor.h"
#include "../Header/ARCH/MSRs/vm_cr.h"
#include "../Header/ARCH/VMCB/vmcb.h"
#include "../Header/ARCH/MSRs/pat.h"
#include "../Header/ARCH/MSRs/hsave_pa.h"
#include "../Header/ARCH/dtr.h"

extern "C" {
	extern void __sgdt(void* gdtr); // here for now
}

SVM_STATUS inittest() 
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
		print("SVM not supported, womp womp\n");
		return SVM_STATUS::SVM_IS_NOT_SUPPORTED_BY_CPU;
	}

	CPUID::fn_svm_features svm_rev{};
	svm_rev.load();

	if (!svm_rev.svm_feature_identification.nested_paging)
	{
		print("Nested paging not supported, womp womp\n");
		return SVM_STATUS::SVM_NESTED_PAGING_NOT_SUPPORTED;
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



void setupvmcb(vcpu* vcpu) //dis just a test
{
	CONTEXT* ctx = reinterpret_cast<CONTEXT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(CONTEXT), 'sgma'));
	RtlCaptureContext(ctx);

	if (vcpu->is_virtualized) {
		print("already virtualized\n");
		return;
	}

	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();

	static sharedvcpu sharedVcpu{};

	vcpu->guest_vmcb.control.msrpm_base_pa = MmGetPhysicalAddress(&sharedVcpu.shared_msrpm);
	
	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.bits = MmGetPhysicalAddress(&vcpu->host_state_area).QuadPart;
	hsave_pa.store();
	//Set up control area

	//TODO: set interupts blah blah

	vcpu->guest_vmcb.control.vmrun = 1; // VMRUN intercepts muse be enabled 15.5.1

	vcpu->guest_vmcb.control.asid = 1; // Address space identifier "ASID [cannot be] equal to zero" 15.5.1

	// Set up the guest state
	vcpu->guest_vmcb.save_state.cr0 = __readcr0();
	vcpu->guest_vmcb.save_state.cr2 = __readcr2();
	vcpu->guest_vmcb.save_state.cr3 = __readcr3();
	vcpu->guest_vmcb.save_state.cr4 = __readcr4();
	vcpu->guest_vmcb.save_state.efer = __readmsr(MSR::EFER::MSR_EFER);
	vcpu->guest_vmcb.save_state.g_pat = __readmsr(MSR::PAT::MSR_PAT); // very sigma (kinda like MTRRs but for page tables)

	dtr idtr{}; __sidt(&idtr);
	vcpu->guest_vmcb.save_state.idtr = idtr;
	dtr gdtr{}; __sgdt(&gdtr);
	vcpu->guest_vmcb.save_state.gdtr = gdtr;

	//TODO: need to set RSP, RIP, and RFLAGS (This is where the guest will start executing)
	vcpu->guest_vmcb.save_state.rsp = ctx->Rsp;
	vcpu->guest_vmcb.save_state.rip = ctx->Rip;
	vcpu->guest_vmcb.save_state.rflags = ctx->EFlags;

	//TODO: Setup all the segment registers
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


	vcpu->is_virtualized = true;
	ExFreePoolWithTag(ctx, 'sgma');
}

void Unload(PDRIVER_OBJECT pDriverObject);

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath) 
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = Unload;

	if (!inittest()) 
	{
		print("SVM not supported\n");
		return STATUS_UNSUCCESSFUL;
	}

	print("SVM supported\n");

	auto vcpu_count = KeQueryActiveProcessorCount(nullptr);
	auto vcpus = reinterpret_cast<vcpu*>(ExAllocatePoolWithTag(NonPagedPool, vcpu_count * sizeof(vcpu), 'sgma'));

	for (uint32_t i = 0; i < vcpu_count; i++) 
	{
		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);
		print("attempting to set up vcpu %d\n", KeGetCurrentProcessorIndex());
		setupvmcb(vcpus + i);

		KeRevertToUserAffinityThreadEx(original_affinity);
	}




	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	print("Driver Unloaded\n");
}