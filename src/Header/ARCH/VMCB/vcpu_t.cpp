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

register_t& vcpu_t::convert(ZydisRegister reg) {
	switch (reg) {
	case ZYDIS_REGISTER_RAX: return 	guest_stack_frame.rax;
	case ZYDIS_REGISTER_RCX: return 	guest_stack_frame.rcx;
	case ZYDIS_REGISTER_RDX: return 	guest_stack_frame.rdx;
	case ZYDIS_REGISTER_RBX: return 	guest_stack_frame.rbx;
	case ZYDIS_REGISTER_RSP: return 	guest_vmcb.save_state.rsp;
	//case ZYDIS_REGISTER_RBP: return 	guest_stack_frame.rbp;
	case ZYDIS_REGISTER_RSI: return 	guest_stack_frame.rsi;
	case ZYDIS_REGISTER_RDI: return 	guest_stack_frame.rdi;
	case ZYDIS_REGISTER_R8: return 	guest_stack_frame.r8;
	case ZYDIS_REGISTER_R9: return 	guest_stack_frame.r9;
	case ZYDIS_REGISTER_R10: return 	guest_stack_frame.r10;
	case ZYDIS_REGISTER_R11: return 	guest_stack_frame.r11;
	case ZYDIS_REGISTER_R12: return 	guest_stack_frame.r12;
	case ZYDIS_REGISTER_R13: return 	guest_stack_frame.r13;
	case ZYDIS_REGISTER_R14: return 	guest_stack_frame.r14;
	case ZYDIS_REGISTER_R15: return 	guest_stack_frame.r15;
	case ZYDIS_REGISTER_RIP: return 	guest_vmcb.save_state.rip;
	//case ZYDIS_REGISTER_RFLAGS: return 	guest_vmcb.save_state.rflags;
	default:
		return guest_vmcb.save_state.rip; // just so i can compile for now
	}
}