#pragma once
#include <commons.h>

struct rflags_t 
{
	union {
		struct {
			uint64_t CF : 1; // Carry Flag
			uint64_t reserved1 : 1; // Reserved
			uint64_t PF : 1; // Parity Flag
			uint64_t reserved2 : 1; // Reserved
			uint64_t AF : 1; // Adjust Flag
			uint64_t reserved3 : 1; // Reserved
			uint64_t ZF : 1; // Zero Flag
			uint64_t SF : 1; // Sign Flag
			uint64_t TF : 1; // Trap Flag
			uint64_t IF : 1; // Interrupt Enable Flag
			uint64_t DF : 1; // Direction Flag
			uint64_t OF : 1; // Overflow Flag
			uint64_t IOPL : 2; // I/O Privilege Level
			uint64_t NT : 1; // Nested Task
			uint64_t reserved4 : 1; // Reserved
			uint64_t RF : 1; // Resume Flag
			uint64_t VM : 1; // Virtual 8086 Mode
			uint64_t AC : 1; // Alignment Check
			uint64_t VIF : 1; // Virtual Interrupt Flag
			uint64_t VIP : 1; // Virtual Interrupt Pending
			uint64_t ID : 1; // ID Flag
			uint64_t reserved5 : 42; // Reserved
		};
		uint64_t value;
	};
};