#pragma once
#include <commons.h>
#include <cpuid/extended-features/fn_identifiers.h>
#include <cpuid/extended-features/fn_processor_capacity.h>
#include <pages/pages.h>
#include <vmcb/vmcb.h>
#include <smbios/smbios.h>
#include <hypervisor.h>

constexpr decltype(auto) roundup(auto&& var, auto&& num) {
	return ((var) + (num) - 1) / (num);
}

inline bool setup_huge(const uint64_t guest_phys_addr_size, uint64_t*& buffer)
{
	const uint64_t amount_plm4es = roundup(guest_phys_addr_size, plm4e_address_range);
	const uint64_t amount_pdepes = roundup(guest_phys_addr_size, pdpes_address_range);

	print ("amount_plm4es: %u\n", amount_plm4es);
	print ("amount_pdepes: %u\n", amount_pdepes);

	const uint64_t bufsize = sizeof(pml4e_t) * 512 + sizeof(pdpe_t) * amount_pdepes;
	buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory(bufsize, { .QuadPart = -1 }));
	if (!buffer)
	{
		print("buffer failed to be allocated\n");
		return false;
	}
	memset(buffer, 0, bufsize);

	pml4e_t* plm4es = reinterpret_cast<pml4e_t*>(buffer);
	pdpe_t* pdepes = reinterpret_cast<pdpe_t*>(buffer + 512);


	for (uint64_t i = 0; i < amount_plm4es; i++) {

		plm4es[i].present = 1;
		plm4es[i].write = 1;
		plm4es[i].usermode = 1;
		plm4es[i].page_pa = MmGetPhysicalAddress(&pdepes[i * 512]).QuadPart >> 12;
	}

	for (uint64_t i = 0; i < amount_pdepes; i++)
	{
		pdepes[i].huge.present = 1;
		pdepes[i].huge.huge_page = 1;
		pdepes[i].huge.write = 1;
		pdepes[i].huge.usermode = 1;
		pdepes[i].huge.page_pa = i;
	}

	return true;
}

inline bool setup_allusive(const uint64_t guest_phys_addr_size, uint64_t*& buffer)
{
	const uint64_t amount_plm4es = roundup(guest_phys_addr_size, plm4e_address_range);
	const uint64_t amount_pdepes = roundup(guest_phys_addr_size, pdpes_address_range);
	const uint64_t amount_pdes   = roundup(guest_phys_addr_size, pdes_address_range);

	print("amount_plm4es: %u\n", amount_plm4es);
	print("amount_pdepes: %u\n", amount_pdepes);
	print("amount_pdes: %u\n", amount_pdes);

	//this is odd maybe make better later if i get a complaint
	const uint64_t real_pdepes_size = sizeof(pdpe_t) * roundup(amount_pdepes, 512) * 512; //used because pdes must be page alligned
	const uint64_t bufsize = sizeof(pml4e_t) * 512 + sizeof(pdpe_t) * real_pdepes_size + sizeof(pde_t) * amount_pdes;
	buffer = static_cast<uint64_t*>(MmAllocateContiguousMemory(bufsize, { .QuadPart = -1 }));
	if (!buffer)
	{
		print("buffer failed to be allocated\n");
		return false;
	}
	memset(buffer, 0, bufsize);

	pml4e_t* plm4es = reinterpret_cast<pml4e_t*>(buffer);
	pdpe_t* pdepes = reinterpret_cast<pdpe_t*>(buffer + sizeof(pml4e_t) * 512);
	pde_t* pdes = reinterpret_cast<pde_t*>(buffer + sizeof(pml4e_t) * 512 + real_pdepes_size);

	for (uint64_t i = 0; i < amount_plm4es; i++) {

		plm4es[i].present = 1;
		plm4es[i].write = 1;
		plm4es[i].usermode = 1;
		plm4es[i].page_pa = MmGetPhysicalAddress(&pdepes[i * 512]).QuadPart >> 12;
	}

	for (uint64_t i = 0; i < amount_pdepes; i++) 
	{
		pdepes[i].present = 1;
		pdepes[i].write = 1;
		pdepes[i].usermode = 1;
		pdepes[i].page_pa = MmGetPhysicalAddress(&pdes[i * 512]).QuadPart >> 12;
	}

	for (uint64_t i = 0; i < amount_pdes; i++) 
	{
		pdes[i].large.present = 1;
		pdes[i].large.large_page = 1;
		pdes[i].large.write = 1;
		pdes[i].large.usermode = 1;
		pdes[i].large.page_pa = i; // should working tion, need to testingtion
	}

	return true;
}

inline bool initnpts(uint64_t*& nptbuffer)  //TODO: add support for IO devices (dynamiclly add pages)
{

	////maybe want to check the amount of supported TLB shittery and decide if its worth using hugepages (even if they are allowed)

	//uint32_t size = GetSystemFirmwareTable('RSMB', 0, nullptr, 0);
	//if (size == 0)
	//{
	//	print("Failed to get smbios table size\n");
	//	return false;
	//}

	//uint8_t* buffer = static_cast<uint8_t*>(ExAllocatePool(NonPagedPool, size));
	//if (size != GetSystemFirmwareTable('RSMB', 0, buffer, size)) 
	//{
	//	print("Failed to get smbios table\n");
	//	return false;
	//}

	//smbios::Parser parser{ buffer, size };

	//if (!parser.valid())
	//{
	//	print("Failed to parse smbios table\n");
	//	return false;
	//}

	//int version = parser.version();
	//print("smbios version %d\n", version);
	//if (version < smbios::SMBIOS_2_1) 
	//{
	//	print("smbios version too low\n");
	//	return false;
	//}

	//const smbios::Entry* entry{};
	uint64_t guest_phys_addr_size = plm4e_address_range;

	//for (uint64_t i = 0; i > 0x2000 && entry != nullptr; i++) //just set a max to be safe
	//{
	//	entry = parser.next();

	//	if (entry->data.memory.Size == smbios::TYPE::TYPE_MEMORY_DEVICE)
	//		guest_phys_addr_size += entry->data.memory.Size;
	//}

	bool result{};
	CPUID::fn_identifiers ident{};
	ident.load();

	bool huge_page_supported = ident.feature_identifiers_ext.page_1gb;


	//if (guest_phys_addr_size < plm4e_address_range) guest_phys_addr_size = plm4e_address_range;
	//if (guest_phys_addr_size > plm4e_address_range * 512) 
	//{
	//	print("guest_phys_addr_size %p is not within the supported range\n", guest_phys_addr_size);
	//	result = false;
	//	goto end; //need to make smart_pool lols
	//}
	print("guest_phys_addr_size %p\n", guest_phys_addr_size);



	if (huge_page_supported)
	{
		result = setup_huge(guest_phys_addr_size, nptbuffer);
	}
	else 
	{
		print("cuh");
		result = setup_allusive(guest_phys_addr_size, nptbuffer);
	}


	if (!result) 
	{
		print("failed to setup npts\n");
	}

	//end:
	//ExFreePool(buffer);
	return result;
}