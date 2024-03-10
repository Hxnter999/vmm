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
#include "../Header/Util/bitset.h"
extern "C" {
	extern void _sgdt(void* gdtr); // here for now
	extern void testing_vmrun(uint64_t vmcb_pa);
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

// REQUIREST FOR THE CONSISTENCY CHECK I TESTED MOVE TO A DIFFERENT HEADER LATER
namespace cr {
	struct cr0 {
		union {
			uint64_t value;
			struct {
				uint64_t pe : 1;            // pe: protection enabled	 r/w
				uint64_t mp : 1;            // mp: monitor coprocessor   r/w
				uint64_t em : 1;            // em: emulation             r/w
				uint64_t ts : 1;            // ts: task switched         r/w
				uint64_t et : 1;            // et: extension type        r
				uint64_t ne : 1;            // ne: numeric error         r/w
				uint64_t reserved6 : 10;    // reserved		             n/a
				uint64_t wp : 1;            // wp: write protect         r/w
				uint64_t reserved17 : 1;    // reserved					 n/a
				uint64_t am : 1;            // am: alignment mask        r/w
				uint64_t reserved19 : 10;   // reserved					 n/a
				uint64_t nw : 1;            // nw: not write-through     r/w
				uint64_t cd : 1;            // cd: cache disable         r/w
				uint64_t pg : 1;            // pg: paging                r/w
				uint64_t reserved32 : 32;   // reserved					 n/a
			};
		};
	};
	struct cr2 {
		union { // just cause i wanna follow the same format
			uint64_t value;
			struct {
				uint64_t pfl : 64;			// pfl: page-fault linear address
			};
		};
	};
	struct cr3 {
		union {
			uint64_t value;
			struct {
				/* depends on cr4.pcide*/
				/* cr4.pcide=0 : */
				uint64_t reserved0 : 3;     // reserved
				uint64_t pwt : 1;           // pwt: page-level write-through
				uint64_t pcd : 1;           // pcd: page-level cache disable
				uint64_t reserved5 : 7;     // reserved
				/* --- */
				/* cr4.pcide=1 : */
				//uint64_t pci : 12;          // pci: process-context identifier
				/* --- */
				uint64_t pml4 : 40;         // pml4: page-map level-4 base address
				uint64_t reserved52 : 12;   // reserved
			};
		};
	};
	struct cr4 {
		union {
			uint64_t value;
			struct {
				uint64_t vme : 1;           // vme: bit 0. setting vme to 1 enables hardware-supported  performance enhancements for software running in virtual - 8086 mode.clearing vme to 0 disables this support.the enhancements enabled when vme = 1 are referanced in the amd manual.
				uint64_t pvi : 1;           // pvi: protected-mode virtual interrupts (pvi). bit 1. setting pvi to 1 enables support for protectedmode virtual interrupts. clearing pvi to 0 disables this support. when pvi=1, hardware support of two bits in the rflags register, vif and vip, is enabled. only the sti and cli instructions are affected by enabling pvi. unlike the case when cr0.vme=1,  the interrupt - redirection bitmap in the tss cannot be used for selective intn interception
				uint64_t tsd : 1;           // tsd: time-stamp disable (tsd). bit 2. the tsd bit allows software to control the privilege level at  which the time - stamp counter can be read.when tsd is cleared to 0, software running at any privilege level can read the time - stamp counter using the rdtsc or rdtscp instructions.when tsd is set to 1, only software running at privilege - level 0 can execute the rdtsc or rdtscp instructions
				uint64_t de : 1;            // de: debugging extensions (de). bit 3. setting the de bit to 1 enables the i/o breakpoint capability and  enforces treatment of the dr4 and dr5 registers as reserved.software that accesses dr4 or dr5 when de = 1 causes a invalid opcode exception(#ud)
				uint64_t pse : 1;           // pse: the pse bit is ignored when the processor is running in long mode; page-size extensions (pse). bit 4. setting pse to 1 enables the use of 4-mbyte physical pages.  with pse = 1, the physical - page size is selected between 4 kbytes and 4 mbytes using the pagedirectory entry page - size field(ps).clearing pse to 0 disables the use of 4 - mbyte physical pages and restricts all physical pages to 4 kbytes
				uint64_t pae : 1;           // pae: long mode requires pae to be enabled;  physical - address extension(pae).bit 5. setting pae to 1 enables the use of physical - address extensions and 2 - mbyte physical pages.clearing pae to 0 disables these features.
				uint64_t mce : 1;           // mce: bit 6. setting mce to 1 enables the machine-check exception mechanism. clearing this bit to 0 disables the mechanism.when enabled, a machine - check exception (#mc) occurs when an uncorrectable machine - check error is encountered.
				uint64_t pge : 1;           // pge: bit 7. when page translation is enabled, system-software performance  can often be improved by making some page translations global to all tasks and procedures.setting pge to 1 enables the global - page mechanism.clearing this bit to 0 disables the mechanism.
				uint64_t pce : 1;           // pce: bit 8. setting pce to 1 allows software running  at any privilege level to use the rdpmc instruction.software uses the rdpmc instruction to read the performance - monitoring counter msrs, * perfctrn.clearing pce to 0 allows only the most - privileged software(cpl = 0) to use the rdpmc instruction
				uint64_t osfxsr : 1;        // osfxsr: operating system fxsave/fxrstor support 
				uint64_t osxmmexcpt : 1;    // osxmmexcpt: operating system unmasked exception support 
				uint64_t umip : 1;          // umip: user mode instruction prevention 
				uint64_t la57 : 1;          // la57: 5-level paging enable 
				uint64_t reserved13 : 3;    // reserved
				uint64_t fsgsbase : 1;      // fsgsbase: enable rdfsbase, rdgsbase, wrfsbase, and wrgsbase instructions
				uint64_t pcide : 1;         // pcide: process context identifier enable 
				uint64_t osxsave : 1;       // osxsave: xsave and processor extended states enable bit
				uint64_t reserved19 : 1;    // reserved
				uint64_t smep : 1;          // smep: supervisor mode execution prevention
				uint64_t smap : 1;          // smap: supervisor mode access protection 
				uint64_t pke : 1;           // pke: bit 22. enable support for memory protection keys. also enables  support for the rdpkru and wrpkru instructions.a mov to cr4 that changes cr4.pke from 0 to 1 causes all cached entries in the tlb for the logical processor to be invalidated.
				uint64_t cet : 1;           // cet: control-flow enforcement technology
				uint64_t reserved24 : 40;   // reserved
			};
		};
	};
	struct cr8 {
		union {
			uint64_t value;
			struct {
				uint64_t tpr : 4;           // tpr: task-priority register
				uint64_t reserved4 : 60;    // reserved
			};
		};
	};
}

bool isvirt = false;
void setupvmcb(vcpu* vcpu) //dis just a test
{
	CONTEXT* ctx = reinterpret_cast<CONTEXT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(CONTEXT), 'sgma'));
	memset(ctx, 0, sizeof(CONTEXT));
	RtlCaptureContext(ctx);

	if (isvirt) {
		print("already virtualized\n");
		return;
	}
	isvirt = true;

	print("Starting to virtualize...\n");
	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();

	static sharedvcpu sharedVcpu{};
	if (sharedVcpu.shared_msrpm == nullptr)
	{
		PHYSICAL_ADDRESS high{ .QuadPart = -1 };
		sharedVcpu.shared_msrpm = reinterpret_cast<MSR::msrpm*>(MmAllocateContiguousMemory(sizeof(MSR::msrpm), high));
		if (sharedVcpu.shared_msrpm == nullptr)
		{
			print("couldnt allocate msrpm\n");
			return;
		}
		memset(sharedVcpu.shared_msrpm, 0, sizeof(MSR::msrpm));
	}
	//if (sharedVcpu.pt == nullptr)
	//{
	//	sharedVcpu.pt = reinterpret_cast<PT*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(PT), 'sgma'));
	//	if (sharedVcpu.pt == nullptr)
	//	{
	//		print("couldnt allocate page table\n");
	//		return;
	//	}
	//	memset(sharedVcpu.pt, 0, sizeof(PT));
	//	setup_npt(&sharedVcpu);
	//}
	
	vcpu->guest_vmcb.control.msrpm_base_pa = MmGetPhysicalAddress(sharedVcpu.shared_msrpm).QuadPart;


	//Set up control area
	//TODO: set interupts blah blah
	vcpu->guest_vmcb.control.vmrun = 1; // VMRUN intercepts muse be enabled 15.5.1
	vcpu->guest_vmcb.control.vmmcall = 1; // UM call VM

	vcpu->guest_vmcb.control.guest_asid = 1; // Address space identifier "ASID [cannot be] equal to zero" 15.5.1 ASID 0 is for the host

	// Set up the guest state
	vcpu->guest_vmcb.save_state.cr0 = __readcr0();
	vcpu->guest_vmcb.save_state.cr2 = __readcr2();
	vcpu->guest_vmcb.save_state.cr3 = __readcr3();
	vcpu->guest_vmcb.save_state.cr4 = __readcr4();
	vcpu->guest_vmcb.save_state.efer = __readmsr(MSR::EFER::MSR_EFER);
	vcpu->guest_vmcb.save_state.g_pat = __readmsr(MSR::PAT::MSR_PAT); // very sigma (kinda like MTRRs but for page tables)

	dtr idtr{}; __sidt(&idtr);
	vcpu->guest_vmcb.save_state.idtr.base = idtr.base;
	vcpu->guest_vmcb.save_state.idtr.limit = idtr.limit;
	dtr gdtr{}; _sgdt(&gdtr);
	vcpu->guest_vmcb.save_state.gdtr.base = gdtr.base;
	vcpu->guest_vmcb.save_state.gdtr.limit = gdtr.limit;

	//TODO: need to set RSP, RIP, and RFLAGS (This is where the guest will start executing)
	vcpu->guest_vmcb.save_state.rsp = ctx->Rsp;
	vcpu->guest_vmcb.save_state.rip = ctx->Rip;
	vcpu->guest_vmcb.save_state.rflags = ctx->EFlags;

	//vcpu->guest_vmcb.save_state.rax = ctx->Rax; // necessary?

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


	//__svm_vmsave(MmGetPhysicalAddress(&vcpu->guest_vmcb).QuadPart);

	//
	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.bits = MmGetPhysicalAddress(&vcpu->host_state_area).QuadPart;
	hsave_pa.store();

	//__svm_vmsave(MmGetPhysicalAddress(&vcpu->host_vmcb).QuadPart);

	__debugbreak();
	uint64_t test = MmGetPhysicalAddress(&vcpu->guest_vmcb).QuadPart;
	//testing_vmrun(test);
	__svm_vmrun(test);
	
	print("[1] VMEXIT\n");
	print("[1] ExitCode: %llx\n", vcpu->guest_vmcb.control.exit_code);
	print("[1] ExitIntInfo: %llx\n", vcpu->guest_vmcb.control.exit_int_info.bits);
	print("[1] ExitInfo1: %llx\n", vcpu->guest_vmcb.control.exit_info_1.info);
	print("[1] ExitInfo2: %llx\n", vcpu->guest_vmcb.control.exit_info_2.info);
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
	auto vcpus = reinterpret_cast<vcpu*>(ExAllocatePoolWithTag(NonPagedPool, vcpu_count * sizeof(vcpu), 'sgma')); //FREE THIS LATER
	memset(vcpus, 0, vcpu_count * sizeof(vcpu));

	for (uint32_t i = 0; i < vcpu_count; i++) 
	{
		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);
		print("attempting to set up vcpu %d\n", KeGetCurrentProcessorIndex());
		setupvmcb(&vcpus[i]);
		print("vcpu %d set up\n", KeGetCurrentProcessorIndex());
		KeRevertToUserAffinityThreadEx(original_affinity);
	}

	print("virtualized hopefully\n");

	return STATUS_SUCCESS;
}


void Unload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	print("---------\n\n");
}