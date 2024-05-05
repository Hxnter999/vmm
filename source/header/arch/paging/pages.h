#pragma once
#include <commons.h>

struct pml4e_t
{
	union {
		uint64_t value;
		struct {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t ignored6 : 1;
			uint64_t reserved7 : 2; // 0
			uint64_t available_to_software : 3;
			uint64_t page_pa : 40;
			uint64_t available : 11;
			uint64_t no_execute : 1;
		};
	};
};

struct pdpte_1gb_t
{
	union {
		uint64_t value;
		struct {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t dirty : 1; //only if huge
			uint64_t huge_page : 1;
			uint64_t global : 1; //only if huge
			uint64_t available_to_software : 3;
			uint64_t pat : 1;
			uint64_t reserved : 17;
			uint64_t page_pa : 22;
			uint64_t available : 7;
			uint64_t mpk : 4;
			uint64_t no_execute : 1;
		};
	};
};

struct pdpte_t
{
	union {
		uint64_t value;
		struct {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t dirty : 1; //only if huge
			uint64_t huge_page : 1;
			uint64_t global : 1; //only if huge
			uint64_t available_to_software : 3;
			uint64_t page_pa : 40;
			uint64_t available : 11;
			uint64_t no_execute : 1;
		};
	};
};

struct pde_2mb_t
{
	union {
		uint64_t value;
		struct {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t dirty : 1; // only if huge
			uint64_t large_page : 1;
			uint64_t global : 1; // only if huge
			uint64_t available_to_software : 3;
			uint64_t pat : 1;
			uint64_t reserved : 8;
			uint64_t page_pa : 31;
			uint64_t available : 7;
			uint64_t mpk : 4;
			uint64_t no_execute : 1;
		};
	};
};

struct pde_t
{
	union {
		uint64_t value;
		struct {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t dirty : 1; // only if huge
			uint64_t large_page : 1;
			uint64_t global : 1; // only if huge
			uint64_t available_to_software : 3;
			uint64_t page_pa : 40;
			uint64_t available : 11;
			uint64_t no_execute : 1;
		};
	};
};

struct pte_t
{
	union {
		uint64_t value;
		struct {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t dirty : 1;
			uint64_t pat : 1;
			uint64_t global : 1; // 0
			uint64_t available_to_software : 3;
			uint64_t page_pa : 40;
			uint64_t available : 7;
			uint64_t mpk : 4;
			uint64_t no_execute : 1;
		};
	};
};

union virtual_address_t {
	uint64_t address;
	struct {
		uint64_t offset : 12;
		uint64_t pt_index : 9;
		uint64_t pd_index : 9;
		uint64_t pdpt_index : 9;
		uint64_t pml4_index : 9;
		uint64_t reserved : 16;
	};
};

// alternatively use 1gb pages for both, host page tables and nested page tables
struct alignas(0x1000) host_pt_t {
	pml4e_t pml4[512];
	pdpte_t pdpt[512];
	pde_2mb_t pd[64][512];

	static constexpr uint64_t host_pml4e = 255; // the pml4 entry that we use for the host
	static constexpr uint64_t host_pa_base = host_pml4e << (12 + 9 + 9 + 9); // base address of the entry
};

struct alignas(0x1000) npt_data_t {
	static constexpr uint64_t free_page_count = 100;

	pml4e_t pml4[512];
	pdpte_t pdpt[512];
	pde_2mb_t pd[64][512];

	// free pages for page spilliting (page smashing AMD calls it)
	uint8_t free_pages[free_page_count][0x1000];
	uint64_t free_page_pa[free_page_count];
	uint64_t free_pages_used;

	// point hidden pages to a dummy page
	uint8_t dummy[0x1000];
	uint64_t dummy_page_pa;

	pdpte_t* get_pdpte(uint64_t physical_address) {
		virtual_address_t gpa{ physical_address };

		if (gpa.pml4_index != 0)
			return nullptr;

		if (gpa.pdpt_index >= 64) // we only setup 64 entries
			return nullptr;

		return &pdpt[gpa.pdpt_index];
	}

	pde_2mb_t* get_pde(uint64_t physical_address) {
		virtual_address_t gpa{ physical_address };

		if (gpa.pml4_index != 0)
			return nullptr;

		if (gpa.pdpt_index >= 64)
			return nullptr;

		return &pd[gpa.pdpt_index][gpa.pd_index];
	}

	pte_t* get_pte(uint64_t physical_address, bool split) {
		virtual_address_t gpa{ physical_address };

		if (gpa.pml4_index != 0)
			return nullptr;

		if (gpa.pdpt_index >= 64)
			return nullptr;

		auto& pde = pd[gpa.pdpt_index][gpa.pd_index];

		if (pde.large_page) {
			if (!split)
				return nullptr;

			split_pde(pde);

			if (pde.large_page) // failed to split
				return nullptr;
		}

		auto& pte = reinterpret_cast<pte_t*>(host_pt_t::host_pa_base + (pd[gpa.pdpt_index][gpa.pd_index].page_pa << 12))[gpa.pt_index];

		return &pte;
	}

	void split_pde(pde_2mb_t& pde) {
		if (!pde.large_page) // nothing to split 
			return;

		if (free_pages_used == free_page_count) 
			return;

		uint64_t& page_pa = free_page_pa[free_pages_used];
		auto free_pt = reinterpret_cast<pte_t*>(&free_pages[free_pages_used++]);

		for (uint64_t i = 0; i < 512; i++) {
			auto& pte = free_pt[i];

			pte.present = pde.present;
			pte.write = pde.write;
			pte.usermode = pde.usermode;
			pte.page_write_thru = pde.page_write_thru;
			pte.page_cache_disable = pde.page_cache_disable;
			pte.accessed = pde.accessed;
			pte.dirty = pde.dirty;
			pte.pat = pde.pat;
			pte.global = pde.global;
			pte.available_to_software = pde.available_to_software;
			pte.page_pa = (pde.page_pa << 9) + i;
			pte.available = pde.available;
			pte.mpk = pde.mpk;
			pte.no_execute = pde.no_execute;
		}

		pde.page_pa = page_pa;
		pde.large_page = 0;
	}
};

