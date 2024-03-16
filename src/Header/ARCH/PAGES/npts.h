#pragma once
#include "../../commons.h"
#include "../CPUID/Extended Features/fn_identifiers.h"
#include "../PAGES/PAGES.h"
#include "../CPUID/Extended Features/fn_processor_capacity.h"
#include "../VMCB/vmcb.h"

//todo factor in mttrs
//todo gpat



bool setup_huge() 
{
	constexpr uint64_t pdepe_address_range = 0x40000000; //1GB
	constexpr uint64_t plm4e_address_range = pdepe_address_range * 512; //512GB

	CPUID::fn_processor_capacity proc_cap{};
	proc_cap.load();
	const uint64_t guest_phys_addr_size = proc_cap.address_size_identifiers.guest_physical_address_size > 0 ? proc_cap.address_size_identifiers.guest_physical_address_size : proc_cap.address_size_identifiers.physical_address_size; //When this field is zero, refer to the PhysAddrSiz
	print("guest_phys_addr_size %p\n", guest_phys_addr_size);
	const uint64_t amount_pdepes = (guest_phys_addr_size + pdepe_address_range - 1) / pdepe_address_range; //round up
	const uint64_t amount_plm4es = (amount_pdepes + 511) / 512; //round up

	uint64_t* buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory((amount_plm4es + amount_pdepes) * 8, { .QuadPart = -1 }));
	if (!buffer)
	{
		print("buffer failed to be allocated\n");
		return false;
	}
	memset(buffer, 0, (amount_plm4es + amount_pdepes) * 8);

	pml4e_t* plm4es = reinterpret_cast<pml4e_t*>(buffer);
	pdpe_huge_t* pdepes = reinterpret_cast<pdpe_huge_t*>(buffer + amount_plm4es);


	for (uint64_t i = 0; i < amount_plm4es; i++) {

		plm4es[i].present = 1;
		plm4es[i].write = 1;
		plm4es[i].usermode = 1;
		plm4es[i].page_pa = MmGetPhysicalAddress(&pdepes[i * 512]).QuadPart >> PAGE_SHIFT;


		for (uint64_t j = 0; j < min(512, amount_pdepes - i * 512); j++)
		{
			pdepes[j].present = 1;
			pdepes[j].huge_page = 1;
			pdepes[j].write = 1;
			pdepes[j].usermode = 1;

			const uint64_t value = (j * pdepe_address_range) + (i * plm4e_address_range);

			constexpr uint64_t adjustedShift = 28; // 30
			const uint64_t shiftedValue = value >> adjustedShift;
			const uint64_t mask = (1ULL << 22) - 1; // Mask for the 22-bit page_pa field
			const uint64_t maskedValue = shiftedValue & mask;

			print("value %u %u: %llx, %llx\n", i, j, value, maskedValue);

			pdepes[j].page_pa = maskedValue;
		}
	}

	global.npt = buffer;
	return true;
} 

bool setup_allusive() 
{
	constexpr uint64_t pdes_address_range = 0x200000; //2MB
	constexpr uint64_t pdepe_address_range = pdes_address_range * 512;
	constexpr uint64_t plm4e_address_range = pdepe_address_range * 512;

	CPUID::fn_processor_capacity proc_cap{};
	proc_cap.load();
	const uint64_t guest_phys_addr_size = proc_cap.address_size_identifiers.guest_physical_address_size;

	const uint64_t amount_pdes = (guest_phys_addr_size + pdes_address_range - 1) / pdes_address_range; //round up
	const uint64_t amount_pdepes = (amount_pdes + 511) / 512; //round up
	const uint64_t amount_plm4es = (amount_pdepes + 511) / 512; //round up

	uint64_t* buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory((amount_plm4es + amount_pdepes + amount_pdes) * 8, { .QuadPart = -1 }));
	if (!buffer)
	{
		print("buffer failed to be allocated\n");
		return false;
	}
	memset(buffer, 0, (amount_plm4es + amount_pdepes + amount_pdes) * 8);

	pml4e_t* plm4es = reinterpret_cast<pml4e_t*>(buffer);
	pdpe_t* pdepes = reinterpret_cast<pdpe_t*>(buffer + amount_plm4es);
	pde_large_t* pdes = reinterpret_cast<pde_large_t*>(buffer + amount_plm4es + amount_pdepes);

	for (uint64_t i = 0; i < amount_plm4es; i++) {

		plm4es[i].present = 1;
		plm4es[i].write = 1;
		plm4es[i].usermode = 1;
		plm4es[i].page_pa = MmGetPhysicalAddress(&pdepes[i * 512]).QuadPart >> PAGE_SHIFT;


		for (uint64_t j = 0; j < min(512, amount_pdepes - i * 512); j++)
		{
			pdepes[j].present = 1;
			pdepes[j].write = 1;
			pdepes[j].usermode = 1;
			pdepes[j].page_pa = MmGetPhysicalAddress(&pdes[j * 512]).QuadPart >> PAGE_SHIFT;

			for (uint64_t k = 0; k < min(512, amount_pdes - (j * 512) - (i * 512 * 512)); k++) 
			{
				pdes[k].present = 1;
				pdes[k].write = 1;
				pdes[k].usermode = 1;
				pdes[k].large_page = 1;

				const uint64_t value = (k * pdes_address_range) + (j * pdepe_address_range) + (i * plm4e_address_range);
				constexpr uint64_t shiftAmount = 21;
				const uint64_t mask = (1ULL << 31) - 1; // Mask for the 31-bit page_pa field
				const uint64_t shiftedValue = (value >> shiftAmount) & mask;

				pdes[k].page_pa = shiftedValue;
			}
		}
	}
	global.npt = buffer;

	return true;
}

bool initnpts() 
{
	//EDX bit 26 as returned by CPUID function 8000_0001h indicates 1 - Gbyte page support.
	//The EAX register as returned by CPUID function 8000_0019h reports the number of 1 - Gbyte L1 TLB entries supported
	// and EBX reports the number of 1 - Gbyte L2 TLB entries.

	//maybe want to check the amount of supported and decide if its worth using hugepages (even if they are allowed)

	CPUID::fn_identifiers ident{};
	ident.load();

	bool huge_page_supported = ident.feature_identifiers_ext.page_1gb;

	bool result{};
	if (huge_page_supported)
	{
		result = setup_huge();
	}
	else 
	{
		print("Failed (huge pages not supported)\n");
		//result = setup_allusive();
	}

	if (!result) 
	{
		print("failed to setup npts\n");
	}

	return result;
}