#pragma once
#include <vmm.h>
#include <ntdef/def.h>
#include <memory/translation.h>
#include <util/math.h>

/*
* get_base_address()
* - RAX = base address
*/
inline void get_base_address(register_t& reg1) {
	reg1.value = reinterpret_cast<uint64_t>(&__ImageBase);
}

/*
* get_process_cr3()
* -	RAX = process cr3
* -	R8 = process id
*/
inline void get_process_cr3(register_t& process_cr3, const register_t& process_id) {
	if (process_id.value == 4) {
		process_cr3.value = global::system_cr3.value;
		return;
	}

	// Shouldnt have issues accessing kernel memory 
	for (auto current = global::system_process;
		current->ActiveProcessLinks.Flink != &global::system_process->ActiveProcessLinks;
		current = CONTAINING_RECORD(current->ActiveProcessLinks.Flink, _EPROCESS, ActiveProcessLinks)) {

		if (process_id.value == reinterpret_cast<uint64_t>(current->UniqueProcessId)) {
			process_cr3.value = current->Pcb.DirectoryTableBase;
			return;
		}
	}

	process_cr3.value = 0; // process not found
}

/*
* get_process_base()
* -	RAX = process base address
* -	R8 = process id
*/
inline void get_process_base(register_t& process_base, const register_t& process_id) {

	if (process_id.value == 4) {
		process_base.value = reinterpret_cast<uint64_t>(global::system_process->SectionBaseAddress);
		return;
	}

	// Shouldnt have issues accessing kernel memory 
	for (auto current = global::system_process;
		current->ActiveProcessLinks.Flink != &global::system_process->ActiveProcessLinks;
		current = CONTAINING_RECORD(current->ActiveProcessLinks.Flink, _EPROCESS, ActiveProcessLinks)) {

		if (process_id.value == reinterpret_cast<uint64_t>(current->UniqueProcessId)) {
			process_base.value = reinterpret_cast<uint64_t>(current->SectionBaseAddress);
			return;
		}
	}

	process_base.value = 0; // process not found
}

/*
* get_physical_address()
* - RAX = physical address
* - R8 = process cr3
* - R9 = virtual address
*/
inline void get_physical_address(register_t& physical_addr, register_t& process_cr3, const register_t& virtual_addr) {
	if (!process_cr3.value)
		process_cr3.value = global::system_cr3.value;
	

	uint64_t offset_to_next_page{};
	uint64_t physical_address = gva_to_gpa(cr3_t{ process_cr3.value }, virtual_addr.value, offset_to_next_page);

	physical_addr.value = physical_address;
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

	cpu.flush_tlb(tlb_control_id::flush_guest_tlb);
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

	cpu.flush_tlb(tlb_control_id::flush_guest_tlb);
}

/*
* read_physical_memory()
* - RAX = bytes read
* - R8 = destination virtual address
* - R9 = source physical address
* - R10 = bytes to read
*/
inline void read_physical_memory(const cr3_t guest_cr3, register_t& bytes_read, const register_t& dest_gva, const register_t& src_pa, const register_t& size) {
	uint64_t source = src_pa.value + host_pt_t::mapping_base;

	bytes_read = 0;
	while (bytes_read < size.value) {
		uint64_t remaining{};
		auto destination = gva_to_hva(guest_cr3, dest_gva.value + bytes_read, remaining);

		if (!destination) // we dont care about delivering exceptions to our usermode app
			break;

		uint64_t bytes_to_copy = util::min(remaining, size.value - bytes_read);
		memcpy(reinterpret_cast<void*>(destination), reinterpret_cast<void*>(source + bytes_read), bytes_to_copy);

		bytes_read.value += bytes_to_copy;
	}
}

/*
* write_physical_memory()
* - RAX = bytes written
* - R8 = destination physical address
* - R9 = source virtual address
* - R10 = bytes to write
*/
inline void write_physical_memory(const cr3_t guest_cr3, register_t& bytes_written, const register_t& dest_pa, const register_t& src_gva, const register_t& size) {

	uint64_t bytes_written = 0;
	while (bytes_written < size.value) {
		uint64_t remaining{};
		auto source = gva_to_hva(guest_cr3, src_gva.value + bytes_written, remaining);

		if (!source) // we dont care about delivering exceptions to our usermode app
			break;

		uint64_t bytes_to_copy = util::min(remaining, size.value - bytes_written);
		memcpy(reinterpret_cast<void*>(dest_pa.value + bytes_written), reinterpret_cast<void*>(source), bytes_to_copy);

		bytes_written.value += bytes_to_copy;
	}
}

/*
* read_virtual_memory()
* - RAX = bytes read
* - R8 = destination virtual address
* - R9 = source virtual address
* - R10 = bytes to read
* - R11 = process cr3
*/
inline void read_virtual_memory(const cr3_t& guest_cr3, register_t& bytes_read, const register_t& dest_gva, const register_t& src_gva, const register_t& size, const register_t& process_cr3) {

	cr3_t p_cr3{ !process_cr3.value ? global::system_cr3.value : process_cr3.value };

	while (bytes_read < size.value) {
		uint64_t source_remaining{}, destination_remaining{};

		auto source = gva_to_hva(p_cr3, src_gva.value + bytes_read, source_remaining);
		if (!source)
			break;

		auto destination = gva_to_hva(guest_cr3, dest_gva.value + bytes_read, destination_remaining);
		if (!destination)
			break;

		uint64_t bytes_to_copy = util::min(util::min(source_remaining, destination_remaining), size.value - bytes_read);
		memcpy(reinterpret_cast<void*>(destination), reinterpret_cast<void*>(source), bytes_to_copy);

		bytes_read.value += bytes_to_copy;
	}
}

/*
* write_virtual_memory()
* - RAX = bytes written
* - R8 = destination virtual address
* - R9 = source virtual address
* - R10 = bytes to write
* - R11 = process cr3
*/
inline void write_virtual_memory(const cr3_t& guest_cr3, register_t& bytes_written, const register_t& dest_gva, const register_t& src_gva, const register_t& size, const register_t& process_cr3) {

	while (bytes_written < size.value) {
		uint64_t source_remaining{}, destination_remaining{};

		auto source = gva_to_hva(guest_cr3, src_gva.value + bytes_written, source_remaining);
		if (!source)
			break;

		auto destination = gva_to_hva(cr3_t{ process_cr3.value }, dest_gva.value + bytes_written, destination_remaining);
		if (!destination)
			break;

		uint64_t bytes_to_copy = util::min(util::min(source_remaining, destination_remaining), size.value - bytes_written);
		memcpy(reinterpret_cast<void*>(destination), reinterpret_cast<void*>(source), bytes_to_copy);

		bytes_written.value += bytes_to_copy;
	}
}