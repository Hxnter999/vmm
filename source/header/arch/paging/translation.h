#pragma once
#include <paging/pages.h>
#include <vcpu/vcpu.h>

// we pass guest cr3 as an arguement so we can translate from other contexts outside the current guest
inline uint64_t gva_to_gpa(cr3_t guest_cr3, uint64_t address, uint64_t& offset_to_next_page) {
	static constexpr uint64_t plm4_address_range = 0x1000000000; //256GB
	static constexpr uint64_t pdpt_address_range = 0x40000000; //1GB
	static constexpr uint64_t pd_address_range = 0x200000; //2MB
	static constexpr uint64_t pt_address_range = 0x1000; //4KB

	virtual_address_t va{ address };

	auto& base = host_pt_t::host_pa_base;

	uint64_t offset{};

	auto pml4e = reinterpret_cast<pml4e_t*>(base + (guest_cr3.pml4 << 12))[va.pml4_index];
	if (!pml4e.present) {
		return 0;
	}

	auto pdpte = reinterpret_cast<pdpte_t*>(base + (pml4e.page_pa << 12))[va.pdpt_index];
	if (!pdpte.present) {
		return 0;
	}

	// 1gb
	if (pdpte.huge_page) {
		offset = (va.pd_index << (12 + 9)) + (va.pt_index << 12) + va.offset;
		offset_to_next_page = pdpt_address_range - offset;
		return (pdpte.page_pa << (12 + 9 + 9)) + offset;
	}

	auto pde = reinterpret_cast<pde_t*>(base + (pdpte.page_pa << 12))[va.pd_index];
	if (!pde.present) {
		return 0;
	}

	// 2mb
	if (pde.large_page) {
		offset = (va.pt_index << 12) + va.offset;
		offset_to_next_page = pd_address_range - offset;
		return (pde.page_pa << (12 + 9)) + offset;
	}

	// 4kb
	auto pte = reinterpret_cast<pte_t*>(base + (pde.page_pa << 12))[va.pt_index];
	if (!pte.present) {
		return 0;
	}

	offset = va.offset;
	offset_to_next_page = pt_address_range - offset;
	return (pte.page_pa << 12) + offset;
}

inline uint64_t gva_to_hva(cr3_t guest_cr3, uint64_t address, uint64_t& offset_to_next_page) {
	uint64_t gpa = gva_to_gpa(guest_cr3, address, offset_to_next_page);
	if (!gpa)
		return 0;

	return gpa + host_pt_t::host_pa_base;
}