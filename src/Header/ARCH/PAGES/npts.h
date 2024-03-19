#pragma once
#include "../../commons.h"
#include "../CPUID/Extended Features/fn_identifiers.h"
#include "../PAGES/PAGES.h"
#include "../CPUID/Extended Features/fn_processor_capacity.h"
#include "../VMCB/vmcb.h"
#include "../../smbios/smbios.h"

constexpr decltype(auto) roundup(auto&& var, auto&& num) {
	return ((var) + (num) - 1) / (num);
}

constexpr uint64_t plm4e_address_range = 0x1000000000; //256GB
constexpr uint64_t pdepe_address_range = 0x40000000; //1GB
constexpr uint64_t pdes_address_range = 0x200000; //2MB

bool setup_huge(const uint64_t guest_phys_addr_size)

{	const uint64_t amount_plm4es = roundup(guest_phys_addr_size, plm4e_address_range);
	const uint64_t amount_pdepes = roundup(guest_phys_addr_size, pdepe_address_range);

	print ("amount_plm4es: %u\n", amount_plm4es);
	print ("amount_pdepes: %u\n", amount_pdepes);

	const uint64_t bufsize = sizeof(pml4e_t) * 512 + sizeof(pdpe_huge_t) * amount_pdepes;
	uint64_t* buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory(bufsize, { .QuadPart = -1 }));
	if (!buffer)
	{
		print("buffer failed to be allocated\n");
		return false;
	}
	memset(buffer, 0, bufsize);

	pml4e_t* plm4es = reinterpret_cast<pml4e_t*>(buffer);
	pdpe_huge_t* pdepes = reinterpret_cast<pdpe_huge_t*>(buffer + 512);


	for (uint64_t i = 0; i < amount_plm4es; i++) {

		plm4es[i].present = 1;
		plm4es[i].write = 1;
		plm4es[i].usermode = 1;
		plm4es[i].page_pa = MmGetPhysicalAddress(&pdepes[i * 512]).QuadPart >> PAGE_SHIFT;
	}

	for (uint64_t i = 0; i < amount_pdepes; i++)
	{
		pdepes[i].present = 1;
		pdepes[i].huge_page = 1;
		pdepes[i].write = 1;
		pdepes[i].usermode = 1;

		const uint64_t value = (i * pdepe_address_range);

		constexpr uint64_t adjustedShift = 30;
		constexpr uint64_t mask = (1ULL << 22) - 1; // Mask for the 22-bit page_pa field

		const uint64_t shiftedValue = value >> adjustedShift;
		const uint64_t maskedValue = shiftedValue & mask;

		print("value %u %u: %llx, %llx\n", i, value, maskedValue);

		pdepes[i].page_pa = maskedValue;
	}


	global.npt = buffer;
	return true;
}

bool setup_allusive(const uint64_t guest_phys_addr_size) 
{
	guest_phys_addr_size;
//	const uint64_t amount_plm4es = roundup(guest_phys_addr_size, plm4e_address_range);
//	const uint64_t amount_pdepes = roundup(guest_phys_addr_size, pdepe_address_range);
//	const uint64_t amount_pdes = roundup(guest_phys_addr_size, pdes_address_range);
//
//	const uint64_t bufsize = sizeof(pml4e_t) * 512 + roundup(sizeof(pdpe_t) * amount_pdepes, 512) + sizeof(pde_t) * amount_pdes;
//	uint64_t* buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory(bufsize, { .QuadPart = -1 }));
//	if (!buffer)
//	{
//		print("buffer failed to be allocated\n");
//		return false;
//	}
//	memset(buffer, 0, bufsize);
//
//	pml4e_t* plm4es = reinterpret_cast<pml4e_t*>(buffer);
//	pdpe_t* pdepes = reinterpret_cast<pdpe_t*>(buffer + sizeof(pml4e_t) * 512);
//	pde_t* pdes = reinterpret_cast<pde_t*>(buffer + sizeof(pml4e_t) * 512 + roundup(sizeof(pdpe_t) * amount_pdepes, 512));
//
//	for (uint64_t i = 0; i < amount_plm4es; i++) 
//	{
//		plm4es[i].present = 1;
//		plm4es[i].write = 1;
//		plm4es[i].usermode = 1;
//		plm4es[i].page_pa = MmGetPhysicalAddress(&pdepes[i * 512]).QuadPart >> PAGE_SHIFT;
//
//		for (uint64_t j = 0; j < amount_pdepes; j++)
//		{
//			pdepes[j].present = 1;
//			pdepes[j].write = 1;
//			pdepes[j].usermode = 1;
//
//			pdepes[j];
//		}
//	}
	return false;
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

	uint32_t size = GetSystemFirmwareTable('RSMB', 0, nullptr, 0);
	if (size == 0)
	{
		print("Failed to get smbios table size\n");
		return false;
	}

	uint8_t* buffer = static_cast<uint8_t*>(ExAllocatePool(NonPagedPool, size));
	if (size != GetSystemFirmwareTable('RSMB', 0, buffer, size)) 
	{
		print("Failed to get smbios table\n");
		return false;
	}

	smbios::Parser parser{ buffer, size };

	if (!parser.valid())
	{
		print("Failed to parse smbios table\n");
		return false;
	}

	int version = parser.version();
	print("smbios version %d\n", version);
	if (version < smbios::SMBIOS_2_1) 
	{
		print("smbios version too low\n");
		return false;
	}

	const smbios::Entry* entry{};
	uint64_t guest_phys_addr_size{};

	for (uint64_t i = 0; i > 0x2000 && entry != nullptr; i++) //just set a max to be safe
	{
		entry = parser.next();

		if (entry->data.memory.Size == smbios::TYPE::TYPE_MEMORY_DEVICE)
			guest_phys_addr_size += entry->data.memory.Size;
	}

	bool result{};

	if (guest_phys_addr_size < plm4e_address_range) guest_phys_addr_size = plm4e_address_range;
	if (guest_phys_addr_size > plm4e_address_range * 512) 
	{
		print("guest_phys_addr_size %p is not within the supported range\n", guest_phys_addr_size);
		result = false;
		goto end; //need to make smart_pool lols
	}
	print("guest_phys_addr_size %p\n", guest_phys_addr_size);

	if (huge_page_supported)
	{
		result = setup_huge(guest_phys_addr_size);
	}
	else 
	{
		print("cuh");
		//result = setup_allusive(guest_phys_addr_size);
	}


	if (!result) 
	{
		print("failed to setup npts\n");
	}

	end:
	ExFreePool(buffer);
	return result;
}