#pragma once
#include "commons.h"
#include "ARCH/CPUID/Extended Features/fn_identifiers.h"
#include "ARCH/PAGES/PAGES.h"
#include "ARCH/CPUID/Extended Features/fn_processor_capacity.h"

//todo factor in mttrs
//todo gpat

PML4E* plm4es;
PDPE* pdepes;
PDE* pdes;

bool setup_huge() 
{
	constexpr uint64_t pdepe_address_range = 0x40000000; //1GB
	constexpr uint64_t plm4e_address_range = pdepe_address_range * 512; //512GB

	CPUID::fn_processor_capacity proc_cap{};
	proc_cap.load();
	const uint64_t guest_phys_addr_size = proc_cap.address_size_identifiers.guest_physical_address_size;
	
	const uint64_t amount_pdes = (guest_phys_addr_size + pdepe_address_range - 1) / pdepe_address_range; //round up
	const uint64_t amount_plm4es = (amount_pdes + 511) / 512; //round up

	plm4es = reinterpret_cast<PML4E*>(MmAllocateContiguousMemory(amount_plm4es * sizeof(PML4E), {.QuadPart = -1}));
	if (!plm4es)
	{
		print("plm4es failed to be allocated\n");
		return false;
	}

	pdepes = reinterpret_cast<PDPE*>(MmAllocateContiguousMemory(amount_pdes * sizeof(PDPE), { .QuadPart = -1 }));
	if (!pdepes)
	{
		print("pdeps failed to be allocated\n");
		return false;
	}
	memset(plm4es, 0, amount_plm4es * sizeof(PML4E));
	memset(pdepes, 0, amount_pdes * sizeof(PDPE));

	for (uint64_t i = 0; i < amount_plm4es; i++) {

		plm4es[i].present = 1;
		plm4es[i].write = 1;
		plm4es[i].usermode = 1;
		plm4es[i].page_pa = MmGetPhysicalAddress(&pdepes[i * 512]).QuadPart >> PAGE_SHIFT;


		for (uint64_t j = 0; j < min(512, amount_pdes - i * 512); j++)
		{
			pdepes[j].present = 1;
			pdepes[j].huge_page = 1;
			pdepes[j].write = 1;
			pdepes[j].usermode = 1;

			pdepes[j].uhuge_page.pat = 1;
			pdepes[j].uhuge_page.page_pa = (j * pdepe_address_range) + (i * plm4e_address_range);
		}
	}

	return true;
} 

bool setup_allusive() 
{

}

void initnpts() 
{
	//EDX bit 26 as returned by CPUID function 8000_0001h indicates 1 - Gbyte page support.
	//The EAX register as returned by CPUID function 8000_0019h reports the number of 1 - Gbyte L1 TLB entries supported
	// and EBX reports the number of 1 - Gbyte L2 TLB entries.

	//maybe want to check the amount of supported and decide if its worth using hugepages (even if they are allowed)

	CPUID::fn_identifiers ident{};
	ident.load();

	bool huge_page_supported = ident.feature_identifiers_ext.page_1gb;

	if (huge_page_supported)
	{
		setup_huge();
	}
	else 
	{
		setup_allusive();
	}
}

void deletenpts() 
{
	if(plm4es)
		MmFreeContiguousMemory(plm4es);
	if(pdeps)
		MmFreeContiguousMemory(pdeps);
	if(pdes)
		MmFreeContiguousMemory(pdes);
}