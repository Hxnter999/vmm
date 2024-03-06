#pragma once
#include "commons.h"

enum class interrupt_type
{
	EXTERNAL_INTERRUPT = 0, //INTR
	NMI = 2, //NMI
	HARDWARE_EXCEPTION = 3, //Fault or Trap
	SOFTWARE_INTERRUPT = 4, //INT
};

struct exit_int_info 
{
	union 
	{
		struct 
		{
			uint64_t vector : 8;
			interrupt_type type : 3;
			uint64_t reserved : 19;
			uint64_t valid : 1;
			uint64_t error_code : 32;
		};
		uint64_t bits;
	};
};