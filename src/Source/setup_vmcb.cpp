#include <Hypervisor.h>
#include <MSRs/hsave_pa.h>
#include <MSRs/pat.h>

void Hypervisor::setup_vmcb(vcpu_t& vcpu, const CONTEXT& ctx) //should make it a reference
{
	memset(&vcpu, 0, sizeof(vcpu_t));

	vcpu.is_virtualized = true;
	vcpu.should_exit = false;

	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.bits = MmGetPhysicalAddress(&vcpu.host_vmcb).QuadPart;
	hsave_pa.store();

	vcpu.guest_vmcb.control.msrpm_base_pa = MmGetPhysicalAddress(shared_msrpm).QuadPart;

	// ------------------- Setup guest state -------------------
	vcpu.guest_vmcb.control.vmrun = 1; // VMRUN intercepts muse be enabled 15.5.1
	vcpu.guest_vmcb.control.vmmcall = 1; // explicit vmexits back to host
	vcpu.guest_vmcb.control.vmload = 1;
	vcpu.guest_vmcb.control.vmsave = 1;
	vcpu.guest_vmcb.control.clgi = 1;
	vcpu.guest_vmcb.control.msr_prot = 1;
	
	////nmi intercept bit
	//vcpu.guest_vmcb.control.nmi = 1;

	////nmi virtualization
	//vcpu.guest_vmcb.control.v_nmi_enable = 1;

	vcpu.guest_vmcb.control.guest_asid = 1; // Address space identifier "ASID [cannot be] equal to zero" 15.5.1 ASID 0 is for the host
	vcpu.guest_vmcb.control.v_intr_masking = 1; // 15.21.1 & 15.22.2
	vcpu.guest_vmcb.control.xsetbv = 1;

	//vcpu.guest_vmcb.control.cr3_read = 1;
	//vcpu.guest_vmcb.control.cr3_write = 1;

	vcpu.guest_vmcb.control.cpuid = 1;

	vcpu.guest_vmcb.control.shutdown = 1;

	if (npt) {
		vcpu.guest_vmcb.control.np_enable = 1;
		vcpu.guest_vmcb.control.n_cr3 = MmGetPhysicalAddress(npt).QuadPart;
		print("NPT: %p\n", MmGetPhysicalAddress(npt).QuadPart);
	}

	// Set up the guest state
	vcpu.guest_vmcb.save_state.cr0.value = __readcr0();
	vcpu.guest_vmcb.save_state.cr2.value = __readcr2();
	vcpu.guest_vmcb.save_state.cr3.value = __readcr3();
	vcpu.guest_vmcb.save_state.cr4.value = __readcr4();
	vcpu.guest_vmcb.save_state.efer.bits = __readmsr(MSR::EFER::MSR_EFER);
	vcpu.guest_vmcb.save_state.g_pat = __readmsr(MSR::PAT::MSR_PAT); // very sigma (kinda like MTRRs but for page tables)

	descriptor_table_register idtr{}, gdtr{}; __sidt(&idtr); _sgdt(&gdtr);
	vcpu.guest_vmcb.save_state.idtr.base = idtr.base;
	vcpu.guest_vmcb.save_state.idtr.limit = idtr.limit;

	vcpu.guest_vmcb.save_state.gdtr.base = gdtr.base;
	vcpu.guest_vmcb.save_state.gdtr.limit = gdtr.limit;

	//TODO: need to set RSP, RIP, and RFLAGS (This is where the guest will start executing)
	vcpu.guest_vmcb.save_state.rsp = ctx.Rsp;
	vcpu.guest_vmcb.save_state.rip = ctx.Rip;
	vcpu.guest_vmcb.save_state.rflags.value = ctx.EFlags;

	//Setup all the segment registers
	vcpu.guest_vmcb.save_state.cs.limit = __segmentlimit(ctx.SegCs);
	vcpu.guest_vmcb.save_state.ds.limit = __segmentlimit(ctx.SegDs);
	vcpu.guest_vmcb.save_state.es.limit = __segmentlimit(ctx.SegEs);
	vcpu.guest_vmcb.save_state.ss.limit = __segmentlimit(ctx.SegSs);

	vcpu.guest_vmcb.save_state.cs.selector.value = ctx.SegCs;
	vcpu.guest_vmcb.save_state.ds.selector.value = ctx.SegDs;
	vcpu.guest_vmcb.save_state.es.selector.value = ctx.SegEs;
	vcpu.guest_vmcb.save_state.ss.selector.value = ctx.SegSs;

	vcpu.guest_vmcb.save_state.cs.get_attributes(gdtr.base);
	vcpu.guest_vmcb.save_state.ds.get_attributes(gdtr.base);
	vcpu.guest_vmcb.save_state.es.get_attributes(gdtr.base);
	vcpu.guest_vmcb.save_state.ss.get_attributes(gdtr.base);

	vcpu.guest_vmcb_pa = MmGetPhysicalAddress(&vcpu.guest_vmcb).QuadPart;
	vcpu.host_vmcb_pa = MmGetPhysicalAddress(&vcpu.host_vmcb).QuadPart;
	vcpu.self = &vcpu;

	// ------------------- Setup host state -------------------
	auto& host_cr3 = vcpu.host_vmcb.save_state.cr3;
	host_cr3.value = 0;
	host_cr3.pml4 = MmGetPhysicalAddress(&shared_host_pt.pml4).QuadPart >> 12;

	print("Writing host cr3: %zX\n", host_cr3.value);
	__writecr3(host_cr3.value);
	__svm_vmsave(vcpu.host_vmcb_pa);


	__svm_vmsave(vcpu.guest_vmcb_pa); // needed here cause the vmrun loop loads guest state before everything, if there isnt a guest saved already it wont work properly
}