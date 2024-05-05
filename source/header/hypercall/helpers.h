#pragma once
#include <vmm.h>
#include <ntdef/def.h>
#include <paging/translation.h>

/*
* get_process_cr3()
* -	RAX = process cr3
* -	R8 = process pid
*/
inline void get_process_cr3(vcpu_t& cpu) {
	uint64_t target_pid = cpu.ctx.r8.value;

	if (target_pid == 4) {
		cpu.ctx.rax.value = global::system_cr3.value;
		return;
	}

	// Shouldnt have issues accessing kernel memory 
	for (auto current = global::system_process;
		current->ActiveProcessLinks.Flink != &global::system_process->ActiveProcessLinks;
		current = CONTAINING_RECORD(current->ActiveProcessLinks.Flink, _EPROCESS, ActiveProcessLinks)) {

		if (target_pid == reinterpret_cast<uint64_t>(current->UniqueProcessId)) {
			cpu.ctx.rax.value = current->Pcb.DirectoryTableBase;
			break;
		}
	}

	cpu.ctx.rax.value = 0; // process not found
}

/*
* get_physical_address()
* - RAX = physical address
* - R8 = process cr3
* - R9 = virtual address
*/
inline void get_physical_address(vcpu_t& cpu) {
	cr3_t process_cr3{ cpu.ctx.r8.value };
	uint64_t virtual_address = cpu.ctx.r9.value;

	if (process_cr3.value == 0) {
		process_cr3.value = global::system_cr3.value;
	}

	uint64_t offset_to_next_page{};
	uint64_t physical_address = gva_to_gpa(process_cr3, virtual_address, offset_to_next_page);

	cpu.ctx.rax.value = physical_address;
}

/*
* hide_physical_page()
* - RAX = status
* - R8 = page physical address
*/
inline void hide_physical_page(vcpu_t& cpu) {
	uint64_t physical_address = cpu.ctx.r8.value;

	auto pte = cpu.npts.get_pte(physical_address, true); // split the page
	if (!pte) {
		cpu.ctx.rax.value = 0;
		return;
	}

	pte->page_pa = cpu.npts.dummy_page_pa;
	cpu.ctx.rax.value = 1;

	cpu.flush_tlb(tlb_control_id::flush_guest_tlb);
}

/*
* unhide_physical_page()
* - RAX = status
* - R8 = page physical address
*/
inline void unhide_physical_page(vcpu_t& cpu) {
	uint64_t physical_address = cpu.ctx.r8.value;

	auto pte = cpu.npts.get_pte(physical_address, false); 
	if (!pte) {
		cpu.ctx.rax.value = 0;
		return;
	}

	pte->page_pa = physical_address;
	cpu.ctx.rax.value = 1;

	cpu.flush_tlb(tlb_control_id::flush_guest_tlb);
}