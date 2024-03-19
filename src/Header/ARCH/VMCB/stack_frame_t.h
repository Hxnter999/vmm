#pragma once
#include "../../commons.h"
struct register_t {
	union {
		uint64_t value;
		struct {
			uint32_t low;
			uint32_t high;
		};
	};

	inline bool operator==(uint64_t val) {
		return value == val;
	}

	inline bool operator!=(uint64_t val) {
		return value != val;
	}
};

struct stack_frame_t
{
	register_t rax; // rax is in vmcb
	register_t rcx;
	register_t rbx;
	register_t rdx;
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

	M128A xmm0;
	M128A xmm1;
	M128A xmm2;
	M128A xmm3;
	M128A xmm4;
	M128A xmm5;
};