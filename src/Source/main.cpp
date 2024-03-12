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
#include "../Header/Util/bitset.h"
extern "C" {
	extern void _sgdt(void* gdtr); // here for now
	extern void testcall();
	extern bool vmexit_handler(vcpu* vcpu) {
		UNREFERENCED_PARAMETER(vcpu);
		__debugbreak();
		print("VMEXIT\n");
		print("vcpu: %p\n", vcpu);
		print("Exit: %X", vcpu->guest_vmcb.control.exit_code);

		//true to continue
		//false to devirt
		return true;
	}
	extern void WHATS_A_GOOD_NAME(vcpu* guest_vmcb_pa);
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

shared global{};
void setupvmcb(vcpu* vcpu) //dis just a test
{
	global.current_vcpu = vcpu;

	CONTEXT* ctx = reinterpret_cast<CONTEXT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(CONTEXT), 'sgma'));
	memset(ctx, 0, sizeof(CONTEXT));
	RtlCaptureContext(ctx);

	if (global.current_vcpu->is_virtualized) {
		__debugbreak();
		print("already virtualized\n");
		testcall();
		testcall();
		return;
	}
	global.current_vcpu->is_virtualized = true;

	print("Starting to virtualize...\n");
	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();
	
	vcpu->guest_vmcb.control.msrpm_base_pa = MmGetPhysicalAddress(global.shared_msrpm).QuadPart;


	//Set up control area
	//TODO: set interupts blah blah
	vcpu->guest_vmcb.control.vmrun = 1; // VMRUN intercepts muse be enabled 15.5.1
	vcpu->guest_vmcb.control.vmmcall = 1; // UM call VM
	vcpu->guest_vmcb.control.vmmload = 1; // VMLOAD intercepts must be enabled 15.5.1
	vcpu->guest_vmcb.control.vmmsave = 1; // VMSAVE intercepts must be enabled 15.5.1

	vcpu->guest_vmcb.control.guest_asid = 1; // Address space identifier "ASID [cannot be] equal to zero" 15.5.1 ASID 0 is for the host

	// Set up the guest state
	vcpu->guest_vmcb.save_state.cr0.value = __readcr0();
	vcpu->guest_vmcb.save_state.cr2.value = __readcr2();
	vcpu->guest_vmcb.save_state.cr3.value = __readcr3();
	vcpu->guest_vmcb.save_state.cr4.value = __readcr4();
	vcpu->guest_vmcb.save_state.efer = __readmsr(MSR::EFER::MSR_EFER);
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

	__svm_vmsave(MmGetPhysicalAddress(&vcpu->guest_vmcb).QuadPart);

	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.bits = MmGetPhysicalAddress(&vcpu->host_state_area).QuadPart;
	hsave_pa.store();

	//__svm_vmsave(MmGetPhysicalAddress(&vcpu->host_vmcb).QuadPart);
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

	// setup the vcpus
	global.vcpu_count = KeQueryActiveProcessorCount(nullptr);
	global.vcpus = reinterpret_cast<vcpu*>(ExAllocatePoolWithTag(NonPagedPool, global.vcpu_count * sizeof(vcpu), 'sgma')); //FREE THIS LATER
	memset(global.vcpus, 0, global.vcpu_count * sizeof(vcpu));

	global.shared_msrpm = reinterpret_cast<MSR::msrpm*>(MmAllocateContiguousMemory(sizeof(MSR::msrpm), { .QuadPart = -1 }));
	if (global.shared_msrpm == nullptr)
	{
		print("couldnt allocate msrpm\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memset(global.shared_msrpm, 0, sizeof(MSR::msrpm));
	__debugbreak();
	for (uint32_t i = 0; i < global.vcpu_count; i++)
	{
		KeSetSystemAffinityThreadEx(1ll << i);
		print("attempting to set up vcpu %d\n", KeGetCurrentProcessorIndex());

		setupvmcb(&global.vcpus[i]);
		WHATS_A_GOOD_NAME(&global.vcpus[i]);

		// this wont be executed eitherway
		//KeRevertToUserAffinityThreadEx(original_affinity);
	}

	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	print("---------\n\n");
}