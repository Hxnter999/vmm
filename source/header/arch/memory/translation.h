#pragma once
#include <memory/page_tables.h>
#include <vcpu/vcpu.h>

// we pass guest cr3 as an arguement so we can translate from other address spaces outside the current guest
inline uint64_t gva_to_gpa(cr3_t guest_cr3, uint64_t address, uint64_t& offset_to_next_page) {
	static constexpr uint64_t plm4_address_range = 0x80'0000'0000; //512GB
	static constexpr uint64_t pdpt_address_range = 0x4000'0000; //1GB
	static constexpr uint64_t pd_address_range = 0x20'0000; //2MB
	static constexpr uint64_t pt_address_range = 0x1'000; //4KB

	virtual_address_t va{ address };
	auto base = host_pt_t::mapping_base;
	uint64_t offset{};

	auto pml4e = reinterpret_cast<pml4e_t*>(base + (static_cast<uint64_t>(guest_cr3.pml4) << 12))[va.pml4_index];
	if (!pml4e.present) {
		return 0;
	}

	auto pdpte_1gb = reinterpret_cast<pdpte_1gb_t*>(base + (static_cast<uint64_t>(pml4e.page_pa) << 12))[va.pdpt_index];
	pdpte_t pdpte{ .value = pdpte_1gb.value };

	if (!pdpte_1gb.present) {
		return 0;
	}

	// 1gb
	if (pdpte_1gb.large_page) {
		offset = (static_cast<uint64_t>(va.pd_index) << 21) + (static_cast<uint64_t>(va.pt_index) << 12) + va.offset;
		offset_to_next_page = pdpt_address_range - offset;
		return (static_cast<uint64_t>(pdpte_1gb.page_pa) << 30) + offset;
	}

	auto pde_2mb = reinterpret_cast<pde_2mb_t*>(base + (static_cast<uint64_t>(pdpte.page_pa) << 12))[va.pd_index];
	pde_t pde{ .value = pde_2mb.value };

	if (!pde_2mb.present) {
		return 0;
	}

	// 2mb
	if (pde_2mb.large_page) {
		offset = (static_cast<uint64_t>(va.pt_index) << 12) + va.offset;
		offset_to_next_page = pd_address_range - offset;
		return (static_cast<uint64_t>(pde_2mb.page_pa) << 21) + offset;
	}

	// 4kb
	auto pte = reinterpret_cast<pte_t*>(base + (static_cast<uint64_t>(pde.page_pa) << 12))[va.pt_index];
	if (!pte.present) {
		return 0;
	}

	offset = va.offset;
	offset_to_next_page = pt_address_range - offset;
	return (static_cast<uint64_t>(pte.page_pa) << 12) + offset;
}

inline uint64_t gva_to_hva(cr3_t guest_cr3, uint64_t address, uint64_t& offset_to_next_page) {
	uint64_t gpa = gva_to_gpa(guest_cr3, address, offset_to_next_page);
	if (!gpa)
		return 0;

	return gpa + host_pt_t::mapping_base;
}