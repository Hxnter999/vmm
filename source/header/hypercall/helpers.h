#pragma once
#include <vmm.h>
#include <ntdef/def.h>
#include <paging/translation.h>
#include <util/math.h>

/*
* get_base_address()
* - RAX = base address
*/
inline void get_base_address(vcpu_t& cpu) {
	cpu.ctx.rax.value = reinterpret_cast<uint64_t>(&__ImageBase);
}

/*
* get_process_cr3()
* -	RAX = process cr3
* -	R8 = process id
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
			return;
		}
	}

	cpu.ctx.rax.value = 0; // process not found
}

/*
* get_process_base()
* -	RAX = process base address
* -	R8 = process id
*/
inline void get_process_base(vcpu_t& cpu) {
	uint64_t target_pid = cpu.ctx.r8.value;

	if (target_pid == 4) {
		cpu.ctx.rax.value = reinterpret_cast<uint64_t>(global::system_process->SectionBaseAddress);
		return;
	}

	// Shouldnt have issues accessing kernel memory 
	for (auto current = global::system_process;
		current->ActiveProcessLinks.Flink != &global::system_process->ActiveProcessLinks;
		current = CONTAINING_RECORD(current->ActiveProcessLinks.Flink, _EPROCESS, ActiveProcessLinks)) {

		if (target_pid == reinterpret_cast<uint64_t>(current->UniqueProcessId)) {
			cpu.ctx.rax.value = reinterpret_cast<uint64_t>(current->SectionBaseAddress);
			return;
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

	if (!process_cr3.value)
		process_cr3.value = global::system_cr3.value;
	

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
	uint64_t page_pa = cpu.ctx.r8.value;

	auto pte = cpu.npt.get_pte(page_pa, true); // split the page
	if (!pte) {
		cpu.ctx.rax.value = 0;
		return;
	}

	pte->page_pa = cpu.npt.dummy_page_pa;
	cpu.ctx.rax.value = 1;

	//cpu.flush_tlb(tlb_control_id::flush_guest_tlb);
}

/*
* unhide_physical_page()
* - RAX = status
* - R8 = page physical address
*/
inline void unhide_physical_page(vcpu_t& cpu) {
	uint64_t page_pa = cpu.ctx.r8.value;

	auto pte = cpu.npt.get_pte(page_pa, false);
	if (!pte) {
		cpu.ctx.rax.value = 0;
		return;
	}

	pte->page_pa = page_pa;
	cpu.ctx.rax.value = 1;

	//cpu.flush_tlb(tlb_control_id::flush_guest_tlb);
}

/*
* read_physical_memory()
* - RAX = bytes read
* - R8 = destination virtual address
* - R9 = source physical address
* - R10 = bytes to read
*/
inline void read_physical_memory(vcpu_t& cpu) {
	uint64_t destination_gva = cpu.ctx.r8.value;
	uint64_t source = cpu.ctx.r9.value + host_pt_t::host_pa_base;
	uint64_t size = cpu.ctx.r10.value;

	uint64_t bytes_read = 0;
	while (bytes_read < size) {
		uint64_t remaining{};
		auto destination = gva_to_hva(cpu.guest.state.cr3, destination_gva + bytes_read, remaining);

		if (!destination) // we dont care about delivering exceptions to our usermode app
			break;

		uint64_t bytes_to_copy = util::min(remaining, size - bytes_read);
		memcpy(reinterpret_cast<void*>(destination), reinterpret_cast<void*>(source + bytes_read), bytes_to_copy);

		bytes_read += bytes_to_copy;
	}
	cpu.ctx.rax.value = bytes_read;
}

/*
* write_physical_memory()
* - RAX = bytes written
* - R8 = destination physical address
* - R9 = source virtual address
* - R10 = bytes to write
*/
inline void write_physical_memory(vcpu_t& cpu) {
	uint64_t destination = cpu.ctx.r8.value + host_pt_t::host_pa_base;
	uint64_t source_gva = cpu.ctx.r9.value;
	uint64_t size = cpu.ctx.r10.value;

	uint64_t bytes_written = 0;
	while (bytes_written < size) {
		uint64_t remaining{};
		auto source = gva_to_hva(cpu.guest.state.cr3, source_gva + bytes_written, remaining);

		if (!source) // we dont care about delivering exceptions to our usermode app
			break;

		uint64_t bytes_to_copy = util::min(remaining, size - bytes_written);
		memcpy(reinterpret_cast<void*>(destination + bytes_written), reinterpret_cast<void*>(source), bytes_to_copy);

		bytes_written += bytes_to_copy;
	}
	cpu.ctx.rax.value = bytes_written;
}

/*
* read_virtual_memory()
* - RAX = bytes read
* - R8 = destination virtual address
* - R9 = source virtual address
* - R10 = bytes to read
* - R11 = process cr3
*/
inline void read_virtual_memory(vcpu_t& cpu) {
	uint64_t destination_gva = cpu.ctx.r8.value;
	uint64_t source_gva = cpu.ctx.r9.value;
	uint64_t size = cpu.ctx.r10.value;
	cr3_t process_cr3{ cpu.ctx.r11.value };

	if (!process_cr3.value)
		process_cr3.value = global::system_cr3.value;

	uint64_t bytes_read = 0;
	while (bytes_read < size) {
		uint64_t source_remaining{}, destination_remaining{};

		auto source = gva_to_hva(process_cr3, source_gva + bytes_read, source_remaining);
		if (!source)
			break;

		auto destination = gva_to_hva(cpu.guest.state.cr3, destination_gva + bytes_read, destination_remaining);
		if (!destination)
			break;

		uint64_t bytes_to_copy = util::min(util::min(source_remaining, destination_remaining), size - bytes_read);
		memcpy(reinterpret_cast<void*>(destination), reinterpret_cast<void*>(source), bytes_to_copy);

		bytes_read += bytes_to_copy;
	}
	cpu.ctx.rax.value = bytes_read;
}

/*
* write_virtual_memory()
* - RAX = bytes written
* - R8 = destination virtual address
* - R9 = source virtual address
* - R10 = bytes to write
* - R11 = process cr3
*/
inline void write_virtual_memory(vcpu_t& cpu) {
	uint64_t destination_gva = cpu.ctx.r8.value;
	uint64_t source_gva = cpu.ctx.r9.value;
	uint64_t size = cpu.ctx.r10.value;
	cr3_t process_cr3{ cpu.ctx.r11.value };

	if (!process_cr3.value)
		process_cr3.value = global::system_cr3.value;

	uint64_t bytes_written = 0;
	while (bytes_written < size) {
		uint64_t source_remaining{}, destination_remaining{};

		auto source = gva_to_hva(cpu.guest.state.cr3, source_gva + bytes_written, source_remaining);
		if (!source)
			break;

		auto destination = gva_to_hva(process_cr3, destination_gva + bytes_written, destination_remaining);
		if (!destination)
			break;

		uint64_t bytes_to_copy = util::min(util::min(source_remaining, destination_remaining), size - bytes_written);
		memcpy(reinterpret_cast<void*>(destination), reinterpret_cast<void*>(source), bytes_to_copy);

		bytes_written += bytes_to_copy;
	}
	cpu.ctx.rax.value = bytes_written;
}