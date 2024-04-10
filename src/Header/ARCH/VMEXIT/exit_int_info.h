#pragma once
#include <commons.h>

//"Only events raised by the INTn instruction (opcode CDh) are
// considered software interrupts."
// everthing else is handled by this struct
enum class INTERRUPT_TYPE : uint64_t
{
	EXTERNAL_INTERRUPT = 0, //INTR
	NMI = 2, //NMI
	HARDWARE_EXCEPTION = 3, //Fault or Trap
	SOFTWARE_INTERRUPT = 4, //INT
};

//https://wiki.osdev.org/Exceptions
enum class EXCEPTION_VECTOR : uint64_t
{
	DE = 0,
	DB = 1,
	NMI = 2,
	BP = 3,
	OF = 4,
	BR = 5,
	UD = 6,
	NM = 7,
	DF = 8,
	CSO = 9,
	TS = 10,
	NP = 11,
	SS = 12,
	GP = 13,
	PF = 14,
	MF = 16,
	AC = 17,
	MC = 18,
	XM = 19,
	XF = 19,
	VE = 20,
	CP = 21,
	HV = 28,
	VC = 29,
	SX = 30
};

// "When an exception triggers an intercept, the EXITCODE, and optionally EXITINFO1 and
// EXITINFO2, fields always reflect the intercepted exception, while EXITINTINFO, if marked valid,
// indicates the prior (not our VMEXIT) exception the guest was attempting to deliver when the intercept occurred."
// This is EXITINTINFO

struct exit_int_info_t 
{
	union 
	{
		struct 
		{


			union {
				struct {
					//NMI doesnt use this field (alpha)
					EXCEPTION_VECTOR evector : 8; //it can be other stuff but for now guh
					INTERRUPT_TYPE type : 3;
					uint64_t ev : 1;
					uint64_t reserved : 19;
					uint64_t valid : 1;
					uint64_t error_code : 32;
				};

				struct {
					//NMI doesnt use this field (alpha)
					uint64_t vector : 8; //it can be other stuff but for now guh
					INTERRUPT_TYPE type : 3;
					uint64_t ev : 1;
					uint64_t reserved : 19;
					uint64_t valid : 1;
					uint64_t error_code : 32;
				};
			};
		};
		uint64_t bits;
	};
};