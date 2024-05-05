#include <vmm.h>
#include <cpuid/fn_identifiers.h>
#include <cpuid/fn_processor_capacity.h>
#include <cpuid/fn_svm_features.h>
#include <cpuid/fn_vendor.h>
#include <msrs/efer.h>
#include <msrs/vm_cr.h>
#include <msrs/hsave.h>
#include <msrs/pat.h>
#include <util/memory.h>
#include <hypercall/hypercall.h>
#include <ntdef/def.h>

extern "C" void __vmlaunch(uint64_t* guest_vmcb_pa);

using per_cpu_callback_t = bool(*)(uint32_t);
inline bool execute_on_all_cpus(per_cpu_callback_t callback)
{
	for (uint32_t i = 0; i < global::vcpu_count; i++)
	{
		auto original_affinity = KeSetSystemAffinityThreadEx(1ll << i);
		bool result = callback(i);
		KeRevertToUserAffinityThreadEx(original_affinity);
		if (!result) return false;
	}
	return true;
}

void setup_msrpm(vcpu_t& cpu) {
	memset(&cpu.msrpm, 0, sizeof(cpu.msrpm));
	cpu.msrpm.set(MSR::EFER::MSR_EFER, MSR::access::read);
	cpu.msrpm.set(MSR::EFER::MSR_EFER, MSR::access::write);
	
	//cpu.msrpm.set(MSR::HSAVE_PA::MSR_VM_HSAVE_PA, MSR::access::read);
	//cpu.msrpm.set(MSR::HSAVE_PA::MSR_VM_HSAVE_PA, MSR::access::write);
}

void map_physical_memory();
void setup_npt(vcpu_t& cpu);
void setup_guest(vcpu_t& cpu, CONTEXT& ctx);
void setup_host(vcpu_t& cpu);
svm_status check_svm_support();

bool setup_vcpu(uint32_t index)
{
	vcpu_t& cpu = global::vcpus[index];

	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();

	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.bits = MmGetPhysicalAddress(&cpu.host).QuadPart;
	hsave_pa.store();
 
	CONTEXT& ctx = *reinterpret_cast<CONTEXT*>(cpu.host_stack); // use the currently unused space we have already allocated
	RtlCaptureContext(&ctx);

	print("Checking efer\n");
	// efer.svme will be 0 when we read it in a virtualized state, this is because the host hides svme from the guest
	// this can be cleaned up and optimized by doing it thru assembling and passing the address of a label to RIP that returns true, but this is fine for now
	MSR::EFER guest_efer{}; guest_efer.load();
	if (!guest_efer.svme) {
		//__debugbreak();
		return true;
	}

	print("Setting up guest\n");
	setup_guest(cpu, ctx);

	print("Setting up host\n");
	setup_host(cpu);

	memset(&cpu.host_stack, 0, sizeof(CONTEXT));
	print("Entering vm\n");
	__vmlaunch(&cpu.guest_vmcb_pa);

	// shouldnt reach this point, if so something went wrong
	return false;
}

