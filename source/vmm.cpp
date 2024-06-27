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

void map_physical_memory();
void setup_npt(vcpu_t& cpu);
void setup_guest(vcpu_t& cpu);
void setup_host(vcpu_t& cpu, volatile bool& is_virtualized);
svm_status check_svm_support();

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
	cpu.msrpm.set(MSR::EFER::MSR_EFER, MSR::access::read);
	cpu.msrpm.set(MSR::EFER::MSR_EFER, MSR::access::write);
	
	cpu.msrpm.set(MSR::HSAVE_PA::MSR_VM_HSAVE_PA, MSR::access::read);
	cpu.msrpm.set(MSR::HSAVE_PA::MSR_VM_HSAVE_PA, MSR::access::write);
}

bool setup_cpu(uint32_t index)
{
	auto& cpu = global::vcpus[index];
	volatile bool is_virtualized{};

	// enable svm instructions
	MSR::EFER efer{};
	efer.load();
	efer.svme = 1;
	efer.store();

	print("Setting up guest\n");
	setup_guest(cpu); 
	if (is_virtualized) {
		return true;
	}

	print("Setting up host\n");
	setup_host(cpu, is_virtualized); // this is pretty pointless to pass in the boolean but compiler keeps optimizing it out otherwise..

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

	map_physical_memory();

	// Setup each processor specific structure and virtualize the processor
	if (!execute_on_all_cpus([](uint32_t index) -> bool {
		print("Virtualizing [%d]...\n", index);
		if (!setup_cpu(index))
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

void setup_guest(vcpu_t& cpu)
{
	auto& control = cpu.guest.control;
	auto& state = cpu.guest.state;

	// ------------------- Setup control area -------------------
	// Intercept all the AMD-SVM instructions and properly handle their exceptions and functionality
	control.vmmcall = 1; // explicit vmexits back to host
	control.vmrun = 1;
	control.vmload = 1;
	control.vmsave = 1;
	control.clgi = 1;
	control.stgi = 1;
	control.skinit = 1;

	control.guest_asid = 1; // Address space identifier, 0 is reserved for host.
	//cpu.guest.control.v_intr_masking = 1; // 15.21.1; Virtualize TPR and eflags.if, host eflags.if controls physical interrupts and guest eflags.if controls virtual interrupts

	/*control.msr_prot = 1;
	control.msrpm_base_pa = MmGetPhysicalAddress(&cpu.msrpm).QuadPart;
	setup_msrpm(cpu);*/

	// nested paging should not be disabled as it controls virtualization of critical guest state including control registers that handle paging related settings
	control.np_enable = 1; 
	control.n_cr3 = MmGetPhysicalAddress(&cpu.npt).QuadPart;
	setup_npt(cpu);

	// ------------------- Setup state area -------------------
	state.cr0.value = __readcr0();
	state.cr2.value = __readcr2();
	state.cr3.value = __readcr3();
	state.cr4.value = __readcr4();
	state.efer.value = __readmsr(MSR::EFER::MSR_EFER);
	state.g_pat = __readmsr(MSR::PAT::MSR_PAT); // only for nested paging

	// This is where the guest will start executing
	state.rsp = reinterpret_cast<uint64_t>(_AddressOfReturnAddress()) + 8; // 8 bytes for the return address
	state.rip = reinterpret_cast<uint64_t>(_ReturnAddress()); // Could also use _AddressOfNextInstruction in setup_cpu to insert a label and take the address of it
	state.rflags.value = __getcallerseflags();

	descriptor_table_register_t idtr{}, gdtr{}; __sidt(&idtr); _sgdt(&gdtr);
	state.idtr.base = idtr.base;
	state.idtr.limit = idtr.limit;

	state.gdtr.base = gdtr.base;
	state.gdtr.limit = gdtr.limit;

	// Setup all the segment registers
	auto& cs = state.cs;
	auto& ds = state.ds;
	auto& es = state.es;
	auto& ss = state.ss;

	cs.selector.value = __read_cs();
	ds.selector.value = __read_ds();
	es.selector.value = __read_es();
	ss.selector.value = __read_ss();

	cs.limit = __segmentlimit(cs.selector.value);
	ds.limit = __segmentlimit(ds.selector.value);
	es.limit = __segmentlimit(es.selector.value);
	ss.limit = __segmentlimit(ss.selector.value);

	cs.get_attributes(gdtr.base);
	ds.get_attributes(gdtr.base);
	es.get_attributes(gdtr.base);
	ss.get_attributes(gdtr.base);

	// pass initial values to store as shadow copies
	cpu.shadow.efer.value = state.efer.value;
	cpu.shadow.efer.svme = 0; // treated as disabled until the guest enables it
	cpu.shadow.hsave_pa.value = 0;
	
	// stack setup for vmlaunch
	cpu.guest_vmcb_pa = MmGetPhysicalAddress(&cpu.guest).QuadPart;
	cpu.host_vmcb_pa = MmGetPhysicalAddress(&cpu.host).QuadPart;
	cpu.self = &cpu;

	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.value = cpu.host_vmcb_pa;
	hsave_pa.store();

	__svm_vmsave(cpu.host_vmcb_pa);
	__svm_vmsave(cpu.guest_vmcb_pa); // needed here cause the vmrun loop loads guest state before everything, if there isnt a guest saved already it wont work properly
}

void setup_host(vcpu_t& cpu, volatile bool& is_virtualized) {

	// ------------------- Isolate the host ---------------------
	// We make changes directly to the host and the cpu is gonna implicitly store them in the host vmcb whenever we vmrun and reload them when we vmexit

	// -------

	auto& host_cr3 = cpu.host.state.cr3;
	host_cr3.value = 0;
	host_cr3.pml4 = MmGetPhysicalAddress(&global::shared_host_pt.pml4).QuadPart >> 12;

	__writecr3(host_cr3.value);

	// -------

	MSR::PAT host_pat{};

	host_pat.pa0 = MSR::PAT::attribute_type::write_back;
	host_pat.pa1 = MSR::PAT::attribute_type::write_through;
	host_pat.pa2 = MSR::PAT::attribute_type::uncacheable_no_write_combinining;
	host_pat.pa3 = MSR::PAT::attribute_type::uncacheable;

	host_pat.pa4 = MSR::PAT::attribute_type::write_back;
	host_pat.pa5 = MSR::PAT::attribute_type::write_through;
	host_pat.pa6 = MSR::PAT::attribute_type::uncacheable_no_write_combinining;
	host_pat.pa7 = MSR::PAT::attribute_type::uncacheable;

	host_pat.store();

	// -------

	is_virtualized = true;
};

void setup_npt(vcpu_t& cpu)
{
	auto& npt = cpu.npt;

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
	auto& host_pt = global::shared_host_pt;

	auto& pml4e = host_pt.pml4[host_pt.host_pml4e];
	pml4e.present = 1;
	pml4e.write = 1;
	pml4e.page_pa = MmGetPhysicalAddress(&host_pt.pdpt).QuadPart >> 12;

	for (int i = 0; i < 64; i++) {
		auto& pdpte = host_pt.pdpt[i];
		pdpte.present = 1;
		pdpte.write = 1;
		pdpte.page_pa = MmGetPhysicalAddress(&host_pt.pd[i]).QuadPart >> 12;

		for (int j = 0; j < 512; j++) {
			auto& pde = host_pt.pd[i][j];
			pde.present = 1;
			pde.write = 1;
			pde.large_page = 1;
			pde.page_pa = (i << 9) + j;
		}
	}

	global::system_process = reinterpret_cast<_EPROCESS*>(PsInitialSystemProcess);
	global::system_cr3 = { global::system_process->Pcb.DirectoryTableBase };

	auto system_pml4 = reinterpret_cast<pml4e_t*>(MmGetVirtualForPhysical({ .QuadPart = static_cast<int64_t>(global::system_cr3.pml4 << 12) }));

	// NOTE: this is a shallow copy, a deep copy would be required if an aggressive anticheat trashed the deeper levels of the page tables before a VMEXIT
	// its resource intensive both for us and the anticheat to setup a deep copy. Since they also have to do it, we can just assume were gonna be fine for now.
	memcpy(&host_pt.pml4[256], &system_pml4[256], sizeof(pml4e_t) * 256);
}

// The unload routines are temporary, the code is gonna change in the future.
void unload_single_cpu(vcpu_t& cpu)
{
	auto& state = cpu.guest.state;
	auto& control = cpu.guest.control;

	_disable();
	__svm_stgi(); // re-enable GIF since its implicitly disabled for the host on vmexit 

	// pass some context to the asm devirtualization handler, this order specifically for iret instruction which pops all 5 values from the stack
	cpu.guest_rip = control.nrip;
	cpu.cs_selector = state.cs.selector.value;
	cpu.rflags = state.rflags.value;
	cpu.guest_rsp = state.rsp;
	cpu.ss_selector = state.ss.selector.value;

	MSR::PAT pat{};
	pat.value = state.g_pat;
	pat.store();

	__writecr3(state.cr3.value);

	MSR::HSAVE_PA hsave_pa{};
	hsave_pa.store();

	auto& efer = state.efer;
	efer.svme = 0;
	efer.store();

	/*
	descriptor_table_register gdtr{};
	gdtr.base = state.gdtr.base;
	gdtr.limit = static_cast<uint16_t>(state.gdtr.limit);
	_lgdt(&gdtr);

	descriptor_table_register idtr{};
	idtr.base = state.idtr.base;
	idtr.limit = static_cast<uint16_t>(state.idtr.limit);
	__lidt(&idtr);

	segment_selector tr{};
	tr.value = state.tr.selector.value;
	(reinterpret_cast<segment_descriptor*>(gdtr.base)
		+ tr.index)->type = 0x9;
	__write_tr(tr.value);

	__write_ds(state.ds.selector.value);
	__write_es(state.es.selector.value);
	__write_fs(state.fs.selector.value);
	__write_gs(state.gs.selector.value);
	__write_ldtr(state.ldtr.selector.value);

	_writefsbase_u64(state.fs.base);
	_writegsbase_u64(state.gs.base);*/
}


void devirtualize() {
	print("Unloading Hypervisor...\n");

	execute_on_all_cpus([](uint32_t index) -> bool {
		print("Unloading [%d]...\n", index);

		__vmmcall({ .code = hypercall_code::unload, .key = hypercall_key });

		return true;
	});

	if (global::vcpus) { 
		util::free_pool(global::vcpus);
		global::vcpus = nullptr;
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

	CPUID::fn_identifiers id{};
	id.load();

	if (!id.feature_identifiers.svm)
	{
		print("SVM not supported...\n");
		return svm_status::SVM_IS_NOT_SUPPORTED_BY_CPU;
	}

	CPUID::fn_svm_features svm_rev{};
	svm_rev.load();

	if (!svm_rev.svm_feature_identification.nested_paging)
	{
		print("Nested paging not supported...\n");
		return svm_status::SVM_NESTED_PAGING_NOT_SUPPORTED;
	}

	if (!svm_rev.svm_feature_identification.n_rip) // necessary otherwise we have to emulate it which is a pain
	{
		print("Next RIP not supported...\n");
		return svm_status::SVM_NEXT_RIP_NOT_SUPPORTED;
	}

	MSR::VM_CR vm_cr{};
	vm_cr.load();

	if (!vm_cr.svmdis)
	{
		print("SVM can be enabled.\n");
		return svm_status::SVM_IS_CAPABLE_OF_BEING_ENABLED;
	}

	if (!svm_rev.svm_feature_identification.svm_lock) // && vm_cr.svmdis
	{
		// When the SVM-Lock feature is not available, hypervisors can use the read - only VM_CR.SVMDIS bit to detect SVM(see Section 15.4)
		// the user must change a platform firmware setting to enable SVM
		print("SVM lock bit not supported, disabled from BIOS...\n");
		return svm_status::SVM_DISABLED_AT_BIOS_NOT_UNLOCKABLE;
	}
	else // if (vm_cr.svmdis || vm_cr.lock)
	{
		// SVMLock may be unlockable; consult platform firmware or TPM to obtain the key
		print("SVM is locked...\n");
		return svm_status::SVM_DISABLED_WITH_KEY;
	}
}