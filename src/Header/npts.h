#pragma once
#include "commons.h"
#include "ARCH/CPUID/Extended Features/fn_identifiers.h"
#include "ARCH/PAGES/PAGES.h"
#include "ARCH/CPUID/Extended Features/fn_processor_capacity.h"


//todo factor in mttrs
//todo gpat

bool setup_huge() 
{
	CPUID::fn_processor_capacity proc_cap{};
	proc_cap.load();
	uint64_t GuestPhysAddrSize = proc_cap.address_size_identifiers.guest_physical_address_size;
	//our pdeps address 1 gb
	constexpr uint64_t pageaddressrange = 1'073'741'824;
	const uint64_t amountofpages = GuestPhysAddrSize / (GuestPhysAddrSize + pageaddressrange - 1);
	const uint64_t amountofplm4es = (amountofpages + 511) / 512;

	PML4E* plm4es = reinterpret_cast<PML4E*>(MmAllocateContiguousMemory(amountofplm4es * sizeof(PML4E), {.QuadPart = -1}));
	if (!plm4es)
	{
		print("plm4es failed to be allocated\n");
		return false;
	}

	PDPE* pdeps = reinterpret_cast<PDPE*>(MmAllocateContiguousMemory(amountofpages * sizeof(PDPE), { .QuadPart = -1 }));
	if (!pdeps) 
	{
		print("pdeps failed to be allocated\n");
		return false;
	}
	memset(plm4es, 0, amountofplm4es * sizeof(PML4E));
	memset(pdeps, 0, amountofpages * sizeof(PDPE));

	for (uint64_t i = 0; i < amountofplm4es; i++)
	{
		plm4es[i].present = 1;
		plm4es[i].write = 1;
		plm4es[i].usermode = 1;
		plm4es[i].page_pa = MmGetPhysicalAddress(&pdeps[i * 512]).QuadPart >> 12;
	}


	for (uint64_t i = 0; i < 512; i++)
	{
		pdeps[i].present = 1;
		pdeps[i].huge_page = 1;
		pdeps[i].write = 1;

		pdeps[i].uhuge_page.pat = 1;
		pdeps[i].uhuge_page.page_pa = (i << 48); //this is prob wrong
	}

	return true;
} 

void setup_allusive() 
{
	PDPE pdeps[512]{};

	PML4E plm4e{};
	plm4e.present = 1;
	plm4e.write = 1;
	plm4e.page_pa = reinterpret_cast<uint64_t>(&pdeps) << 12;

	for (uint64_t i = 0; i < 512; i++)
	{
		PDE pdes[512]{};

		pdeps[i].present = 1;
		pdeps[i].write = 1;
		if (i < 256)
		{
			pdeps[i].usermode = 1;
		}

		pdeps[i].page_pa = MmGetPhysicalAddress(pdes);

		for (uint64_t j = 0; j < 512; j++) 
		{

		}
	}
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