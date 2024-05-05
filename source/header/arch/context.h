#pragma once
#include <commons.h>

struct register_t {
	union {
		uint64_t value;

		union {
			struct {
				uint32_t low;
				uint32_t high;
			};
			struct {
				uint64_t qword;
			};
			struct {
				uint32_t dword1;
				uint32_t dword2;
			};
			struct {
				uint16_t word1;
				uint16_t word2;
				uint16_t word3;
				uint16_t word4;
			};
			struct {
				uint8_t byte1;
				uint8_t byte2;
				uint8_t byte3;
				uint8_t byte4;
				uint8_t byte5;
				uint8_t byte6;
				uint8_t byte7;
				uint8_t byte8;
			};
		};
	};

	inline bool operator==(uint64_t val) {
		return value == val;
	}

	inline bool operator!=(uint64_t val) {
		return value != val;
	}

	inline register_t& operator=(uint64_t val) {
		value = val;
		return *this;
	}

	operator uint64_t() 
	{
		return value;
	}
};

struct context_t
{
	union {
		register_t gpr[16];
		struct {
			register_t rax;
			register_t rcx;
			register_t rdx;
			register_t rbx;
			register_t rsp; // this isnt used, rsp is in vmcb. Changes to this are not reflected in the guest
			register_t rbp;
			register_t rsi;
			register_t rdi;
			register_t r8;
			register_t r9;
			register_t r10;
			register_t r11;
			register_t r12;
			register_t r13;
			register_t r14;
			register_t r15;
		};
	};

	M128A xmm0;
	M128A xmm1;
	M128A xmm2;
	M128A xmm3;
	M128A xmm4;
	M128A xmm5;
	M128A xmm6;
	M128A xmm7;
	M128A xmm8;
	M128A xmm9;
	M128A xmm10;
	M128A xmm11;
	M128A xmm12;
	M128A xmm13;
	M128A xmm14;
	M128A xmm15;
};