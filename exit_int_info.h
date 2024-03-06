#pragma once
#include "commons.h"

//"Only events raised by the INTn instruction (opcode CDh) are
// considered software interrupts."
// everthing else is handled by this struct
enum class interrupt_type
{
	EXTERNAL_INTERRUPT = 0, //INTR
	NMI = 2, //NMI
	HARDWARE_EXCEPTION = 3, //Fault or Trap
	SOFTWARE_INTERRUPT = 4, //INT
};

// "When an exception triggers an intercept, the EXITCODE, and optionally EXITINFO1 and
// EXITINFO2, fields always reflect the intercepted exception, while EXITINTINFO, if marked valid,
// indicates the prior (not our VMEXIT) exception the guest was attempting to deliver when the intercept occurred."
// This is EXITINTINFO
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