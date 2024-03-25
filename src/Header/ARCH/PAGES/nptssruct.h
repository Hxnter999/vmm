#pragma once
#include <pages/pages.h>
#include <pages/npts.h>


class npts_t
{
	static constexpr uint64_t plm4e_address_range = 0x1000000000; //256GB
	static constexpr uint64_t pdepe_address_range = 0x40000000; //1GB
	static constexpr uint64_t pdes_address_range = 0x200000; //2MB

	uint64_t capacity{};

	uint64_t amount_plm4es{};
	uint64_t amount_pdepes{};
	uint64_t amount_pdes{};

	union {
		pml4e_t* plm4es;
		uint64_t* buffer{};
	};
	union {
		pdpe_t* pdepes{};
		pdpe_huge_t* pdepes_huge;
	};
	pde_large_t* pdes{};


	bool setup_huge(const uint64_t guest_phys_addr_size)
	{
		amount_plm4es = roundup(guest_phys_addr_size, plm4e_address_range);
		amount_pdepes = roundup(guest_phys_addr_size, pdepe_address_range);

		capacity = 512 * sizeof(pml4e_t) + (roundup(amount_pdepes, 512) * 512) * sizeof(pdpe_t);
		buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory(capacity, { .QuadPart = -1 }));
		if (!buffer)
		{
			print("buffer failed to be allocated\n");
			return;
		}
		memset(buffer, 0, capacity);

		pdepes_huge = reinterpret_cast<pdpe_huge_t*>(buffer + 512);


		for (uint64_t i = 0; i < amount_plm4es; i++) {

			pdepes_huge[i].present = 1;
			pdepes_huge[i].write = 1;
			pdepes_huge[i].usermode = 1;
			pdepes_huge[i].page_pa = MmGetPhysicalAddress(&pdepes_huge[i * 512]).QuadPart >> PAGE_SHIFT;
		}

		for (uint64_t i = 0; i < amount_pdepes; i++)
		{
			pdepes_huge[i].present = 1;
			pdepes_huge[i].huge_page = 1;
			pdepes_huge[i].write = 1;
			pdepes_huge[i].usermode = 1;
			pdepes_huge[i].page_pa = i;
		}
	}

	/*bool setup_large(const uint64_t guest_phys_addr_size)
	{
		const uint64_t amount_plm4es = roundup(guest_phys_addr_size, plm4e_address_range);
		const uint64_t amount_pdepes = roundup(guest_phys_addr_size, pdepe_address_range);
		const uint64_t amount_pdes = roundup(guest_phys_addr_size, pdes_address_range);

		//this is odd maybe make better later if i get a complaint
		const uint64_t real_pdepes_size = sizeof(pdpe_t) * roundup(amount_pdepes, 512) * 512; //used because pdes must be page alligned
		const uint64_t bufsize = sizeof(pml4e_t) * 512 + sizeof(pdpe_t) * real_pdepes_size + sizeof(pde_large_t) * amount_pdes;
		buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory(bufsize, { .QuadPart = -1 }));
		if (!buffer)
		{
			print("buffer failed to be allocated\n");
			return;
		}
		memset(buffer, 0, bufsize);

		pml4e_t* plm4es = reinterpret_cast<pml4e_t*>(buffer);
		pdpe_t* pdepes = reinterpret_cast<pdpe_t*>(buffer + sizeof(pml4e_t) * 512);
		pde_large_t* pdes = reinterpret_cast<pde_large_t*>(buffer + sizeof(pml4e_t) * 512 + real_pdepes_size);

		for (uint64_t i = 0; i < amount_plm4es; i++) {

			plm4es[i].present = 1;
			plm4es[i].write = 1;
			plm4es[i].usermode = 1;
			plm4es[i].page_pa = MmGetPhysicalAddress(&pdepes[i * 512]).QuadPart >> PAGE_SHIFT;
		}

		for (uint64_t i = 0; i < amount_pdepes; i++)
		{
			pdepes[i].present = 1;
			pdepes[i].write = 1;
			pdepes[i].usermode = 1;
			pdepes[i].page_pa = MmGetPhysicalAddress(&pdes[i * 512]).QuadPart >> PAGE_SHIFT;
		}

		for (uint64_t i = 0; i < amount_pdes; i++)
		{
			pdes[i].present = 1;
			pdes[i].large_page = 1;
			pdes[i].write = 1;
			pdes[i].usermode = 1;
			pdes[i].page_pa = i; // should working tion, need to testingtion
		}
	}*/

	void addPageHuge(const uint64_t pdepe_index)
	{
		if (pdepe_index <= amount_plm4es) 
		{
			//UH OH!
			print("pdepe_index: %u is already mapped UH OH", pdepe_index);
			__debugbreak();
			return;
		}

		//if there is a plm4e that addresses (because we round up to 512 amount_pdepes)
		//then we dont need to realloc (move)
		if (pdepe_index / 512 < amount_plm4es) // guh
		{
			//We simply need to add the entry in this case
			for (uint64_t i = amount_pdepes - 1; i <= pdepe_index; i++)
			{
				pdepes_huge[i].present = 1;
				pdepes_huge[i].huge_page = 1;
				pdepes_huge[i].write = 1;
				pdepes_huge[i].usermode = 1;
				pdepes_huge[i].page_pa = i;
			}
			return;
		} 

		//If there isnt a plm4e that addresses this we need to move buffer (realloc)
		//Calc amount of plm4e's that are needed
		const uint64_t new_amount_plm4es = roundup(pdepe_index, 512);
		const uint64_t new_capacity = 512 * sizeof(pml4e_t) + (roundup(amount_pdepes, 512) * 512) * sizeof(pdpe_t);
		uint64_t* const new_buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory(capacity, { .QuadPart = -1 }));
		memcpy(new_buffer, buffer, capacity);

	}

	void addPageLarge(const uint64_t pde_index) 
	{
		
	}

public:
	void identitymap(const uint64_t guest_phys_addr_size)
	{
		CPUID::fn_identifiers ident{};
		ident.load();

		bool huge_page_supported = ident.feature_identifiers_ext.page_1gb;

		if (huge_page_supported) 
		{
			setup_huge(guest_phys_addr_size);
		}
		else 
		{
			
		}

		print("amount_plm4es: %u\n", amount_plm4es);
		print("amount_pdepes: %u\n", amount_pdepes);
		print("amount_pdes: %u\n", amount_pdes);
	}

	//delete copy ctor and assignment operator
	npts_t(const npts_t&) = delete;
	npts_t& operator=(const npts_t&) = delete;

	~npts_t() 
	{
		MmFreeContiguousMemory(buffer);
	}

	//Parameter is pde or pdepe depending if huge 
	void addPage(const uint64_t bottom_index) 
	{
		if (huge()) 
		{
			addPageHuge(bottom_index);
		}
		else 
		{
			addPageLarge(bottom_index);
		}
	}

	bool vaild()
	{
		return plm4es != nullptr;
	}

	bool huge() 
	{
		return pdes == nullptr;
	}
};