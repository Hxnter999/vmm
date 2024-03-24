#pragma once
#include "../PAGES/PAGES.h"
#include "npts.h"
#include "../CR/control_registers.h"
#include <_EPROCESS.h>"

class custom_cr3_t 
{
	static constexpr uint64_t plm4e_address_range = 0x1000000000; //256GB
	static constexpr uint64_t pdepe_address_range = 0x40000000; //1GB
	static constexpr uint64_t pdes_address_range = 0x200000; //2MB


	union {
		pml4e_t* plm4es;
		uint64_t* buffer{};
	};


public:
	custom_cr3_t()
	{

	}

	custom_cr3_t(const custom_cr3_t&) = delete;
	custom_cr3_t& operator=(const custom_cr3_t&) = delete;

	~custom_cr3_t() 
	{
		if(buffer)
			MmFreeContiguousMemory(buffer);
	}

	bool vaild() const
	{
		return buffer != nullptr;
	}

	operator bool() const
	{
		return vaild();
	}
};