bool virtualize() {
	using namespace global;

	print("Initializing Hypervisor...\n");

	// Allocate the necessary memory for the required structures
	vcpu_count = { KeQueryActiveProcessorCount(nullptr) };
	vcpus = util::allocate_pool<vcpu_t>(NonPagedPoolNx, vcpu_count * sizeof(vcpu_t));
	if (vcpus == nullptr) {
		print("Failed to allocate vcpus\n");
		return false;
	}

	shared_host_pt = util::allocate_pool<host_pt_t>();
	if (shared_host_pt == nullptr) {
		print("Failed to allocate host page tables\n");
		return false;
	}

	// Setup each processor specific structure and virtualize the processor
	if (!execute_on_all_cpus([](uint32_t index) -> bool {
		print("Virtualizing [%d]...\n", index);
		if (!setup_vcpu(index))
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

void setup_guest(vcpu_t& cpu, CONTEXT& ctx)
{
	// ------------------- Setup control area -------------------
	// Intercept all the AMD-SVM instructions and properly handle their exceptions cause the efer.svme is hidden from the guest
	cpu.guest.control.vmmcall = true; // explicit vmexits back to host
	cpu.guest.control.vmrun = true;
	cpu.guest.control.vmload = true;
	cpu.guest.control.vmsave = true;
	cpu.guest.control.clgi = true;
	cpu.guest.control.stgi = true;
	cpu.guest.control.skinit = true;
	//cpu.guest.control.cpuid = true;

	cpu.guest.control.guest_asid = 1; // Address space identifier, 0 is reserved for host.
	cpu.guest.control.v_intr_masking = true; // 15.21.1; Virtualize TPR and eflags.if, host eflags.if controls physical interrupts and guest eflags.if controls virtual interrupts

	/* if ur considering disabling this, change the code in the virtualize routine which uses msr intercepts to check 
	if were currently running in guest mode and exits the function to avoid virtualization loop */
	cpu.guest.control.msr_prot = true;
	cpu.guest.control.msrpm_base_pa = MmGetPhysicalAddress(&cpu.msrpm).QuadPart;
	setup_msrpm(cpu);

	// nested paging should not be disabled as it controls virtualization of critical guest state including control registers
	cpu.guest.control.np_enable = true; 
	cpu.guest.control.n_cr3 = MmGetPhysicalAddress(&cpu.npts).QuadPart;
	setup_npt(cpu);


	// ------------------- Setup state area -------------------
	cpu.guest.state.cr0.value = __readcr0();
	cpu.guest.state.cr2.value = __readcr2();
	cpu.guest.state.cr3.value = __readcr3();
	cpu.guest.state.cr4.value = __readcr4();
	cpu.guest.state.efer.bits = __readmsr(MSR::EFER::MSR_EFER);
	cpu.guest.state.g_pat = __readmsr(MSR::PAT::MSR_PAT); // only for nested paging

	descriptor_table_register idtr{}, gdtr{}; __sidt(&idtr); _sgdt(&gdtr);
	cpu.guest.state.idtr.base = idtr.base;
	cpu.guest.state.idtr.limit = idtr.limit;

	cpu.guest.state.gdtr.base = gdtr.base;
	cpu.guest.state.gdtr.limit = gdtr.limit;

	// This is where the guest will start executing
	cpu.guest.state.rsp = ctx.Rsp;
	cpu.guest.state.rip = ctx.Rip;
	cpu.guest.state.rflags.value = ctx.EFlags;

	// Setup all the segment registers
	cpu.guest.state.cs.limit = __segmentlimit(ctx.SegCs);
	cpu.guest.state.ds.limit = __segmentlimit(ctx.SegDs);
	cpu.guest.state.es.limit = __segmentlimit(ctx.SegEs);
	cpu.guest.state.ss.limit = __segmentlimit(ctx.SegSs);

	cpu.guest.state.cs.selector.value = ctx.SegCs;
	cpu.guest.state.ds.selector.value = ctx.SegDs;
	cpu.guest.state.es.selector.value = ctx.SegEs;
	cpu.guest.state.ss.selector.value = ctx.SegSs;

	cpu.guest.state.cs.get_attributes(gdtr.base);
	cpu.guest.state.ds.get_attributes(gdtr.base);
	cpu.guest.state.es.get_attributes(gdtr.base);
	cpu.guest.state.ss.get_attributes(gdtr.base);

	cpu.guest_vmcb_pa = MmGetPhysicalAddress(&cpu.guest).QuadPart;
	cpu.host_vmcb_pa = MmGetPhysicalAddress(&cpu.host).QuadPart;
	cpu.self = &cpu;

	__svm_vmsave(cpu.host_vmcb_pa);
	__svm_vmsave(cpu.guest_vmcb_pa); // needed here cause the vmrun loop loads guest state before everything, if there isnt a guest saved already it wont work properly
}

void setup_host(vcpu_t& cpu) {

	// ------------------- Isolate the host ---------------------
	// We make changes directly to the host and the cpu is gonna implicitly store them in the host vmcb whenever we vmrun and reload them when we vmexit

	// -------

	map_physical_memory();
	auto& host_cr3 = cpu.host.state.cr3;
	host_cr3.value = 0;
	host_cr3.pml4 = MmGetPhysicalAddress(&global::shared_host_pt->pml4).QuadPart >> 12;

	__writecr3(host_cr3.value);

	// -------

	MSR::PAT host_pat{};
	host_pat.load();

	host_pat.pa0 = MSR::PAT::page_attribute_type::write_back;
	host_pat.pa1 = MSR::PAT::page_attribute_type::write_through;
	host_pat.pa2 = MSR::PAT::page_attribute_type::uncacheable_no_write_combinining;
	host_pat.pa3 = MSR::PAT::page_attribute_type::uncacheable;

	host_pat.pa4 = MSR::PAT::page_attribute_type::write_back;
	host_pat.pa5 = MSR::PAT::page_attribute_type::write_through;
	host_pat.pa6 = MSR::PAT::page_attribute_type::uncacheable_no_write_combinining;
	host_pat.pa7 = MSR::PAT::page_attribute_type::uncacheable;

	host_pat.store();

	// -------
};

void setup_npt(vcpu_t& cpu)
{
	auto& npt = cpu.npts;

	for (size_t i = 0; i < npt.free_page_count; i++) {
		npt.free_page_pa[i] = MmGetPhysicalAddress(npt.free_pages[i]).QuadPart >> 12;
	}

	npt.dummy_page_pa = MmGetPhysicalAddress(npt.dummy).QuadPart >> 12;

	// TODO: read the mtrr later and set page attributes accordingly
	auto& pml4e = npt.pml4[0];
	pml4e.present = 1;
	pml4e.write = 1;
	pml4e.usermode = 1;
	pml4e.page_pa = MmGetPhysicalAddress(&npt.pdpt).QuadPart >> 12;

	for (int i = 0; i < 64; i++) {
		auto& pdpte = npt.pdpt[i];
		pdpte.present = 1;
		pdpte.write = 1;
		pdpte.usermode = 1;
		pdpte.page_pa = MmGetPhysicalAddress(&npt.pd[i]).QuadPart >> 12;

		for (int j = 0; j < 512; j++) {
			auto& pde = npt.pd[i][j];
			pde.present = 1;
			pde.write = 1;
			pde.usermode = 1;
			pde.large_page = 1;
			pde.page_pa = (i << 9) + j;
		}
	}
}

void map_physical_memory() {
	print("Setting up host page tables\n");

	// map all the physical memory and share it between the hosts to be able to access physical memory directly.
	auto& pml4e = global::shared_host_pt->pml4[global::shared_host_pt->host_pml4e];
	pml4e.present = 1;
	pml4e.write = 1;
	pml4e.page_pa = MmGetPhysicalAddress(&global::shared_host_pt->pdpt).QuadPart >> 12;

	for (int i = 0; i < 64; i++) {
		auto& pdpte = global::shared_host_pt->pdpt[i];
		pdpte.present = 1;
		pdpte.write = 1;
		pdpte.page_pa = MmGetPhysicalAddress(&global::shared_host_pt->pd[i]).QuadPart >> 12;

		for (int j = 0; j < 512; j++) {
			auto& pde = global::shared_host_pt->pd[i][j];
			pde.present = 1;
			pde.write = 1;
			pde.large_page = 1;
			pde.page_pa = (i << 9) + j;
		}
	}

	global::system_process = reinterpret_cast<_EPROCESS*>(PsInitialSystemProcess);
	global::system_cr3 = { global::system_process->Pcb.DirectoryTableBase};

	auto system_pml4 = reinterpret_cast<pml4e_t*>(MmGetVirtualForPhysical({ .QuadPart = static_cast<int64_t>(global::system_cr3.pml4 << 12) }));

	// NOTE: this is a shallow copy, a deep copy would be required if an aggressive anticheat trashed the deeper levels of the page tables before a VMEXIT
	// its resource intensive both for us and the anticheat to setup a deep copy. Since they also have to do it, we can just assume were gonna be fine for now.
	memcpy(&global::shared_host_pt->pml4[256], &system_pml4[256], sizeof(pml4e_t) * 256);
}

// The unload routines are temporary, the code is gonna change in the future.
void unload_single_vcpu(vcpu_t& cpu) 
{
	print("Exiting...\n");

	// devirtualize current cpu, later in the vmrun loop we restore rsp and jump to guest_rip.
	cpu.guest_rip = cpu.guest.control.nrip;
	cpu.guest_rsp = cpu.guest.state.rsp;

	__svm_vmload(cpu.guest_vmcb_pa);

	_disable();
	__svm_stgi(); // re-enable GIF since its implicitly disabled for the host on vmexit

	MSR::EFER efer{}; efer.load(); efer.svme = 0; efer.store();
	__writeeflags(cpu.guest.state.rflags.value);
}

void devirtualize() {
	print("Unloading Hypervisor...\n");

	execute_on_all_cpus([](uint32_t index) -> bool {
		print("Devirtualizing [%d]...\n", index);
		__vmmcall({ .code = hypercall_code::unload, .key = hypercall_key });
		return true;
		});

	if (global::vcpus) { 
		util::free_pool(global::vcpus);
		global::vcpus = nullptr;
	}
	if (global::shared_host_pt) {
		util::free_pool(global::shared_host_pt);
		global::shared_host_pt = nullptr;
	}
}

svm_status check_svm_support()
{
	CPUID::fn_vendor vendor_check{};
	vendor_check.load();

	if (!vendor_check.is_amd_vendor())
	{
		print("Vendor check failed...\n");
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
		print("SVM not enabled but can be enabled\n");
		return svm_status::SVM_IS_CAPABLE_OF_BEING_ENABLE;
	}

	if (!svm_rev.svm_feature_identification.svm_lock)
	{
		print("SVM lock bit not set, disabled by BIOS...\n");
		return svm_status::SVM_DISABLED_AT_BIOS_NOT_UNLOCKABLE;
	}

	print("SVM lock bit set, disabled\n");
	return svm_status::SVM_DISABLED_WITH_KEY;
}