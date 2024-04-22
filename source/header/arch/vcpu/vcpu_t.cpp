#include "vmcb.h"
#include <Hypervisor.h>
#include <pages/npts.h>

bool vcpu_t::gva_to_gpa(virtual_address_t gva, uint64_t& modifiable_size, uint64_t& gpa) {
	print("gva_to_gpa\n");
	//__debugbreak();
	const auto& guest_cr3 = guest_vmcb.save_state.cr3;
	uint64_t offset{};


	pml4e_t pml4e{};
	if (!Hypervisor::readPhys((reinterpret_cast<pml4e_t*>(guest_cr3.pml4 << 12) + gva.pml4_index), pml4e)) return false;
	if (!pml4e.present) {
		return false;
	}

	pdpe_t pdpte{};
	if (!Hypervisor::readPhys((reinterpret_cast<pdpe_t*>(pml4e.page_pa << 12) + gva.pdpt_index), pdpte)) return false;
	if (!pdpte.present) {
		return false;
	}

	// 1gb
	if (pdpte.huge_page) {
		offset = (gva.pd_index << (12 + 9)) + (gva.pt_index << 12) + gva.offset;
		modifiable_size = pdpes_address_range - offset;
		gpa = (pdpte.page_pa << (12 + 9 + 9)) + offset;
		return true;
	}

	pde_t pde{};
	if (!Hypervisor::readPhys((reinterpret_cast<pde_t*>(pdpte.page_pa << 12) + gva.pd_index), pde)) return false;
	if (!pde.present) {
		return false;
	}

	// 2mb
	if (pde.large_page) {
		offset = (gva.pt_index << 12) + gva.offset;
		modifiable_size = pdes_address_range - offset;
		gpa = (pde.page_pa << (12 + 9)) + offset;
		return true;
	}

	// 4kb
	pte_t pte{};
	if (!Hypervisor::readPhys((reinterpret_cast<pte_t*>(pde.page_pa << 12) + gva.pt_index), pte)) return false;
	if (!pte.present) {
		return false;
	}

	offset = gva.offset;
	modifiable_size = ptes_address_range - offset;
	gpa = (pte.page_pa << 12) + offset;
	return true;
}