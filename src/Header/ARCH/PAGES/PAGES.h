#pragma once
#include "../../datatypes.h"

struct PML4e 
{
	uint64_t present : 1;
	uint64_t write : 1;
	uint64_t supervisor : 1;
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

struct PDPE 
{
	uint64_t present : 1;
	uint64_t write : 1;
	uint64_t supervisor : 1;
	uint64_t page_write_thru : 1;
	uint64_t page_cache_disable : 1;
	uint64_t accessed : 1;
	uint64_t dirty : 1; //only if huge
	uint64_t huge_page : 1;
	uint64_t global : 1; //only if huge
	uint64_t available_to_software : 3;
	union {
		struct {
			uint64_t page_pa : 40;
			uint64_t available : 11;
		};
		struct {
			uint64_t pat : 1;
			uint64_t reserved : 17;
			uint64_t page_pa : 22;
			uint64_t available : 7;
			uint64_t mpk : 4;
		} huge_page;
	};

	uint64_t no_execute : 1;
};

struct PDE
{
	uint64_t present : 1;
	uint64_t write : 1;
	uint64_t supervisor : 1;
	uint64_t page_write_thru : 1;
	uint64_t page_cache_disable : 1;
	uint64_t accessed : 1;
	uint64_t dirty : 1; // only if huge
	uint64_t large_page : 1;
	uint64_t global : 1; // only if huge
	uint64_t available_to_software : 3;
	union 
	{
		struct {
			uint64_t page_pa : 40;
			uint64_t available : 11;
		};
		struct {
			uint64_t pat : 1;
			uint64_t reserved : 8;
			uint64_t page_pa : 31;
			uint64_t available : 7;
			uint64_t mpk : 4;
		} large_page;
	};

	uint64_t no_execute : 1;
};

struct PTE
{
	uint64_t present : 1;
	uint64_t write : 1;
	uint64_t supervisor : 1;
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