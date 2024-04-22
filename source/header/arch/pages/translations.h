#pragma once
#include <pages/pages.h>
#include <vcpu/vmcb.h>

static constexpr uint64_t plm4e_address_range = 0x1000000000; //256GB
static constexpr uint64_t pdpes_address_range = 0x40000000; //1GB
static constexpr uint64_t pdes_address_range = 0x200000; //2MB
static constexpr uint64_t ptes_address_range = 0x1000; //4KB

inline uint64_t gva_to_gpa(vcpu_t& vcpu, uint64_t address, uint64_t& modifiable_size) {
	virtual_address_t va{ address };

	auto& guest_cr3 = vcpu.guest_vmcb.state.cr3;
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
		modifiable_size = pdpes_address_range - offset;
		return (pdpte.page_pa << (12 + 9 + 9)) + offset;
	}

	auto pde = reinterpret_cast<pde_t*>(base + (pdpte.page_pa << 12))[va.pd_index];
	if (!pde.present) {
		return 0;
	}

	// 2mb
	if (pde.large_page) {
		offset = (va.pt_index << 12) + va.offset;
		modifiable_size = pdes_address_range - offset;
		return (pde.page_pa << (12 + 9)) + offset;
	}

	// 4kb
	auto pte = reinterpret_cast<pte_t*>(base + (pde.page_pa << 12))[va.pt_index];
	if (!pte.present) {
		return 0;
	}

	offset = va.offset;
	modifiable_size = ptes_address_range - offset;
	return (pte.page_pa << 12) + offset;
}

inline uint64_t gva_to_hva(vcpu_t& vcpu, uint64_t address, uint64_t& modifiable_size) {
	uint64_t gpa = gva_to_gpa(vcpu, address, modifiable_size);
	if (!gpa) 
		return 0;
	

	return gpa + host_pt_t::host_pa_base;
}