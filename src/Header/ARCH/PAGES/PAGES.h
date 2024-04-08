#pragma once
#include <commons.h>

struct pml4e_t 
{
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

struct pdpe_t 
{
	union {
		struct {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t reserved1 : 1;
			uint64_t huge_page : 1;
			uint64_t reserved2 : 1;
			uint64_t available_to_software : 3;
			uint64_t page_pa : 40;
			uint64_t available : 11;
			uint64_t no_execute : 1;
		};
		struct pdpe_huge_t {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t dirty : 1;
			uint64_t huge_page : 1;
			uint64_t global : 1;
			uint64_t available_to_software : 3;
			uint64_t pat : 1;
			uint64_t reserved : 17;
			uint64_t page_pa : 22;
			uint64_t available : 7;
			uint64_t mpk : 4;
			uint64_t no_execute : 1;
		} huge;
	};
};

struct pde_t
{
	union {
		struct {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t reserved1 : 1;
			uint64_t large_page : 1;
			uint64_t reserved2 : 1;
			uint64_t available_to_software : 3;
			uint64_t page_pa : 40;
			uint64_t available : 11;
			uint64_t no_execute : 1;
		};
		struct pde_large_t {
			uint64_t present : 1;
			uint64_t write : 1;
			uint64_t usermode : 1;
			uint64_t page_write_thru : 1;
			uint64_t page_cache_disable : 1;
			uint64_t accessed : 1;
			uint64_t dirty : 1;
			uint64_t large_page : 1;
			uint64_t global : 1;
			uint64_t available_to_software : 3;
			uint64_t pat : 1;
			uint64_t reserved : 8;
			uint64_t page_pa : 31;
			uint64_t available : 7;
			uint64_t mpk : 4;
			uint64_t no_execute : 1;
		} large;
	};
};

struct pte_t
{
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

struct alignas(0x1000) host_pt_t {
	pml4e_t pml4[512];
	pdpe_t pdpt[512];
	pde_t pd[64][512];
	
	static constexpr uint64_t phys_pml4e = 255;
	static constexpr uint64_t host_pa_base = phys_pml4e << (12 + 9 + 9 + 9);
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

	virtual_address_t(uint64_t v) : address(v) {}
};