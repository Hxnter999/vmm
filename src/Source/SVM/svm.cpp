#include "../../Header/commons.h"
#include "svm.h"
extern "C" void vmenter(uint64_t* guest_vmcb_pa);

bool vmexit_handler(vcpu_t* vcpu) {
	vcpu->guest_vmcb.save_state.rip = vcpu->guest_vmcb.control.nrip;

	// guest rax overwriten by host after vmexit
	vcpu->guest_stack_frame.rax.value = vcpu->guest_vmcb.save_state.rax;
	switch (vcpu->guest_vmcb.control.exit_code) {

	case svm_exit_code::VMEXIT_VMMCALL:
		hypercall_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_MSR:
		msr_handler(vcpu);
		break;

	case svm_exit_code::VMEXIT_INVALID:
		print("INVALID GUEST STATE, EXITING...\n");
		vcpu->should_exit = true;
		break;

	case svm_exit_code::VMEXIT_NPF:
		print("[NPF] Error code: %X\n", vcpu->guest_vmcb.control.exit_info_1.page_fault.error_code);
		print("[NPF] Address: %p\n", vcpu->guest_vmcb.control.exit_info_2.page_fault.faulting_address);

	default:
		// event inject a gp/ud
		print("Unhandled VMEXIT: %d\n", vcpu->guest_vmcb.control.exit_code);
		break;
	}
	// the cpu handles guest rax for us
	vcpu->guest_vmcb.save_state.rax = vcpu->guest_stack_frame.rax.value;

	//true to continue
	//false to devirt
	if (vcpu->should_exit) {
		devirtualize(vcpu); // devirtualize current vcpu and alert all others
		return false;
	};

	return true;
}

void setup_vmcb(vcpu_t* vcpu, CONTEXT* ctx) //dis just a test
{
	vcpu->is_virtualized = true;

	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();

	vcpu->guest_vmcb.control.msrpm_base_pa = MmGetPhysicalAddress(global.shared_msrpm).QuadPart;
	
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
	vcpu->guest_vmcb.control.n_cr3 = MmGetPhysicalAddress(global.npt).QuadPart;
	
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

	__svm_vmsave(vcpu->guest_vmcb_pa);

	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.bits = MmGetPhysicalAddress(&vcpu->host_state_area).QuadPart;
	hsave_pa.store();

	//__svm_vmsave(MmGetPhysicalAddress(&vcpu->host_vmcb).QuadPart); // idk about this
}

bool virtualize(vcpu_t* vcpu) {
	CONTEXT* ctx = reinterpret_cast<CONTEXT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(CONTEXT), 'sgma'));
	memset(ctx, 0, sizeof(CONTEXT));
	RtlCaptureContext(ctx);

	if (global.current_vcpu->is_virtualized) {
		//__debugbreak();
		return true;
	}
	setup_vmcb(vcpu, ctx);
	vmenter(&vcpu->guest_vmcb_pa);
	return false;
}

void devirtualize(vcpu_t* vcpu) {

	for (uint32_t i = 0; i < global.vcpu_count; i++) // alert all other vcpus
	{
		global.vcpus[i].should_exit = true;
		if (&global.vcpus[i] == vcpu) print("Exiting [%d]...\n", i);
	}

	// devirtualize current vcpu
	// rcx -> nrip
	// rbx -> rsp
	vcpu->guest_stack_frame.rcx.value = vcpu->guest_vmcb.control.nrip;
	vcpu->guest_stack_frame.rbx.value = vcpu->guest_vmcb.save_state.rsp;

	__svm_vmload(vcpu->guest_vmcb_pa);

	_disable();
	__svm_stgi();

	MSR::EFER efer{}; efer.load(); efer.svme = 0; efer.store();
	__writeeflags(vcpu->guest_vmcb.save_state.rflags);
}

bool setup_msrpm() {
	using namespace MSR;

	global.shared_msrpm = reinterpret_cast<msrpm_t*>(MmAllocateContiguousMemory(sizeof(msrpm_t), { .QuadPart = -1 }));
	if (global.shared_msrpm == nullptr)
		return false;

	memset(global.shared_msrpm, 0, sizeof(msrpm_t));

	// msrpm->set(msr, bit, value = true)
	// bit is either 0 (read) or 1 (write)

	global.shared_msrpm->set(EFER::MSR_EFER, access::read);
	global.shared_msrpm->set(EFER::MSR_EFER, access::write);

	global.shared_msrpm->set(HSAVE_PA::MSR_VM_HSAVE_PA, access::read);
	global.shared_msrpm->set(HSAVE_PA::MSR_VM_HSAVE_PA, access::write);
	return true;
}

SVM_STATUS initialize() {
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